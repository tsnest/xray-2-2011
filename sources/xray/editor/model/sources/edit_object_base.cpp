////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_base.h"
#include "model_editor.h"
#include "property_grid_panel.h"
#include "lod_view_panel.h"
#include "unique_name_helper.h"
#include "object_portals_panel.h"
#include "portal_generation_view.h"
#include "bsp_tree.h"
#include "ide.h"
#include "input_actions.h"
#include "render_model_wrapper.h"
#include "sectors_generator.h"
#include "sectors_view.h"

#pragma managed( push, off )
#	include <xray/render/facade/editor_renderer.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/editor/base/collision_object_types.h>
#	include <xray/collision/space_partitioning_tree.h>
#pragma managed( pop )

using namespace System::Windows::Forms;

namespace xray {
namespace model_editor {

using editor_base::action_delegate;
using editor_base::execute_delegate_managed;
using editor_base::checked_delegate_managed;
using editor_base::enabled_delegate_managed;



edit_object_mesh::edit_object_mesh( model_editor^ me, bool is_skinned )
:super								( me ),
m_need_refresh_surfaces_visibility	( false ),
m_need_refresh_properties			( false ),
m_model_visible						( false ),
m_hq_collision_object				( NULL ),
m_bsp_tree							( NULL ),
m_sectors_generator					( NULL ),
m_is_skinned						( is_skinned )
{
	m_type_name			= "mesh" ;
	m_model				= gcnew render_model_wrapper( );
	m_settings_cfg		= NEW(configs::lua_config_ptr)();
	m_collision_cfg		= NEW(configs::lua_config_ptr)();
	m_physics_settings	= gcnew rigid_body_construction_info_ref( this );
	m_lods				= gcnew array<lod_descriptor^>(3);
	m_lods[0]			= gcnew lod_descriptor(this);
	m_lods[1]			= gcnew lod_descriptor(this);
	m_lods[2]			= gcnew lod_descriptor(this);
	m_lods[0]->name		= "LOD0";
	m_lods[1]->name		= "LOD1";
	m_lods[2]->name		= "LOD2";

	m_unique_portal_name_helper		= gcnew unique_name_helper();
	m_object_surfaces_panel			= gcnew property_grid_panel( );
	m_collision_panel				= gcnew collision_property_grid_panel( );
	m_collision_panel->Text			= "Collision";

	create_lod_view					( );
	m_object_portals_panel			= gcnew object_portals_panel( this );

	m_bsp_tree						= NEW ( bsp_tree )( );
	m_portal_generation_view		= gcnew portal_generation_view( m_bsp_tree );
	this->update_portals_generation_property_view();
	m_current_mode					= edit_mode::edit_none;
	m_mass_center_item	= gcnew mass_center_item( this );
	m_sectors_view					= gcnew sectors_view();
}

edit_object_mesh::~edit_object_mesh( )
{
	delete		m_sectors_view;
	DELETE		(m_sectors_generator);
	delete		m_portal_generation_view;
	DELETE		( m_bsp_tree );
	R_ASSERT	( m_collision_object == NULL, "Collision geometry isn't deleted. Call \"destroy_collision_geometry\" before" );
	R_ASSERT	( m_surfaces.Count==0, "Object not empty, call Unload before" );
	delete		m_model;
	DELETE		( m_settings_cfg );
	DELETE		( m_collision_cfg );
	delete		m_unique_portal_name_helper;
	delete		m_lod_view_panel;
	delete		m_object_portals_panel;
	delete		m_object_surfaces_panel;
	delete		m_collision_panel;
	delete		m_physics_settings;
	delete		m_mass_center_item;
}

void edit_object_mesh::tick( )
{
	super::tick();

	if ( m_need_refresh_surfaces_visibility )
	{
		refresh_surfaces_visibility_impl	( );
		m_need_refresh_surfaces_visibility	= false;
	}

	if ( m_need_refresh_properties )
	{
		m_need_refresh_properties			= false;
		m_object_surfaces_panel->set_property_container( get_surfaces_property_container() );
		m_model_editor->refresh_caption		( );
		fill_lod_view						( );
	}

	render::debug::renderer& debug_renderer =  m_model_editor->editor_renderer()->debug();

	if(m_current_mode==edit_mode::edit_portals)
	{
		for each ( edit_portal^ portal in m_portals )
			portal->render( m_model_editor->scene(), debug_renderer);
	}else
	if(m_current_mode==edit_mode::edit_collision)
	{
		//rgba
		u32 const colors_table_size = 4;
		static math::color clr_table[colors_table_size]={
			math::color(104, 253, 73, 180),
			math::color(248, 253, 73, 180),
			math::color(78, 222, 255, 180),
			math::color(235, 77, 255, 180)
		};
		
		math::color clr_sel	(255, 147, 147, 200);

		editor_base::transform_control_object^ o = get_model_editor()->m_transform_control_helper->m_object;
		u32 clr_index = 0;
		for each ( collision_primitive_item^ prim in m_collision_primitives )
		{
			bool is_selected =  (o==prim);
			math::color clr = (is_selected)? clr_sel : clr_table[clr_index%colors_table_size];
			prim->render( m_model_editor->scene(), debug_renderer, clr );
			clr_index++;
		}
		m_mass_center_item->render( m_model_editor->scene(), debug_renderer );
	}

	if( m_draw_object_collision )
	{
		//collision::geometry_ptr g			= m_model->get_collision_geometry();
		//if(g.c_ptr())
		//	g->render( m_model_editor->scene(), debug_renderer, math::float4x4().identity() );

		get_model_editor()->collision_tree->render( m_model_editor->scene(), debug_renderer );
	}

	m_bsp_tree->render( m_model_editor->scene(), debug_renderer);
	if ( m_sectors_generator )
		m_sectors_generator->render( m_model_editor->scene(), debug_renderer);
}

void edit_object_mesh::clear_resources( )
{
	super::clear_resources	( );

	R_ASSERT( m_model_editor->editor_renderer() );
	
	clear_lod_view				( );

	for each( lod_descriptor^ d in m_lods)
		d->m_surfaces.Clear();

	for each ( edit_surface^ s in m_surfaces.Values )
		delete s;
	m_surfaces.Clear();

	for each ( edit_portal^ p in m_portals )
		delete p;
	m_portals.Clear();

	for each ( collision_primitive_item^ p in m_collision_primitives )
		delete p;
	m_collision_primitives.Clear();

	if(has_preview_model())
		remove_preview_model();

	reset_modified	( );
	delete			m_lod_view_panel;
}

pcstr edit_object_mesh::model_file_extension( )
{
	return m_is_skinned ? ".skinned_model" : ".model";
}

void edit_object_mesh::load( System::String^ model_full_name )
{
	m_full_name				= model_full_name;
	unmanaged_string uname	( m_full_name );

	fs_new::virtual_path_string	model_name;
	model_name.assignf("resources/models/%s%s/render", uname.c_str(), model_file_extension() );
	
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, 
															&edit_object_mesh::on_model_folder_fs_iterator_ready), 
															g_allocator);

	resources::query_vfs_iterator(
		model_name.c_str(), 
		boost::bind(&fs_iterator_delegate::callback, q, _1),
		g_allocator,
		resources::recursive_true
		);

	// query for collision
	fs_new::virtual_path_string			collision_name;
	collision_name.assignf	("resources/models/%s%s/collision/primitives", uname.c_str(), model_file_extension() );
	
	query_result_delegate* qr = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, 
														&edit_object_mesh::on_collision_settings_ready), 
														g_allocator);

	resources::query_resource(
		collision_name.c_str(),
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, qr, _1),
		g_allocator,
		0,
		0,
		assert_on_fail_false
		);
	m_portal_generation_view->set_model_name( m_full_name );
}

void edit_object_mesh::on_model_settings_ready( resources::queries_result& data )
{
	if( data.is_successful() )
	{
		(*m_settings_cfg)			= static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());
	}else
		(*m_settings_cfg)			= configs::create_lua_config();

	bool load_result				= load_model_settings( );

	if(!load_result)
		set_modified	( );

	on_surfaces_loaded				( );
}

void edit_object_mesh::on_model_folder_fs_iterator_ready( vfs::vfs_locked_iterator const & in_fs_it )
{
	R_ASSERT		( m_surfaces.Count == 0 );
	
	if( in_fs_it.is_end() ) // not completed model (maybe deleted)
		return;
	
	vfs::vfs_iterator fs_it				= in_fs_it.children_begin();

	while(!fs_it.is_end())
	{
		if( fs_it.is_folder() )
		{
			System::String^ sname		= gcnew System::String(fs_it.get_name());
			edit_surface^ s				= gcnew edit_surface( this, sname );
			m_surfaces.Add				( sname, s );
			set_surface_lod				( s, m_lods[0], false );
		}
		++fs_it;
	}

	for each ( edit_surface^ s in m_surfaces.Values )
		s->query_export_properties	( );

	m_model_editor->refresh_caption();

	unmanaged_string uname	( m_full_name );
	fs_new::virtual_path_string			model_name;
	model_name.assignf		("resources/models/%s%s/settings", uname.c_str(), model_file_extension() );
	
	query_result_delegate* qr = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, 
														&edit_object_mesh::on_model_settings_ready), 
														g_allocator);

	resources::query_resource(
		model_name.c_str(),
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, qr, _1),
		g_allocator
		);
}

bool edit_object_mesh::load_model_settings( )
{
	bool result = true;
	configs::lua_config_value const& root = (*m_settings_cfg)->get_root();

	if(root.size())
	{
		configs::lua_config_value const& msettings = root["material_settings"];
		for each ( edit_surface^ s in m_surfaces.Values )
		{
			unmanaged_string sg_name( s->name );
			if(msettings.value_exists(sg_name.c_str()))
			{
				configs::lua_config_value const& current	= msettings[sg_name.c_str()];
				s->load_material_settings					( current );
			}else
			{
				s->setup_default_material_settings			( );
				result										= false;
			}
		}

		configs::lua_config_value const& lods_table = root["lod_hierrarchy"];
		for each ( lod_descriptor^ l in m_lods )
		{
			unmanaged_string lod_name( l->name );

			if(lods_table.value_exists(lod_name.c_str()))
			{
				configs::lua_config_value const& current	= lods_table[lod_name.c_str()];
				l->load_settings							( current );
			}
		}	

		if((int)msettings.size() != m_surfaces.Count)
			result								= false;
	}
	else
	{
		for each ( edit_surface^ s in m_surfaces.Values )
			s->setup_default_material_settings( );

		result = true;
	}

	load_portals					( root );
	m_physics_settings->load_from	( *m_settings_cfg );

#if 0	
	geometry_utils::geometry_collector collector;
	string_list							selected_surfaces;

	for each ( edit_surface^ s in m_surfaces.Values )
		selected_surfaces.Add(s->name);

	collect_surfaces( m_full_name, %selected_surfaces, &collector );
	collector.write_obj_file("c:\\collected.obj", 1.0f);
#endif

	return result;
}

bool edit_object_mesh::load_portals( configs::lua_config_value const& cfg_root )
{
	for each ( edit_portal^ portal in m_portals )
	{	
		delete portal;
	}
	m_portals.Clear();	
	m_unique_portal_name_helper->clear();
	
	if ( cfg_root.value_exists("portals") )
	{
		configs::lua_config_value const& cfg = cfg_root["portals"];
		configs::lua_config::const_iterator portal_it		=	cfg.begin( );
		configs::lua_config::const_iterator portal_end_it	=	cfg.end( );
		for( ; portal_it != portal_end_it; ++portal_it ) 
		{
			edit_portal^ portal = gcnew edit_portal( this, m_unique_portal_name_helper, %m_portals, *portal_it );
			m_portals.Add( portal );
		}
	}
	update_portals_edit_property_view( );
	return true;
}

void edit_object_mesh::on_surfaces_loaded( )
{
	query_preview_model				( );
	m_object_surfaces_panel->set_property_container( get_surfaces_property_container() );
	m_model_editor->refresh_caption	( );
	fill_lod_view					( );
}

void edit_object_mesh::apply_surface_material( System::String^ surface_name, resources::unmanaged_resource_ptr mtl )
{
	edit_surface^ surface = get_surface( surface_name);
	if(!has_preview_model())
		return;

	xray::render::editor::renderer* r = m_model_editor->editor_renderer();

	r->scene().set_model_material	(	m_model->get_render_model_(),
								unmanaged_string(surface->name).c_str(), 
								mtl );
}

void edit_object_mesh::refresh_surfaces_visibility_impl( )
{
	if(!has_preview_model())
		return;

	xray::render::editor::renderer* r = m_model_editor->editor_renderer();

	for each ( edit_surface^ s in m_surfaces.Values )
	{
		bool vis	= s->Visible;
		if(get_view_isolate_selected())
			vis		&= s->Selected;

		r->scene().set_model_visible(m_model->get_render_model_(), 
								unmanaged_string(s->name).c_str(), 
								vis );
	}
}

bool edit_object_mesh::save( )
{
	if(!are_portal_vertices_complanar() )
		System::Windows::Forms::MessageBox::Show( "Not all triangles in portal(s) are coplanar", 
		"Model Assembler", 
		MessageBoxButtons::OK, MessageBoxIcon::Exclamation );

	// save impl..
	configs::lua_config_ptr config_ptr	= configs::create_lua_config( );
	configs::lua_config_value& root		= config_ptr->get_root();

	for each ( edit_surface^ s in m_surfaces.Values )
	{
		unmanaged_string sg_name( s->name );
		configs::lua_config_value current	= root["material_settings"][sg_name.c_str()];
		s->save_material_settings			( current );
	}	

	configs::lua_config_value const& lods_table = root["lod_hierrarchy"];
	for each ( lod_descriptor^ d in m_lods )
	{
		unmanaged_string lod_name( d->name );

		configs::lua_config_value current	= lods_table[lod_name.c_str()];
		d->save_settings					( current );
	}	

	if ( m_portals.Count != 0 )
	{
		configs::lua_config_value const& portals_table = root["portals"];
		for each ( edit_portal^ portal in m_portals )
		{
			unmanaged_string portal_name( portal->Name );
			configs::lua_config_value current	=	portals_table[ portal_name.c_str() ];
			portal->save_portal						( current );	
		}
	}

	m_physics_settings->save_to(config_ptr);
	(*m_settings_cfg)->get_root().assign_lua_value	( config_ptr->get_root() );

	fs_new::virtual_path_string					config_name;

	config_name.assignf				( "resources/models/%s%s/settings", 
										unmanaged_string(m_full_name).c_str(),
										model_file_extension()
										);

	(*m_settings_cfg)->save_as		( config_name.c_str(), configs::target_sources );

	save_collision					( );
	return super::save				( );
}


void edit_object_mesh::revert( )
{
	load_model_settings	( );
	load_collision		( );
	super::revert		( );
}

math::aabb edit_object_mesh::focused_bbox( )
{
	for each ( edit_portal^ portal in m_portals )
	{
		if ( portal->is_selected( ) )
			return portal->get_aabb( );
	}

	if ( m_bsp_tree->is_created() )
		return m_bsp_tree->get_aabb();


	math::aabb bbox		= math::create_invalid_aabb();
	bool btest			= false;
	bool selected_only	= get_view_isolate_selected();

	for each ( edit_surface^ s in m_surfaces.Values )
	{
		if(!s->Visible)
			continue;

		if(!s->Selected && selected_only)
			continue;

		if(btest)
			bbox.modify( *s->m_bbox );
		else
		{
			bbox	= *s->m_bbox;
			btest	= true;
		}
	}

	return (btest) ? bbox : math::create_identity_aabb();
}

math::float3 edit_object_mesh::focused_point( )
{
	return focused_bbox().center();
}

void edit_object_mesh::refresh_properties_in_grid( )
{
	m_need_refresh_properties = true;
}

void edit_object_mesh::action_draw_object_collision( )
{
	super::action_draw_object_collision();
	refresh_surfaces_visibility	( );
}

void edit_object_mesh::add_portal_clicked( button^ )
{
	System::String^ name = m_unique_portal_name_helper->get_unique_name( "new_portal_" );
	m_portals.Add( gcnew edit_portal( this, m_unique_portal_name_helper, %m_portals, name ) );
	update_portals_edit_property_view( );
	set_modified();
}

void edit_object_mesh::delete_selected_portals_clicked( button^ )
{
	if ( 
		m_portals.Exists(gcnew System::Predicate<edit_portal^>( &edit_portal::is_selected ) ) &&
		System::Windows::Forms::MessageBox::Show( "Are you sure you want to delete selected portals?", "Confirm", MessageBoxButtons::OKCancel, MessageBoxIcon::Question ) == DialogResult::OK 
		) 
	{
		for ( int i = 0; i < m_portals.Count; ++i )
		{
			if ( m_portals[ i ]->is_selected() )
			{
				m_unique_portal_name_helper->remove( m_portals[ i ]->Name );
				delete m_portals[ i ];
				m_portals[ i ] = nullptr;
			}
		}
		while ( m_portals.Remove(nullptr) );
		update_portals_edit_property_view( );
		set_modified( );
		get_model_editor()->m_transform_control_helper->m_object = nullptr;
	}
}

void edit_object_mesh::reset_selection( )
{
	for each ( edit_portal^ portal in m_portals )
	{
		portal->set_selected( false );
	}
}

void edit_object_mesh::initialize_object_collision( )
{
	collision::geometry_ptr g		= m_model->get_collision_geometry();
	if( g.c_ptr()==NULL)
		return;

	m_hq_collision_object = NEW ( collision::collision_object ) ( g_allocator, editor_base::collision_object_type_dynamic, &*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), g.c_ptr()) );
	m_model_editor->collision_tree->insert( m_hq_collision_object , m_hq_collision_object->get_matrix( ) );
}

void edit_object_mesh::destroy_object_collision( )
{
	if ( m_hq_collision_object  )
	{
		m_model_editor->collision_tree->erase( m_hq_collision_object  );
		DELETE ( m_hq_collision_object  );
		m_hq_collision_object  = NULL;
	}
}

bool edit_object_mesh::are_portal_vertices_complanar( )
{
	using namespace System::Windows::Forms;
	for each ( edit_portal^ portal in m_portals )
	{
		if ( portal->are_vertices_coplanar() )
			continue;
		String^ message = String::Format( 
			"Vertices of portal \"{0}\" are not coplanar. Would you like it will be fixed automatically? Otherwise you should fix it manually",
			portal->Name );
		DialogResult result = System::Windows::Forms::MessageBox::Show( message, "Some vertices are not coplanar", MessageBoxButtons::YesNo, MessageBoxIcon::Question );
		if ( result == DialogResult::No )
			return false;
		portal->make_vertices_coplanar();
	}
	return true;
}

void edit_object_mesh::set_default_layout( )
{
	m_lod_view_panel->Show			( m_model_editor->get_ide()->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockRight );
	m_object_surfaces_panel->Show	( m_model_editor->get_ide()->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockLeft );
	//m_object_portals_panel->Show	( m_model_editor->get_ide()->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockLeft );
}

IDockContent^ edit_object_mesh::find_dock_content(System::String^ persist_string)
{
	if(persist_string=="xray.model_editor.lod_view_panel")
		return m_lod_view_panel;
	else if(persist_string=="xray.model_editor.property_grid_panel")
		return m_object_surfaces_panel;
	else if(persist_string=="xray.model_editor.collision_property_grid_panel")
		return m_collision_panel;
	else if(persist_string=="xray.model_editor.object_portals_panel")
		return m_object_portals_panel;
	else if(persist_string=="xray.model_editor.portal_generation_view")
		return m_portal_generation_view;
	else if(persist_string=="xray.model_editor.sectors_view")
		return m_sectors_view;
	return (nullptr);
}

bool edit_object_mesh::complete_loaded( )
{
	return has_preview_model();
}

void edit_object_mesh::update_portals_edit_property_view( )
{
	m_object_portals_panel->set_property_container( get_portals_edit_property_container( ) );
}

void edit_object_mesh::update_portals_generation_property_view( )
{
	m_portal_generation_view->set_property_container( get_portals_generation_property_container( ) );
}


void edit_object_mesh::update_surfaces_property_view( )
{
	m_object_surfaces_panel->refresh_properties( );
}

void edit_object_mesh::action_view_isolate_selected( )
{
	super::action_view_isolate_selected	( );
	refresh_surfaces_visibility			( );
}

void edit_object_mesh::register_actions( bool bregister )
{
	super::register_actions						( bregister );

	System::String^ action_name;
	editor_base::input_engine^ input_engine		= m_model_editor->get_input_engine();
	editor_base::gui_binder^ gui_binder			= m_model_editor->get_gui_binder();
	System::Windows::Forms::MenuStrip^ ide_menu_strip = m_model_editor->get_ide()->top_menu;
	editor_base::action_delegate^ a				= nullptr;

	action_name										= "Collision View";
	if(bregister)
	{
		a												= gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &edit_object_mesh::action_collision_panel_view) );
		a->set_checked( gcnew checked_delegate_managed	( this, &edit_object_mesh::collision_panel_visible));
		input_engine->register_action					( a, "" );
		gui_binder->add_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem", 0);
	}else
	{
		input_engine->unregister_action					( action_name );
		gui_binder->remove_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem" );
	}

	action_name										= "LOD View";
	if(bregister)
	{
		a												= gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &edit_object_mesh::action_lod_panel_view) );
		a->set_checked( gcnew checked_delegate_managed	( this, &edit_object_mesh::lod_panel_visible) );
		input_engine->register_action					( a, "" );
		gui_binder->add_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem", 0);
	}else
	{
		input_engine->unregister_action					( action_name );
		gui_binder->remove_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem" );
	}

	action_name										= "Portals View";
	if(bregister)
	{
		a												= gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &edit_object_mesh::action_portals_panel_view) );
		a->set_checked( gcnew checked_delegate_managed	( this, &edit_object_mesh::portals_panel_visible) );
		input_engine->register_action					( a, "" );
		gui_binder->add_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem", 0);
	}else
	{
		input_engine->unregister_action					( action_name );
		gui_binder->remove_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem" );
	}

	action_name										= "BSP-tree view";
	if(bregister)
	{
		a												= gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &edit_object_mesh::action_portal_generation_view) );
		a->set_checked( gcnew checked_delegate_managed	( this, &edit_object_mesh::portal_generation_view_visible) );
		input_engine->register_action					( a, "" );
		gui_binder->add_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem", 0);
	}else
	{
		input_engine->unregister_action					( action_name );
		gui_binder->remove_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem" );
	}

	action_name										= "Sectors view";
	if(bregister)
	{
		a												= gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &edit_object_mesh::action_sectors_view) );
		a->set_checked( gcnew checked_delegate_managed	( this, &edit_object_mesh::sectors_view_visible) );
		input_engine->register_action					( a, "" );
		gui_binder->add_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem", 0);
	}else
	{
		input_engine->unregister_action					( action_name );
		gui_binder->remove_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem" );
	}

	action_name										= "Surfaces View";
	if(bregister)
	{
		a												= gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &edit_object_mesh::action_surfaces_panel_view) );
		a->set_checked( gcnew checked_delegate_managed	( this, &edit_object_mesh::surfaces_panel_visible));
		input_engine->register_action					( a, "" );
		gui_binder->add_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem", 0);
	}else
	{
		input_engine->unregister_action					( action_name );
		gui_binder->remove_action_menu_item				( ide_menu_strip, action_name, "ViewMenuItem" );
	}



	action_name = "Add vertex to portal";
	if (bregister)
	{
		a = gcnew action_delegate( action_name, gcnew execute_delegate_managed(this, &edit_object_mesh::action_add_vertex_to_portal) );
		a->set_enabled( gcnew editor_base::enabled_delegate_managed(this, &edit_object_mesh::is_portals_edit_mode));
		input_engine->register_action( a, "Ctrl+LButton(View)" );
	}
	else
	{
		input_engine->unregister_action	( action_name );
	}

	action_name		= "Delete selected vertex in portal";
	if (bregister)
	{
		a = gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &edit_object_mesh::action_delete_vertex_in_portal));
		a->set_enabled( gcnew editor_base::enabled_delegate_managed(this, &edit_object_mesh::is_portals_edit_mode));
		input_engine->register_action( a, "Delete(View)" );
	}
	else
	{
		input_engine->unregister_action	( action_name );
	}
}

void edit_object_mesh::action_surfaces_panel_view( )
{
	set_dock_panel_visible( m_object_surfaces_panel );
}

void edit_object_mesh::action_collision_panel_view( )
{
	set_dock_panel_visible(  m_collision_panel );
}

void edit_object_mesh::action_portals_panel_view( )
{
	set_dock_panel_visible( m_object_portals_panel );
}

void edit_object_mesh::action_lod_panel_view( )
{
	set_dock_panel_visible(  m_lod_view_panel );
}

bool edit_object_mesh::surfaces_panel_visible( )
{
	return !m_object_surfaces_panel->IsHidden;
}

bool edit_object_mesh::portals_panel_visible( )
{
	return !m_object_portals_panel->IsHidden;
}

bool edit_object_mesh::lod_panel_visible( )
{
	return !m_lod_view_panel->IsHidden;
}

void edit_object_mesh::set_dock_panel_visible( WeifenLuo::WinFormsUI::Docking::DockContent^ panel )
{
	if( !panel->IsHidden )
		panel->Hide();
	else
	{
		if( panel->DockPanel ) 
			panel->Show	( m_model_editor->get_ide()->main_dock_panel );
		else
			panel->Show	( m_model_editor->get_ide()->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockLeft );
	}
}

bool edit_object_mesh::is_portals_edit_mode( )
{
	return ( m_current_mode==edit_mode::edit_portals);
}

bool edit_object_mesh::is_collision_edit_mode( )
{
	return ( m_current_mode==edit_mode::edit_collision);
}

void edit_object_mesh::make_portals_coplanar_clicked( button^ )
{
	for each ( edit_portal^ portal in m_portals )
		portal->make_vertices_coplanar();
}

void edit_object_mesh::action_add_vertex_to_portal( )
{
	for each ( edit_portal^ portal in m_portals )
		if ( portal->is_selected() )
		{
			portal->add_vertex();
			return;
		}
}

void edit_object_mesh::action_delete_vertex_in_portal( )
{
	for each ( edit_portal^ portal in m_portals )
		if ( portal->is_selected() && portal->delete_selected_vertices() )
			return;
	delete_selected_portals_clicked( nullptr );
}


bool edit_object_mesh::collision_panel_visible( )
{
	return !m_collision_panel->IsHidden;
}

void edit_object_mesh::action_portal_generation_view( )
{
	this->set_dock_panel_visible( m_portal_generation_view );
}

bool edit_object_mesh::portal_generation_view_visible( )
{	
	return !m_portal_generation_view->IsHidden;
}

bool edit_object_mesh::sectors_view_visible( )
{	
	return !m_sectors_view->IsHidden;
}

void edit_object_mesh::action_sectors_view()
{
	this->set_dock_panel_visible( m_sectors_view );
}

void edit_object_mesh::switch_to_collision_clicked( button^ )
{
	set_edit_mode( edit_mode::edit_collision );
}

void edit_object_mesh::generate_bsp_tree_clicked( button^ /*button*/  )
{
//?	button->IsEnabled = false;
	collision::geometry_ptr g = m_model->get_collision_geometry();
	R_ASSERT( g.c_ptr() != NULL );
	m_bsp_tree->generate_bsp_tree( g->vertices(), g->vertex_count(), g->indices(), g->index_count() );
	m_bsp_tree->fill_tree_view( m_portal_generation_view->tree_view );
}
void edit_object_mesh::switch_to_portals_clicked( button^ )
{
	set_edit_mode( m_current_mode != edit_mode::edit_portals ? edit_mode::edit_portals : edit_mode::edit_none );
}

void edit_object_mesh::generate_portals_clicked( button^ /*button*/ )
{
//?	button->IsEnabled = false;
	m_bsp_tree->generate_portals();
}

void edit_object_mesh::generate_sectors_clicked	( button^ /*button*/ )
{
	if ( !m_sectors_generator )
	{
//?		button->IsEnabled = false;
		collision::geometry_ptr g = m_model->get_collision_geometry();
		R_ASSERT( g.c_ptr() != NULL );
		m_sectors_generator = NEW( sectors_generator )( g->vertices(), g->vertex_count(), g->indices(), g->index_count() );
		generated_portals_type const& tree_portals = m_bsp_tree->get_portals();
		generated_portals_type::const_iterator const tree_portals_end = tree_portals.end();
		for ( generated_portals_type::const_iterator it = tree_portals.begin(); it != tree_portals_end; ++it )
		{
			m_sectors_generator->add_portal( it->to_sector_generator_portal() );
		}

		generated_portals_type edited_portals;
		for each ( edit_portal^ portal in m_portals )
		{
			m_sectors_generator->add_portal( portal->to_sector_generator_portal() );
		}
		m_sectors_generator->generate_sectors();
		xray::editor::wpf_controls::property_container^ sectors_properties = gcnew xray::editor::wpf_controls::property_container();
		m_sectors_generator->fill_property_container( sectors_properties );
		m_sectors_view->set_property_container( sectors_properties );
	}
}



void edit_object_mesh::set_edit_mode( edit_mode mode )
{
	m_current_mode = mode;

	{// collision
		bool activate = (mode == edit_mode::edit_collision);
		for each ( collision_primitive_item^ prim in m_collision_primitives )
			prim->activate( activate );
		
		// mass center
		m_mass_center_item->activate( activate );
	}
}

} // namespace model_editor
} // namespace xray

