////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_ANIMATIONS_H_INCLUDED
#define TOOL_ANIMATIONS_H_INCLUDED

#include "tool_base.h"

namespace xray {
namespace editor {

public ref class tool_animations : public tool_base
{
	typedef tool_base	super;

public:
							tool_animations					( level_editor^ le );
	virtual					~tool_animations				( );

	virtual object_base^	create_library_object_instance	( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual object_base^	load_object						( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual void			destroy_object					( object_base^ o ) override;

	virtual tool_tab^		ui								( ) override;

protected:
	void					load_library					( );

private:
	void					on_fs_iterator_ready			( vfs::vfs_locked_iterator const & fs_it );
	void					process_recursive_names			( vfs::vfs_iterator const & it, System::String^ path );
}; // class tool_animations

} // namespace editor
} // namespace xray

#endif // #ifndef TOOL_ANIMATIONS_H_INCLUDED