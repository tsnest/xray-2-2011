////////////////////////////////////////////////////////////////////////////
//	Created		: 07.06.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_COOKER_H_INCLUDED
#define OBJECT_COOKER_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace stalker2 {
class game;

class object_cooker :		public resources::unmanaged_cook,
							private boost::noncopyable
{
	typedef resources::unmanaged_cook	super;
public:
									object_cooker			( game& game );
	virtual	mutable_buffer			allocate_resource		( resources::query_result_for_cook&	in_query, 
															 const_buffer						raw_file_data, 
															 bool								file_exist );

	virtual void					deallocate_resource		( pvoid buffer );

	virtual void					create_resource			( resources::query_result_for_cook &	in_out_query, 
												 			  const_buffer							raw_file_data, 
															  mutable_buffer						in_out_unmanaged_resource_buffer );

	virtual void					destroy_resource		( resources::unmanaged_resource *		resource );

private:
	game&							m_game;
}; //class object_cooker

struct object_scene_cooker : public resources::translate_query_cook,
							private boost::noncopyable
{
	typedef resources::translate_query_cook		super;

					object_scene_cooker		( game& game );
	virtual	void	translate_query			( resources::query_result_for_cook&	parent );

	virtual void	delete_resource			( resources::resource_base* resource );

private:
			void	on_subresources_loaded	( resources::queries_result& data, resources::query_result_for_cook* parent_query );
			void	on_scene_creation		( resources::query_result_for_cook* parent );	


private:
	game&							m_game;
}; //object_scene_cooker


} // namespace stalker2

#endif // #ifndef OBJECT_COOKER_H_INCLUDED