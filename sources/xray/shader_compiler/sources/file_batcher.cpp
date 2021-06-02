////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "file_batcher.h"
#include "utility_functions.h"
#include "command_line_parameters.h"
//#include <xray/fs_path.h>

namespace xray {
namespace shader_compiler {

bool files_batcher::next_file( )
{
	if ( ++m_current_file_index >= (s32)m_file_names.size() )
		return false;

	return true;
}


static bool isDirectory(char const* szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

files_batcher::files_batcher( const char* in_valid_shaders_extensions ) :
	m_current_file_index( -1 ),
	m_error_occured( false )
{
	char curr_dir[513];
	GetCurrentDirectory(512,(LPTSTR)curr_dir);

	std::string command_line = GetCommandLine();

	std::string shader_path;

#if 0//NDEBUG
	s32 l_pos_app_name  = 0;
	get_text_in_quotes(command_line,shader_path,l_pos_app_name);
#else
	// application name
	s32 l_pos_app_name  = command_line.find("\"");
	l_pos_app_name		= command_line.find("\"",l_pos_app_name+1);
	get_text_in_quotes(command_line,shader_path,l_pos_app_name+1);
#endif
	
	std::string file_name_or_dir = shader_path;
	
	s32 last_index = file_name_or_dir.length()-1;
	bool is_directory = isDirectory(file_name_or_dir.c_str());
	
	if (!check_file_exists(shader_path.c_str()) && !is_directory)
	{
		//char temp[260];
		//strcpy_s	( temp, shader_path.c_str() );
		//pcstr const file_name = fs_new::file_name_from_path( fs_new::convert_to_portable_in_place( temp ) );
		printf( "cannot open file \"%s\"\r\n", shader_path.c_str() );
		m_error_occured	= true;
		//_getch();
		return;
	}
	
	std::string valid_shaders_extensions(in_valid_shaders_extensions);
	
	bool inner_folders = g_recurse.is_set();
	
	std::string all_extensions;
	
	xray::fixed_string<128> out_buff;
	if (g_ext.is_set_as_string(&out_buff))
		all_extensions = out_buff.c_str();
	
	m_current_file_index = -1;
	
	std::vector<std::string> temp_file_names;
	temp_file_names = m_file_names;
	
	if ( is_directory )
	{
		if (file_name_or_dir[last_index]!='\\' && file_name_or_dir[last_index]!='/')
			file_name_or_dir+="/";
		get_files(file_name_or_dir.c_str(),"*.*",m_file_names,inner_folders);
	}
	else
		m_file_names.push_back(file_name_or_dir);
	
	for (u32 i=0; i<m_file_names.size(); i++)
	{
		std::string file_name = m_file_names[i];
		
		std::string file_extension;
		get_file_extension(file_name.c_str(),file_extension);
		
		// -ext option
		if ( is_directory 
		&& all_extensions.length() 
		&& file_extension.length() 
		&& find_simple_text_sensitive(all_extensions.c_str(), file_extension.c_str())==-1 )
			continue;
		
		// shader extensions
		if (valid_shaders_extensions.size() 
		&& find_simple_text_sensitive(valid_shaders_extensions.c_str(),file_extension.c_str())==-1)
			continue;
		
		temp_file_names.push_back(file_name);
	}
	m_file_names = temp_file_names;
	
	SetCurrentDirectory(curr_dir);
}

std::string files_batcher::get_general_path			( ) const
{
	if (!m_file_names.size())
		return "";
	
	std::string path;
	get_file_path(m_file_names[0].c_str(),path);

	std::string result = m_file_names[0];

	std::vector<std::string> file_paths;

	for (u32 i=0; i<m_file_names.size(); i++)
	{
		std::string path;
		get_file_path(m_file_names[i].c_str(),path);
		file_paths.push_back(path);
	}

	for (u32 i=0; i<file_paths.size(); i++)
	{
		std::string& curr = file_paths[i];
		
		bool found = true;
		
		for (u32 j=0; j<file_paths.size(); j++)
		{
			if (i==j)
				continue;

			if (!strstr(curr.c_str(),file_paths[j].c_str()))
				found = false;
		}
		if (found)
			return curr;
	}
	return path;
}

const char* files_batcher::get_file_name			( ) const
{
	return m_file_names[m_current_file_index].c_str();
}

std::string files_batcher::get_file_data			( ) const
{
	std::string current_file_name = m_file_names[m_current_file_index];
	std::string result;
	
	char* buffer		= 0;
	u32   buffer_size	= 0;

	read_file(current_file_name.c_str(),buffer,buffer_size);

	if (buffer)
	{
		result.assign(buffer,buffer+buffer_size);
	}
	return result;
}

} // namespace xray
} // namespace shader_compiler