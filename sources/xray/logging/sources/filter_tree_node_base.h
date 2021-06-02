////////////////////////////////////////////////////////////////////////////
//	Created		: 30.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RULE_TREE_NODE_BASE_H_INCLUDED
#define RULE_TREE_NODE_BASE_H_INCLUDED

#include <boost/intrusive/set.hpp>

namespace xray {
namespace logging {
namespace filter_tree {

enum	{ max_name_length = 32 };
typedef fixed_string<max_name_length>	node_name_type;
class	node_base;

struct compare_nodes	
{
	bool	operator () (node_base const & left, node_base const & right) const;
	bool	operator () (pcstr const left, node_base const & right) const;
	bool	operator () (node_base const & left, pcstr const right) const;
};

typedef boost::intrusive::set_member_hook< boost::intrusive::link_mode<boost::intrusive::auto_unlink> >		
						nodes_member_hook;
class node_base
{
public:
	node_base								(pcstr name) : name(name) {}
	nodes_member_hook						tree_hook;
	node_name_type							name;
};

typedef boost::intrusive::member_hook	<	node_base, nodes_member_hook, & node_base::tree_hook	>
						nodes_tree_option;

typedef	boost::intrusive::multiset< node_base, 
							  	 	nodes_tree_option, 
							  	 	boost::intrusive::compare< compare_nodes >,
							  	 	boost::intrusive::constant_time_size<false> >	nodes_tree_type;

} // namespace filter_tree
} // namespace logging
} // namespace xray

#endif // #ifndef RULE_TREE_NODE_BASE_H_INCLUDED