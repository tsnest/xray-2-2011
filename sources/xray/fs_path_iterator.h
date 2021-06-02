////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PATH_ITERATOR_H_INCLUDED
#define PATH_ITERATOR_H_INCLUDED

#include <xray/fixed_string.h>

namespace xray {
namespace fs {

//-----------------------------------------------------------------------------------
// path_iterator
//-----------------------------------------------------------------------------------

class path_iterator
{
public:
							path_iterator	(pcstr path_str);
							path_iterator	(pcstr path_str, int len);

	static 
	path_iterator			end				() { return path_iterator(NULL); }

	void					to_string		(buffer_string& out_str) const;
	bool					equal			(const char* s) const;

	pcstr					cur_end			() const { return m_cur_end; }

	void					operator ++		();
	bool					operator == 	(const path_iterator& it) const;
	bool					operator != 	(const path_iterator& it) const;

protected:
	path_iterator() {}		// intentionally

	pcstr 					m_path_str;
	pcstr 					m_path_end;
	pcstr 					m_cur_str;
	pcstr 					m_cur_end;
};

//-----------------------------------------------------------------------------------
// reverse_path_iterator
//-----------------------------------------------------------------------------------

class reverse_path_iterator : public path_iterator
{
public:
	reverse_path_iterator				(pcstr path_str);
	reverse_path_iterator				(pcstr path_str, int len);

	static 
	reverse_path_iterator	end			() { return reverse_path_iterator(NULL); }

	void					operator ++	();
	bool					operator == (const reverse_path_iterator& it) const;
	bool					operator != (const reverse_path_iterator& it) const;
};

} // namespace fs
} // namespace xray

#include "fs_path_iterator_inline.h"

#endif // PATH_ITERATOR_H_INCLUDED