////////////////////////////////////////////////////////////////////////////
//	Module 		: log_file.h
//	Created 	: 24.09.2006
//  Modified 	: 24.09.2006
//	Author		: Dmitriy Iassenev
//	Description : log file class
////////////////////////////////////////////////////////////////////////////

#ifndef LOG_FILE_H_INCLUDED
#define LOG_FILE_H_INCLUDED

class log_file {
private:
	typedef cs_vector<str_shared>	LINES;

private:
	string256			m_file_name;
	LINES				m_lines;
	threading::mutex	m_mutex;

public:
					log_file		();
					~log_file		();
			void	flush			();
			void	add_line		(pcstr line);
};

#include "log_file_inline.h"

#endif // #ifndef LOG_FILE_H_INCLUDED