////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_SCALING_H_INCLUDED
#define TRANSFORM_CONTROL_SCALING_H_INCLUDED

#include "transform_control_base.h"

struct transform_control_scaling_internal;

namespace xray {

namespace collision {

struct engine;
struct space_partitioning_tree;
struct geometry;

}//namespace collision


namespace editor {

class editor_world;
class collision_object;

public ref class transform_control_scaling : public transform_control_base
{

	typedef transform_control_base	super;

public:


	transform_control_scaling						( level_editor^ le);
	~transform_control_scaling						( );

	virtual		void		initialize				( ) override;
	virtual		void		destroy					( ) override;
	virtual		void		start_input				( ) override;
	virtual		void		execute_input			( ) override;
	virtual		void		end_input				( ) override;
	virtual		void		collide					( collision_object const* object, float3 const& position ) override;
	virtual		void		draw					( math::float4x4 const& view_matrix ) override;

	virtual		void		set_transform			( math::float4x4 const& transform ) override;
	virtual		void		resize					( float size ) override;
	virtual		math::float4x4	create_object_transform	( math::float4x4 const& object_tranform, enum_transform_mode mode ) override;

private:

	void					set_active_axis			( enum_transform_axis axis );
	void					update_visibility		( );
	void					draw_impl				( math::float4x4 const& view_matrix, bool transparent_mode  );
	void					draw_last_position		( xray::math::float4x4 const& view_matrix, bool transparent_mode );

private:
	transform_control_scaling_internal*			m_data;		
	float										m_arrow_length;
	const float									m_arrow_head_radius;
	const float									m_center_box_drag_factor;

	bool										m_initialized;
	bool										m_plane_mode;
}; // class transform_control_scaling

}//namespace editor
}//namespace xray

		

#endif // #ifndef TRANSFORM_CONTROL_SCALING_H_INCLUDED