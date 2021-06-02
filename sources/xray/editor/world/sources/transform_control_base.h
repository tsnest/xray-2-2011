////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef TRANSFORM_CONTROL_BASE_H_INCLUDED
#define TRANSFORM_CONTROL_BASE_H_INCLUDED

#include "editor_control_base.h"

namespace xray {
namespace collision {

struct engine;
struct space_partitioning_tree;
struct geometry;

}//namespace collision


namespace editor {

ref class level_editor;
ref class object_base;

class collision_object;
class collision_object_transform_control;
ref class command_apply_control_transform;
struct transform_control_base_internal;

typedef vector<collision_object*>		collision_objects_vec;
typedef vector<collision::geometry*>	geometries_vec;

enum enum_transform_axis{
	enum_transform_axis_cam,
	enum_transform_axis_x,
	enum_transform_axis_y,
	enum_transform_axis_z,
	enum_transform_axis_all,
	enum_transform_axis_none,
	enum_transform_axis_COUNT
};

enum enum_transform_mode{
	enum_transform_mode_local,
	enum_transform_mode_control,
};

public ref class transform_control_base abstract: public editor_control_base
{
	typedef editor_control_base super;
protected:
							transform_control_base	( level_editor^ le );
	virtual					~transform_control_base	( );

public:
	virtual		void		set_transform			( float4x4 const& transform );
	virtual		void		update					( ) override;
	virtual		void		show					( bool show ) override;
	virtual		void		set_draw_geomtry		( bool draw ) override;
	virtual		float4x4	create_object_transform	( float4x4 const& object_tranform, enum_transform_mode mode ) = 0;
	virtual		void		activate				( bool bactivate ) override;
	virtual		void		initialize				( ) override;
	virtual		void		destroy					( ) override;
	virtual		void		set_mode_modfiers		( bool plane_mode, bool free_mode ) override;
	virtual		u32			acceptable_collision_type( ) override;

public:
	static		bool		plane_ray_intersect	(	float3 const& plane_origin, 
													float3 const& plane_normal, 
													float3 const& ray_origin, 
													float3 const& ray_direction,
													float3 & intersect );

protected:
	float				calculate_fixed_size_scale	( float3 const& pos, float distance, float4x4 const& inv_view );
	float4x4			calculate_fixed_size_transform( float4x4 const& transform, float distance, float4x4 const& inv_view );
	bool				rey_nearest_point			(	float3 const& origin1, 
														float3 direction1, 
														float3 const& origin2, 
														float3 direction2,
														float3 & intersect );


	void				update_colision_tree();
	float3				get_axis_vector		( enum_transform_axis mode );
	bool				one_covers_another	(	float3 position1, float radius1, 
												float3 position2, float radius2,  
												float4x4 const& inv_view );

protected:
	void			reset_colors			( );

	level_editor^								m_level_editor;
	bool										m_control_visible;
	bool										m_draw_collision_geomtery;
	bool										m_dragging;
	bool										m_preview_mode;

	bool										m_plane_mode_modifier;
	bool										m_free_mode_modifier;
	u32											m_transparent_line_pattern;
	const float									m_cursor_thickness;
	const float									m_last_position_vertex_radius;
	const float									m_distance_from_cam;
	enum_transform_axis							m_active_axis;
	command_apply_control_transform^			m_apply_command;

	float4x4 const&								get_view_transform		();			
	float4x4 const&								get_transform			();

	geometries_vec&								get_geometries			();					
	collision_objects_vec&						get_collision_objects	();					
	color&										get_color				(int idx);			
	color const&								selection_color			();
	color const&								last_position_line_color();
	bool										is_axis_visible			(int idx);			
	void										set_axis_visible		(bool val, int idx);
private:
	transform_control_base_internal*			m_data;
	object_base^								m_aim_object;
}; // class transform_control_base

}// namespace editor
}// namespace xray

		
#endif // #ifndef TRANSFORM_CONTROL_BASE_H_INCLUDED