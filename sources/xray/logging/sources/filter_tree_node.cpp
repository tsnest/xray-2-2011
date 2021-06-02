////////////////////////////////////////////////////////////////////////////
//	Created 	: 01.09.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifdef SN_TARGET_PS3
	namespace scestd = std;
	#include <assert.h>
	namespace std {
		void _SCE_Assert(const char *first, const char *second);
		void _Assert(const char *first, const char *second);
	} // namespace std
#endif // #ifdef SN_TARGET_PS3

#include "filter_tree_node.h"
#include "logging.h"
#include "path_parts.h"

using xray::logging::filter_tree::node;
using xray::logging::filter_tree::node_base;
using xray::logging::filter_tree::compare_nodes;
using xray::logging::path_parts;

node::~node				()
{
}

void node::set			(pcstr const							initiator_path, 
						 int const								verbosity, 
						 u32 const								thread_id, 
						 xray::memory::base_allocator * const	allocator,
						 xray::memory::base_allocator * const	allocator_to_clean)
{
	if ( !initiator_path || !*initiator_path )
	{
		m_verbosity 					=	verbosity & ~recurse_0;
		m_thread_id						=	thread_id;
		if ( !(verbosity & recurse_0) )
			clean							(allocator_to_clean);
		return;
	}

	pcstr const next_path_portion		=	strchr(initiator_path, initiator_separator);

	node_name_type	path_portion;
	if ( next_path_portion )
		path_portion.assign					(initiator_path, next_path_portion);
	else
		path_portion					=	initiator_path;

	nodes_tree_type::iterator	it		=	m_children.find(path_portion.c_str(), m_children.key_comp());
 	node * child						=	NULL;
 	if ( it != m_children.end() )
 	{
 		child							=	static_cast<node *>(& * it);
 	}
 	else
 	{
 		child							=	XRAY_NEW_IMPL(allocator, node)(path_portion.c_str(), 0);
		m_children.insert					(* child);
 	}
	
	child->set								(next_path_portion ? next_path_portion + 1 : NULL, 
											 verbosity, thread_id, allocator, allocator_to_clean);
}

void node::clean						(xray::memory::base_allocator * allocator)
{
	while ( node * dying = static_cast<node *>(m_children.unlink_leftmost_without_rebalance()) )
	{
		ASSERT								(allocator);
		dying->clean						(allocator);
		XRAY_DELETE_IMPL					(allocator, dying);
	}

	m_children.clear						();
}

int node::get_verbosity					(path_parts * path, int inherited_verbosity)
{
	int verbosity = 
		(m_thread_id != u32(-1) && m_thread_id != threading::current_thread_id()) ?
		silent :
		(
			m_verbosity != 0 ?
			m_verbosity :
			inherited_verbosity
		);

	pcstr cur_part						=	path->get_current_element();
	if ( !cur_part || cur_part[0] == NULL )
	{
		// last element in path
		return								verbosity;		
	}

 	nodes_tree_type::iterator const it	=	m_children.find(cur_part, m_children.key_comp());
 
 	if ( it == m_children.end() ) // no special rule?
 		return								verbosity;

	node * const child					=	static_cast<node *>(& * it);
 
 	path->to_next_element					();
 	return									child->get_verbosity(path, verbosity);
}

static bool   compare_parts				(pcstr s1, pcstr s2)
{
	using namespace	xray::logging;
	int ret								=	0;
	// compare strings treating initiator_separator as zero
	while ( !ret && *s2 && ( *s1 != initiator_separator ) && ( *s2 != initiator_separator ) ) 
	{
		ret								=	*( u8* )s1 - *( u8* )s2;				
		++s1;
		++s2;
	}

	return									( ret < 0 );
}

bool   compare_nodes::operator ()		(node_base const & left, node_base const & right) const
{
	return									left.name < right.name;
}

bool   compare_nodes::operator ()		(pcstr const left, node_base const & right) const
{
	return									compare_parts(left, right.name.c_str());
}

bool   compare_nodes::operator ()		(node_base const & left, pcstr const right) const
{
	return									compare_parts(left.name.c_str(), right);
}