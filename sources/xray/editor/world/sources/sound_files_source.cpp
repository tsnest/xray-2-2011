////////////////////////////////////////////////////////////////////////////
//	Created		: 15.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_files_source.h"
#include <xray/editor/base/managed_delegate.h>
#include "sound_editor.h"

using namespace System::IO;

using xray::editor::sound_editor;

namespace xray
{
	namespace editor
	{
		tree_view^			sound_files_source::parent::get			()
		{
			return m_parent;
		}

		void				sound_files_source::parent::set			(tree_view^ value)
		{
			m_parent = value;
		}
	
		void				sound_files_source::refresh				()
		{
			fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &sound_files_source::on_fs_iterator_ready));
			resources::query_fs_iterator(	unmanaged_string(sound_editor::sound_resources_path+"sources").c_str(), 
				boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator);
		}

		void				sound_files_source::on_fs_iterator_ready	(xray::resources::fs_iterator fs_it)
		{
			process_fs(fs_it.children_begin(), "");
		}

		void				sound_files_source::process_fs			(xray::resources::fs_iterator it, String^ file_path)
		{
			while( !it.is_end() )
			{
				if(it.is_folder())
				{
					String^ folder_name = gcnew String(it.get_name());
					m_parent->add_group(file_path+folder_name);
					process_fs	(it.children_begin(), file_path+folder_name+"/");
				}
				else
				{
					if(Path::GetExtension(gcnew String(it.get_name())) != ".options")
					{
						String^ file_name = Path::GetFileNameWithoutExtension(gcnew String(it.get_name()));

						m_parent->add_item(file_path+file_name);
					}
				}
				++it;
			}
		}
	}
}