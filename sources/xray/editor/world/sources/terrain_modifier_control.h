////////////////////////////////////////////////////////////////////////////
//	Created		: 14.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_MODIFIER_CONTROL_H_INCLUDED
#define TERRAIN_MODIFIER_CONTROL_H_INCLUDED

#include "terrain_quad.h"
#include <xray/render/facade/editor_renderer.h>
#include "attribute.h"

namespace xray {
namespace collision{ class object;}
namespace editor {

class collision_object_dynamic;
ref class level_editor;
ref class tool_terrain;
ref class terrain_control_cursor;
ref class terrain_modifier_command;

public ref class terrain_control_base abstract: public editor_base::editor_control_base
{
	typedef editor_base::editor_control_base super;
public:
								terrain_control_base	(level_editor^ le, tool_terrain^ tool);
	virtual						~terrain_control_base	();
	virtual		void			activate				( bool b_activate )			override;
	virtual		void			update					( ) 						override;
	virtual		void			start_input				( int mode, int button, xray::collision::object const* coll, math::float3 const& pos )	override;
	virtual		void			end_input				( ) 						override;
	virtual		void			draw					( render::scene_view_pointer scene_view, render::render_output_window_pointer output_window ) override;
	virtual		u32				acceptable_collision_type( )						override;
	virtual		void			do_work					( )	= 0;
	virtual		void			change_property			( System::String^ prop_name, float const dx, float const dy ) override;
	virtual		void			load_settings			( RegistryKey^ key )		override;
	virtual		void			save_settings			( RegistryKey^ key )		override;
	virtual		void			pin						( bool bpin )				override;
	virtual		bool			has_properties			( )							override {return true;}
				float3			get_picked_point		( );
public:
	property modifier_shape		shape_type;

	[DisplayNameAttribute("radius"), DescriptionAttribute("tool radius"), CategoryAttribute("general")]
	property float				radius
	{
		float	get()			{return m_radius_property;}
		void	set(float v)	{m_radius_property = math::clamp_r(v, 0.5f, 50.0f);}
	}

	[DisplayNameAttribute("affect objects snap"), DescriptionAttribute(""), CategoryAttribute("general")]
	property bool	affect_objects_snap;

	level_editor^				m_level_editor;
	tool_terrain^				m_tool_terrain;

protected:
	virtual		void			initialize				( ) 						override;
	virtual		void			destroy					( ) 						override;
	float						m_radius_property;
	terrain_modifier_command^	m_apply_command;
	
	key_vert_id_dict			m_active_id_list;

	System::Drawing::Point		m_last_screen_xy;
//	math::int2					get_mouse_position			();

	float3*						m_center_position_global;

	bool						m_visible;
	terrain_control_cursor^		m_terrain_cursor;
	
	virtual		void			on_working_set_changed		( ) {};
	virtual		void			create_command				( )	= 0;
	virtual		void			create_cursor				( render::scene_ptr const& scene )	= 0;
	virtual		void			refresh_active_working_set	( modifier_shape type ) = 0;
}; // class terrain_modifier_control_base

public ref class terrain_modifier_control_base	abstract : public terrain_control_base
{
	typedef terrain_control_base	super;
public:
								terrain_modifier_control_base	( level_editor^ le, tool_terrain^ tool );
	virtual						~terrain_modifier_control_base	( );
	virtual		void			load_settings			( RegistryKey^ key )		override;
	virtual		void			save_settings			( RegistryKey^ key )		override;

	[DisplayNameAttribute("hardness"), DescriptionAttribute("tool hardness"), CategoryAttribute("general")]
	property float				hardness
	{
		float	get()			{return m_hardness_property;}
		void	set(float v)	{m_hardness_property = math::clamp_r(v, 0.1f, 1.0f);}
	}
	[DisplayNameAttribute("power"), DescriptionAttribute("tool power"), CategoryAttribute("general")]
	property float				power
	{
		float	get()			{return m_power_property;}
		void	set(float v)	{m_power_property = math::clamp_r(v, m_min_power, m_max_power);}
	}

	float						power_value					( );
	virtual		void			end_input					( ) 						override;
	virtual		void			change_property				( System::String^ prop_name, float const dx, float const dy ) override;
protected:
	float						calc_influence_factor		( float const& distance );
	virtual		void			refresh_active_working_set	( modifier_shape type ) override;

	float						m_power_property;
	float						m_hardness_property;
	float						m_min_power;
	float						m_max_power;
	key_vert_id_dict			m_processed_id_list;

}; // terrain_modifier_control_base

public ref class terrain_modifier_height abstract: public terrain_modifier_control_base
{
	typedef terrain_modifier_control_base	super;
public:
								terrain_modifier_height		( level_editor^ le, tool_terrain^ tool );
	virtual						~terrain_modifier_height	( );
	virtual		void			start_input					( int mode, int button, collision::object const* coll, math::float3 const& pos )	override;
	virtual		void			end_input					( ) override;
	virtual		void			change_property				( System::String^ prop_name, float const dx, float const dy ) override;

protected:
	virtual		void			on_working_set_changed		( ) override;
	virtual		void			create_command				( ) override;
	virtual		void			create_cursor				( render::scene_ptr const& scene ) override;
	key_height_diff_dict		m_processed_values_list;
	key_height_diff_dict		m_stored_values_list;

protected: // smooth support
				void			do_smooth();
			timing::timer*		m_smooth_timer;

				void			init_smooth		( );
				bool			m_smooth_mode;
	cli::array<float, 2>^		m_gaussian_matrix;
				int				m_smooth_matrix_dim;
}; // class terrain_modifier_height



} // namespace editor
} // namespace xray

#endif // #ifndef TERRAIN_MODIFIER_CONTROL_H_INCLUDED