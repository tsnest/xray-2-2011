////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "objects_library.h"
#include "project_items.h"
#include "tool_base.h"

namespace xray{
namespace editor{

lib_item::lib_item( configs::lua_config_value const& lv )
{ 
	m_config				= NEW( configs::lua_config_value )( lv );
	m_name					= "";
}

lib_item::lib_item( )
:m_config	( NULL )
{
}

lib_item::~lib_item( )
{
	DELETE ( m_config );
}

System::String^ lib_folder::full_path_name( )
{
	if(m_parent_folder)
		return m_parent_folder->full_path_name( ) + "/" + m_name;
	else
		return m_name;
}

System::String^ lib_item::full_path_name( )
{
	if(m_parent_folder)
		return m_parent_folder->full_path_name() + "/" + m_name;
	else
		return m_name;
}

objects_library::objects_library( )
{
	m_library_cfg		= NEW( configs::lua_config_ptr )();
	m_tool_library		= gcnew library_items;
	m_folders			= gcnew library_folders;
}

objects_library::~objects_library( )
{
	for each (lib_item^ itm in m_tool_library)
		delete				itm;

	DELETE ( m_library_cfg );
}

library_items^ objects_library::get_all_library_tems( )					
{ 
	return m_tool_library; 
}

lib_item^ objects_library::get_library_item_by_guid( System::Guid guid )
{
	for each (lib_item^ li in m_tool_library)
		if(guid == li->m_guid)
			return li;

	return			nullptr;
}

lib_item^ objects_library::get_library_item_by_full_path( System::String^ library_item_full_path, bool b_assert )
{
	lib_item^ result		= nullptr;
	for each (lib_item^ li in m_tool_library)
	{
		if(library_item_full_path == li->full_path_name())
		{
			result	= li;
			break;
		}
	}
	R_ASSERT		(!b_assert || result, "library item [%s] doesnt exist in library", unmanaged_string(library_item_full_path).c_str() );
	return			result;
}

lib_item^ objects_library::load_lib_item( configs::lua_config_value const& t )
{
	lib_item^ result				= gcnew lib_item( t );
	return							result;
}

void objects_library::on_commit( lib_item^ lib_item )
{
	int idx = m_tool_library->IndexOf	( lib_item );
	
	if(idx==-1) //edit or new 
		m_tool_library->Add				( lib_item );
}

lib_item^ objects_library::new_lib_item( System::String^ library_name )
{
	System::Guid guid			= System::Guid::NewGuid();
	unmanaged_string			s(guid.ToString());
	configs::lua_config_value v= (**m_library_cfg)[s.c_str()];

	lib_item^ li				= gcnew lib_item( v );
	li->m_guid					= guid;
	System::String^ dir			= System::IO::Path::GetDirectoryName( library_name )->Replace('\\', '/');
	System::String^ item_name	= System::IO::Path::GetFileName( library_name );

	li->m_parent_folder			= get_folder_item_by_full_path( dir, true );
	
	li->m_name					= item_name;

	return li;
}

void objects_library::delete_library_item( lib_item^ itm )
{
	m_tool_library->Remove( itm );
	delete					itm;
}

lib_folder^ objects_library::get_folder_item( lib_folder^ parent, System::String^ folder_name )
{
	for each (lib_folder^ f in m_folders)
	{
		if( (folder_name==f->m_name) && (parent==f->m_parent_folder) )
			return f;
	}
	return nullptr;
}

lib_folder^ objects_library::get_folder_item_by_full_path( System::String^ folder_full_path, bool b_create )
{
	lib_folder^ result				= nullptr;
	
	if(folder_full_path->Length==0)
		return result;

	array<System::String^>^ tokens	= folder_full_path->Split('/');
	for(int i=0; i<tokens->Length; ++i)
	{
		System::String^ curr	= tokens[i];
		lib_folder^ curr_folder = get_folder_item( result, curr );

		if( curr_folder )
		{
			result = curr_folder;
		}else
		if(b_create)
		{// create
			lib_folder^ f		= gcnew lib_folder;
			f->m_name			= curr;
			f->m_parent_folder	= result;
			m_folders->Add		( f );
			result				= f;
		}else
			return nullptr;

	}

	return result;
}

lib_item^ objects_library::add_simple( System::String^ full_name )
{
	lib_item^ itm				= gcnew lib_item;
	m_tool_library->Add			( itm );

	System::String^ dir			= System::IO::Path::GetDirectoryName( full_name )->Replace('\\', '/');
	System::String^ item_name	= System::IO::Path::GetFileName( full_name );

	itm->m_parent_folder		= get_folder_item_by_full_path( dir, true );
	
	itm->m_name					= item_name;
	return						itm;
}

void objects_library::fill_tree( tree_node_collection^ root, tool_base^ tool, bool show_empty_folders, System::String^ filter )
{
	if(show_empty_folders)
	{
		for each( lib_folder^ fitm in m_folders )
		{
			System::String^ fullpath = fitm->full_path_name();
			
			if( filter && !fullpath->StartsWith(filter) )
				continue;

			tree_node^ node	= make_hierrarchy( root, fullpath );
			node->Tag		= fitm;	
		}
	}

	for each( lib_item^ itm in m_tool_library )
	{
		System::String^ fullpath	= itm->full_path_name();

		if( filter && !fullpath->StartsWith(filter) || fullpath->StartsWith("#") )
				continue;

		tree_node^	node			= make_hierrarchy( root, fullpath );
		node->Tag					= itm;

		node->ImageIndex			= (itm->m_image_index==0)? tool->default_image_index : itm->m_image_index;
		node->SelectedImageIndex	= node->ImageIndex;
	}
}

lib_item^ objects_library::extract_library_item( configs::lua_config_value const& cfg )
{
	if(cfg.value_exists("lib_guid"))
	{
		System::String^ library_guid_str	= gcnew System::String( cfg["lib_guid"] );
		System::Guid	library_guid		( library_guid_str );
		lib_item^ li						= get_library_item_by_guid( library_guid );
		
		if(li==nullptr)
		{
			pcstr library_name	= cfg["lib_name"];
			R_ASSERT			("library item [%s] not found", library_name );
			return nullptr;
		}else
			return li;
	}else
	{
		System::String^ library_name	= gcnew System::String( cfg["lib_name"] );
		lib_item^ li					= get_library_item_by_full_path( library_name, false );
		R_ASSERT(li!=nullptr);
		return li;
	}
}

lib_folder^ objects_library::add_new_folder( System::String^ full_name )
{
	System::String^ n	= full_name;
	lib_folder^ result	= get_folder_item_by_full_path( n, false );
	
	if(result)// already exist
	{
		int idx = 0;
		for (;;)
		{
			n	= System::String::Format("{0}_{1:d4}", full_name, idx);
			result	= get_folder_item_by_full_path( n, false );
			if(result==nullptr)
				break;
			else
				++idx;
		}
	}
	
	return get_folder_item_by_full_path( n, true ); // true==create
}

bool has_parent_folder( lib_folder^ parent, lib_folder^ who )
{
	if(who->m_parent_folder==nullptr)
		return false;

	if(who->m_parent_folder==parent)
		return true;

	return has_parent_folder( parent, who->m_parent_folder);
}

void objects_library::delete_folder_item( lib_folder^ folder )
{
	library_folders		folders_to_delete;
	library_items		items_to_delete;

	folders_to_delete.Add( folder );

	for each (lib_folder^ f in m_folders)
	{
		if(has_parent_folder(folder, f))
			folders_to_delete.Add(f);
	}

	for each (lib_item^ itm in m_tool_library)
		if( (itm->m_parent_folder) &&
			(itm->m_parent_folder==folder || has_parent_folder(folder, itm->m_parent_folder))
			)
			items_to_delete.Add(itm);

	for each (lib_folder^ f in folders_to_delete)
		m_folders->Remove( f );

	for each (lib_item^ itm in items_to_delete)
		delete_library_item( itm );
}

} //namespace xray
} //namespace editor
