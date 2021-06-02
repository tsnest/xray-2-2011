////////////////////////////////////////////////////////////////////////////
//	Created		: 16.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray.editor.wpf_controls.hypergraph.commands
{
	class unlink_command: command
	{
		public unlink_command( hypergraph_control hypergraph, link link )
		{
			//m_hypergraph	= hypergraph;
			//m_link_source	= link.output;
			////if( link.input is node )
			////    m_link_node		= link.input.node;
			////else
			////    m_link_dest		= link.output;

			//m_link = link;
		}

		//readonly hypergraph_control		m_hypergraph;
		//readonly link_point				m_link_source;
		//readonly link					m_link;
		//link_point						m_link_dest;
		//node							m_link_node;

		public override bool commit( )
		{
			//return m_link.remove( );
			return true;
		}

		public override void rollback( )
		{
			//System.Diagnostics.Debug.Assert( m_link_source.try_link( m_link ) != null , "rollback can not fail" );
//			m_hypergraph.add_edge( m_link );
		}
	}
}
