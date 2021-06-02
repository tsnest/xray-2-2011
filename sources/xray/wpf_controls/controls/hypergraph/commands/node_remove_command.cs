////////////////////////////////////////////////////////////////////////////
//	Created		: 20.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Diagnostics;
using System.Windows;

namespace xray.editor.wpf_controls.hypergraph.commands
{
	class node_remove_command: command
	{
		public node_remove_command( hypergraph_control hypergraph, properties_node properties_node )
		{
			m_hypergraph		= hypergraph;
			m_node_position		= properties_node.position;
			m_node_key			= properties_node.id;
		}

		private hypergraph_control	m_hypergraph;
		//private String				m_node_type_name;
		private String				m_node_key;
		private Point				m_node_position;	

		public override bool		commit			( )		
		{
			//return m_hypergraph.get_node( m_node_key ).remove( );
			return true;
		}
		public override void		rollback		( )		
		{
//            var node	= new node { node_type_name = m_node_type_name };
//            var args	= new node_event_args( node, m_node_type_name );
////			m_hypergraph.raise_node_adding( args );

//            Debug.Assert( args.m_is_successfull );
////			m_hypergraph.add_node( node );
//            node.position	= m_node_position;
		}
	}
}
