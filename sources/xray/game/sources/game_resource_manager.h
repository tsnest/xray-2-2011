////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESOURCE_MANAGER_H_INCLUDED
#define GAME_RESOURCE_MANAGER_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace stalker2{

class solid_visual_cooker : public resources::translate_query_cook 
{
private:
	typedef resources::translate_query_cook	super;

public:
					solid_visual_cooker	( );

	virtual	void	translate_query			( resources::query_result& parent );
	virtual void	delete_resource			( resources::unmanaged_resource* resource );
	virtual	void	translate_request_path	( pcstr request_path, fs::path_string& new_request ) const;

private:
	void			on_sub_resources_loaded	( resources::queries_result& data );
}; // class solid_visual_cook

//class game;
//
//typedef fastdelegate::FastDelegate< void ( xray::resources::unmanaged_resource_ptr data ) >	query_resource_callback;
//
//class game_resource_manager : public boost::noncopyable
//{
//public:
//					game_resource_manager	( game& g );
//
//	void			tick					( );
//	void			query					( pcstr id, game_object* who, resources::class_id type, query_resource_callback callback);
//	u32				active_queries_count	( ) const {return m_active_queries.size();}
//	void			on_discard_				( game_object* who );
//
//private:	
//	void			on_resource_loaded		(xray::resources::queries_result& data);
//
//	typedef vector<xray::resources::unmanaged_resource_ptr>	game_resources;
//	typedef game_resources::iterator		game_resources_it;
//	typedef game_resources::const_iterator	game_resources_cit;
//
////	game_resources							m_resources;
//
//	struct query_{
//		long					query_id;
//		game_object_ptr			customer;
//		query_resource_callback	callback;	
//		
//		query_(long id, game_object* who, query_resource_callback& cb):query_id(id),customer(who),callback(cb){}
//		bool operator == (long id) const{ return query_id == id;}
//		bool operator == (game_object* who) const{ return customer == who;}
//	};
//	typedef vector<query_>					game_queries;
//	typedef game_queries::iterator			game_queries_it;
//	game_queries							m_active_queries;
//	game&									m_game;
//}; //class resource_manager

} //namespace stalker2


#endif //GAME_RESOURCE_MANAGER_H_INCLUDED