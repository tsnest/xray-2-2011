////////////////////////////////////////////////////////////////////////////
//	Created		: 04.10.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_WIRE_SET_H_INCLUDED
#define OBJECT_WIRE_SET_H_INCLUDED

#include "object_base.h"
#include <xray/render/facade/model.h>

using xray::editor::wpf_controls::control_containers::button;

namespace xray {

namespace physics{ class bt_soft_body_rope; }

namespace editor {

class collision_object_wire_point;
ref class tool_misc;
ref class object_wire_set;

ref struct wire_item: public editor_base::transform_control_object
{
	typedef editor_base::transform_control_object super;
	typedef DisplayNameAttribute DisplayNameAttribute;

					wire_item		( object_wire_set^ parent );
					~wire_item		( );

	float3*			m_offset1;


	[DisplayNameAttribute("stiftness"), CategoryAttribute("simulation settings")] 
	property float stiftness;

	[DisplayNameAttribute("iterations"), CategoryAttribute("simulation settings")] 
	property int iterations;

	[DisplayNameAttribute("fragments"), CategoryAttribute("simulation settings")] 
	property int fragments;

	[DisplayNameAttribute("margin"), CategoryAttribute("simulation settings")] 
	property float margin;

	[DisplayNameAttribute("timescale"), CategoryAttribute("simulation settings")] 
	property float timescale;

	[DisplayNameAttribute("wire width"), CategoryAttribute("visual")] 
	property float wire_width;

	physics::bt_soft_body_rope*	m_physic_body;
	object_wire_set^			m_parent;
	int							m_selected_point;
	float3*						m_captured_points;
	u32							m_captured_points_count;
	float3						current_point			( );

//transform_control_object
	float4x4*						m_start_transform;
	virtual void					start_modify			( editor_base::transform_control_base^ control ) override;
	virtual void					execute_preview			( editor_base::transform_control_base^ control ) override;
	virtual void					end_modify				( editor_base::transform_control_base^ control ) override;
	virtual float4x4				get_ancor_transform		( ) override;
	virtual u32						get_collision			( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% ) override;
};

ref class object_wire_set : public object_base
{
	typedef object_base super;
public:
						object_wire_set			( tool_misc^ tool );
						~object_wire_set		( );
	virtual void		load_contents			( )										override;
	virtual void		unload_contents			( bool )								override;
	virtual void		save					( configs::lua_config_value t )			override;
	virtual void		load_props				( configs::lua_config_value const& t )	override;
	virtual void		render					( )										override;
	virtual	void		set_visible				( bool bvisible )						override;
	virtual	void		destroy_collision		( )										override;
	virtual	void		set_transform			( float4x4 const& transform )			override;
	virtual aabb		get_focus_aabb			( )										override;

	virtual wpf_controls::property_container^	get_property_container	( )				override;

public:
	void				rebuild					( button^ );
	void				capture					( button^ );
	void				update_collision		( );
	void				update_bbox				( );
	void				on_visual_loaded		( resources::queries_result& data );
	void				query_create_visual		( );
			wire_item^				m_item;
			tool_misc^				m_tool_misc;

	collision_object_wire_point*	m_start_point_collision;
	collision_object_wire_point*	m_end_point_collision;

	render::render_model_instance_ptr*			m_visual;
}; // class object_wire_set

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_WIRE_SET_H_INCLUDED