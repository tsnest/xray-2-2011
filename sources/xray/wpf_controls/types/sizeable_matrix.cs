////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;

namespace xray.editor.wpf_controls
{
	public class sizeable_matrix<TItem>
	{

		public sizeable_matrix	( )								
		{
			m_inner_matrix = new List<List<TItem>>( );
		}
		public sizeable_matrix	( Int32 width, Int32 height )	
		{
			m_width			= width;
			m_height		= height;

			m_inner_matrix	= new List<List<TItem>>( height );

			for( var i = 0; i < height; ++i )
			{
				m_inner_matrix.Add( new List<TItem>( width ) );

				for( var j = 0; j < width; ++j )
					m_inner_matrix[i].Add( default( TItem ) );
			}
		}

		private readonly		List<List<TItem>>			m_inner_matrix;
		private	Int32			m_width;
		private	Int32			m_height;

		public	TItem			this[Int32 x, Int32 y]		
		{
			get
			{
				return m_inner_matrix[y][x];
			}
			set
			{
				m_inner_matrix[y][x] = value;
			}
		}
		public	Int32			width						
		{
			get
			{
				return m_width;
			}
		}
		public	Int32			height						
		{
			get
			{
				return m_height;
			}
		}

		public	Boolean			is_item_exists				( Int32 x, Int32 y )						
		{
			if( y >= m_inner_matrix.Count || y < 0 )
				return false;

			if( x >= m_inner_matrix[y].Count || x < 0 )
				return false;

			if( !( typeof(TItem).IsValueType ) && m_inner_matrix[y][x] == null )
				return false;

			return true;
		}
		public	void			add_row						( )											
		{
			insert_row( m_height );
		}	
		public	void			add_row						( List<TItem> new_row  )					
		{
			insert_row( m_height, new_row );
		}	
		public	void			insert_row					( Int32 index )								
		{
			var new_row = new List<TItem>( m_width );
			m_inner_matrix.Insert( index, new_row );

			++m_height;

			for( var i = 0; i < m_width; ++i )
				new_row.Add( default( TItem ) );
		}
		public	void			insert_row					( Int32 index, List<TItem> new_row )		
		{
			if ( m_inner_matrix.Count == 0 )
				m_width = new_row.Count;

			if( new_row.Count != m_width )
				throw new ArgumentException( "new_row must contain " + m_width + " elements." );

			m_inner_matrix.Insert( index, new_row );

			++m_height;
		}
		public	void			remove_row_at				( Int32 index )								
		{
			m_inner_matrix.RemoveAt( index );
			--m_height;
		}
		public	void			add_column					( )											
		{
			insert_column( m_width );
		}
		public	void			add_column					( List<TItem> new_column )					
		{
			insert_column( m_width, new_column );
		}
		public	void			insert_column				( Int32 index )								
		{
			if( m_inner_matrix.Count == 0 )
				m_inner_matrix.Add( new List<TItem> ( ) );

			foreach( var row in m_inner_matrix )
			{
				row.Insert( index, default( TItem ) );
			}

			++m_width;

		}
		public	void			insert_column				( Int32 index, List<TItem> new_column )		
		{
			if( m_inner_matrix.Count == 0 )
			{
				for ( var i = 0; i < new_column.Count; ++i )
				{
					m_inner_matrix.Add(new List<TItem>());
				}
				
				m_height = new_column.Count;
			}

			if( new_column.Count != m_height )
				throw new ArgumentException( "new_column must contain " + m_height + " elements." );

			for( var i = 0; i < m_inner_matrix.Count; ++i )
				m_inner_matrix[i].Insert( index, new_column[i] );

			++m_width;

		}
		public	void			remove_column_at			( Int32 index )								
		{
			foreach( var row in m_inner_matrix )
				row.RemoveAt( index );

			--m_width;
		}
		public	void			clear						( )											
		{
			m_inner_matrix.Clear	( );
			m_width					= 0;
			m_height				= 0;
		}
	}
}
