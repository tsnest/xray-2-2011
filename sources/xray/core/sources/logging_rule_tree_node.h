////////////////////////////////////////////////////////////////////////////
//	Created 	: 01.09.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LOGGING_RULE_TREE_NODE_H_INCLUDED
#define LOGGING_RULE_TREE_NODE_H_INCLUDED

#include "logging.h"
#include "logging_path_parts.h"

namespace xray {
namespace logging {

class path_parts;

namespace rule_tree {

class node : private boost::noncopyable {
public:
	inline	explicit	node			(int rule) : m_verbosity(rule) {}
						~node			();
			void		set				(pcstr initiator_path, int rule);
	inline	int			get_verbosity	(path_parts* path) { return get_verbosity(path, silent); }
			void		clean			();

private:
			int			get_verbosity	(path_parts* path, int inherited_verbosity);

private:
	struct compare_predicate {
		inline bool		operator()		(pcstr s1, pcstr s2) const
		{
			int ret		= 0;
			// compare strings treating initiator_separator as zero
			while ( !ret && *s2 && ( *s1 != initiator_separator ) && ( *s2 != initiator_separator ) ) {
				ret = *( u8* )s1 - *( u8* )s2;				
				++s1;
				++s2;
			}

			return		( ret < 0 );
		}
	}; // struct compare_predicate

private:
	typedef associative_vector<pcstr, node*, vector, compare_predicate> Folder;

private:
	Folder	m_folder;
	int		m_verbosity;
}; // class node

} // namespace rule_tree
} // namespace logging
} // namespace xray

#endif // #ifndef LOGGING_RULE_TREE_NODE_H_INCLUDED