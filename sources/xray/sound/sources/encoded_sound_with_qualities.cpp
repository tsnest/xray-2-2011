////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/encoded_sound_with_qualities.h>
#include "sound_world.h"

namespace xray {
namespace sound {

using namespace resources;

encoded_sound_with_qualities::encoded_sound_with_qualities	( ) : 
				unmanaged_resource		( max_quality_levels_count ),
				m_increasing_quality	( false ),
				m_parent_query			( NULL ),
				m_current_quality		( 0 ),
				m_req_path				( "" ),
				m_sound_interface_type	( unknown_data_class )
{
}

encoded_sound_with_qualities::~encoded_sound_with_qualities	( )
{
	for ( s32 i	= array_size( m_qualities ) - 1; i >= 0; --i )
	{
		if ( m_qualities[i] )
		{
			m_qualities[i].set_zero		( );
		}
	}
}

encoded_sound_child_ptr const& encoded_sound_with_qualities::get_encoded_sound( world const& world, resources::positional_unmanaged_resource const* const resource_user )
{
	XRAY_UNREFERENCED_PARAMETERS		( resource_user, &world );
	//float distance	= .0f; //world->get_distance_to_listener( resource_user->current_matrix( ).lines[3].xyz() );


	for ( s32 i	= array_size( m_qualities ) - 1; i >= 0; --i )
	{
		if ( m_qualities[i] )
		{
			//if ( math::similar( satisfaction_with ( i, resource_user, 0 ), 1.f ) )
			m_current_quality	= i;
			return				m_qualities[i];
		}
	}

	UNREACHABLE_CODE					( return m_qualities[0] );
}

encoded_sound_child_ptr const& encoded_sound_with_qualities::get_encoded_sound( )
{
	for ( s32 i	= array_size( m_qualities ) - 1; i >= 0; --i )
	{
		if ( m_qualities[i] )
		{
			//if ( math::similar( satisfaction_with ( i, resource_user, 0 ), 1.f ) )
			m_current_quality	= i;
			return				m_qualities[i];
		}
	}

	UNREACHABLE_CODE					( return m_qualities[0] );
}

bool encoded_sound_with_qualities::add_quality (encoded_sound_ptr child, u32 quality)
{
	for ( u32 i=0; i<array_size(m_qualities); ++i )
	{
		if ( !m_qualities[i] )
		{
			m_qualities[i].initialize_as_quality(this, child.c_ptr(), quality);
			return						true;
		}
	}

	return								false;
}

void encoded_sound_with_qualities::increase_quality_to_target	(query_result_for_cook * parent_query)
{
	m_parent_query					= parent_query;

	u32 const target_quality		= target_quality_level();

	request all_requests [max_quality_levels_count];
	fixed_string<128> names[max_quality_levels_count];

	float satisfactions[max_quality_levels_count];
	u32	quality_indexes[max_quality_levels_count];

	if ( m_req_path.empty( ) )
	{
		m_req_path				= parent_query->get_requested_path( );
	}

	if ( m_sound_interface_type == unknown_data_class )
	{
		m_sound_interface_type			= ogg_encoded_sound_interface_class; // default value

		//query_result_for_cook const* p	= parent_query;

		//while ( p )
		//{
		//	resources::user_data_variant* user_data	= p->user_data( );
		//	if ( user_data )
		//	{
		//		class_id_enum sound_interface_type;
		//		user_data->try_get( sound_interface_type );
		//		R_ASSERT	( sound_interface_type == ogg_encoded_sound_interface_class || sound_interface_type == wav_encoded_sound_interface_class );
		//		m_sound_interface_type	= sound_interface_type;
		//		break;
		//	}
		//	p = p->get_parent_query( );
		//}
	}

	for ( u32 i = 0; i <  max_quality_levels_count; ++i )
	{
		names[i].append				( m_req_path.begin( ), m_req_path.end( ) );
		if ( m_sound_interface_type == ogg_encoded_sound_interface_class )
		{
			switch ( i )
			{
			case 0:		names[i].append	( ".high" );	break;
			case 1:		names[i].append	( ".medium" );	break;
			case 2:		names[i].append	( ".low" );		break;
			default:	NODEFAULT( );
			}
		}
	}

	for ( u32 i = 0; i < max_quality_levels_count; ++i )
	{
		quality_indexes[i]			=	i;

		all_requests[i].path	=	names[i].c_str( );
		all_requests[i].id		=	m_sound_interface_type;
		satisfactions[i]		=	m_parent_query ? 
									m_parent_query->satisfaction(i) : satisfaction(i);

	}


	request* requests	= &*all_requests + target_quality;
	u32 const current_quality		=	current_quality_level();
	u32 const requests_count		=	((current_quality == u32(-1)) ? 
										 quality_levels_count() : current_quality) - target_quality;

	query_resource_params				params (requests, 
												NULL, 
												requests_count, 
												boost::bind(& encoded_sound_with_qualities::on_quality_loaded, this, _1),
												resources::unmanaged_allocator(), 
												satisfactions, 
												NULL, NULL, parent_query, NULL, 
												quality_indexes);

	m_increasing_quality			= true;
	query_resources					( params );
}

void encoded_sound_with_qualities::decrease_quality (u32 new_best_quality)
{
	for ( u32 i=0; i<array_size(m_qualities); ++i )
	{
		u32 const child_quality		=	this->child_quality( m_qualities[i].c_ptr() );
		if ( child_quality != u32(-1) && child_quality < new_best_quality )
			m_qualities[i].set_zero		( );
	}
}

float encoded_sound_with_qualities::satisfaction_with (u32 quality_level, positional_unmanaged_resource const * resource_user, u32 positional_users_count) const
{
	XRAY_UNREFERENCED_PARAMETERS	(positional_users_count, resource_user);
	float const satisfaction		= (max_quality_levels_count - quality_level) * 1.f / max_quality_levels_count;
	return							satisfaction;
}

//struct sound_world {
//	threading::atomic_ptr<math::float3>	m_listener_position;
//	math::float3						m_listener_positions[2];
//
//};

void encoded_sound_with_qualities::on_quality_loaded	(queries_result & resources)
{
	R_ASSERT						( m_increasing_quality );
	u32 const target_quality		= target_quality_level( );
//	m_current_quality				= target_quality;

	bool const increasing			= !m_parent_query;

	bool some_quality_loaded		= false;
	bool some_qualities_failed		= false;

	for ( int i = resources.size() - 1; i >= 0; i-- )
	{
		u32 const current_result_quality	= target_quality + i;
		if ( resources[i].is_successful() )
		{
			R_ASSERT					(!some_qualities_failed);
		}
		else
		{
			LOG_DEBUG					("grm", "%s %s failed", 
										increasing ? "increasing" : "loading",
										resources[i].get_requested_path());
			some_qualities_failed	=	true;
			continue;
		}

		some_quality_loaded			=	true;

		encoded_sound_ptr quality_child =	static_cast_resource_ptr<encoded_sound_ptr>(resources[i].get_unmanaged_resource());
		add_quality						(quality_child, current_result_quality);
	}

	LOG_DEBUG							("grm", "%s qualities(->%d) for %s %s", 
										increasing ? "increasing" : "loading",
										current_quality_level(),
										resources::log_string(this).c_str(),
										some_quality_loaded ? "successful" : "failed");
	if ( m_parent_query )
	{
		m_parent_query->finish_query	(some_quality_loaded ? 
										 cook_base::result_success : cook_base::result_error);
		m_parent_query				=	NULL;
	}

	m_increasing_quality			= false;
}

bool encoded_sound_with_qualities::is_increasing_quality	( ) const 
{ 
	return	m_increasing_quality; 
}

encoded_sound_child_ptr const& encoded_sound_with_qualities::dbg_get_encoded_sound( u32 quality ) const
{
	if ( m_qualities[quality] )
	{
		m_current_quality	= quality;
		return				m_qualities[quality];
	}

	// else return best availiable quality
	for ( s32 i	= array_size( m_qualities ) - 1; i >= 0; --i )
	{
		if ( m_qualities[i] )
		{
			m_current_quality	= quality;
			return				m_qualities[i];
		}
	}

	UNREACHABLE_CODE					( return m_qualities[0] );
}

} // namespace sound
} // namespace xray
