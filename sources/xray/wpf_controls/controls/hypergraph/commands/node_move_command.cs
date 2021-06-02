////////////////////////////////////////////////////////////////////////////
//	Created		: 16.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;

namespace xray.editor.wpf_controls.hypergraph.commands
{
	class node_move_command: command
	{
		public node_move_command( hypergraph_control hypergraph, properties_node properties_node, Point old_position, Point new_position )
		{
			m_hypergraph	= hypergraph;
			m_node_key		= properties_node.id;
			m_old_position	= old_position;
			m_new_position	= new_position;
		}

		hypergraph_control	m_hypergraph;
		String				m_node_key;
		Point				m_new_position;
		Point				m_old_position;
		Boolean				has_runed;

		public override bool commit()
		{
			if( !has_runed )
			{
				has_runed = true;
				return true;
			}
			m_hypergraph.get_node(m_node_key).position = m_new_position;
			return true;
		}

		public override void rollback()
		{
			m_hypergraph.get_node(m_node_key).position = m_old_position;
		}
	}
}
