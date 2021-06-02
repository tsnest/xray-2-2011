////////////////////////////////////////////////////////////////////////////
//	Created		: 22.10.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_MISC_H_INCLUDED
#define TOOL_MISC_H_INCLUDED

#include "tool_base.h"
#include <xray/render/engine/base_classes.h>

using namespace System;
using namespace System::Collections::Generic;

namespace xray {
namespace editor {

ref class misc_tool_tab;
ref class object_collision_geometry_set_plane_action;

public ref class tool_misc : public tool_base
{
	typedef tool_base	super;

public:
						tool_misc					( level_editor^ le );
	virtual				~tool_misc					( );

private:
	misc_tool_tab^										m_tool_tab;

	object_collision_geometry_set_plane_action^			m_collision_geometry_set_plane_action;

public:
	property object_collision_geometry_set_plane_action^	collision_geometry_set_plane_action
	{
		object_collision_geometry_set_plane_action^	get( ) { return m_collision_geometry_set_plane_action; }
	}

public:
	virtual object_base^	create_library_object_instance	( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual object_base^	load_object						( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual void			destroy_object					( object_base^ o ) override;
	virtual void			on_new_project					( render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )	override;
	void					load_library					( );
	virtual tool_tab^		ui								( ) override;

	List< String^ >^		animated_model_instances_names	( ) { return m_animated_model_names; }

protected:
	virtual void			on_library_loaded				( ) override;

private:
	void					load_animated_models_names		( );
	void				on_animated_models_fs_iterator_ready( vfs::vfs_locked_iterator const& fs_it );
	void					process_recursive_names			( vfs::vfs_iterator const& in_it );
	object_base^			create_raw_object				( System::String^ id, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view );

private:
	List< String^ >^		m_animated_model_names;
}; // class tool_misc

} // namespace editor
} // namespace xray

#endif // #ifndef TOOL_MISC_H_INCLUDED