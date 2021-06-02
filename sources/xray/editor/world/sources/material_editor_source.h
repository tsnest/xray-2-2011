////////////////////////////////////////////////////////////////////////////
//	Created		: 23.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_EDITOR_SOURCE_H_INCLUDED
#define MATERIAL_EDITOR_SOURCE_H_INCLUDED

#include "project_defines.h"

namespace xray
{
	namespace editor
	{

		ref class material_editor_source: public controls::tree_view_source
		{
		public:
			material_editor_source( System::String^ source_path, System::String^ source_ext )
			:m_source_path(source_path),
			m_source_ext(source_ext)
			{
			}
		
		private:
			controls::tree_view^	m_parent;
			System::String^			m_source_path;
			System::String^			m_source_ext;

		public:
			string_list				m_items;

			virtual property	controls::tree_view^		parent
			{
				controls::tree_view^	get	( )								{ return m_parent; }
				void					set	( controls::tree_view^ value)	{ m_parent = value; }
			};
		
		private:
			void	on_fs_iterator_ready	( vfs::vfs_locked_iterator const & fs_it );
			void	process_fs				( vfs::vfs_iterator const & it, System::String^ file_path );

		public:
			virtual void refresh		( );


		}; // class texture_editor_source

	} // namespace editor
} // namespace xray

#endif // #ifndef MATERIAL_EDITOR_SOURCE_H_INCLUDED