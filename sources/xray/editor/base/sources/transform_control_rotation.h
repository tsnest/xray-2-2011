////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_ROTATION_H_INCLUDED
#define TRANSFORM_CONTROL_ROTATION_H_INCLUDED

#include "transform_control_base.h"

namespace xray {

namespace editor_base {

struct transform_control_rotation_internal : private boost::noncopyable
{
	float4x4				m_translation_transform;
	float4x4				m_draw_transform;
	float3					m_rolling_axis;
	float3					m_drag_start;
	float3					m_curr_axis_vector;
	float3					m_start_radius_vector;
	float3					m_start_plane_vector;
};

public ref class transform_control_rotation : public transform_control_base
{
	typedef transform_control_base	super;
public:

						transform_control_rotation	( transform_control_helper^ helper );
	virtual				~transform_control_rotation	( );

	virtual float4x4		calculate				( float4x4 const& m )	override;
	virtual		void		update					( ) override;
	virtual		void		start_input				( int mode, int button, collision::object const* coll, math::float3 const& pos ) override;
	virtual		void		execute_input			( ) override;	
	virtual		void		end_input				( )	override;
	virtual		void		draw					( render::scene_view_pointer scene_view, render::render_output_window_pointer output_window ) override;
	virtual		void		one_step_execute		( enum_transform_axis axis, float value ) override;
	virtual		void		set_matrix				( math::float4x4 const& m ) override;

protected:
	virtual		void		initialize				( ) override;
private:

				void		draw_axis_ellipse		( math::float4x4 const& draw_transform, math::float3 rotation, enum_transform_axis axis, bool stripe_lines );
				void		draw_inner_outer_lines	( xray::math::float4x4 const& draw_tranform,  math::float4x4 const& rotate_to_view, bool stripe_lines );

				void		draw_last_position		( bool transparent_mode );
				math::float3 create_rotation_axis	( math::float4x4 tranform );

private:
	static float const		step_value			= 10.0f*(math::pi/180.0f);

	transform_control_rotation_internal*		m_data;

	float								m_radius;
	const float							m_outer_radius_ratio;
	const float							m_inner_radius_ratio;
	float								m_outer_radius;
	float								m_inner_radius;
	const float							m_last_pos_segment_step;

	float								m_current_angle;

	bool								m_rolling_mode;
	bool								m_rolling_first_step;

	collision_object_transform_control*	m_outer_ring;

}; // class transform_control_rotation

}//namespace editor_base
}//namespace xray


#endif // #ifndef TRANSFORM_CONTROL_ROTATION_H_INCLUDED