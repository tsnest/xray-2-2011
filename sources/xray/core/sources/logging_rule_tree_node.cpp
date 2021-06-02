////////////////////////////////////////////////////////////////////////////
//	Created 	: 01.09.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "logging_rule_tree_node.h"
#include "logging.h"
#include "logging_path_parts.h"

using xray::logging::rule_tree::node;
using xray::logging::path_parts;

node::~node				()
{
}

void node::set			(pcstr initiator_path, int rule)
{
	if ( !initiator_path || !*initiator_path )
	{
		m_verbosity = rule & ~recurse_0;
		if ( !(rule & recurse_0) )
		{
			clean();
		}
		return;
	}

	Folder::iterator it = m_folder.find( initiator_path );

	pcstr next_path_portion = strchr(initiator_path, initiator_separator);

	node* child;
	if ( it != m_folder.end() )
	{
		child = it->second;
	}
	else
	{
		child = LOG_NEW(node)(0);
		m_folder.insert( std::make_pair(initiator_path, child) );
	}
	
	return child->set(next_path_portion ? next_path_portion+1 : NULL, rule);
}

void node::clean		()
{
	Folder::iterator	i = m_folder.begin( );
	Folder::iterator	e = m_folder.end( );
	for ( ; i != e; ++i ) {
		(*i).second->clean	( );
		LOG_DELETE		( (*i).second );
	}

	m_folder.clear		( );

//	delete_data	(m_folder);
}

int node::get_verbosity	(path_parts* path, int inherited_verbosity)
{
	int verbosity = m_verbosity != 0 ? m_verbosity : inherited_verbosity;

	pcstr cur_part = path->get_current_element();
	if ( !cur_part || cur_part[0] == NULL )
	{
		// last element in path
		return verbosity;		
	}

	Folder::iterator it = m_folder.find( cur_part );

	if ( it == m_folder.end() ) // no special rule?
	{
		return verbosity;
	}

	path->to_next_element();
	return it->second->get_verbosity(path, verbosity);
}