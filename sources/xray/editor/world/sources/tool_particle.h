////////////////////////////////////////////////////////////////////////////
//	Created		: 19.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_PATRICLE_H_INCLUDED
#define TOOL_PATRICLE_H_INCLUDED

#include "tool_base.h"

namespace xray {
namespace editor {

ref class solid_visual_tool_tab;

public ref class tool_particle :public tool_base
{
	typedef tool_base	super;
public:
						tool_particle			( level_editor^ le );
	virtual				~tool_particle			( );

	virtual object_base^ load_object			( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual object_base^ create_library_object_instance	( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual void		destroy_object			( object_base^ o ) override;
	virtual tool_tab^	ui						( ) override;

	void				load_library			( );
	virtual void		initialize				( ) override;
private:
	object_base^		create_raw_object			( System::String^ id, render::scene_ptr const& scene );
	void				process_recursive_names		( vfs::vfs_iterator const & it, System::String^	path );
	void				on_library_fs_iterator_ready( vfs::vfs_locked_iterator const & fs_it );

	solid_visual_tool_tab^	m_tool_tab;
}; // class tool_solid_visual

} // namespace editor
} // namespace xray


#endif // #ifndef TOOL_PATRICLE_H_INCLUDED