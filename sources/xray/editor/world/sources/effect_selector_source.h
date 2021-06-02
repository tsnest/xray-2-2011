////////////////////////////////////////////////////////////////////////////
//	Created		: 04.08.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_SELECTOR_SOURCE_H_INCLUDED
#define EFFECT_SELECTOR_SOURCE_H_INCLUDED

using namespace System::Collections::Generic;

namespace xray 
{
	namespace editor 
	{

		ref class effect_selector_source : controls::tree_view_source
		{
		public:
			effect_selector_source (controls::tree_view^ parent)
			{
				this->parent = parent;
			}

		private:
			controls::tree_view^	m_parent;
			static System::String^	c_source_effects_path = "resources/effects";

		public:
			property bool								is_for_texture;
			property HashSet<System::String^>^			existing;
			
			virtual property	controls::tree_view^	parent
			{
				controls::tree_view^	get	(){return m_parent;}
				void					set	(controls::tree_view^ value){m_parent = value;}
			};

			property	System::String^						effect_stage;
		
		private:
			void	on_fs_iterator_ready	( vfs::vfs_locked_iterator const & fs_it);
			void	process_fs				( vfs::vfs_iterator const & it,  System::String^ file_path);

		public:
			virtual void refresh();

		}; // class effect_selector_source

	} // namespace editor
} // namespace xray

#endif // #ifndef EFFECT_SELECTOR_SOURCE_H_INCLUDED