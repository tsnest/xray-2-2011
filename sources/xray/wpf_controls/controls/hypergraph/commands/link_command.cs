// ////////////////////////////////////////////////////////////////////////////
////	Created		: 16.12.2010
////	Author		: Evgeniy Obertyukh
////	Copyright (C) GSC Game World - 2010
//////////////////////////////////////////////////////////////////////////////

//using System;

//namespace xray.editor.wpf_controls.hypergraph.commands
//{
//    class link_command: command	
//    {
//        public link_command( hypergraph_control hypergraph, property_link_place link_source, property_link_place link_dest )
//        {
//            m_hypergraph	= hypergraph;
//            m_link_source	= link_source;
//            m_link_dest		= link_dest;
//            //m_link_key		= "Link_" + ++hypergraph.links_count;
//            m_source_path	= link_source.full_path;
//            m_dest_path		= link_dest.full_path;
//        }

//        public link_command( hypergraph_control hypergraph, property_link_place link_source, node link_node )
//        {
//            m_hypergraph	= hypergraph;
//            m_link_source	= link_source;
//            m_link_node		= link_node;
//            //m_link_key		= "Link_" + ++hypergraph.links_count;
//            m_source_path	= link_source.full_path;
//            m_dest_path		= link_node.key;
//        }

//        String					m_link_key;
//        String					m_source_path;
//        String					m_dest_path;
//        hypergraph_control		m_hypergraph;
//        property_link_place		m_link_source;
//        property_link_place		m_link_dest;
//        node					m_link_node;
//        link					m_link;

//        public override bool commit( )
//        {
//            if( m_link == null )
//            {
//                //m_link		= ( m_link_dest != null ) ? m_link_source.try_link_to( m_link_dest ) : m_link_source.try_link_to( m_link_node ) ;
//                m_link.key	= m_link_key;
//            }
//            else
//                //m_link_source.try_link( m_link );

//            if( m_link != null )
//            {
//                //m_hypergraph.add_edge( m_link );
//                return true;
//            }
//            return false;
//        }

//        public override void rollback( )
//        {
//            //System.Diagnostics.Debug.Assert( m_link.remove( ), "rollback can not fail" );
//        }
//    }
//}
