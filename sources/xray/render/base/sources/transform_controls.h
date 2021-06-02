////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.01.2009
//	Author		: Armen Abroyan
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROLS_H_INCLUDED
#define TRANSFORM_CONTROLS_H_INCLUDED

#include <xray/render/base/transform_control.h>
#include "debug_renderer.h"	// Needed for debug_vertices_type, debug_indices_type	

namespace xray {
namespace render {

namespace debug{

struct renderer;

} // namespace debug

namespace editor {
namespace detail {

class renderer;

class transform_control_base: 
	public transform_control,
	private boost::noncopyable 
{
public:
// 	enum enum_mode{ 
// 		enum_mode_translation, 
// 		enum_mode_scale, 
// 		enum_mode_rotation
// 	};

public:
	transform_control_base					( debug::renderer& helper_renderer);
	~transform_control_base					( );

	virtual void	set_transform			( const float4x4& transform );
	virtual const float4x4& get_transform	( );

	virtual void	set_view_matrix			( const float4x4& transform );

protected: 
	float4x4	calculate_fixed_size_transform	( );

protected:

	debug::renderer&    m_helper_renderer;
	float4x4			m_transform;
	float4x4			m_view;
	const u32			m_x_axis_color;
	const u32			m_y_axis_color;
	const u32			m_z_axis_color;
	const u32			m_center_color;
};


class translation_control: 
	public transform_control_base
{
public:
	translation_control		( debug::renderer& helper_renderer);
	~translation_control	( );

	virtual void	draw					( );
	virtual void	draw_collision_object	( );

public:

	enum enum_move_plane{ enum_move_plane_xy, enum_move_plane_yz, enum_move_plane_zx, enum_move_plane_camrea };

private:

	const float									m_arrow_size;
	const float									m_arrow_head_length;
	const float									m_arrow_head_radius;
	enum_move_plane								m_plane_move_mode;

	debug::debug_renderer::debug_vertices_type	m_center_plane_points;
	debug::debug_renderer::debug_indices_type	m_torus_outer_indexes;
};

class scaling_control: 
	public transform_control_base
{
public:
	scaling_control		( debug::renderer& helper_renderer);
	~scaling_control	( );

	void	draw			( );
	virtual void	draw_collision_object	( );

private:
	const float			m_arrow_size;
	const float			m_arrow_head_radius;
};



class rotation_control: 
	public transform_control_base
{

public:
	rotation_control						( debug::renderer& helper_renderer);
	~rotation_control						( );

	void	draw							( );
	virtual void	draw_collision_object	( );

private:
	typedef transform_control_base	super;

private:

	const float			m_radius;
	const float			m_outer_radius;
	const float			m_inner_radius;

	debug::debug_renderer::debug_vertices_type	m_torus_vertexes;
	debug::debug_renderer::debug_indices_type	m_torus_indexes;

	debug::debug_renderer::debug_vertices_type	m_torus_outer_vertexes;
	debug::debug_renderer::debug_indices_type	m_torus_outer_indexes;

	debug::debug_renderer::debug_vertices_type	m_working_vertexes;
};


} // namespace detail
} // namespace editor
} // namespace render
} // namespace xray

#endif // #ifndef TRANSFORM_CONTROLS_H_INCLUDED