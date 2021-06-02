////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATIONS_LIST_FILES_SOURCE_H_INCLUDED
#define ANIMATIONS_LIST_FILES_SOURCE_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using xray::editor::controls::tree_view_source;
using xray::editor::controls::tree_view;

namespace xray {
namespace animation_editor {
	ref class animations_list_node_sorter : public System::Collections::IComparer 
	{
	public:
		virtual int Compare	(Object^ x, Object^ y);
	}; // ref class animations_list_node_sorter

	ref class animations_list_files_source : public xray::editor::controls::tree_view_source
	{
	public:
								animations_list_files_source	();
		virtual					~animations_list_files_source	();
		virtual	void			refresh							();
				void			fs_callback						(xray::vfs::vfs_notification const & info);
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
		System::Collections::ArrayList^ m_animations_list;
		file_watcher_delegate*			m_callback;
		u32								m_subscribe_id;
	}; // class animations_list_files_source
}//namespace animation_editor
}//namespace xray
#endif // #ifndef ANIMATIONS_LIST_FILES_SOURCE_H_INCLUDED