////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_PATH_PART_ITERATOR_H_INCLUDED
#define XRAY_FS_PATH_PART_ITERATOR_H_INCLUDED

namespace xray {
namespace fs_new {

//-----------------------------------------------------------------------------------
// path_part_iterator
//-----------------------------------------------------------------------------------

// if you want to include "" (empty sub-path) in iteration, set include_empty_string_in_iteration to true
enum include_empty_string_in_iteration_bool {	include_empty_string_in_iteration_false, include_empty_string_in_iteration_true };

class path_part_iterator
{
public:
							path_part_iterator	(pcstr path_str, char separator, include_empty_string_in_iteration_bool = include_empty_string_in_iteration_false);
							path_part_iterator	(pcstr path_str, int len, char separator, include_empty_string_in_iteration_bool = include_empty_string_in_iteration_false);

	static 
	path_part_iterator		end					() { return path_part_iterator(NULL, 0); }

	template <class out_string_type>
	void					append_to_string	(out_string_type & out_string) const;
	template <class out_string_type>
	void					assign_to_string	(out_string_type & out_string) const { out_string.clear(); append_to_string(out_string); }
	bool					equal				(const char* s) const;

	pcstr					cur_end				() const { return m_cur_end; }

	void					operator ++			();
	bool					operator == 		(const path_part_iterator& it) const;
	bool					operator != 		(const path_part_iterator& it) const;

protected:
	path_part_iterator(char separator) : m_separator(separator) {}		// intentionally

	include_empty_string_in_iteration_bool	m_include_empty_string_in_iteration;
	pcstr 					m_path_str;
	pcstr 					m_path_end;
	pcstr 					m_cur_str;
	pcstr 					m_cur_end;
	char					m_separator;
};

//-----------------------------------------------------------------------------------
// reverse_path_part_iterator
//-----------------------------------------------------------------------------------

class reverse_path_part_iterator : public path_part_iterator
{
public:
	reverse_path_part_iterator			(pcstr path_str, char separator);
	reverse_path_part_iterator			(pcstr path_str, int len, char separator);

	static 
	reverse_path_part_iterator	end		() { return reverse_path_part_iterator(NULL, 0); }

	void					operator ++	();
	bool					operator == (const reverse_path_part_iterator& it) const;
	bool					operator != (const reverse_path_part_iterator& it) const;
};

} // namespace fs_new
} // namespace xray

#include <xray/fs/path_part_iterator_inline.h>

#endif // #ifndef XRAY_FS_PATH_PART_ITERATOR_H_INCLUDED