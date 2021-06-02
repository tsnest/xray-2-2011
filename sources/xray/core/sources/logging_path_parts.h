////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LOGGING_PATH_PARTS_H_INCLUDED
#define LOGGING_PATH_PARTS_H_INCLUDED

#include <xray/buffer_string.h>

namespace xray {
namespace logging {

char const initiator_separator	=	':';

class path_parts : private boost::noncopyable {
public:
	inline			path_parts			() : m_index(0), m_current_element(0) {}
	inline	pcstr	get_current_element () { return m_current_element; }
			void	add_part			(pcstr part);
			void	to_next_element		();
			void	concat2buffer		(xray::buffer_string& buffer);

private:
	typedef fixed_vector<pcstr, 4>		Parts;

private:
	Parts	m_parts;
	pcstr	m_current_element;
	int		m_index;
}; // class path_parts

} // namespace logging
} // namespace xray

#endif // #ifndef LOGGING_PATH_PARTS_H_INCLUDED