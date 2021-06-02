////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_SOLID_VISUAL_H_INCLUDED
#define OBJECT_SOLID_VISUAL_H_INCLUDED

#include "object_base.h"

#pragma managed( push, off )
#include <xray/render/facade/model.h>
#include <xray/physics/model.h>
#include <xray/physics/rigid_body.h>
#pragma managed( pop )

using xray::editor::wpf_controls::control_containers::button;

namespace xray {
namespace editor {

ref class tool_solid_visual;

ref class object_solid_visual : public object_base
{
	typedef object_base		super;
public:
					object_solid_visual		( tool_solid_visual^ t, render::scene_ptr const& scene );
	virtual			~object_solid_visual	( );
	virtual void	destroy_collision		( )										override;

	virtual void	save					( configs::lua_config_value t )			override;
	virtual void	load_props				( configs::lua_config_value const& t )	override;
	virtual	void	set_transform			( float4x4 const& transform )			override;
	virtual	void	on_selected				( bool selected )						override;
	virtual	void	set_visible				( bool bvisible )						override;
	virtual enum_terrain_interaction get_terrain_interaction( )						override;
	virtual wpf_controls::property_container^ get_property_container( )				override;
	virtual void	set_library_name		( System::String^ name )				override;
	virtual void	render					( )										override;

	virtual void	load_contents			( )	override;
	virtual void	unload_contents			( bool bdestroy ) override;

protected:
	void			tick					( );
	void			activate_physics_shell	( );
	void			deactivate_physics_shell( );
	void			initialize_collision	( );
	void			on_model_loaded			( resources::queries_result& data );

	void			on_physic_loaded		( resources::queries_result& data );
	void			edit_in_model_assembler	( button^);
	bool			m_snap_to_terrain;
	bool			m_ghost_mode;
	bool			m_physic_mode_now;

	physics::model_type	m_physics_model_type;

	// bullet physics testing
	physics::bt_collision_shape_ptr*	m_collision_shape;
	physics::bt_rigid_body*				m_rigid_body;

public:
	render::static_model_ptr*	m_model_instance;
	collision::geometry_ptr*	m_hq_collision;
	render::scene_ptr*			m_scene;
	tool_solid_visual^			m_tool_solid_visual;
	query_result_delegate*		m_model_query;


	[DisplayNameAttribute("terrain snap"), DescriptionAttribute("terrain snap interaction"), CategoryAttribute("general"), 
	ReadOnlyAttribute(false)]
	property bool	terrain_snap{
		bool			get			( )				{ return m_snap_to_terrain; }
		void			set			( bool v )		{ m_snap_to_terrain=v; }
	}

	[DisplayNameAttribute("ghost"), DescriptionAttribute("ghost mode"), CategoryAttribute("general"), 
	ReadOnlyAttribute(false)]
	property bool	ghost_mode{
		bool			get			( )				{ return m_ghost_mode; }
		void			set			( bool v );
	}

	[ DisplayNameAttribute("type"), DescriptionAttribute("physics type"), CategoryAttribute("physics") ]
	property int			physics_type{
		int					get		( )				{ return (int)m_physics_model_type; }
		void				set		( int t )		{ m_physics_model_type = physics::model_type ( t ); };
	}

	[DisplayNameAttribute("physic"), DescriptionAttribute("physic"), CategoryAttribute("general"), 
	ReadOnlyAttribute(false)]
	property bool	physic_mode{
		bool			get			( )				{ return m_physic_mode_now; }
		void			set			( bool v );
	}

}; // class object_solid_visual

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_SOLID_VISUAL_H_INCLUDED
