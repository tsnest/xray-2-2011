////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_SPEEDTREE_H_INCLUDED
#define TOOL_SPEEDTREE_H_INCLUDED

#include "tool_base.h"

namespace xray {
namespace editor {

ref class solid_visual_tool_tab;

public ref class tool_speedtree :public tool_base
{
	typedef tool_base	super;
public:
						tool_speedtree					( level_editor^ le );
	virtual				~tool_speedtree					( );
	
	virtual object_base^ load_object					( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual object_base^ create_library_object_instance	( System::String^ name, 
														  render::scene_ptr const& scene, 
														  render::scene_view_ptr const& scene_view 
														 ) override;
	virtual void		destroy_object					( object_base^ o ) override;
	virtual tool_tab^	ui								( ) override;

	virtual void		item_double_clicked				( lib_item_base^ item ) override;
	void				load_library					( );
	virtual void		initialize						( ) override;
private:
	object_base^		create_raw_object				( System::String^ id, render::scene_ptr const& scene );
	void				process_recursive_names			( vfs::vfs_iterator const & it, System::String^	path );
	void				on_library_fs_iterator_ready	( vfs::vfs_locked_iterator const & fs_it );

	solid_visual_tool_tab^	m_tool_tab;
}; // class tool_speedtree

} // namespace editor
} // namespace xray


#endif // #ifndef TOOL_SPEEDTREE_H_INCLUDED