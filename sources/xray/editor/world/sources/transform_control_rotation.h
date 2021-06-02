////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_ROTATION_H_INCLUDED
#define TRANSFORM_CONTROL_ROTATION_H_INCLUDED

#include "transform_control_base.h"

struct transform_control_rotation_internal;

namespace xray {

namespace collision {

struct engine;
class loose_oct_tree;
struct geometry;

}//namespace collision


namespace editor {

class collision_object;

public ref class transform_control_rotation : public transform_control_base
{

	typedef transform_control_base	super;

public:


	transform_control_rotation						( level_editor^ le );
	~transform_control_rotation						( );

	virtual		void		initialize				( ) override;
	virtual		void		destroy					( ) override;
	virtual		void		update					( ) override;
	virtual		void		start_input				( ) override;
	virtual		void		execute_input			( ) override;	
	virtual		void		end_input				( )	override;
	virtual		void		collide					( collision_object const* object, float3 const& position ) override;
	virtual		void		draw					( math::float4x4 const& view_matrix ) override;

	virtual		void		set_transform			( math::float4x4 const& transform ) override;
	virtual		void		resize					( float size ) override;
	virtual		math::float4x4	create_object_transform	( math::float4x4 const& object_tranform, enum_transform_mode mode ) override;

private:

				void		draw_axis_ellipse		( math::float4x4 const& draw_transform, math::float3 rotation, enum_transform_axis axis, bool stripe_lines );
				void		draw_inner_outer_lines	( xray::math::float4x4 const& draw_tranform,  math::float4x4 const& rotate_to_view, bool stripe_lines );

				void		set_active_axis			( enum_transform_axis axis );
				void		draw_last_position		( math::float4x4 const& view_matrix, bool transparent_mode );
				math::float3 create_rotation_axis	( math::float4x4 tranform );

private:
	transform_control_rotation_internal*		m_data;

	float								m_radius;
	const float							m_outer_radius_ratio;
	const float							m_inner_radius_ratio;
	float								m_outer_radius;
	float								m_inner_radius;
	const float							m_last_pos_segment_step;

	float								m_current_angle;

	bool								m_initialized;
	bool								m_rolling_mode;
	bool								m_rolling_first_step;

	collision_object_transform_control*	m_outer_ring;

}; // class transform_control_rotation

}//namespace editor
}//namespace xray


#endif // #ifndef TRANSFORM_CONTROL_ROTATION_H_INCLUDED