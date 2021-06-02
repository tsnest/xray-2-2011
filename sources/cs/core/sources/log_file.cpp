////////////////////////////////////////////////////////////////////////////
//	Module 		: log_file.cpp
//	Created 	: 24.09.2006
//  Modified 	: 24.09.2006
//	Author		: Dmitriy Iassenev
//	Description : log file class inline functions
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "log_file.h"

log_file::log_file				()
{
	sz_cat(
		sz_concat				(
			m_file_name,
#if CS_PLATFORM_XBOX_360
			"DEVKIT:\\",
#endif // #if CS_PLATFORM_XBOX_360
			cs::core::core.m_application_name,
			"_",
			cs::core::core.m_user_name
		),
		sizeof(m_file_name),
		".log"
	);
}

log_file::~log_file				()
{
	flush					();
}

void log_file::flush			()
{
	m_mutex.lock			();

	fs::writer				writer = fs::file_writer(m_file_name);

	LINES::const_iterator	I = m_lines.begin();
	LINES::const_iterator	E = m_lines.end();
	for ( ; I != E; ++I) {
		writer->w			(**I,sizeof(char)*I->length());
		writer->w			("\r\n",sizeof(char)*2);
	}

	m_mutex.unlock			();
}

void log_file::add_line			(pcstr line)
{
	str_shared				temp = line;
	m_mutex.lock			();
	m_lines.push_back		(temp);
	m_mutex.unlock			();
}
