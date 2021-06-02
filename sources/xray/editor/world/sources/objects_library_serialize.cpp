////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "objects_library.h"

namespace xray{
namespace editor{

void add_referenced_folders( library_folders^ dest, lib_folder^ f )
{
	if(!dest->Contains(f))
	{
		dest->Add( f );

		if(f->m_parent_folder)
			add_referenced_folders( dest, f->m_parent_folder );
	}
}

void objects_library::save_as( pcstr filename )
{
	(*m_library_cfg)->get_root().clear();

	configs::lua_config_value root = (*m_library_cfg)->get_root()["library"];

	library_folders^ referenced_folders = gcnew library_folders;

	for each (lib_item^ itm in m_tool_library)
	{
		System::String^ guid_str = itm->m_guid.ToString();
		unmanaged_string s(guid_str);
		unmanaged_string fullpath	(itm->full_path_name());
		(*itm->m_config)["name"]	= fullpath.c_str();
		root[s.c_str()].assign_lua_value(*itm->m_config);
		
		if(itm->m_parent_folder)
			add_referenced_folders	( referenced_folders, itm->m_parent_folder );
	}

	root = (*m_library_cfg)->get_root()["empty_folders"];
	for each( lib_folder^ fldr in m_folders)
	{
		if(referenced_folders->Contains(fldr))
			continue;

		unmanaged_string s(fldr->full_path_name());
		root[s.c_str()] = 0;
	}

	(*m_library_cfg)->save_as	( filename, configs::target_sources );
}

void objects_library::load( configs::lua_config* cfg )
{
	(*m_library_cfg)								= cfg;
	configs::lua_config_value empty_folders			= (*m_library_cfg)->get_root()["empty_folders"];
	configs::lua_config::const_iterator	it			= empty_folders.begin();
	configs::lua_config::const_iterator	const it_e	= empty_folders.end();
	for ( ;it!=it_e; ++it ) 
	{
		System::String^ folder_name			= gcnew System::String((pcstr)it.key());
		get_folder_item_by_full_path		( folder_name, true ); // true==create
	}

	process_library_lodaing			( (*(*m_library_cfg))["library"] );
}

void objects_library::process_library_lodaing( configs::lua_config_value t )
{
	configs::lua_config::const_iterator	it			= t.begin();
	configs::lua_config::const_iterator	const it_e	= t.end();

	for ( ;it!=it_e; ++it ) 
	{
		configs::lua_config_value current	= *it;
		System::String^ guid_str			= gcnew System::String((pcstr)it.key());

		System::String^ full_name		= gcnew System::String(current["name"]);

		lib_item^ itm					= load_lib_item( *it  );
		m_tool_library->Add				( itm );

		System::String^ folder			= System::IO::Path::GetDirectoryName( full_name )->Replace('\\', '/');
		System::String^ item_name		= System::IO::Path::GetFileName( full_name );

		itm->m_parent_folder			= get_folder_item_by_full_path( folder, true );
		itm->m_name						= item_name;
		itm->m_guid						= System::Guid(guid_str);
	}
}

// old format
//void objects_library::process_library_lodaing( configs::lua_config_value t, System::String^ path )
//{
//	configs::lua_config::const_iterator	it			= t.begin();
//	configs::lua_config::const_iterator	const it_e	= t.end();
//	for ( ;it!=it_e; ++it ) 
//	{
//		System::String^ name				= gcnew System::String((pcstr)it.key());
//		configs::lua_config_value current	= t[(pcstr)it.key()];
//
//		System::String^ current_path		= path;
//		if(current_path->Length)
//			current_path					+= "/";
//
//		current_path						+= name;
//
//		if(current.value_exists("name"))
//		{
//			lib_item^ itm					= load_lib_item( current );
//			m_tool_library->Add				( itm );
//
//			System::String^ folder			= System::IO::Path::GetDirectoryName( current_path )->Replace('\\', '/');
//			System::String^ item_name		= System::IO::Path::GetFileName( current_path );
//
//			itm->m_parent_folder			= get_folder_item_by_full_path( folder, true );
//			itm->m_name						= item_name;
//			R_ASSERT						( itm->m_name == name, "invalid library class description for [%s]", unmanaged_string(current_path).c_str() );
//		}else
//		{// it is folder
//			process_library_lodaing			( current, current_path );
//		}
//	}
//}

} //namespace xray
} //namespace editor
