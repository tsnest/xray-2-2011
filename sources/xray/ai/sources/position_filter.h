////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef POSITION_FILTER_H_INCLUDED
#define POSITION_FILTER_H_INCLUDED

#include "base_filter.h"
#include <xray/ai/filter_types.h>

namespace xray {
namespace ai {

struct movement_target;

namespace planning {

struct movement_target_wrapper
{
	movement_target_wrapper		(		
		float3 const& target_position,
		float3 const& eyes_direction,
		float3 const& preferable_velocity,		
		pcstr animation
	);
	
	float3						position;
	float3						direction;
	float3						velocity;
	fs_new::virtual_path_string	animation_name;
};

class position_filter : public base_filter
{
public:
					position_filter		( bool need_to_be_inverted );
	virtual			~position_filter	( );

			void	add_filtered_item	(
						float3 const& target_position,
						float3 const& eyes_direction,
						float3 const& preferable_velocity,		
						pcstr animation
					);
	inline	void	set_filter_type		( position_filter_types_enum const type ) { m_filter_type = type; }
	
private:
	typedef list< movement_target_wrapper > filtered_items_type;

private:
	virtual	bool	is_passing_filter	( object_instance_type const& object ) const;
			bool	contains_item		( movement_target const& item ) const;

private:
	filtered_items_type					m_filtered_items;
	position_filter_types_enum			m_filter_type;
}; // class position_filter

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef POSITION_FILTER_H_INCLUDED