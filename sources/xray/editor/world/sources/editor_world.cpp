////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project_tab.h"
#include "object_inspector_tab.h"
#include "tools_tab.h"
#include "editor_world.h"
#include "project.h"
#include "editor_cell_manager.h"
#include "texture_editor.h"
#include "material_editor.h"
#include "render_options_editor.h"

#include "options_page_general.h"
#include "options_page_project.h"
#include "options_page_view_panel.h"
#include "console_wrapper.h"
#include "tool_window_storage.h"
#include <xray/editor/base/collision_object_types.h>
#include "window_ide.h"
#include "actions.h"
#include "level_editor.h"
#include "editor_contexts_manager.h"

#pragma managed( push, off )
#	include <xray/exit_codes.h>
#	include <xray/engine/console.h>
#	include <xray/editor/world/engine.h>
#	include <xray/collision/object.h>
#	include <xray/collision/collision_object.h>
#	include <xray/collision/space_partitioning_tree.h>
#	include <xray/sound/world.h>
#	include <xray/sound/world_user.h>
#	include <xray/sound/sound_debug_stats.h>
#	include <xray/ui/api.h>
#	include <xray/ui/world.h>
#	include <xray/editor/animation/api.h>
#	include <xray/editor/sound/api.h>
#	include <xray/render/facade/debug_renderer.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/render/facade/editor_renderer.h>
#	include <xray/render/engine/base_classes.h>
#	include <xray/console_command_processor.h>
#	include <xray/console_command.h>

#	include <xray/physics/api.h>
#	include <xray/physics/world.h>

#	include <xray/ai_navigation/world.h>
#	include <xray/ai_navigation/sources/graph_generator.h>
#pragma managed( pop )

namespace xray {
namespace editor {

void register_texture_cook();
void unregister_texture_cook();

editor_world::editor_world		( xray::editor::engine& engine ) :
	m_engine					( engine ),
	m_editor_mode				( true ),
	m_paused					( false ),
	m_level_editor				( nullptr ),
	m_b_draw_collision			( false ),
	m_object_focus_distance		( 20.0f ),
	m_cells_flood_depth			( 3 ),
	m_frame_id					( 0 ),
	m_exit_query				( false ),
	m_sound_stats_mode			( 0 ),
	m_culling_view_matrix_inverted	( float4x4().identity() )
{
	xray::base_registry_key::company_name = XRAY_ENGINE_COMPANY_ID;
	xray::base_registry_key::product_name = XRAY_ENGINE_ID;

	editor_base::editor_creator::set_memory_allocator(g_allocator);
	register_texture_cook		( );
	m_command_engine			= gcnew xray::editor_base::command_engine( 100 );
	m_window_ide				= gcnew window_ide( *this );
	m_view_window				= m_window_ide->view_window();
	m_camera_view_window		= m_window_ide->camera_view_panel();
	m_scene						= NEW(render::scene_ptr);
	m_scene_view				= NEW(render::scene_view_ptr);
	m_view_output_window		= NEW(render::render_output_window_ptr);
	m_camera_view_output_window = NEW(render::render_output_window_ptr);
	m_sound_scene				= NEW(sound::sound_scene_ptr);
	m_console_wrapper			= gcnew console_wrapper;
	m_tool_windows				= gcnew tool_window_storage;

	m_view_window->setup_view_matrix( float3( 0.f, 10.f, 0.f ), float3( 0.0f,  10.0f, 1.0f) );
	m_camera_view_window->setup_view_matrix( float3( 0.f, 10.f, 0.f ), float3( 0.0f,  10.0f, 1.0f) );

	m_input_engine				= gcnew editor_base::input_engine( xray::editor_base::input_keys_holder::ref , gcnew editor_contexts_manager ( *this ) );

	// initializes gui binding to actions
	m_gui_binder				= gcnew editor_base::gui_binder( input_engine() ); 
	m_gui_binder->set_image_list( "base", m_window_ide->images16x16() );
	register_actions			( );
	
	ide()->get_options_manager()->register_page( "General", gcnew options_page_general( *this ) );
	ide()->get_options_manager()->register_page( "General\\Input", gcnew xray::editor_base::options_page_input( m_input_engine ) );
	ide()->get_options_manager()->register_page( "Controls\\Wpf Editor", gcnew xray::editor_base::wpf_controls_page( ) );
	ide()->get_options_manager()->register_page( "General\\Project", gcnew options_page_project( *this ) );
	ide()->get_options_manager()->register_page( "General\\View", gcnew options_page_view_panel( m_view_window ) );

	editor_base::resource_chooser::register_chooser( gcnew material_chooser(*this, material_editor_mode::edit_material));
	editor_base::resource_chooser::register_chooser( gcnew material_chooser(*this, material_editor_mode::edit_instances));

	physics::set_memory_allocator	( memory::g_crt_allocator );
	m_physics_world					= physics::create_world_bt( memory::g_crt_allocator, *this );
	m_physics_world->initialize		( );

//	m_physics_world->create_test_scene( );

//	xray::ai::navigation::set_memory_allocator( *g_allocator );

}

editor_world::~editor_world(  )
{
	delete					m_level_editor;
	m_tool_windows->destroy	( );

	collision::delete_space_partitioning_tree( m_view_window->m_collision_tree );
	delete					m_input_engine;
	delete					m_command_engine;
	DELETE					( m_sound_stats );
	DELETE					( m_camera_view_output_window );
	DELETE					( m_view_output_window );
	DELETE					( m_scene_view );
	DELETE					( m_scene );
	DELETE					( m_sound_scene );
	delete					m_tool_windows;
}

bool editor_world::close_query( )
{
	if(!m_level_editor->close_query())
		return false;

	if(!m_tool_windows->close_query() )
		return false;

	return true;
}

void editor_world::on_window_closing( )
{
	save_settings					( );
	clear_resources					( );
	tick							( );
}


public ref class render_mode_strip_button: public xray::editor::controls::mode_strip_button
{
public:
	render_mode_strip_button(editor_world& in_editor_world):
		m_editor_world(in_editor_world)
	{
	}
	
	virtual void	on_click					( ) override
	{
		m_editor_world.set_renderer_configuration(
			unmanaged_string(current_mode_name).c_str()
		);
	}
private:
	editor_world&	m_editor_world;
};

void editor_world::load_settings( )
{
	m_level_editor->load_settings	( );

	RegistryKey^ product_key		= base_registry_key::get();
	RegistryKey^ editor_key 		= get_sub_key(product_key, "general");

	m_view_window->load_settings	( editor_key );
	m_cells_flood_depth				= System::Convert::ToUInt32(editor_key->GetValue("cells_flood_depth", 3 ));
	m_object_focus_distance			= convert_to_single(editor_key->GetValue( "focus_distance", "20.0"));
	
	m_selected_renderer_configuration = unmanaged_string(editor_key->GetValue( "selected_renderer_configuration", "")->ToString()).c_str();
	
	xray::editor_base::transform_control_base::gizmo_size = convert_to_single(editor_key->GetValue( "gizmo_size", "0.8") );

	xray::editor::wpf_controls::settings::load( product_key );

	editor_key->Close			();
	product_key->Close			();
	
	m_window_ide->get_menu_strip()->Items->Add	(gcnew System::Windows::Forms::ToolStripLabel("Render config:"));
	m_render_options_button						= gcnew render_mode_strip_button( *this );
	m_render_options_button->ToolTipText		= "Select render configuration";
	m_window_ide->get_menu_strip()->Items->Add	(m_render_options_button);
	
	updated_render_options_list					();
}

void editor_world::save_settings( )
{
	m_level_editor->save_settings( );
	RegistryKey^ product_key	= base_registry_key::get();
	RegistryKey^ editor_key		= get_sub_key(product_key, "general" );

	m_view_window->save_settings( editor_key );

	editor_key->SetValue		( "gizmo_size", convert_to_string(xray::editor_base::transform_control_base::gizmo_size) );
	editor_key->SetValue		( "focus_distance", convert_to_string(m_object_focus_distance) );
	editor_key->SetValue		( "cells_flood_depth", m_cells_flood_depth );
	
	editor_key->SetValue		( "selected_renderer_configuration", gcnew System::String(m_selected_renderer_configuration.c_str()) );
	
	xray::editor::wpf_controls::settings::save( product_key );
	
	editor_key->Close			( );
	product_key->Close			( );
}

void editor_world::set_renderer_configuration( pcstr in_render_config_name )
{
	if (m_selected_renderer_configuration == in_render_config_name)
		return;
	
	get_renderer().set_renderer_configuration	( in_render_config_name );
	
	fs_new::virtual_path_string					command_str;
	command_str.assignf							("r_current_render_configuration %s", in_render_config_name);
	
	m_selected_renderer_configuration			= in_render_config_name;
	
	xray::console_commands::execute				(command_str.c_str(), xray::console_commands::execution_filter_all);
}

void editor_world::on_fs_iterator_render_options_ready( fs_new::virtual_path_string const & /*options_path*/, xray::vfs::vfs_locked_iterator const & fs_it )
{
	vfs::vfs_iterator it					= fs_it.children_begin();
	vfs::vfs_iterator it_e					= fs_it.children_end();

	List<String^>^ modes					= gcnew List<String^>( );
	
	bool has_options						= false;
	int mode_index							= 0;
	int selected_mode_index					= 0;
	
	bool selected_found						= false;
	
	for ( ; it!=it_e; ++it )
	{
		pcstr mname							= it.get_name();
		
		if(it.is_folder())
		{
			// TODO: folders support
			continue;
		}
		
		if (strstr(mname, ".orig") || !strstr(mname, ".cfg"))
			continue;
		
		fs_new::virtual_path_string	file_name			(mname);
		file_name.set_length				(file_name.rfind('.'));
		
		modes->Add(gcnew System::String(file_name.c_str()));
		
		if (m_selected_renderer_configuration == file_name)
		{
			selected_mode_index				= mode_index;
			selected_found					= true;
		}
		
		has_options							= true;
		mode_index++;
	}
	
	if (!selected_found && has_options)
	{
		m_selected_renderer_configuration	= unmanaged_string(modes[0]).c_str();
		selected_mode_index					= 0;
	}
	
	if (has_options)
	{
		m_render_options_button->can_iterate_modes		= false; 
		m_render_options_button->modes					= modes; 
		m_render_options_button->current_mode			= selected_mode_index;
		m_render_options_button->Invalidate				();
	}
}

void editor_world::updated_render_options_list()
{
	pcstr render_options_path					= "resources/render";
	resources::query_vfs_iterator				(
		render_options_path,
		boost::bind(&editor_world::on_fs_iterator_render_options_ready, this, render_options_path, _1),
		g_allocator,
		resources::recursive_true
	);
}

void editor_world::load( )
{
	g_allocator->user_current_thread_id( );
	
	xray::editor::wpf_controls::library_initializer::initialize( );
	
	m_view_window->m_collision_tree			= &(*(collision::new_space_partitioning_tree( g_allocator, 1.f, 1024 )));
} 

void editor_world::run				( ) 
{
	query_scene								( );
	System::Windows::Forms::Application::Run( m_window_ide );
}

void editor_world::clear_resources( )
{
	m_level_editor->clear_resources	( );
	m_tool_windows->clear_resources	( );

	DELETE	(m_console_wrapper->m_console);
	m_console_wrapper->m_console = NULL;
	delete	m_console_wrapper;

	m_sound_stats->clear_resources			( m_engine.get_sound_world().get_editor_world_user() );

	ui::destroy_world		( m_ui_world );
	m_ui_world				= NULL;

	m_view_window->clear_resources		( );
	m_view_window->clear_scene			( );

	m_camera_view_window->clear_resources		( );
	m_camera_view_window->clear_scene			( );

	m_engine.get_sound_world().get_editor_world_user().remove_sound_scene( *m_sound_scene);
	*m_sound_scene								= 0;

}

HWND editor_world::main_handle		( )
{
	int	result = m_window_ide->Handle.ToInt32( );
	return			( *( HWND* )&result  );
}

HWND editor_world::view_handle		( )
{
	System::Int32 result = m_view_window->view_handle();
	return			( *( HWND* )&result  );
}
HWND editor_world::camera_view_handle		( )
{
	System::Int32 result = m_camera_view_window->view_handle();
	return			( *( HWND* )&result  );
}

extern bool g_show_render_statistics;

void editor_world::exit	( int const exit_code )
{
	m_exit_query		= true;
	set_exit_code		( exit_code );
	m_window_ide->close	( );
}

class end_frame_guard : private boost::noncopyable {
public:
	end_frame_guard	(
			editor_world& editor_world,
			u32& frame_id
		) :
		m_editor_world	( editor_world ),
		m_frame_id		( frame_id ),
		skip_frame		( false )
	{
	}

	~end_frame_guard( )
	{
		if ( skip_frame )
			return;

		m_editor_world.get_renderer().end_frame		( );
		++m_frame_id;
		m_editor_world.engine().on_after_editor_tick( );
	}

private:
	editor_world&	m_editor_world;
	u32&			m_frame_id;

public:
	bool			skip_frame;
}; // class end_frame_guard

void editor_world::tick( )
{
	m_physics_world->tick			( );

	end_frame_guard	end_frame_guard_instance( *this, m_frame_id );

	resources::dispatch_callbacks	( );

	u32 const pending_queries_count	= resources::pending_queries_count();

	if ( !m_exit_query ) {
		engine().set_exit_code		(
			!pending_queries_count ?
				exit_code_success :
				exit_code_editor_level_not_all_objects_loaded
		);

		if ( engine().is_closing_timeout_set() && !pending_queries_count ) {
			exit					( exit_code_success );
			return;
		}
	}

	// to prevent ticks when render is still working on previous frames
	if ( !m_engine.on_before_editor_tick() ) {
		end_frame_guard_instance.skip_frame	= true;
		return;
	}

	if ( !m_level_editor )
		return;

	if ( m_window_ide->is_closing() )
		return;

	System::String^ reason = gcnew System::String("");
	m_level_editor->get_project()->set_modified_caption( m_level_editor->get_project() && (m_level_editor->save_needed() || m_level_editor->has_uncommited_changes(reason)) );

	m_view_window->tick				( );

	bool const is_minimized			= (m_window_ide->WindowState == System::Windows::Forms::FormWindowState::Minimized);

	if ( m_editor_mode )
	{
		if( xray::editor_base::editors_helper::is_active_form_belong_to( m_window_ide ) || engine().is_closing_timeout_set() )
		{
			execute_input						( );

			m_level_editor->tick				( );

			
			// LE can raise EXIT event
			if ( m_window_ide->is_closing() )
				return;

			if( !is_minimized && m_scene->c_ptr() && m_sound_scene->c_ptr() )
			{
				if ( m_level_editor->get_culling_view() ) {
					get_renderer().scene().set_view_matrix_only( *m_scene_view, math::invert4x3( m_view_window->get_inverted_view_matrix() ) );
					get_renderer().debug().draw_frustum_solid(
						*m_scene,
						math::deg2rad( m_view_window->vertical_fov() ),
						m_view_window->near_plane(),
						m_view_window->far_plane(),
						m_view_window->aspect_ratio(),
						m_culling_view_matrix_inverted.c.xyz(),
						m_culling_view_matrix_inverted.k.xyz(),
						float3( 0.f, 1.f, 0.f ),
						math::color( 0, 127, 0, 127 ),
						true
					);
					get_renderer().debug().draw_frustum(
						*m_scene,
						math::deg2rad( m_view_window->vertical_fov() ),
						m_view_window->near_plane(),
						m_view_window->far_plane(),
						m_view_window->aspect_ratio(),
						m_culling_view_matrix_inverted.c.xyz(),
						m_culling_view_matrix_inverted.k.xyz(),
						float3( 0.f, 1.f, 0.f ),
						math::color( 0, 127, 127 ),
						true
					);
				}
				else {
					m_culling_view_matrix_inverted	= m_view_window->get_inverted_view_matrix();
					get_renderer().scene().set_view_matrix( *m_scene_view, math::invert4x3( m_view_window->get_inverted_view_matrix() ) );
				}

				engine().get_sound_world().get_editor_world_user().set_listener_properties_interlocked( 
					*m_sound_scene,
					m_view_window->get_inverted_view_matrix().c.xyz(),
					m_view_window->get_inverted_view_matrix().k.xyz(),
					m_view_window->get_inverted_view_matrix().j.xyz());

				get_renderer().scene().set_projection_matrix		( *m_scene_view, m_view_window->get_projection_matrix() );
				math::float4 sel_color;
				m_view_window->m_selection_color->get_RGBA(sel_color.x, sel_color.y, sel_color.z, sel_color.w );
				get_renderer().set_selection_parameters(sel_color, m_view_window->m_selection_rate);

				m_view_window->tick				( );

				float3 p, d;
				m_view_window->get_camera_props	(p, d);
				System::Drawing::Point cur		= view_window()->get_mouse_position();
				System::String^ cam_props		= System::String::Format("Cam: X={0:f1} Y={1:f1} Z={2:f1} Cur[{3} {4}]", p.x, p.y, p.z, cur.X, cur.Y );
				ide()->set_status_label			( 0, cam_props );

				float4x4 const& projection_matrix	= m_view_window->get_projection_matrix();
				float4x4 const& view_matrix			= math::invert4x3( m_view_window->get_inverted_view_matrix() );
				m_ai_navigation_world->get_graph_generator()->set_frustum		( math::frustum( mul4x4(view_matrix, projection_matrix ) ) );
				m_ai_navigation_world->tick				( );

				if( m_console_wrapper->get_active() )
					m_console_wrapper->tick		( );
			}
		}

		m_tool_windows->tick				( );
	}

	System::String^ props	=	System::String::Format( "Q: {0}", pending_queries_count);
	ide()->set_status_label		( 2, props );

	m_gui_binder->update_items			( );

	// Perform editor rendering.
	if ( m_editor_mode && !is_minimized && m_scene->c_ptr() ) {
		render							( 0, math::float4x4() );
		
		if ( g_show_render_statistics && m_ui_world )
			get_renderer().scene().draw_render_statistics( *m_ui_world );

		if ( m_sound_stats && m_sound_stats->is_stats_available( ) )
			m_sound_stats->draw( *m_scene );

		m_physics_world->debug_render	( *m_scene, get_renderer().debug() );
		
		get_renderer().draw_scene		( *m_scene, *m_scene_view, *m_view_output_window, render::viewport_type( float2(0.f, 0.f), float2(1.f, 1.f) ) );
	}

	// Camera View Rendering
	if( m_editor_mode && m_camera_view_window->Visible && m_scene->c_ptr() )
	{
		if( xray::editor_base::editors_helper::is_active_form_belong_to( m_window_ide ) || engine().is_closing_timeout_set() )
			execute_input				( );
		
		m_gui_binder->update_items		( );
		
		get_renderer().scene().set_view_matrix			( *m_scene_view, math::invert4x3(m_camera_view_window->get_inverted_view_matrix()) );
		get_renderer().scene().set_projection_matrix	( *m_scene_view, m_camera_view_window->get_projection_matrix() );
		m_camera_view_window->tick						( );
		get_renderer().draw_scene						( *m_scene, *m_scene_view, *m_camera_view_output_window, render::viewport_type( float2(0.f, 0.f), float2(1.f, 1.f) ) );
	} // End Camera View Rendering
}

struct render_predicate : private boost::noncopyable {
	inline			render_predicate( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) :
		m_scene						( scene ),
		m_renderer					( renderer )
	{
	}

	inline	void	operator ( )	( collision::object const* const object ) const
	{
		ASSERT						( object );
		object->render				( m_scene, m_renderer );
	}

	xray::render::scene_ptr const&	m_scene;
	xray::render::debug::renderer&	m_renderer;
}; // struct render_predicate


void editor_world::render( u32 frame_id, xray::math::float4x4 view_matrix )
{
	XRAY_UNREFERENCED_PARAMETERS( frame_id, view_matrix );

	if( m_editor_mode )
	{
		m_view_window->render();// grid, etc

		if(m_b_draw_collision) {

			render_predicate	predicate( *m_scene, get_renderer().debug() );

			get_collision_tree()->for_each	( collision::space_partitioning_tree::predicate_type(&predicate, &render_predicate::operator()) );
		}
	}
}

void editor_world::on_idle_start	( )	
{
}

void editor_world::on_idle			( )
{
}

void editor_world::on_idle_end		( )
{
}

bool editor_world::execute_input					( )
{

	m_input_engine->execute();

	return true; 
}
	
collision::space_partitioning_tree* editor_world::get_collision_tree( )
{
	return m_view_window->m_collision_tree;
}


int editor_world::rollback( int steps )
{
	int res = m_command_engine->rollback( steps );
	m_level_editor->update_object_inspector( );
	return res;
}

bool editor_world::undo_stack_empty( )
{
	return m_command_engine->undo_stack_empty();
}

bool editor_world::redo_stack_empty( )
{
	return m_command_engine->redo_stack_empty();
}

void editor_world::show_sound_editor( )
{
	xray::editor_base::tool_window^ ed = m_tool_windows->get("sound_editor");
	if(ed==nullptr)
	{
		sound_editor::editor_creator::set_memory_allocator( g_allocator );
		ed = sound_editor::editor_creator::create_editor( gcnew System::String( engine().get_resources_path() ), engine().get_sound_world(), engine().get_renderer_world(), m_tool_windows );
		ed->load_settings( base_registry_key::get() );
	}
	ed->Show( nullptr, nullptr );
}

void editor_world::show_animation_editor( )
{
	xray::editor_base::tool_window^ ed = m_tool_windows->get("animation_editor");
	if(ed==nullptr)
	{
		animation_editor::editor_creator::set_memory_allocator(g_allocator);
		ed = animation_editor::editor_creator::create_editor(gcnew System::String(engine().get_resources_path()), engine().get_renderer_world(), *m_ui_world, m_tool_windows);
		ed->load_settings( base_registry_key::get() );
	}
	ed->Show( nullptr, nullptr );
}

void editor_world::show_dialog_editor( )
{
	xray::editor_base::tool_window^ ed = m_tool_windows->get("dialog_editor");
	if(ed==nullptr)
	{
		dialog_editor::editor_creator::set_memory_allocator(g_allocator);
 		ed = dialog_editor::editor_creator::create_editor(gcnew System::String(engine().get_resources_path()), m_tool_windows);
		ed->load_settings( base_registry_key::get() );
	}
	ed->Show( nullptr, nullptr );

}

void editor_world::show_particle_editor()
{
	xray::editor_base::tool_window^ ed = m_tool_windows->get("particle_editor");

	if(ed==nullptr)
	{
		particle_editor::editor_creator::set_memory_allocator(g_allocator);
		ed = particle_editor::editor_creator::create_editor (gcnew System::String(engine().get_resources_path()), engine().get_renderer_world(), *m_ui_world, m_tool_windows);
		ed->load_settings( base_registry_key::get() );
	}

	ed->Show( nullptr, nullptr );
}

xray::editor_base::tool_window^ editor_world::create_model_editor( )
{
	model_editor::editor_creator::set_memory_allocator(g_allocator);
	xray::editor_base::tool_window^ ed = model_editor::editor_creator::create_editor( engine().get_renderer_world(),
																						m_tool_windows,
																						engine().get_resources_path());

	return ed;
}

void editor_world::show_model_editor( )
{
//	for(int i=0; i<10; ++i)
	{
		xray::editor_base::tool_window^ ed = create_model_editor( );
		ed->load_settings( base_registry_key::get() );
		ed->Show( nullptr, nullptr );
	}
}

void editor_world::show_texture_editor( )
{
	xray::editor_base::tool_window^ tab	= gcnew texture_editor(*this);
	tab->Show							( nullptr, nullptr );
}

void editor_world::show_material_editor( )
{
	xray::editor_base::tool_window^ tab	= gcnew material_editor(*this, material_editor_mode::edit_material, false );
	tab->load_settings( base_registry_key::get() );
	tab->Show							( nullptr, nullptr );
}

void editor_world::show_render_options_editor( )
{
	xray::editor_base::tool_window^ tab	= gcnew render_options_editor(*this);
	tab->load_settings( base_registry_key::get() );
	tab->Show							( nullptr, nullptr );
}

void editor_world::show_material_instance_editor( )
{
	xray::editor_base::tool_window^ tab	= gcnew material_editor(*this, material_editor_mode::edit_instances, false );
	tab->load_settings( base_registry_key::get() );
	tab->Show							( nullptr, nullptr );
}

void editor_world::on_active_control_changed( editor_base::editor_control_base^ c )
{
	if(c)
		m_view_window->set_view_cursor( editor_base::enum_editor_cursors::dragger );
	else
		m_view_window->set_view_cursor( editor_base::enum_editor_cursors::default );
}

void editor_world::register_actions( )
{
	// Maya style mouse actions 
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_move_xy		( "view maya move xy",	m_view_window ), "Alt+MButton(View)" );
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_move_z		( "view maya move z",	m_view_window ), "Alt+RButton(View)" );
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_rotate_around( "view maya rotate", m_view_window ), "Alt+LButton(View)" );

//.	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_roll			( "view roll", m_view_window ), "" );
	
	// Old LE style mouse actions 
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_rotate		( "view LE rotate", m_view_window ), "Space+LButton+RButton(View)" );
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_move_y_reverse( "view LE move y", m_view_window ), "Space+RButton(View)" );
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_move_xz_reverse( "view LE move xz",m_view_window ), "Space+LButton(View)" );

	ide()->create_tool_strip		( "MainToolStrip", 0);

	System::String^ action_name		= "switch editor mode";
	m_input_engine->register_action	( gcnew action_editor_mode( action_name, this ), "");
	m_gui_binder->register_image	( "base", action_name,	editor_base::mode_editor );
	ide()->add_action_button_item	( m_gui_binder, action_name, "MainToolStrip", 1);

	action_name						= "pause";
	m_input_engine->register_action( gcnew action_pause( action_name,	this ), "");
	m_gui_binder->register_image	( "base", action_name, editor_base::mode_pause );
	ide()->add_action_button_item	( m_gui_binder, action_name, "MainToolStrip", 2);

	action_name						= "reload modified shaders";
	m_input_engine->register_action( gcnew action_reload_shaders	( action_name,	this ), "");
	m_gui_binder->register_image	( "base", action_name,		editor_base::reload_shaders );
	ide()->add_action_button_item	( m_gui_binder, action_name, "RenderOptionsStrip", 1);
	
	action_name						= "reload modified textures";
	m_input_engine->register_action( gcnew action_reload_modified_textures	( action_name,	this ), "");
	m_gui_binder->register_image	( "base", action_name,		editor_base::reload_modified_textures );
	ide()->add_action_button_item	( m_gui_binder, action_name, "RenderOptionsStrip", 1);
	
	action_name						= "terrain debug mode";
	m_input_engine->register_action( gcnew action_terrain_debug_mode	( action_name,	this ), "");
	m_gui_binder->register_image	( "base", action_name,	editor_base::terrain_debug_mode );
	ide()->add_action_button_item	( m_gui_binder, action_name, "RenderOptionsStrip", 2);

	action_name						= "show render statistics";
	m_input_engine->register_action( gcnew action_show_render_statistics	( action_name,	this ), "");
	m_gui_binder->register_image	( "base", action_name,	editor_base::show_render_statistics );
	ide()->add_action_button_item	( m_gui_binder, action_name, "RenderOptionsStrip", 2);

	m_view_window->register_actions	( m_input_engine, m_gui_binder, false );

	// Editing actions
	action_name						= "undo";
	m_input_engine->register_action( gcnew action_undo( action_name, this ), "Control+Z" );
	ide()->add_action_menu_item		( m_gui_binder, action_name, "EditMenuItem", 1);

	action_name						= "redo";
	m_input_engine->register_action( gcnew action_redo( action_name, this ), "Control+Y" );
	ide()->add_action_menu_item		( m_gui_binder, action_name, "EditMenuItem", 2);
	

	action_name						= "animation editor";
	m_input_engine->register_action( gcnew action_show_animation_editor( action_name, this ), "");
	ide()->add_action_menu_item		( m_gui_binder, action_name, "ToolsMenuItem", 1);
	
	action_name						= "dialog editor";
	m_input_engine->register_action( gcnew action_show_dialog_editor( action_name, this ), "");
	ide()->add_action_menu_item		( m_gui_binder, action_name, "ToolsMenuItem", 2);

	action_name						= "particle editor";
	m_input_engine->register_action( gcnew action_show_particle_editor( action_name, this ), "Control+P(Global)");
	ide()->add_action_menu_item		( m_gui_binder, action_name, "ToolsMenuItem", 7);

	action_name						= "texture editor";
	m_input_engine->register_action( gcnew action_show_texture_editor( action_name, this ), "");
	ide()->add_action_menu_item		( m_gui_binder, action_name, "ToolsMenuItem", 10);

	action_name						= "material editor";
	m_input_engine->register_action( gcnew action_show_material_editor( action_name, this ), "");
	ide()->add_action_menu_item		( m_gui_binder, action_name, "ToolsMenuItem", 3);

	action_name						= "material instance editor";
	m_input_engine->register_action( gcnew action_show_material_instance_editor	( action_name, this ), "");
	ide()->add_action_menu_item		( m_gui_binder, action_name, "ToolsMenuItem", 3);

	action_name						= "model editor";
	m_input_engine->register_action( gcnew action_show_model_editor	( action_name, this ), "");
	ide()->add_action_menu_item		( m_gui_binder, action_name, "ToolsMenuItem", 4);

		action_name						= "sound editor";
	m_input_engine->register_action( gcnew action_show_sound_editor( action_name, this ), "");
	ide()->add_action_menu_item		( m_gui_binder, action_name, "ToolsMenuItem", 8);
	
	action_name						= "render options editor";
	m_input_engine->register_action( gcnew action_show_render_options_editor	( action_name, this ), "");
	ide()->add_action_menu_item		( m_gui_binder, action_name, "ToolsMenuItem", 4);
}

WeifenLuo::WinFormsUI::Docking::IDockContent^ editor_world::find_dock_content(System::String^ type_name)
{
	return m_level_editor->find_dock_content(type_name);
}

void editor_world::pause(bool bpause)
{
	m_paused = bpause;
}

void editor_world::editor_mode(bool beditor_mode)
{
	m_input_engine->keys_holder->reset();

	if( beditor_mode ) 
	{
		engine().unload_level	( );

		engine().enable_game	( false );
		
		while ( ShowCursor(TRUE) <= 0 );

		m_view_window->unclip_cursor();
	}else
	{
		m_level_editor->get_project()->save_intermediate( ); 

		engine().enable_game			( true );
		fs_new::virtual_path_string					project_resource_name;	
		project_resource_name.assignf	("#%s", m_level_editor->get_project()->cfg_resource_name().c_str());
		
		//fs_new::virtual_path_string	project_resource_path = le->get_project()->project_resources_path();
		fs_new::virtual_path_string	project_resource_path = unmanaged_string(m_level_editor->get_project()->project_name()).c_str();
		engine().load_level				( project_resource_name.c_str(), project_resource_path.c_str() );

		m_view_window->clip_cursor		( );

		while ( ShowCursor(FALSE) >= 0 );
	}

	m_editor_mode						= beditor_mode;
}

void editor_world::load_editors( )
{
	m_level_editor					= gcnew level_editor(*this);
	m_level_editor->initialize		( );
}

void editor_world::set_default_layout( )
{
	if ( !m_level_editor )
		return;

	m_level_editor->set_default_layout( );
}

void editor_world::toggle_console( )
{
	R_ASSERT( m_console_wrapper->m_console );

	if(m_console_wrapper->get_active())
		m_console_wrapper->on_deactivate( xray::editor_base::input_keys_holder::ref );
	else
		m_console_wrapper->on_activate( xray::editor_base::input_keys_holder::ref );
}

int	editor_world::exit_code			( ) const
{
	return m_engine.get_exit_code	( );
}

void editor_world::set_exit_code	( int exit_code )
{
	return m_engine.set_exit_code	( exit_code );
}

void editor_world::on_render_resources_created(resources::queries_result& data)
{
	R_ASSERT						( data.is_successful() );
	
	*m_scene						= static_cast_resource_ptr< render::scene_ptr >( data[0].get_unmanaged_resource() );
	*m_scene_view					= static_cast_resource_ptr< render::scene_view_ptr >( data[1].get_unmanaged_resource() );
	*m_view_output_window			= static_cast_resource_ptr< render::render_output_window_ptr >( data[2].get_unmanaged_resource() );
	*m_camera_view_output_window	= static_cast_resource_ptr< render::render_output_window_ptr >( data[3].get_unmanaged_resource() );

	m_view_window->setup_scene		( m_scene->c_ptr(), m_scene_view->c_ptr(), get_renderer(), true );

	m_camera_view_window->set_render_view_mode(m_view_window->render_view_mode());
	m_camera_view_window->setup_scene( m_scene->c_ptr(), m_scene_view->c_ptr(), get_renderer(), false );
	
	m_ui_world						= ui::create_world( *this, get_renderer().ui(), *g_allocator, *m_scene_view );
	m_console_wrapper->m_console	= m_engine.create_editor_console( *m_ui_world );
	R_ASSERT						( m_console_wrapper->m_console );

	m_ai_navigation_world			= xray::ai::navigation::create_world( *this, *m_scene, get_renderer().debug() );

	load_editors					( );

	m_ai_navigation_world			= xray::ai::navigation::create_world( *this, *m_scene, get_renderer().debug() );

	on_after_render_scene_created	( );

	load_settings						( );
	m_window_ide->load_contents_layout	( );
}

void editor_world::on_sound_scene_created( resources::queries_result& data )
{
	R_ASSERT						( data.is_successful() );
	
	*m_sound_scene					= static_cast_resource_ptr< sound::sound_scene_ptr >( data[0].get_unmanaged_resource() );
	m_engine.get_sound_world().get_editor_world_user().set_active_sound_scene( *m_sound_scene, 0, 0 );

	init_sound_statistics			( );
}

void editor_world::query_scene		( )
{
	render::editor_renderer_configuration			render_configuration;
	render_configuration.m_create_terrain			= true;
	render_configuration.m_create_particle_world	= true;
	
	resources::user_data_variant* temp_data[] = { NEW(resources::user_data_variant), 0, NEW(resources::user_data_variant), NEW(resources::user_data_variant)};
	temp_data[0]->set(render_configuration);
	temp_data[2]->set( view_handle( ) );
	temp_data[3]->set( camera_view_handle( ) );
	
	resources::user_data_variant const* data[] = {temp_data[0], temp_data[1], temp_data[2], temp_data[3]};
	
	const_buffer					temp_buffer( "", 1 );
	resources::creation_request		requests[] = 
	{
		resources::creation_request( "editor_scene", temp_buffer, resources::scene_class ),
		resources::creation_request( "editor_scene_view", temp_buffer, resources::scene_view_class ),
		resources::creation_request( "editor_view_render_output_window", temp_buffer, resources::render_output_window_class ),
		resources::creation_request( "editor_camera_view_render_output_window", temp_buffer, resources::render_output_window_class ),
	};
 	resources::query_create_resources(
 		requests,
 		boost::bind(&editor_world::on_render_resources_created, this, _1),
 		g_allocator,
 		data
 	);

	DELETE(temp_data[0]);
	DELETE(temp_data[1]);
	DELETE(temp_data[2]);
	DELETE(temp_data[3]);

	while ( !m_scene->c_ptr() ) {
		resources::dispatch_callbacks	( );
		threading::yield				( 10 );
	}

 	resources::query_create_resource
	(
		"editor_sound_scene",
		temp_buffer,
		resources::sound_scene_class,
		boost::bind(&editor_world::on_sound_scene_created, this, _1),
		g_allocator
	);

	// to be sure window is foreground
	m_window_ide->Show					( );
}

void editor_world::on_after_render_scene_created	( )
{
	if(s_animation_editor_scene_key.is_set())
		show_animation_editor();

	if(s_sound_editor_start.is_set())
		show_sound_editor();

	m_level_editor->on_editor_renderer_initialized( );
}

void editor_world::set_cells_flood_depth( u32 value )
{
	if(m_cells_flood_depth!=value)
	{
		m_cells_flood_depth				= value;
		m_level_editor->get_project()->m_editor_cell_manager->on_depth_changed();
	}
}

ImageList^ editor_world::images16x16( ) 
{ 
	return m_window_ide->images16x16(); 
}

level_editor^ editor_world::get_level_editor( ) 
{ 
	return m_level_editor; 
}

void editor_world::message_out( pcstr message )
{
	XRAY_UNREFERENCED_PARAMETER	( message );
	NOT_IMPLEMENTED();
}

editor_base::gui_binder^ editor_world::get_gui_binder( )
{
	return m_gui_binder;
}

void editor_world::init_sound_statistics		( )
{
	R_ASSERT						( m_ui_world );
	m_sound_stats					= NEW( sound::sound_debug_stats )( g_allocator, m_engine.get_sound_world().get_editor_world_user(), *m_sound_scene, *m_ui_world );
}

} // namespace editor
} // namespace xray
