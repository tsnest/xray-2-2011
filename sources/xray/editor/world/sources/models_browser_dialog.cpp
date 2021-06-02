////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"

#include "models_browser_dialog.h"

namespace xray {
namespace editor {

void models_browser_dialog::build_tree	()
{
	fs_iterator_delegate* q = NEW( fs_iterator_delegate )( gcnew fs_iterator_delegate::Delegate( this, &models_browser_dialog::on_fs_iterator_ready ), g_allocator );
	resources::query_fs_iterator(
		"resources/models/actors", 
		boost::bind( &fs_iterator_delegate::callback, q, _1 ),
		g_allocator,
		resources::is_recursive_true 
	);
}

void models_browser_dialog::on_fs_iterator_ready( resources::fs_iterator fs_it )
{
	trv_models->Nodes->Clear	( );
	trv_models->Nodes->Add		( "actors/" );
	
	process_fs					( fs_it.children_begin(), "actors/" );
	trv_models->ExpandAll		( );
}

void models_browser_dialog::process_fs( resources::fs_iterator it, String^ file_path )
{
	XRAY_UNREFERENCED_PARAMETERS( it, file_path );
	/*
	while ( !it.is_end() )
	{
		if ( it.is_folder() )
		{
			String^ folder_name = gcnew String( it.get_name() );
			trv_models->Nodes-> (file_path+folder_name, editor_base::folder_closed, editor_base::folder_open);
			process_fs( it.children_begin(), file_path+folder_name+"/" );
		}
		else
		{
			if ( System::IO::Path::GetExtension( gcnew String( it.get_name() ) ) == ".model" )
			{
				String^ file_name = System::IO::Path::GetFileNameWithoutExtension( gcnew String( it.get_name() ) );
				m_parent->add_item(file_path+file_name, editor_base::node_resource);
				System::String^ anim_name = (file_path+file_name)->Remove(0, 11);
				if(!(m_animations_list->Contains(anim_name)))
					m_animations_list->Add(anim_name);
			}
		}
		++it;
	}
	*/
}

} // namespace editor
} // namespace xray