////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.10.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LOGGING_LOG_FILE_H_INCLUDED
#define LOGGING_LOG_FILE_H_INCLUDED

#include <xray/fs_path_string.h>

namespace xray {

namespace core {
	enum log_file_usage;
} // namespace core

namespace logging {

class XRAY_CORE_API log_file : private detail::noncopyable {
public:		
						log_file				( core::log_file_usage log_file_usage, pcstr file_name );
						~log_file				( );

    // writing data
			void		append					( pcstr data, u32 length );
			void		flush					( pcstr file_name );

	// reading lines
			void		start_transaction		( );
			void		end_transaction			( );
			void		goto_line				( u32 line );
	
	template <int buffer_size>
	inline	bool		read_next_line			( char (&buffer)[buffer_size]) { return read_next_line(buffer, buffer_size); }
			bool		read_next_line			( pstr const buffer, const u32 buffer_size);

			u32			get_lines_count			( ) const;
	inline	pcstr		file_name				( ) const { return m_file_name; }
			bool		initialized				( ) const { return !!m_file; }

private:
	void				assert_transaction_in_current_thread	( ) const;

	enum { cache_size	= 1024 };
	char				m_cache[cache_size];

private:
	typedef xray::mt::vector< int >				line_groups_type;

private:
	string_path			m_file_name;
	line_groups_type*	m_line_groups;			// m_line_groups[x] = group_size*x-th line pos
	FILE*               m_file;
	u32					m_last_line;
	u32					m_current_pos;
	u32					m_file_size;
	int					m_cache_start;
	int					m_cache_size;
	u32					m_transaction_thread_id;

private:
			char		read_next_char			( );

	template <typename processor_type>
	inline	bool		process_next_line		( u32 buffer_size, processor_type const& processor );

			bool		skip_next_line			( );
}; // class log_file

extern xray::fs::path_string	g_log_file_name;

XRAY_CORE_API log_file* get_log_file			( );

} // namespace logging
} // namespace xray

#endif // #ifndef LOGGING_LOG_FILE_H_INCLUDED