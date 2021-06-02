////////////////////////////////////////////////////////////////////////////
//	Created		: 13.01.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_TERRAIN_H_INCLUDED
#define TOOL_TERRAIN_H_INCLUDED

#include "tool_base.h"
#include "terrain_quad.h"

namespace xray {
namespace editor {

ref class terrain_modifier_bump;
ref class terrain_modifier_raise_lower;
ref class terrain_modifier_flatten;
ref class terrain_modifier_smooth;	
ref class terrain_modifier_detail_painter;
ref class terrain_modifier_uv_relax;
ref class terrain_modifier_diffuse_painter;
ref class terrain_tool_tab;
ref class terrain_core;
ref class terrain_selection_control;

public ref class tool_terrain :public tool_base
{
	typedef tool_base	super;
public:
						tool_terrain			( level_editor^ le );
	virtual				~tool_terrain			( );

	virtual void		tick					( ) override;

	virtual object_base^ create_library_object_instance	( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )		override;
	virtual object_base^ load_object			( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )	override;
	virtual void		destroy_object			( object_base^ o )							 override;

	void				load_library			( );
	virtual void		on_new_project			( render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual tool_tab^	ui						( ) override;

	virtual void		load_settings			( RegistryKey^ key )	override;
	virtual void		save_settings			( RegistryKey^ key )	override;
	virtual	void		clear_resources			( )					override;
	
	terrain_core^		get_terrain_core		( );
			void		focus_camera_to			( terrain_node_key^ k );
private:
			object_base^ create_raw_object		( System::String^ id, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view );
	terrain_tool_tab^							m_tool_tab;
	terrain_core^								m_terrain_core;
public: // tmp
	terrain_modifier_bump^				m_terrain_modifier_bump;
	terrain_modifier_raise_lower^		m_terrain_modifier_raise_lower;
	terrain_modifier_flatten^			m_terrain_modifier_flatten;
	terrain_modifier_smooth^			m_terrain_modifier_smooth;
	terrain_modifier_detail_painter^	m_terrain_modifier_detail_painter;
	terrain_modifier_uv_relax^			m_terrain_modifier_uv_relax;
	terrain_modifier_diffuse_painter^	m_terrain_modifier_diffuse_painter;
	terrain_selection_control^			m_terrain_export_control;
}; // class tool_terrain

} // namespace editor
} // namespace xray

#endif // #ifndef TOOL_TERRAIN_H_INCLUDED