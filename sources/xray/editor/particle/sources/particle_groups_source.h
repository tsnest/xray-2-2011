////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_GROUPS_SOURCE_H_INCLUDED
#define PARTICLE_GROUPS_SOURCE_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;

using xray::editor::controls::tree_view_source;
using xray::editor::controls::tree_view;

namespace xray
{
	namespace particle_editor
	{
		ref class particle_graph_node;
		ref class xray::editor::controls::tree_node;
		ref class particle_editor;

		ref class particle_groups_source : tree_view_source
		{
		public:		
			particle_groups_source(particle_editor^ editor){
				m_parent_editor			= editor;
				m_node_to_refresh_path	= "";
			}

		private:
			tree_view^						m_parent;
			particle_editor^				m_parent_editor;
			String^							m_node_to_refresh_path;

		public:
			virtual property tree_view^		parent
			{
				tree_view^			get	(){return m_parent;};
				void				set	(tree_view^ value){m_parent = value;};
			};
			
		private:
			void	on_fs_iterator_ready	(vfs::vfs_locked_iterator const &  fs_it);
			void	process_fs				(vfs::vfs_iterator const &  it,  String^ file_path);
			
		public:
			virtual void refresh();
					void refresh(String^ path);

		}; // class particle_groups_source
	}//namespace particle_editor
}//namespace xray

#endif // #ifndef PARTICLE_GROUPS_SOURCE_H_INCLUDED