////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef TREE_VIEW_FILES_SOURCE_H_INCLUDED
#define TREE_VIEW_FILES_SOURCE_H_INCLUDED

using namespace System;
using xray::editor::controls::tree_view_source;
using xray::editor::controls::tree_view;

namespace xray {
namespace sound_editor {
	ref class tree_view_files_node_sorter : public System::Collections::IComparer 
	{
	public:
		virtual int Compare	(Object^ x, Object^ y);
	}; // ref class tree_view_files_node_sorter

	public enum class tree_view_files_source_mode
	{
		show_single			= 1,
		show_composite		= 2,
		show_collections	= 3,
		show_all			= 4
	}; // enum class tree_view_files_source_mode

	public ref class tree_view_files_source: public xray::editor::controls::tree_view_source 
	{
	public:
								tree_view_files_source		(System::Windows::Forms::ContextMenuStrip^ context_menu);
		virtual					~tree_view_files_source		();
		virtual	void			refresh						();
				void			fs_callback					(xray::vfs::vfs_notification const& info);
		virtual property		tree_view^		parent
		{
			tree_view^			get							();
			void				set							(tree_view^ value);
		};
		property tree_view_files_source_mode mode;
		property bool do_not_refresh;

	private:
				void			on_fs_iterator_ready		(vfs::vfs_locked_iterator const &  fs_it);
				void			process_fs					(vfs::vfs_iterator const & it, String^ file_path);

	private:
		tree_view^						m_parent;
		file_watcher_delegate*			m_callback;
		System::Windows::Forms::ContextMenuStrip^ m_context_menu;
		u32								m_watcher_subscriber_id;
	}; // class tree_view_files_source
} // namespace sound_editor
} // namespace xray
#endif // #ifndef TREE_VIEW_FILES_SOURCE_H_INCLUDED