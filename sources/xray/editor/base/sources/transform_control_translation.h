////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_TRANSLATION_H_INCLUDED
#define TRANSFORM_CONTROL_TRANSLATION_H_INCLUDED

#include "transform_control_base.h"
namespace xray {
	namespace render {
		class render_output_window;
	}
}

namespace xray {

namespace editor_base {

public ref class transform_control_translation: public transform_control_base
{
	typedef transform_control_base	super;

public:

							transform_control_translation	( transform_control_helper^ helper );
	virtual					~transform_control_translation	( );

	virtual float4x4		calculate				( float4x4 const& m )	override;
	virtual		void		update					( )						override;
	virtual		void		start_input				( int mode, int button, xray::collision::object const* coll, math::float3 const& pos ) override;
	virtual		void		execute_input			( ) override;
	virtual		void		end_input				( ) override;
	virtual		void		draw					( render::scene_view_pointer scene_view, xray::render::render_output_window_pointer output_window ) override;
	virtual		void		set_matrix				( float4x4 const& m ) override;
				void		set_snap_mode			( snap_mode m ){ m_snap_mode = m;}
	virtual		void		one_step_execute		( enum_transform_axis axis, float value ) override;

protected:
	virtual		void		initialize				( ) override;
				void		execute_input_no_snap	( );
				void		execute_input_step_snap	( );
				void		execute_input_vf_snap	( );

private:
				void		set_active_plane	( enum_transform_axis plane );

				float4x4	create_plane_box_rotation	( enum_transform_axis mode, float4x4 const& transform );

				void		update_active_mode	( );
				void		update_visibility	( );
				void		draw_last_position	( bool transparent_mode );
				void		draw_impl			( bool transparent_mode );
	
	static float const		step_value			= 0.1f;

	float3					get_drag_vector		( );
	float										m_arrow_size;
	const float									m_arrow_head_length;
	const float									m_arrow_head_radius;
	float3*										m_mouse_drag_start;
	float3*										m_origin_drag_start;
	enum_transform_axis							m_active_plane;
	
	bool										m_plane_mode;
	bool										m_axis_dragging;
	bool										m_draw_arrow_heads;

	snap_mode									m_snap_mode;
	float3*										m_step_snap_rem;
}; // class transform_control_translation

}// namespace editor_base
}// namespace xray

#endif // #ifndef TRANSFORM_CONTROL_TRANSLATION_H_INCLUDED