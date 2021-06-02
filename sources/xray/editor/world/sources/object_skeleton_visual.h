////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_SKELETON_VISUAL_H_INCLUDED
#define OBJECT_SKELETON_VISUAL_H_INCLUDED

#include "object_base.h"
#include <xray/render/world.h>
#include <xray/render/facade/model.h>

namespace xray {
namespace editor {

ref class tool_solid_visual;

ref class object_skeleton_visual : public object_base
{
	typedef object_base		super;
public:
					object_skeleton_visual	( tool_solid_visual^ t, render::scene_ptr const& scene );
	virtual			~object_skeleton_visual	( );
	virtual void	destroy_collision		( )										override;

	virtual void	save					( configs::lua_config_value t )			override;
	virtual	void	set_transform			( float4x4 const& transform )			override;
	virtual	void	on_selected				( bool selected )						override;
	virtual	void	set_visible				( bool bvisible )						override;
	virtual enum_terrain_interaction get_terrain_interaction( )						override	{return enum_terrain_interaction::tr_vertical_snap;}

	virtual void	load_contents			( )	override;
	virtual void	unload_contents			( bool bdestroy ) override;

protected:
			void	initialize_collision	( );
			void	update_bbox				( );

protected:
	void			on_resource_loaded		( xray::resources::queries_result& data );

public:
	render::skeleton_model_ptr*	m_model_instance;
	render::scene_ptr*			m_scene;
	tool_solid_visual^			m_tool_solid_visual;
}; // class object_solid_visual

} // namespace editor
} // namespace xray


#endif // #ifndef OBJECT_SKELETON_VISUAL_H_INCLUDED
