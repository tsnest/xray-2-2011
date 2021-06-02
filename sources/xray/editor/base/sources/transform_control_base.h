////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef TRANSFORM_CONTROL_BASE_H_INCLUDED
#define TRANSFORM_CONTROL_BASE_H_INCLUDED

#include "editor_control_base.h"
#include <xray/editor/base/transform_control_defines.h>

namespace xray {
namespace collision{ class object; class geometry; }

namespace editor_base {

ref class transform_control_object;
class collision_object_transform_control;
struct transform_control_base_internal;

typedef vector<collision_object_transform_control*>		collision_objects_vec;
typedef vector<collision::geometry*>					geometries_vec;

public enum class snap_mode{no_snap, vertex_snap, face_snap, edge_snap, step_snap};


public ref class transform_control_helper abstract
{
public:
			transform_control_helper(
			render::scene_pointer					scene,
			render::scene_view_pointer				scene_view,
			render::render_output_window_pointer	render_output_window
		):
		m_changed				( true ),
		m_collision_tree		( NULL ),
		m_scene					( scene ),
		m_scene_view			( scene_view ),
		m_render_output_window	( render_output_window )
	{
		R_ASSERT				( scene );
		R_ASSERT				( scene_view );
		R_ASSERT				( render_output_window );
	}

	virtual float4x4	get_inverted_view_matrix	( )						= 0;
	virtual float4x4	get_projection_matrix		( )						= 0;

	virtual void		get_mouse_ray				( float3& origin, float3& direction) = 0;
	bool				empty						( ) { return m_object==nullptr;}

	transform_control_object^				m_object;
	collision::space_partitioning_tree*		m_collision_tree;
	render::editor::renderer*				m_editor_renderer;
	render::scene_pointer					m_scene;
	render::scene_view_pointer				m_scene_view;
	render::render_output_window_pointer	m_render_output_window;
	bool									m_changed;
}; // class transform_control_helper

public ref class transform_control_base abstract: public editor_control_base
{
	typedef editor_control_base super;
protected:
							transform_control_base	( transform_control_helper^ helper );
	virtual					~transform_control_base	( );

public:
	virtual		void		start_input				( int mode, int button, collision::object const* coll, math::float3 const& pos ) override;
	virtual		void		execute_input			( ) override;
	virtual		void		end_input				( ) override;
	virtual		void		set_matrix				( float4x4 const& m );
	virtual		void		update					( ) override;
				void		show_					( bool show );

	virtual		void		activate				( bool bactivate ) override;
	virtual		u32			acceptable_collision_type( ) override;

	virtual float4x4		calculate				( float4x4 const& m )			= 0;
	static		bool		plane_ray_intersect		(
								float3 const& plane_origin, 
								float3 const& plane_normal, 
								float3 const& ray_origin, 
								float3 const& ray_direction,
								float3 & intersect
							);
	transform_control_helper^					helper					( ) { return m_helper; }

	virtual void			one_step_execute	( enum_transform_axis axis, float value ) = 0;
	static float			gizmo_size	= 0.8f;
protected:
	virtual		void		initialize				( ) override;
	virtual		void		destroy					( ) override;
	xray::render::editor::renderer&	get_editor_renderer( );

				void		set_mode_modfiers		( bool plane_mode, bool free_mode );


	float				calculate_fixed_size_scale		( float3 const& pos );
	float4x4			calculate_fixed_size_transform	( float4x4 const& transform );

	bool				ray_nearest_point			(	float3 const& origin1, 
														float3 const& direction1, 
														float3 const& origin2, 
														float3 const& direction2,
														float3& intersect_point );


	float3				get_axis_vector				( enum_transform_axis mode );
	bool				one_covers_another			(	float3 position1, float radius1, 
														float3 position2, float radius2 );

protected:
	bool										m_shown;
	bool										m_dragging;
	bool										m_preview_mode;

	bool										m_plane_mode_modifier;
	bool										m_free_mode_modifier;

	u32											m_transparent_line_pattern;
	float										m_cursor_thickness;
	const float									m_last_position_vertex_radius;
	enum_transform_axis							m_active_axis;
	enum_transform_axis							m_picked_axis;

	float4x4 const&								get_inv_view			( );			
	float4x4 const&								get_matrix				( );

	geometries_vec&								get_geometries			( );					
	collision_objects_vec&						get_collision_objects	( );					
	color const&								get_color				( int idx );
	color const&								selection_color			( );
	color const&								last_position_line_color( );
	bool										is_axis_visible			( int idx );
	void										set_axis_visible		( bool val, int idx );

protected:
	transform_control_helper^					m_helper;
	math::float4x4*								m_result;
	render::viewport_type*						m_viewport;

	enum_transform_axis							pick_axis				( );

private:
	transform_control_base_internal*			m_data;
}; // class transform_control_base

}// namespace editor
}// namespace xray

		
#endif // #ifndef TRANSFORM_CONTROL_BASE_H_INCLUDED