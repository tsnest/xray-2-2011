////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef UTILITY_FUNCTIONS_H_INCLUDED
#define UTILITY_FUNCTIONS_H_INCLUDED

namespace xray {
namespace shader_compiler {

void	get_files					( const char *home, char const* const search_mask, std::vector<std::string>& out_file_names, bool recursive = true	);
void	file_to_string				( const char* file_name, std::string& out_string																	);
bool	read_file					( const char* file_name, char*& out_buffer, u32& out_buffer_size													);
bool	get_file_path				( const char* file_name, std::string& out_file_path																	);
bool	get_file_extension			( const char* file_name, std::string& out_buffer																	);
s32		get_text_in_quotes			( const std::string& text, std::string& out_text, u32 start_pos = 0													);
s32		find_simple_text_sensitive	( const char* buffer, const char* found_text, bool search_in_comment_line = true, s32 start_pos = 0					);
std::string get_file_name			( const char* long_file_name																						);
bool	check_file_exists			( char const* dir_name																								);

} // namespace shader_compiler
} // namespace xray

#endif // #ifndef UTILITY_FUNCTIONS_H_INCLUDED