////////////////////////////////////////////////////////////////////////////
//	Created		: 04.08.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_selector_source.h"
#include "resource_editor.h"

using namespace System::Windows::Forms;

namespace xray{
namespace editor{


void effect_selector_source::on_fs_iterator_ready( vfs::vfs_locked_iterator const & fs_it )
{
	m_parent->nodes->Clear();
	//if( !is_for_texture )   
	//	m_parent->add_item("none", xray::editor_base::node_resource);

	process_fs					( fs_it.children_begin(), "" );
	m_parent->Sort				( );
	m_parent->on_items_loaded	( );
}

void effect_selector_source::process_fs( vfs::vfs_iterator const & in_it, System::String^ file_path )
{
	vfs::vfs_iterator it				=	in_it;
	while( !it.is_end() )
	{
		if(it.is_folder())
		{
			System::String^ folder_name = gcnew System::String( it.get_name( ) );
			TreeNode^ node = m_parent->add_group( file_path+folder_name, xray::editor_base::folder_closed, xray::editor_base::folder_open );
			process_fs	( it.children_begin(), file_path+folder_name+"/" );
			if( node->Nodes->Count == 0 )
				m_parent->Nodes->Remove( node );
		}
		else
		{
			System::String^ path = gcnew System::String( it.get_name( ) );
			if( Path::GetExtension( path ) == ".effect" )
			{
				System::String^ file_name = Path::GetFileNameWithoutExtension( path );
				if( existing == nullptr || !existing->Contains( file_path+file_name ) )
					m_parent->add_item( file_path+file_name, xray::editor_base::node_resource );
			}
		}
		++it;
	}
}

void effect_selector_source::refresh( )
{
	System::String^ request_path	= c_source_effects_path;
	
	if( effect_stage != nullptr && effect_stage != "" )
		request_path += "/" + effect_stage;

	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &effect_selector_source::on_fs_iterator_ready), g_allocator);

	resources::query_vfs_iterator(	unmanaged_string( request_path ).c_str(), 
									boost::bind(&fs_iterator_delegate::callback, q, _1), 
									g_allocator, 
									resources::recursive_true);
}

} // namespace editor
} // namespace xray