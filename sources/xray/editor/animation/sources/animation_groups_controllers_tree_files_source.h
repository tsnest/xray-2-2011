////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_GROUPS_CONTROLLERS_TREE_FILES_SOURCE_H_INCLUDED
#define ANIMATION_GROUPS_CONTROLLERS_TREE_FILES_SOURCE_H_INCLUDED

using namespace System;
using xray::editor::controls::tree_view_source;
using xray::editor::controls::tree_view;

namespace xray {
namespace animation_editor {
	public ref class animation_groups_controllers_tree_files_source: public xray::editor::controls::tree_view_source 
	{
	public:
								animation_groups_controllers_tree_files_source();
		virtual					~animation_groups_controllers_tree_files_source();
		virtual	void			refresh							();
				void			fs_callback						(xray::vfs::vfs_notification const& info);
		virtual property		tree_view^		parent
		{
			tree_view^			get								();
			void				set								(tree_view^ value);
		};

	private:
				void	on_fs_iterator_ready			(xray::vfs::vfs_locked_iterator const & fs_it);
				void	process_fs						(xray::vfs::vfs_iterator const & it, String^ file_path);

	private:
		tree_view^						m_parent;
		file_watcher_delegate*			m_callback;
		u32								m_watcher_subscribe_id;
	}; // class animation_groups_controllers_tree_files_source
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_GROUPS_CONTROLLERS_TREE_FILES_SOURCE_H_INCLUDED