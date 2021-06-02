////////////////////////////////////////////////////////////////////////////
//	Created		: 04.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_COOK_H_INCLUDED
#define COLLISION_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace collision {

struct collision_cook : public resources::translate_query_cook
{
	typedef resources::translate_query_cook				super;

								collision_cook			( );
	virtual	void				translate_query			( resources::query_result_for_cook&	parent );

	virtual void				delete_resource			( resources::resource_base* resource );
private:
			void				query_triangle_mesh		( resources::query_result_for_cook * parent_query );
			void				on_triangle_mesh_collision_loaded( resources::queries_result &		data, 
																   resources::query_result_for_cook* parent_query );

			void				on_primitives_collision_config_loaded( resources::queries_result& data, 
																resources::query_result_for_cook* parent_query );
};

} // namespace collision
} // namespace xray

#endif // #ifndef COLLISION_COOK_H_INCLUDED