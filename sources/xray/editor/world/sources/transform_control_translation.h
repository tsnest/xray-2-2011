////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_TRANSLATION_H_INCLUDED
#define TRANSFORM_CONTROL_TRANSLATION_H_INCLUDED

#include "transform_control_base.h"

struct transform_control_translation_internal;

namespace xray {

namespace collision {
	struct engine;
	struct space_partitioning_tree;
	struct geometry;
}//namespace collision


namespace editor {
class collision_object;

ref class transform_control_translation: public transform_control_base
{
	typedef transform_control_base	super;

public:


							transform_control_translation( level_editor^ le );
	virtual					~transform_control_translation( );

	virtual		void		initialize				( ) override;
	virtual		void		destroy					( ) override;
	virtual		void		update					( ) override;
	virtual		void		start_input				( ) override;
	virtual		void		execute_input			( ) override;
	virtual		void		end_input				( ) override;
	virtual		void		collide					( collision_object const* object, float3 const& position ) override;
	virtual		void		draw					( float4x4  const& view_matrix ) override;

	virtual		void		set_transform			( float4x4  const& transform ) override;
	virtual		void		resize					( float size ) override;
	virtual		float4x4	create_object_transform	( float4x4 const& object_tranform, enum_transform_mode mode ) override;

private:
				void		set_active_axis		( enum_transform_axis axis );
				void		set_active_plane	( enum_transform_axis plane );

				float4x4	create_plane_box_rotation	( enum_transform_axis mode, float4x4 const& transform, float4x4 const& view );

				void		update_active_mode	( );
				void		update_colors		( );
				void		update_visibility	( );
				void		draw_last_position	( float4x4 const& view_matrix, bool transparent_mode );
				void		draw_impl			( float4x4  const& view_matrix, bool transparent_mode );
private:
	transform_control_translation_internal*		m_data;

	float										m_arrow_size;
	const float									m_arrow_head_length;
	const float									m_arrow_head_radius;
	const float									m_center_box_radius;
	enum_transform_axis							m_active_plane;
	
	bool										m_initialized;
	bool										m_plane_mode;
	bool										m_axis_dragging;
	bool										m_draw_arrow_heads;

//	float										m_draw_distance_from_cam;

	collision_object_transform_control*			m_center_box;
}; // class transform_control_translation

}// namespace editor
}// namespace xray

#endif // #ifndef TRANSFORM_CONTROL_TRANSLATION_H_INCLUDED