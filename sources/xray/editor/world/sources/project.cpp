////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/exit_codes.h>
#include "project.h"
#include "project_tab.h"
#include "object_inspector_tab.h"
#include "scene_graph_tab.h"
#include "level_editor.h"
#include "editor_world.h"
#include "object_base.h"
#include "object_environment.h"
#include "object_sky.h"
#include "tool_base.h"
#include "editor_cell_manager.h"
#include "property_connection_editor.h"
#include "object_in_project_chooser.h"
#include "object_in_compound_chooser.h"
#include <xray/math_randoms_generator.h>
#include "window_ide.h"

#pragma managed( push, off )
#	include <xray/ai_navigation/sources/graph_generator.h>
#	include <xray/ai_navigation/world.h>

	namespace xray {
	namespace build {
		XRAY_CORE_API	u32		calculate_build_id			( pcstr current_date );
		XRAY_CORE_API	pcstr	build_date					( );
		XRAY_CORE_API	u32		build_station_build_id		( );
	} // namespace build
	} // namespace xray
#pragma managed( pop )

namespace xray {
namespace editor {

#define UNTITLED_PROJECT_NAME	"untitled project"

project::project( level_editor^ le )
:m_level_editor			( le ),
m_focused_folder		( nullptr ),
m_root_folder			( nullptr ),
m_root_scene			( nullptr ),
m_project_name			( UNTITLED_PROJECT_NAME ),
m_selection_pin			( false ),
m_intermediate_save		( false ),
track_active_item		( true ),
m_changed				( false )
{	
	m_config			= NEW(configs::lua_config_ptr)();
	m_project_mount		= NEW(resources::mount_ptr)();
	m_project_tab		= m_level_editor->get_project_explorer();
	m_selection_list	= gcnew project_items_list;
	m_link_storage		= gcnew link_storage;
	m_missing_resource_registry = gcnew missing_resource_registry;
	m_objects_query		= gcnew query_objects_list();

	m_editor_cell_manager = gcnew editor_cell_manager( this );
	subscribe_on_selection_changed( gcnew selection_changed(this, &project::on_selection_changed) );
	
	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &project::on_config_created), g_allocator);

	pcstr str_buf		= " ";
	resources::query_create_resource(
						"#editor_project",
						xray::memory::buffer(str_buf, strings::length(str_buf)),
						xray::resources::lua_config_class,
						boost::bind(&query_result_delegate::callback, q, _1),
						g_allocator
						);

	editor_base::resource_chooser::register_chooser( gcnew object_in_project_chooser(this));
	editor_base::resource_chooser::register_chooser( gcnew object_in_compound_chooser(this));
}

void project::on_config_created( resources::queries_result& data )
{
	if ( data.is_successful() )
		(*m_config)	= static_cast_checked<configs::lua_config*>( data[0].get_unmanaged_resource().c_ptr() );
}

void project::on_project_config_loaded( resources::queries_result& data )
{
	if ( data.is_successful() )
	{
		(*m_config)->get_root().clear();

		LOG_INFO	( "loaded config %s", data[0].get_requested_path() );
		configs::lua_config_ptr arrived	= static_cast_checked<configs::lua_config*>( data[0].get_unmanaged_resource().c_ptr() );

		if( !arrived->empty() )
		{
			(*m_config)->get_root().assign_lua_value( arrived->get_root());
			load_project_config		( (*m_config)->get_root() );
		}
	}else
	{
		LOG_ERROR	( "couldn't load project %s", data[0].get_requested_path() );		
		if ( !m_level_editor->get_editor_world().engine().is_closing_timeout_set() )
		{
			::DialogResult result = m_level_editor->ShowMessageBox( "The project '" +
																	project_name() +
																	"' couldn't be found or loaded.\nDo you want to select other project?",
																	MessageBoxButtons::YesNo, MessageBoxIcon::Warning );
			if ( result == ::DialogResult::Yes )
			{
				m_level_editor->open_project_action( );
			}else
			{
				clear							   ( );
				create_new						   ( );
			}
		}else
		{
			clear								   ( );
			m_level_editor->get_editor_world().exit( exit_code_editor_level_test_project_not_loaded );
		}
	}

	process_project_items_query( );
}

void project::process_project_items_query()
{
	for each( query_object^ query_obj in m_objects_query )
	{
		query_obj->d( project_item_base::object_by_full_name( query_obj->full_name ) );
	}
	m_objects_query->Clear( );
}

System::String^ project::project_name( ) 
{
	return m_project_name;
}

fs_new::native_path_string project::load_project_resources_path( )
{
	fs_new::native_path_string	p;
	p.assignf_with_conversion	(
		"%s/sources/projects/%s",
		m_level_editor->get_editor_world().engine().get_resources_path(),
		unmanaged_string(project_name()).c_str()
	);
	return			p;
}

fs_new::native_path_string project::save_project_resources_path( )
{
	if(m_intermediate_save)
	{
		fs_new::native_path_string p = fs_new::native_path_string::convert( unmanaged_string(project_resources_intermediate_path()).c_str() ) ;
		return p;
	}else
		return load_project_resources_path();
}

System::String^ project::project_resources_intermediate_path( )
{
	return m_project_intermediate_path;
}

fs_new::virtual_path_string project::cfg_resource_name( )
{
	return (*m_config)->reusable_request_name ();
}

project::~project( )
{
	DELETE				( m_config );
	DELETE				( m_project_mount );
	delete				( m_editor_cell_manager );
}

xray::editor::project_ui project::ui( )
{
	return m_project_tab;
}

bool project::empty( )
{
	return (m_root_folder==nullptr);
}

void project::on_selection_changed( )
{
}

bool project::name_assigned( )
{
	return (project_name()->Length>0) && (project_name() != UNTITLED_PROJECT_NAME);
}

void project::create_new( )
{
	m_project_name				= UNTITLED_PROJECT_NAME;
	ASSERT						( empty() );
	m_root_folder				= gcnew project_item_folder( this );

	project_item_folder^ object_set_folder = gcnew project_item_folder( this );
	object_set_folder->assign_id	( 0 );
	object_set_folder->m_guid	= System::Guid::NewGuid();
	object_set_folder->set_name	( "objects" );
	m_root_folder->add			( object_set_folder );
	set_focused_folder			( object_set_folder );

	const xray::render::scene_ptr		scene = m_level_editor->get_editor_world().scene();
	const xray::render::scene_view_ptr	scene_view = m_level_editor->get_editor_world().scene_view();

	get_tool("terrain")->on_new_project	( scene, scene_view );
	get_tool("scripting")->on_new_project( scene, scene_view );
	get_tool("light")->on_new_project	( scene, scene_view );
	get_tool("misc")->on_new_project	( scene, scene_view );
		
	fill_tree_view				( m_project_tab->treeView->nodes );

	project_background_item^ bkg= gcnew project_background_item;
	bkg->m_texture_name			= "tests\\bk_01.bmp";
	bkg->m_position_lt			= System::Drawing::Point(-5, 5);
	bkg->m_scale				= 1; // 1m per pixel
	m_map_backgrounds.Add		( bkg );
	setup_intermediate_path		( nullptr );

	m_level_editor->view_window()->setup_view_matrix( float3( 0.f, 10.f, 0.f ), float3(0.0f,  10.0f, 1.0f) );
	m_changed					= true;
	set_unchanged				( );
}

#define uid_size (8)

char session_uid[uid_size+1];
xray::math::random32	uid_random	(GetTickCount());

void project::clear( )
{
	m_project_name						= "";

	if ( !empty() )
	{
		select_object			(nullptr, enum_selection_method_set);

		m_root_folder->clear	( true );
		delete					m_root_folder;
		m_root_folder			= nullptr;
		m_root_scene			= nullptr;
	}

	m_ignore_list.Clear					( );
	m_selection_list->Clear				( );
	m_editor_cell_manager->clear		( );
	m_project_tab->treeView->clear		( );
	fill_tree_view						( m_project_tab->treeView->nodes );
	m_map_backgrounds.Clear				( );
	m_missing_resource_registry->clear	( );
	m_level_editor->get_editor_world().get_ai_navigation_world().get_graph_generator()->clear_geometry	( );

	for(u16 i=0; i<uid_size; ++i)
		session_uid[i] = 'a' + (char)uid_random.random(u32('z'-'a'));
	session_uid[uid_size] = 0;

	m_level_editor->on_project_clear	( );
}

//void project::save_links(System::String^ project_name)
//{
//	System::String^ ext					= System::IO::Path::GetExtension (project_name);
//	System::String^ links_file_name		= project_name->Remove( project_name->Length - ext->Length ) + ".xlnk";
//	unmanaged_string s			(links_file_name);
//	configs::lua_config_ptr cfg = configs::create_lua_config(s.c_str());
//	configs::lua_config_value cfg_links = cfg->get_root()["links"];
//
//	System::Collections::ArrayList^ links = m_link_storage->get_links();
//	for( int i = 0; i < links->Count; ++i )
//	{
//		System::String^ path	= ((property_link^)links[i])->m_src_object->m_owner_project_item->get_path();
//		path					+= ((property_link^)links[i])->m_src_object->get_name();
//		unmanaged_string path_str_src(path);
//		cfg_links[i]["src_name"] = path_str_src.c_str();
//
//		path					= ((property_link^)links[i])->m_dst_object->m_owner_project_item->get_path();
//		path					+= ((property_link^)links[i])->m_dst_object->get_name();
//		unmanaged_string path_str_dst(path);
//		cfg_links[i]["dst_name"] = path_str_dst.c_str();	
//
//		unmanaged_string src_property_name(((property_link^)links[i])->m_src_property_name);
//		cfg_links[i]["src_member"] = src_property_name.c_str();
//
//		unmanaged_string dst_property_name(((property_link^)links[i])->m_dst_property_name);
//		cfg_links[i]["member"] = dst_property_name.c_str();
//	}
//	cfg->save	();
//}

//void project::load_links(System::String^ project_name)
//{
//	m_link_storage->clear_all(	);
//	System::String^ ext = System::IO::Path::GetExtension (project_name);
//	System::String^ links_file_name = project_name->Remove( project_name->Length - ext->Length ) + ".xlnk";
//	unmanaged_string s(links_file_name);
//
//	fs_new::virtual_path_string	request_path = "@";
//	request_path += s.c_str();
//
//	xray::resources::request  requests[] = { request_path.c_str(), xray::resources::lua_config_class };
//
//	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &project::on_project_links_loaded));
//
//	xray::resources::query_resources(requests, 
//		boost::bind(&query_result_delegate::callback, q, _1),
//		g_allocator);
//}

//void project::on_project_links_loaded	(xray::resources::queries_result& data)
//{
//	R_ASSERT						(!data.is_failed());
//	if( data.size() > 0 )
//	{
//		configs::lua_config_ptr cfg = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
//		
//		configs::lua_config_value const& cfg_links = cfg->get_root()["links"];
//
//		//System::Collections::ArrayList^ links = m_link_storage->get_links();
//		configs::lua_config_value::const_iterator	i = cfg_links.begin( );
//		configs::lua_config_value::const_iterator	e = cfg_links.end( );
//
//		for ( ; i != e; ++i )
//		{
//			configs::lua_config_value const& t	= *i;
//			System::String^ src_full_name			= gcnew System::String(t["src_name"]);
//			object_base^ src_obj = get_item_by_path		(src_full_name);
//			System::String^ dst_full_name			= gcnew System::String(t["dst_name"]);
//			object_base^ dst_obj = get_item_by_path		(dst_full_name);
//			ASSERT( src_obj != nullptr && dst_obj != nullptr );
//
//			m_link_storage->create_link( src_obj, gcnew System::String(t["src_member"]), dst_obj, gcnew System::String(t["member"]) );
//		}
//	}
//}

tool_base^ project::get_tool(System::String^ tool_name)
{
	return m_level_editor->get_tool(tool_name);
}

project_item_base^ project::find_by_path( System::String^ name, bool assert_on_fail )
{
	project_item_base^	result		= m_root_folder;
	if(name->Length!=0)
	{
		cli::array<System::String^>^ tokens = name->Split('/');

		for(int i=0; i<tokens->GetLength(0); ++i)
		{
			System::String^ tok			= tokens[i];
			project_item_base^ tmp		= result->get( tok );
			R_ASSERT					( !assert_on_fail || tmp != nullptr );
			result						= tmp;
			if ( result == nullptr )
				break;
		}
	}
	return result;
}

void project::fill_tree_view(tree_node_collection^ nodes)
{
	nodes->Clear									( );
	if(!empty())
	{
		m_root_folder->fill_tree_view				( nodes );

		m_root_folder->m_tree_node_->ImageIndex			= xray::editor_base::default_object_set;
		m_root_folder->m_tree_node_->SelectedImageIndex	= xray::editor_base::default_object_set;
		m_root_folder->m_tree_node_->Expand		( );
		
		set_focused_folder							( get_focused_folder() );// colorize, tmp
	}else
	{
		controls::tree_node^ n = gcnew controls::tree_node("Create new or load existing");
		nodes->Add				( n );
	}
}

void project::render( )
{
	m_editor_cell_manager->render( );
}

void project::set_focused_folder( project_item_folder^ g ) 
{ 
	m_focused_folder = g;
}

project_item_folder^ project::add_new_folder( System::String^ folder_name, project_item_folder^ parent, u32 id )
{
	project_item_folder^ pg		= gcnew project_item_folder( this );
	pg->assign_id				( id );
	pg->m_guid					= System::Guid::NewGuid();
	parent->add					( pg );
	pg->fill_tree_view			( parent->m_tree_node_->nodes );
	parent->m_tree_node_->Expand( );
	
	if(folder_name==nullptr)
		pg->m_tree_node_->BeginEdit	( );

	return						pg;
}

u32 project::get_all_items_count( )
{
	if(m_root_folder!=nullptr)
		return get_all_items()->Count;
	else
		return 0;
}

project_items_list^ project::get_all_items( )
{
	project_items_list^ items		= gcnew project_items_list;
	if( !empty() )
		m_root_folder->get_all_items( items, true );

	return							items;
}

project_items_list^ project::selection_list( )
{
	return m_selection_list;
}

project_item_object^ project::add_new_item(
		tool_base^ tool, 
		System::String^ lib_name, 
		float4x4 const& transform, 
		System::String^ parent_group_name,
		u32 set_object_id
	)
{
	object_base^ o					= tool->create_library_object_instance( lib_name, m_level_editor->get_editor_world().scene(), m_level_editor->get_editor_world().scene_view() );
	o->set_transform				( transform );
	project_item_base^ parent		= find_by_path( parent_group_name, true );

	project_item_object^ new_item	= gcnew project_item_object( this, o );
	System::String^ name			= o->get_name();

	if(	parent->check_correct_existance(name, 1) )
		o->set_name		( name, false );

	new_item->assign_id			( set_object_id );
	new_item->m_guid			= System::Guid::NewGuid();
	
	parent->add					( new_item );
	o->set_visible				( true );

	new_item->fill_tree_view	( parent->m_tree_node_->nodes );
	
	if(!parent->m_tree_node_->IsExpanded)
		parent->m_tree_node_->Expand();

	process_project_items_query( );

	return							new_item;
}

void project::remove( project_item_base^ itm, bool b_destroy_object )
{
	project_items_list	all_items;
	itm->get_all_items	( %all_items, true );

	for each(project_item_base^ itc in all_items)
	{
		itc->m_parent->remove	( itc );
		itc->clear				( b_destroy_object );
		delete					itc;
	}
}

void project::remove( System::String^ item_name, bool b_destroy )
{
	project_item_base^ itm = find_by_path( item_name, true );
	R_ASSERT			( itm );
	remove				( itm, b_destroy );
}

void project::show_connection_editor()
{
	object_base^ src		= nullptr;
	object_base^ dst		= nullptr;
	object_base_list^ lst		= m_level_editor->selected_objects();
	
	if(lst->Count>0)
		src					= lst[0];
	if(lst->Count>1)
		dst					= lst[1];

	show_connection_editor	(src, dst);
}

void project::show_connection_editor(object_base^ src, object_base^ dst)
{
	property_connection_editor^ tab	= m_level_editor->get_connection_editor();

	tab->set_src_object	(src);
	tab->set_dst_object	(dst);
	tab->Show			();
}

void project::show_object_inspector()
{
	show_object_inspector			(m_selection_list);
}

void project::show_object_inspector(project_items_list^ items)
{
	object_inspector_tab^ inspector_tab	= m_level_editor->get_object_inspector_tab();
	
	if(inspector_tab)
	{
		inspector_tab->show_properties	(items);
		
		if(!inspector_tab->Visible)
			m_level_editor->ide()->show_tab		(inspector_tab);
	}
}

link_storage^ project::get_link_storage( ) 
{
	return m_link_storage;
}

void project::subscribe_on_selection_changed( selection_changed^ d )
{
	m_selection_changed	+= d;
}

void project::query_project_item( System::String^ queried_item_full_name, queried_object_loaded^ d )
{
	project_item_base^ queried_item = find_by_path( queried_item_full_name, false ); 
	if ( queried_item != nullptr ){
		d( queried_item );
		return;
	}
	query_object^ query_obj = gcnew query_object();
	query_obj->full_name = queried_item_full_name;
	query_obj->d = d;
	m_objects_query->Add(query_obj);
}

void project::on_item_added( project_item_base^ itm )
{
	if(itm->get_object())
		m_editor_cell_manager->register_object(itm->get_object());
}

void project::on_item_removed( project_item_base^ itm )
{
	if(itm->get_object())
		m_editor_cell_manager->unregister_object(itm->get_object());
}

void project::on_item_transform_changed( project_item_base^ itm, math::float3 const& prev_position )
{
	if(itm->get_object())
		m_editor_cell_manager->move_object(itm->get_object(), prev_position);
}


void project::move( System::String^ src_name, System::String^ dst_name )
{
	project_item_base^ src 			= find_by_path( src_name, true );
	project_item_base^ dst 			= find_by_path( dst_name, true );

	src->m_parent->remove			( src );
	src->m_parent->m_tree_node_->nodes->Remove(src->m_tree_node_);
	
	dst->add						( src );
	dst->m_tree_node_->nodes->Add	( src->m_tree_node_ );
	dst->m_tree_node_->Expand		( );
}

void extract_topmost_items_int( project_items_list^ src_list, project_items_list% dst_list )
{
	for each (project_item_base^ to_delete in src_list)
	{
		bool b_add					= true;
		
		for each (project_item_base^ itm in src_list)
		{
			if(	itm!=to_delete && 
				//itm->item_group() && 
				to_delete->is_child_of( itm ) )
			{
				b_add = false;
				break;
			}
		}
		if(b_add)
			dst_list.Add	( to_delete );
	}
}

void project::extract_topmost_items(string_list^ src_names, 
									string_list^% dst_names )
{
	project_items_list src_list;
	project_items_list dst_list;

	for each (System::String^ src_name in src_names)
		src_list.Add( find_by_path( src_name, true ) );

	extract_topmost_items_int( %src_list, dst_list );

	for each (project_item_base^ itm in dst_list)
		dst_names->Add( itm->get_full_name() );
}

pcstr project::session_id( )
{
	return session_uid;
}

bool project::save_needed( )
{
	return m_changed;
}

void project::set_modified_caption( bool const is_modified )
{
	m_level_editor->ide()->Text	= m_project_name + (is_modified ? " (Modified)" : "") + " - X-Ray Editor (Build #" + xray::build::build_station_build_id() + ", " + gcnew System::String( xray::build::build_date() ) + ")";
}

void project::set_changed( )
{
	if ( !m_changed )
		m_level_editor->ide()->Text		= m_project_name + " (Modified) - X-Ray Editor";

	m_changed = true;
}

} // namespace editor
} // namespace xray
