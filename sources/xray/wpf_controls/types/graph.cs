////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace xray.editor.wpf_controls
{
	public class graph<TNodeData, TEdgeData>: IEnumerable<graph_node<TNodeData, TEdgeData>> where TNodeData: new() where TEdgeData: new()
	{
		public		graph	( )		
		{
			m_nodes				= new List<graph_node<TNodeData, TEdgeData>>( );
			m_links				= new List<graph_edge<TNodeData, TEdgeData>>( );
			m_readonly_nodes	= new ReadOnlyCollection<graph_node<TNodeData, TEdgeData>>( m_nodes );
			m_readonly_links	= new ReadOnlyCollection<graph_edge<TNodeData, TEdgeData>>( m_links );
		}

		private readonly	List<graph_node<TNodeData, TEdgeData>>					m_nodes;
		private readonly	List<graph_edge<TNodeData, TEdgeData>>					m_links;
		private readonly	ReadOnlyCollection<graph_node<TNodeData, TEdgeData>>	m_readonly_nodes;
		private readonly	ReadOnlyCollection<graph_edge<TNodeData, TEdgeData>>	m_readonly_links;

		public				ReadOnlyCollection<graph_node<TNodeData, TEdgeData>>	nodes				
		{
			get
			{
				return m_readonly_nodes;
			}
		}
		public				ReadOnlyCollection<graph_edge<TNodeData, TEdgeData>>	links				
		{
			get
			{
				return m_readonly_links;
			}
		}

		private				void						fix_nodes_indices	( Int32 start_index )							
		{
			var count	= m_nodes.Count;
			for( var i = start_index; i < count; ++i )
				--m_nodes[i].index;
		}
		public				void						add_node			( graph_node<TNodeData, TEdgeData> node )		
		{
			node.index		= m_nodes.Count;
			m_nodes.Add		( node );
		}
		public				void						remove_node			( graph_node<TNodeData, TEdgeData> node )		
		{
			remove_node_at( node.index );

			var count = node.edges.Count;
			for( var i = count - 1; i >= 0; --i )
				remove_link( node.edges[i] );
		}
		public				void						remove_node_at		( Int32 index )									
		{
			m_nodes.RemoveAt		( index );
			fix_nodes_indices		( index );
		}
		public				void						add_link			( graph_node<TNodeData, TEdgeData> from, graph_node<TNodeData, TEdgeData> to )	
		{
			add_link( new graph_edge<TNodeData, TEdgeData>( from, to ) );
		}
		public				void						add_link			( graph_edge<TNodeData, TEdgeData> edge )		
		{
			m_links.Add						( edge );
			edge.m_node_from.add_edge		( edge );
			edge.m_node_to.add_edge			( edge );
		}
		public				void						remove_link			( graph_node<TNodeData, TEdgeData> from, graph_node<TNodeData, TEdgeData> to )	
		{
			var count = from.edges.Count;
			for( var i = 0; i < count; i++ )
			{
				if( from.edges[i].m_node_from == from && from.edges[i].m_node_to == to )
				{
					remove_link( from.edges[i] );
					return;
				}
			}
			
		}
		public				void						remove_link			( graph_edge<TNodeData, TEdgeData> edge )		
		{
			m_links.Remove					( edge );
			edge.m_node_from.remove_edge	( edge );
			edge.m_node_to.remove_edge		( edge );
		}

		public IEnumerator<graph_node<TNodeData, TEdgeData>>			GetEnumerator		( )		
		{
			return nodes.GetEnumerator( );
		}
		IEnumerator IEnumerable.GetEnumerator								( )		
		{
			return GetEnumerator( );
		}
	}
}
