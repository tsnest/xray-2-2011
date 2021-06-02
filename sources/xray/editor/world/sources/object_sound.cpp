////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_sound.h"
#include "tool_sound.h"
#include "project.h"
#include "project_items.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/collision/collision_object.h>
#include <xray/editor/base/collision_object_types.h>
#include <xray/sound/sound_instance_proxy.h>

namespace xray{
namespace editor{


object_sound::object_sound( tool_sound^ tool, render::scene_ptr const& scene )	:
	super			( tool ),
	m_tool_sound	( tool ),
	m_loaded		( false ),
	m_positional	( true ),
	m_scene			( NEW(render::scene_ptr) (scene) ),
	m_sound_type		( 0 ),
	m_emitter_type		( 0 )
{
	m_position			= NEW(math::float3)(0,0,0);
	m_direction 		= NEW(math::float3)(0,0,1);
	m_up				= NEW(math::float3)(0,1,0);
	m_sound_emitter_ptr = NEW(sound::sound_emitter_ptr)();
	m_proxy				= NEW(sound::sound_instance_proxy_ptr)();
	m_callback			= NEW(callback_handler);
	m_wav_file_name 	= gcnew System::String("");

	if(!m_collision->initialized())
		initialize_collision();

}

object_sound::~object_sound()
{
	DELETE		( m_position );
	DELETE		( m_direction );
	DELETE		( m_up );
	
	DELETE ( m_proxy );
	DELETE ( m_sound_emitter_ptr );
	DELETE ( m_callback );

	if ( m_collision->initialized() )
		m_collision->destroy( &debug::g_mt_allocator );
}

void object_sound::set_transform(float4x4 const& transform)
{
	super::set_transform			(transform);
	*m_position						= transform.c.xyz();
	*m_direction					= transform.i.xyz();
	if ((*m_proxy).c_ptr())
	{
		if ( m_sound_type == 0 )
			(*m_proxy)->set_position				(*m_position);
		else
			(*m_proxy)->set_position_and_direction	(*m_position, *m_direction);
	}
}

void object_sound::render( )
{
	super::render			( );

	get_debug_renderer().draw_origin	( *m_scene, get_transform(), 2.0f );
	if (m_project_item->get_selected( ))
		get_debug_renderer().draw_line_ellipsoid( *m_scene, get_transform(), color(255, 0, 0));
	else
		get_debug_renderer().draw_line_ellipsoid( *m_scene, get_transform(), color(0, 255, 0));

	if ( m_sound_type == 1 )
	{
		get_debug_renderer().draw_arrow			( *m_scene, *m_position, *m_position + (*m_direction * 5.0f), color(0, 0, 255));
	}

//	if ((*m_proxy).c_ptr())

	//if ( m_proxy && (*m_proxy).c_ptr() )
	//{
	//	if ( (*m_proxy)->is_propagated( ) )
	//	{
	//		u32 propagation_start_pos	= (*m_proxy)->propagation_start_pos ( );
	//		u32 propagation_end_pos		= (*m_proxy)->propagation_end_pos ( );


	//		float start_radius		= propagation_start_pos * get_editor_world( ).engine( ).get_sound_world( ).get_speed_of_sound();
	//		float end_radius		= 0.0f;//propagation_end_pos > 0 ? propagation_end_pos * get_editor_world( ).engine( ).get_sound_world( ).get_speed_of_sound() : 0;

	//		float4x4 scaled_mtx			= get_transform( );
	//		scaled_mtx.set_scale		( float3( start_radius, start_radius, start_radius ));
	//		get_debug_renderer().draw_line_ellipsoid ( *m_scene, scaled_mtx, color( 0, 0, 255 ));
	//		
	//		if ( !math::is_zero( end_radius ) )
	//		{
	//			float4x4 end_scaled_mtx		= get_transform( );
	//			end_scaled_mtx.set_scale	( float3( end_radius, end_radius, end_radius ));
	//			get_debug_renderer().draw_line_ellipsoid ( *m_scene, end_scaled_mtx, color( 0, 0, 255 ));
	//		}
	//	}
	//}
}

void object_sound::on_selected(  bool selected )
{
	XRAY_UNREFERENCED_PARAMETER( selected );
}

void object_sound::load_contents( )
{
	LOG_INFO("load_contents");
}

void object_sound::initialize_collision( )
{
	ASSERT( !m_collision->initialized() );
	float3 extents				(0.3f,0.3f,0.3f);

	collision::geometry_instance* geom	= &*collision::new_box_geometry_instance( &debug::g_mt_allocator, math::create_scale(extents) );
	m_collision->create_from_geometry( true, this, geom, editor_base::collision_object_type_dynamic | editor_base::collision_object_type_touch );
	m_collision->insert			( m_transform );
}

void object_sound::load_sound					( )
{
	if ( m_wav_file_name == "" )
		return;

	query_result_delegate* q					= NEW( query_result_delegate )( gcnew query_result_delegate::Delegate( this, &object_sound::on_sound_loaded ), g_allocator );

	unmanaged_string wav_file_name				( m_wav_file_name );

	resources::class_id_enum class_id			= resources::unknown_data_class;
	switch ( m_emitter_type )
	{
		case 0: class_id						= resources::single_sound_class; break;
		case 1: class_id						= resources::composite_sound_class; break;
		case 2: class_id						= resources::sound_collection_class; break;
		default:								NODEFAULT();
	}
	resources::request request =
	{
		wav_file_name.c_str(),
		class_id
	};

	math::float4x4								matrix;
	matrix.identity								( );
	math::float4x4 const* matrix_pointers[] =
	{
		&matrix
	};
	
	resources::query_resource_params params		(
		&request,
		NULL,
		1, 
		boost::bind( &query_result_delegate::callback, q, _1 ),
		g_allocator, 
		NULL,
		matrix_pointers																								
	);

	query_resources								( params );

	//resources::query_resource
	//(
	//	unmanaged_string(m_wav_file_name).c_str( ),
	//	resources::single_sound_class,
	//	boost::bind(&query_result_delegate::callback, q, _1),
	//	g_allocator
	//);
}

void object_sound::on_sound_loaded( resources::queries_result& data )
{
	ASSERT						(data.is_successful());
	*m_sound_emitter_ptr		= static_cast_resource_ptr<sound::sound_emitter_ptr>(data[0].get_unmanaged_resource());
}

void object_sound::play_clicked		( button^ )
{

	emit							( );

	if ((*m_proxy).c_ptr( ) == 0)
		return;

	(*m_proxy)->set_callback		( boost::bind( &callback_handler::on_finish_playing, m_callback ) );
	if ( m_sound_type == 0 )
		(*m_proxy)->set_position				(*m_position);
	else
		(*m_proxy)->set_position_and_direction	(*m_position, *m_direction);
	(*m_proxy)->play							( sound::once );
}

void object_sound::play_looped_clicked	( button^ )
{
	LOG_DEBUG						( "play_looped_clicked" );
	emit							( );

	if ((*m_proxy).c_ptr( ) == 0)
		return;

	if ( m_sound_type == 0 )
		(*m_proxy)->set_position				(*m_position);
	else
		(*m_proxy)->set_position_and_direction	(*m_position, *m_direction);
	(*m_proxy)->play							( sound::looped );
}

void object_sound::play_once_clicked	( button^ )
{
//	if ( (*m_sound_emitter_ptr).c_ptr() )
//	{
//		sound::sound_instance_proxy_ptr proxy = (*m_sound_emitter_ptr)->emit( get_editor_world().sound_scene(), get_editor_world( ).engine( ).get_sound_world( ).get_editor_world_user( ));
//	}
}

void object_sound::stop_clicked			( button^ )
{
	if ( (*m_proxy).c_ptr( ) )
	{
		(*m_proxy)->stop	( );
		*m_proxy			= 0;
	}
}

void object_sound::pause_clicked	( button^ )
{
	if ( (*m_proxy).c_ptr() )
	{
		(*m_proxy)->pause( );
	}
}

void object_sound::resume_clicked	( button^ )
{
	if ( (*m_proxy).c_ptr() && (*m_proxy)->is_propagating_paused( ) )
	{
		(*m_proxy)->resume( );
	}
}

void object_sound::test_clicked		( button^ )
{
	if (!m_proxy->c_ptr())
		return;

//	for ( int i = 0; i < 10; ++i )
//	{
//		if ( (*m_proxy)->is_propagated( ) )
//		{
////			(*m_proxy)->pause	( );
////			(*m_proxy)->resume	( get_position( ) );
//		}
//	}
}

void object_sound::fatality_clicked	( button^ )
{
//	if (!m_proxy->c_ptr())
//		return;
//
//	if ((*m_proxy)->is_playing( ) || (*m_proxy)->is_paused( ))
//		return;
//
//	if ( (*m_proxy)->is_propagated( ) )
//	{
//		LOG_ERROR		( "attempt to play propagating sound!" );
//		return;
//	}
//
//	(*m_proxy)->play	( get_position( ) );
//
//	for ( int i = 0; i < 10; ++i )
//	{
//		(*m_proxy)->pause	( );
//		(*m_proxy)->resume	( get_position( ) );
//	}
//
//	(*m_proxy)->stop	( );
//	(*m_proxy)->play	( get_position( ) );
//	
//	for ( int i = 0; i < 1; ++i )
//	{
//		(*m_proxy)->pause	( );
//		(*m_proxy)->resume	( get_position( ) );
//	}
//
//	(*m_proxy)->stop	( );
//	(*m_proxy)->play	( get_position( ) );
//	(*m_proxy)->stop	( );
//	(*m_proxy)->play	( get_position( ) );
//
//
//	for ( int i = 0; i < 1; ++i )
//	{
//		(*m_proxy)->pause	( );
//		(*m_proxy)->resume	( get_position( ) );
//	}
//
////	(*m_proxy)->stop	( );
}

void object_sound::brutality_clicked( button^ )
{
	//if (!m_proxy->c_ptr())
	//	return;

	//if (!m_proxy->c_ptr())
	//	return;

	//if ( (*m_proxy)->is_playing( ) )
	//{
	//	for ( int i = 0; i < 100; ++i )
	//	{
	//		(*m_proxy)->pause				( );
	//		(*m_proxy)->resume				( get_position( ) );
	//	}
	//}
}

void object_sound::stop_now_clicked	( button^ )
{
	if (!m_proxy->c_ptr())
		return;
	//if ( (*m_proxy)->is_playing( ) )
//	(*m_proxy)->stop( true );
}

double object_sound::max_seek_val	( )
{
//	if ((*m_proxy).c_ptr()) return (*m_proxy)->get_length_in_msec( ) / 1000.0f;
//	else					return 0.0f;
	return 0.0f;
}

void object_sound::emit	( )
{
	if ( (*m_sound_emitter_ptr).c_ptr() )
	{
		if ( m_sound_type == 0 )
		{
			*m_proxy = ((*m_sound_emitter_ptr)->emit_point_sound
			(
				get_editor_world().sound_scene(),
				get_editor_world().engine().get_sound_world().get_editor_world_user()
			));
		}
		else
		{
			*m_proxy = ((*m_sound_emitter_ptr)->emit_spot_sound
			(
				get_editor_world().sound_scene(),
				get_editor_world().engine().get_sound_world().get_editor_world_user(),
				sound::human
			));
		}
	}
}


wpf_controls::property_container^ object_sound::get_property_container( )
{
	wpf_controls::property_container^ container = super::get_property_container();

	container->properties["sound/emitter_type"]->dynamic_attributes->add( 
		gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( gcnew cli::array<System::String^, 1>(3){ "single", "composite", "collection" } ) 
	);

	Collections::ArrayList^ items;
	switch ( m_emitter_type )
	{
	case 0: items = m_tool_sound->single_sound_files( ); break;
	case 1: items = m_tool_sound->composite_sound_files( ); break;
	case 2: items = m_tool_sound->sound_collections_files( ); break;
	default: NODEFAULT( );
	}

	container->properties["sound/wav"]->dynamic_attributes->add( 
		gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( items ) 
	);

	/*container->properties["sound/wav"]->dynamic_attributes->add( 
		gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( gcnew cli::array<System::String^, 1>(5){ "", "test_sounds/ai1_enemy_lost_0", "test_sounds/guitar_10", "test_sounds/see_counter_3", "npc/pain/pain4" } ) 
	);*/


//	container->properties["sound/seek"]->dynamic_attributes->add( 
//		gcnew wpf_controls::property_editors::attributes::value_range_and_format_attribute( gcnew System::Func<double>( this, &object_sound::min_seek_val), gcnew System::Func<double>( this, &object_sound::max_seek_val ), 1.0f, 2.0f ));

	wpf_controls::property_descriptor^ desc = gcnew wpf_controls::property_descriptor("Playback");
	

	desc->add_button			( "Play", "SomeDescription", gcnew button_delegate(this, &object_sound::play_clicked) );
	desc->add_button			( "Play looped", "SomeDescription", gcnew button_delegate(this, &object_sound::play_looped_clicked) );
//	desc->add_button			( "Play once", "SomeDescription", gcnew button_delegate(this, &object_sound::play_once_clicked) );
	desc->add_button			( "Stop", "SomeDescription", gcnew button_delegate(this, &object_sound::stop_clicked) );
	desc->add_button			( "Pause", "SomeDescription", gcnew button_delegate(this, &object_sound::pause_clicked) );
	desc->add_button			( "Resume", "SomeDescription", gcnew button_delegate(this, &object_sound::resume_clicked) );

	container->properties["sound/type"]->dynamic_attributes->add
	( 
		gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( gcnew cli::array<System::String^, 1>(2){ "point", "cone" } ) 
	);

	container->properties->add	( desc );

	return				container;
}

void object_sound::on_library_fs_iterator_ready( vfs::vfs_locked_iterator const & fs_it )
{
	process_recursive_names	( fs_it.children_begin(), "" );
}

void object_sound::process_recursive_names	( vfs::vfs_iterator const & in_it, System::String^ path )
{
	vfs::vfs_iterator it				= in_it;

	while( !it.is_end() )
	{
		pcstr model_name_prefix			= ".wav";
		u32 model_name_prefix_len		= strings::length(model_name_prefix);
		System::String^ name			= gcnew System::String(it.get_name());
		System::String^ full_name		= path;
		
		if(full_name->Length)
			full_name					+= "/";


		if(strstr(it.get_name(), model_name_prefix))
		{
			name						= name->Remove(name->Length-model_name_prefix_len);
			full_name					+= name;
			LOG_DEBUG					("full_name: %s", full_name );
			//m_library->add_simple		( full_name );
		}
		else if(it.is_folder())
		{
			full_name					+= name;
			process_recursive_names		( it.children_begin(), full_name );
		}
		++it;
	}
}

} // namespace editor
} // namespace xray