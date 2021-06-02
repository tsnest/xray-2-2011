////////////////////////////////////////////////////////////////////////////
//	Created 	: 01.09.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LOGGING_RULE_TREE_NODE_H_INCLUDED
#define LOGGING_RULE_TREE_NODE_H_INCLUDED

#include <xray/logging/extensions.h>
#include "logging.h"
#include "path_parts.h"
#include "filter_tree_node_base.h"

namespace xray {
namespace logging {

class path_parts;

namespace filter_tree {

class node : public node_base, private boost::noncopyable 
{
public:
	inline	explicit	node			(pcstr name, int rule) : node_base(name), m_verbosity(rule), m_thread_id(u32(-1)) {}
						~node			();
			void		set				(pcstr						initiator_path, 
										 int						verbosity, 
										 u32						thread_id, 
										 memory::base_allocator *	allocator,
										 memory::base_allocator *	allocator_to_clean);

	inline	int			get_verbosity	(path_parts* path) { return get_verbosity(path, silent); }
			void		clean			(memory::base_allocator * allocator);

private:
			int			get_verbosity	(path_parts* path, int inherited_verbosity);

private:
	nodes_tree_type						m_children;

	int									m_verbosity;
	u32									m_thread_id;
}; // class node

} // namespace filter_tree
} // namespace logging
} // namespace xray

#endif // #ifndef LOGGING_RULE_TREE_NODE_H_INCLUDED
