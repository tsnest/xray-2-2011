//////////////////////////////////////////////////////////////////////////////
////	Created		: 20.12.2010
////	Author		: Evgeniy Obertyukh
////	Copyright (C) GSC Game World - 2010
//////////////////////////////////////////////////////////////////////////////

//using System;
//using System.Diagnostics;
//using System.Windows;

//namespace xray.editor.wpf_controls.hypergraph.commands
//{
//    class node_create_command: command
//    {
//        public node_create_command( hypergraph_control hypergraph, String node_type_name, Point position )
//        {
//            m_hypergraph		= hypergraph;
//            m_node_type_name	= node_type_name;
//            m_position			= position;
////			m_node_key			= "Node_" + node_type_name + ++hypergraph.nodes_count;
//        }
		
//        private hypergraph_control	m_hypergraph;
//        private String				m_node_type_name;
//        private String				m_node_key;
//        private node				m_node;
//        private Point				m_position;

//        public override bool		commit			( )		
//        {
//            m_node		= new node { node_type_name = m_node_type_name, key = m_node_key};
//            var args	= new node_event_args( m_node, m_node_type_name );
////			m_hypergraph.raise_node_adding( args );

//            if( args.m_is_successfull )
//            {
////				m_hypergraph.add_node( m_node );
//                m_node.position = m_position;
//                return true;
//            }

//            return false;
//        }

//        public override void		rollback		( )		
//        {
//            var args	= new node_event_args( m_node, m_node_type_name );
////			m_hypergraph.raise_node_removing( args );

//            Debug.Assert( args.m_is_successfull );
////			m_hypergraph.remove_node( m_node );
//        }
//    }
//}
