////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_SOUND_H_INCLUDED
#define TOOL_SOUND_H_INCLUDED

#include "tool_base.h"
#include <xray/sound/sound.h>

namespace xray {
namespace editor {

using namespace System;

ref class sound_tool_tab;

public ref class tool_sound : public tool_base
{
	typedef tool_base			super;
public:
								tool_sound				( level_editor^ le );
	virtual						~tool_sound				( );

	virtual object_base^		create_library_object_instance	( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual object_base^		load_object				( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual void				destroy_object			( object_base^ o ) override;

	void						load_library			( );
	virtual tool_tab^			ui						( ) override;

	Collections::ArrayList^		single_sound_files		( ) { return m_single_sounds; }
	Collections::ArrayList^		composite_sound_files	( ) { return m_composite_sounds; }
	Collections::ArrayList^		sound_collections_files	( ) { return m_sound_collections; }
	Collections::ArrayList^		volumetric_sound_files	( ) { return m_volumetric_sounds; }

protected:
	virtual void		on_library_loaded		( ) override;
private:
	void			process_recursive_names		( vfs::vfs_iterator const & it, System::String^ path, System::Collections::ArrayList^ file_names, System::String^ extention );
	void			on_single_sounds_library_fs_iterator_ready		( vfs::vfs_locked_iterator const & fs_it );
	void			on_composite_sounds_library_fs_iterator_ready	( vfs::vfs_locked_iterator const & fs_it );
	void			on_sound_collections_library_fs_iterator_ready	( vfs::vfs_locked_iterator const & fs_it );
	void			on_volumetric_sounds_library_fs_iterator_ready	( vfs::vfs_locked_iterator const & fs_it );

	object_base^		create_raw_object		( System::String^ id, render::scene_ptr const& scene );

	sound_tool_tab^						m_tool_tab;
	sound::sound_scene_ptr*				m_sound_scene;
	System::Collections::ArrayList^		m_single_sounds;
	System::Collections::ArrayList^		m_composite_sounds;
	System::Collections::ArrayList^		m_sound_collections;
	System::Collections::ArrayList^		m_volumetric_sounds;

}; // class tool_sound

} // namespace editor
} // namespace xray

#endif // #ifndef TOOL_SOUND_H_INCLUDED
