////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace xray.editor.wpf_controls
{
	public class graph_node<TNodeData, TEdgeData> where TNodeData: new() where TEdgeData: new()
	{
		public	graph_node	( )
		{
			data				= new TNodeData( );
			m_edges				= new List<graph_edge<TNodeData, TEdgeData>>( );
			m_readonly_edges	= new ReadOnlyCollection<graph_edge<TNodeData, TEdgeData>>( m_edges );
		}
		public	graph_node	( TNodeData data )
		{
			this.data			= data;
			m_edges				= new List<graph_edge<TNodeData, TEdgeData>>( );
			m_readonly_edges	= new ReadOnlyCollection<graph_edge<TNodeData, TEdgeData>>( m_edges );
		}

		private readonly	List<graph_edge<TNodeData, TEdgeData>>					m_edges;
		private readonly	ReadOnlyCollection<graph_edge<TNodeData, TEdgeData>>	m_readonly_edges;

		public		TNodeData									data;

		public		Int32										index		
		{
			get;
			internal set;
		}
		public		ReadOnlyCollection<graph_edge<TNodeData, TEdgeData>>		edges		
		{
			get 
			{
				return m_readonly_edges;
			}
		}

		internal	void										add_edge	( graph_edge<TNodeData, TEdgeData> edge )		
		{
			m_edges.Add		( edge );
		}
		internal	void										remove_edge	( graph_edge<TNodeData, TEdgeData> edge )		
		{
			m_edges.Remove	( edge );
		}
	}
}
