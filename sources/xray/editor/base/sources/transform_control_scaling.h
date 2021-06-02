////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_SCALING_H_INCLUDED
#define TRANSFORM_CONTROL_SCALING_H_INCLUDED

#include "transform_control_base.h"

namespace xray {

namespace editor_base {

struct transform_control_scaling_internal
{
	transform_control_scaling_internal()
	:m_draw_arrows_lengths		( 4.0f, 4.0f, 4.0f ),
	m_scale_vector				( 0, 0, 0 )
	{}

	float3						m_drag_start;
	float3						m_curr_axis;

	float4x4					m_resulting_transform;
	float4x4					m_transform_original;
	float4x4					m_draw_transform;
	float3						m_draw_arrows_lengths;
	float3						m_scale_vector;
};

public ref class transform_control_scaling : public transform_control_base
{

	typedef transform_control_base	super;

public:


							transform_control_scaling	( transform_control_helper^ helper );
							~transform_control_scaling	( );

	virtual float4x4		calculate				( float4x4 const& m )	override;
	virtual		void		start_input				( int mode, int button, collision::object const* coll, math::float3 const& pos ) override;
	virtual		void		execute_input			( ) override;
	virtual		void		end_input				( ) override;
	virtual		void		draw					( render::scene_view_pointer scene_view, render::render_output_window_pointer output_window ) override;
	virtual		void		set_matrix				( math::float4x4 const& m ) override;
	virtual		void		one_step_execute		( enum_transform_axis axis, float value ) override;

protected:
	virtual		void		initialize				( ) override;

private:

	void					update_visibility		( );
	void					draw_impl				( bool transparent_mode  );
	void					draw_last_position		( bool transparent_mode );

private:
	static float const		step_value			= 0.1f;
	transform_control_scaling_internal*			m_data;		
	float										m_arrow_length;
	const float									m_arrow_head_radius;
	const float									m_center_box_drag_factor;

	bool										m_plane_mode;
}; // class transform_control_scaling

}//namespace editor_base
}//namespace xray

		

#endif // #ifndef TRANSFORM_CONTROL_SCALING_H_INCLUDED