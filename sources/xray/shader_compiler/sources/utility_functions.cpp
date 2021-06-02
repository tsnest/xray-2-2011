////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "utility_functions.h"

namespace xray {
namespace shader_compiler {

s32 find_simple_text_sensitive( const char* buffer, const char* found_text, bool search_in_comment_line, s32 start_pos )
{
	(void)&start_pos;

	static const char permissible_chars[] = {'a','z','A','Z','0','9','_','_'};

	std::string str_buffer(buffer);
	std::string str_text(found_text);

	s32 find_pos = 0;
	
	bool found_in_comment = false;
	while (find_pos!=-1)
	{
		find_pos = str_buffer.find(str_text,(find_pos==0)?0:find_pos+1);

		if (find_pos<0)
			return -1;

		const char l_char = (find_pos-1 >= 0) ? str_buffer[find_pos-1] : ' ';
		const char r_char = (find_pos+str_text.length() < str_buffer.length()) ? str_buffer[find_pos + str_text.length()]: ' ';

		for (u32 i=0; i<sizeof(permissible_chars); i+=2)
		{
			const char c_from = permissible_chars[i];
			const char c_to = permissible_chars[i+1];

			if (l_char>=c_from && l_char<=c_to 
				|| r_char>=c_from && r_char<=c_to)
			{
				return -1;
			}
		}

		found_in_comment = false;
		if (!search_in_comment_line)
		{
			// in line comment
			s32 new_line_pos = 0;
			for (s32 i=find_pos; i>=0; i--)
			{
				if (str_buffer[i]=='\n')
				{
					new_line_pos = i;
					break;
				}
			}
			s32 rem_pos = str_buffer.find("//",new_line_pos);
			if (rem_pos!=-1 && rem_pos<find_pos)
			{
				found_in_comment = true;
			}

			// in block comment
			for (s32 i=find_pos; i>=1; i--)
			{
				if (str_buffer[i]=='/' && str_buffer[i-1]=='*')
					return find_pos;
				
				if (str_buffer[i]=='*' && str_buffer[i-1]=='/')
				{
					found_in_comment = true;
				}
			}
			if (found_in_comment)
				continue;
		}
		
		return find_pos;
	}
	if (!search_in_comment_line && found_in_comment)
	{
		return -1;
	}
	return find_pos;
}

bool get_file_path( const char* file_name, std::string& out_file_path )
{
	std::string temp(file_name);

	if (!temp.size())
		return false;

	if (temp.size())
	for (s32 i=temp.size()-1; i>=0; i--)
	{
		if (temp[i]=='\\' || temp[i]=='/')
		{
			for (s32 j=0; j<=i; j++)
				out_file_path+=temp[j];

			return true;
		}
	}
	return false;
}

bool get_file_extension( const char* file_name, std::string& out_buffer )
{
	s32 length = strlen(file_name);

	if (!length)
		return false;

	for ( s32 i=length-1; i>=0; i--)
	{
		if ( file_name[i]=='\'' || file_name[i]=='\"' )
			return false;

		if ( file_name[i]!='.' )
			continue;

		if ( i+1<length )
			for ( s32 d=i+1; d<length; d++)
				out_buffer += file_name[d];

		return true;
	}
	return false;
}

static void process_files( char const* const base_path, char const* const full_path, char const* const search_mask, std::vector<std::string>& out_file_names, bool recursive )
{
	_finddata32i64_t		file_desc;
	string_path				file_spec;
	strcpy_s				( file_spec, full_path );
	strcat_s				( file_spec, search_mask );
	long handle				= (long)_findfirst32i64( file_spec, &file_desc );

	if ( handle	==	-1 ) {
		strcpy_s			( file_spec, full_path );
		strcat_s			( file_spec, "*.*" );
		long handle			= (long)_findfirst32i64( file_spec, &file_desc );
		do {
			if ( !strcmp(".", file_desc.name) )
				continue;

			if ( !strcmp("..", file_desc.name) )
				continue;

			if ( (file_desc.attrib & _A_SUBDIR) == _A_SUBDIR && recursive ) {
				if ( (file_desc.attrib & _A_HIDDEN) == _A_HIDDEN )
					continue;

				string_path		temp;
				strcpy_s		( temp, full_path );
				strcat_s		( temp, file_desc.name );
				strcat_s		( temp, "\\" );
				process_files	( base_path, temp, search_mask, out_file_names, recursive );
				continue;
			}

			continue;
		} while ( _findnext32i64 (handle, &file_desc) != -1 );

		return;
	}

	do {
		if ( !strcmp(".", file_desc.name) )
			continue;

		if ( !strcmp("..", file_desc.name) )
			continue;

		if ( (file_desc.attrib & _A_SUBDIR) == _A_SUBDIR && recursive ) {
			if ( (file_desc.attrib & _A_HIDDEN) == _A_HIDDEN )
				continue;

			string_path		temp;
			strcpy_s		( temp, full_path );
			strcat_s		( temp, file_desc.name );
			strcat_s		( temp, "\\" );
			process_files	( base_path, temp, search_mask, out_file_names, recursive );
			continue;
		}


		string_path			file_name;
		strcpy_s			( file_name, full_path );
		strcat_s			( file_name, file_desc.name );

		out_file_names.push_back(file_name);
	} while ( _findnext32i64 (handle, &file_desc) != -1 );
}

void get_files( const char *home, char const* const search_mask, std::vector<std::string>& out_file_names, bool recursive )
{
	process_files( home,home,search_mask,out_file_names,recursive );
}

bool read_file( const char* file_name, char*& out_buffer, u32& out_buffer_size )
{
	s32	const file_open_flags		=	_O_BINARY | _O_SEQUENTIAL | _O_RDONLY;
	s32 const permission_flags		=	_S_IREAD;
	s32 const sh_permission_flags	=	_SH_DENYWR;

	s32	file_handle					=	0;
	errno_t error_code				=	_sopen_s(& file_handle, file_name, file_open_flags, 
		sh_permission_flags, permission_flags);
	if ( error_code ) 
	{
		Sleep						(1);
		error_code					=	_sopen_s( &file_handle, file_name, file_open_flags,		
			sh_permission_flags, permission_flags);
		if ( error_code ) {
//			printf					( "ERROR: read_file(): cannot open file \"%s\"\n", file_name );
			printf					( "cannot open file \"%s\"\r\n", file_name );
			return					false;
		}
	}

	FILE* const file				=	_fdopen(file_handle, "rb");
	u32 const file_size	= 	_filelength ( _fileno(file) );
	//bool use_stack					=	file_size < 768*1024;
	char* buffer					=	0;
	//XRAY_TODO("problems with _alloca, incorrect memory allocation");
	//if ( use_stack )
	//	buffer						= (char*)_alloca(file_size*sizeof(char));
	//else
	buffer						= (char*)malloc(file_size*sizeof(char));
	//memset(buffer,0,file_size*sizeof(char));
	size_t const read_bytes			= fread( buffer, 1, file_size, file );

	(void)&read_bytes;

	fclose							( file );

	out_buffer = buffer;
	out_buffer_size = file_size;
	return							true;
}

std::string get_file_name( const char* long_file_name )
{
	std::string temp(long_file_name);
	std::string result;

	for ( s32 i=temp.size()-1; i>=0; i-- )
	{
		if ( temp[i]=='\\' || temp[i]=='/' )
		{
			for ( s32 j=i+1; j<(s32)temp.size(); j++ )
				result+=temp[j];
			return result;
		}
	}
	return temp;
}

s32 get_text_in_quotes( const std::string& text, std::string& out_text, u32 start_pos )
{
	s32 right_pos = -1;
	const char string_tabs[] = {'\"','\''};
	for ( s32 i=0; i<sizeof(string_tabs); i++ )
	{
		s32 left_pos  = text.find(string_tabs[i],start_pos+1);
		if ( left_pos!=-1 )
		{
			right_pos = text.find(string_tabs[i],left_pos+1);
			for(s32 c=left_pos+1; c<right_pos; c++)
				out_text += text[c];
			break;
		}
	}
	return right_pos;
}


bool check_file_exists(char const* dir_name) 
{ 
	WIN32_FIND_DATA  data; 
	HANDLE handle = FindFirstFile(dir_name,&data); 
	if (handle != INVALID_HANDLE_VALUE)
	{
		FindClose(handle);
		
		return true;
	}

	return false;
}

} // namespace xray
} // namespace shader_compiler