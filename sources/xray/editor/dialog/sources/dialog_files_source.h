////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_FILES_SOURCE_H_INCLUDED
#define DIALOG_FILES_SOURCE_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using xray::editor::controls::tree_view_source;
using xray::editor::controls::tree_view;

namespace xray {
namespace dialog_editor {
	ref class dialogs_node_sorter : public System::Collections::IComparer 
	{
	public:
		virtual int Compare	(Object^ x, Object^ y);
	}; // ref class dialogs_node_sorter

	ref class dialog_files_source : public xray::editor::controls::tree_view_source
	{
	public:
								dialog_files_source	();
		virtual					~dialog_files_source();
		virtual	void			refresh				();
		virtual property	tree_view^				parent
		{
			tree_view^			get					();
			void				set					(tree_view^ value);
		};
				void			fs_callback			(xray::vfs::vfs_notification const & info);
		
	private:
				void			on_fs_iterator_ready(xray::vfs::vfs_locked_iterator const & fs_it);
				void			process_fs			(xray::vfs::vfs_iterator const & it,  String^ file_path);

	private:
		tree_view^				m_parent;
		file_watcher_delegate*	m_callback;
		u32						m_watcher_subscriber_id;
	}; // class raw_files_source
}//namespace dialog_editor
}//namespace xray
#endif // #ifndef DIALOG_FILES_SOURCE_H_INCLUDED