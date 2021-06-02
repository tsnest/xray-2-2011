////////////////////////////////////////////////////////////////////////////
//	Created		: 18.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CUBIC_SPLINE_SKELETON_ANIMATION_COOK_H_INCLUDED
#define CUBIC_SPLINE_SKELETON_ANIMATION_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace animation {

class cubic_spline_skeleton_animation_cook :
	public resources::managed_cook,
	private boost::noncopyable
{
public:
		cubic_spline_skeleton_animation_cook( );
	virtual	u32		calculate_resource_size	( const_buffer in_raw_file_data, bool file_exist );
	virtual void	create_resource			(
						resources::query_result_for_cook& in_out_query, 
						const_buffer raw_file_data,
						resources::managed_resource_ptr	out_resource
					);
	virtual	void	destroy_resource		( resources::managed_resource* dying_resource ) ;

private:
	typedef resources::managed_cook			super;
}; // class cubic_spline_skeleton_animation_cook

} // namespace animation
} // namespace xray

#endif // #ifndef CUBIC_SPLINE_SKELETON_ANIMATION_COOK_H_INCLUDED