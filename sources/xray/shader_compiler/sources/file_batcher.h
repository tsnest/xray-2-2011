////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FILE_BATCHER_H_INCLUDED
#define FILE_BATCHER_H_INCLUDED

namespace xray {
namespace shader_compiler {

class files_batcher
{
public:
	files_batcher( const char* valid_shaders_extensions );

	struct file_name_data_pair
	{
		std::string name;
		std::string data;
	};

	std::string get_general_path		( ) const;
	const char* get_file_name			( ) const;
	std::string get_file_data			( ) const;
	
	bool next_file						( );
	inline bool error_occured			( ) const { return m_error_occured; }

	bool operator ++					( )	{ return next_file();}
private:
	std::vector<std::string>	m_file_names;
	s32							m_current_file_index;
	bool						m_error_occured;
}; // class files_batcher

} // namespace shader_compiler
} // namespace xray

#endif // #ifndef FILE_BATCHER_H_INCLUDED