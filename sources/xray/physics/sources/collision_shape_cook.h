////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_SHAPE_COOK_H_INCLUDED
#define COLLISION_SHAPE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace physics {

class collision_shape_cook :public resources::translate_query_cook
{
	typedef resources::translate_query_cook		super;
public:
								collision_shape_cook	( bool static_object );
	virtual	void				translate_query			( resources::query_result_for_cook&	parent );

	virtual void				delete_resource			( resources::resource_base* resource );
private:
			void				on_triangle_mesh_collision_loaded( resources::queries_result& data, 
														  resources::query_result_for_cook* parent_query );

			void				on_primitives_collision_loaded( resources::queries_result& data, 
														  resources::query_result_for_cook* parent_query );

			bool				m_static_object;
}; // class collision_shape_cook

} // namespace physics
} // namespace xray

#endif // #ifndef COLLISION_SHAPE_COOK_H_INCLUDED