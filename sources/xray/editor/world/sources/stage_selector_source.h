////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SELECTOR_SOURCE_H_INCLUDED
#define STAGE_SELECTOR_SOURCE_H_INCLUDED

namespace xray
{
	namespace editor
	{
		ref class stage_selector_source : controls::tree_view_source
		{
		public:
			stage_selector_source (controls::tree_view^ parent)
			{
				this->parent = parent;
			}

		private:
			controls::tree_view^	m_parent;
			static System::String^			c_source_effects_path = "resources/stages";

		public:
			virtual property	controls::tree_view^		parent
			{
				controls::tree_view^	get	(){return m_parent;}
				void					set	(controls::tree_view^ value){m_parent = value;}
			};
			property			System::Collections::Generic::ICollection<System::String^>^		existing_items;
		
		private:
			void	on_fs_iterator_ready	(xray::vfs::vfs_locked_iterator const & fs_it);
			void	process_fs				(xray::vfs::vfs_iterator const & it,  System::String^ file_path);

		public:
			virtual void refresh();

		}; // class stage_selector_source

	} // namespace editor
} // namespace xray

#endif // #ifndef STAGE_SELECTOR_SOURCE_H_INCLUDED