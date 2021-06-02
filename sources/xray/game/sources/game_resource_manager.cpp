////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resource_manager.h"

namespace stalker2 {

solid_visual_cooker::solid_visual_cooker( )
:super(resources::solid_visual_class, reuse_false, use_user_thread_id)
{
}

void solid_visual_cooker::translate_query(resources::query_result & parent)
{
	query_resource(	parent.get_requested_path(), 
					resources::visual_class, 
					boost::bind(&solid_visual_cooker::on_sub_resources_loaded, this, _1), 
					parent.get_user_allocator(), 
					NULL,
					0,
					&parent);
}

void solid_visual_cooker::on_sub_resources_loaded( resources::queries_result& data )
{
	R_ASSERT( data.is_successful() );

	resources::query_result_for_cook * parent	=	data.get_parent_query();

	resources::unmanaged_resource_ptr	unmanaged_resource	=	data[0].get_unmanaged_resource();

	if ( unmanaged_resource )
		parent->set_unmanaged_resource	(unmanaged_resource, unmanaged_resource->memory_usage());

	parent->set_managed_resource	(data[0].get_managed_resource());
	parent->finish_query			(result_success);
}

void solid_visual_cooker::delete_resource(resources::unmanaged_resource * resource)
{
	XRAY_UNREFERENCED_PARAMETER	( resource );
	NOT_IMPLEMENTED				( );
}

void solid_visual_cooker::translate_request_path( pcstr request_path, fs::path_string & new_request ) const 
{
	new_request					= "resources/library/static/";
	new_request					+= request_path;
	new_request					+= ".ogf";
}


//game_resource_manager::game_resource_manager( game& game )
//:m_game(game)
//{
//	static solid_visual_cook	cook;
//	resources::register_cook	( &cook );
//}
//
//void game_resource_manager::on_discard_( game_object* o )
//{
//	game_queries_it it			= m_active_queries.begin();
//	game_queries_it it_e		= m_active_queries.end();
//
//	for(; it!=it_e; ++it)
//	{
//		query_&	q = *it;
//		if(q.customer == o)
//		{
//			q.customer			= NULL;
//			LOG_INFO			("cleared");
//		}
//	}
//}
//
//void game_resource_manager::query(pcstr id, game_object* o, resources::class_id type, query_resource_callback callback)
//{
//	switch (type)
//	{
//		case resources::solid_visual_class:
//		{
//			m_active_queries.push_back	( query_(-1, o, callback) );
//			query_& last				= m_active_queries.back();
//
//			last.query_id = query_resource	(
//				id,
//				type,
//				boost::bind(&game_resource_manager::on_resource_loaded, this, _1),
//				g_allocator
//			);
//		} break;
//		case resources::terrain_cell_class:
//		{
//			fs::path_string query_path	= id; // find cached
//
//			pcstr project_name				= m_game.project_path().c_str();
//			resources::user_data_variant	v;
//			v.set							(project_name);
//
//			m_active_queries.push_back	( query_(-1, o, callback) );
//			query_& last				= m_active_queries.back();
//
//			last.query_id = query_resource	(
//				id,
//				type,
//				boost::bind(&game_resource_manager::on_resource_loaded, this, _1),
//				g_allocator,
//				&v
//			);
//		} break;
//		default :
//			NODEFAULT();
//	}
//}
//
//void game_resource_manager::tick()
//{
//}

} // namespace stalker2
