////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

namespace xray.editor.wpf_controls
{
	public class graph_edge<TNodeData, TEdgeData> where TNodeData: new( ) where TEdgeData: new( )
	{
		public graph_edge( graph_node<TNodeData, TEdgeData> node_from, graph_node<TNodeData, TEdgeData> node_to )
		{
			m_node_from		= node_from;
			m_node_to		= node_to;
			m_data			= new TEdgeData( );
		}

		public graph_edge( graph_node<TNodeData, TEdgeData> node_from, graph_node<TNodeData, TEdgeData> node_to, TEdgeData data )
		{
			m_node_from		= node_from;
			m_node_to		= node_to;
			m_data			= data;
		}

		public	TEdgeData	m_data;

		public readonly		graph_node<TNodeData, TEdgeData>		m_node_from;
		public readonly		graph_node<TNodeData, TEdgeData>		m_node_to;
	}
}
