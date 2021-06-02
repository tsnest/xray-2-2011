////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SKELETON_ANIMATION_COOK_H_INCLUDED
#define SKELETON_ANIMATION_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace animation {

class skeleton_animation_cook :
	public resources::translate_query_cook,
	private boost::noncopyable
{
public:
						skeleton_animation_cook		( animation::world &w  );

private:
	typedef translate_query_cook					super;

private:
			void		on_bi_spline_data_arrived	( resources::queries_result& result );
			void		on_cubic_spline_animation_cooked( resources::queries_result& data, resources::unmanaged_resource_ptr check_data );
	virtual	void		translate_query				( resources::query_result_for_cook& parent );
	virtual void		delete_resource				( resources::resource_base* resource );

private:
	animation::world&	m_world;
}; // class skeleton_animation_cook

} // namespace animation
} // namespace xray

#endif // #ifndef SKELETON_ANIMATION_COOK_H_INCLUDED