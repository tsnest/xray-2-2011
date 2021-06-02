////////////////////////////////////////////////////////////////////////////
//	Created		: 04.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.curve_editor.templates
{
	public class curve_template_base: ContentControl
	{
		public curve_template_base( )
		{
			m_template_keys		= new List<visual_curve_key>( );
		}

		private					List<visual_curve_key>	m_template_keys;
		protected				Boolean					m_success;

		protected				float_curve				m_old_curve;
		protected				visual_curve			m_curve;
		protected				Double					m_left_limit;
		protected				Double					m_right_limit;
		protected				visual_curve_key		m_pre_template_key;
		protected				visual_curve_key		m_post_template_key;

		protected				Boolean					m_is_manual_left_limit;
		protected				Boolean					m_is_manual_right_limit;

		protected				List<visual_curve_key>	template_keys		
		{
			get
			{
				return m_template_keys;
			}
		}
		public					visual_curve			curve				
		{
			get 
			{
				return m_curve; 
			}
			set 
			{
				m_curve			= value;
				m_old_curve		= curve.float_curve.clone( );
				initialize		( );
			}
		}

		protected virtual		void			initialize			( )											
		{
			m_template_keys.AddRange	( curve.selected_keys );

			visual_curve_key first_key	= null;
			visual_curve_key last_key	= null;

			if( template_keys.Count > 0 )
			{
				first_key		= m_template_keys[0];
				last_key		= m_template_keys[0];

				foreach( var key in m_template_keys )
				{
					if( first_key.index > key.index )
						first_key = key;

					if( last_key.index < key.index )
						last_key = key;
				}
			}

			if( first_key != null )
				m_left_limit			= first_key.position_x;
			else if( ! Double.IsNaN( curve.parent_panel.field_left_limit ) )
				m_left_limit			= curve.parent_panel.field_left_limit;
			else
			{
				m_left_limit			= 0;
				m_is_manual_left_limit	= true;
			}


			if( last_key != null && last_key != first_key )
				m_right_limit			= last_key.position_x;
			else if( ! Double.IsNaN( curve.parent_panel.field_right_limit ) )
				m_right_limit			= curve.parent_panel.field_right_limit;
			else 
			{
                m_right_limit			= m_left_limit + 100;
				m_is_manual_right_limit	= true;
			}
			

			if( m_template_keys.Count == 0 )
			{
				m_template_keys.AddRange	( curve.keys );
				m_template_keys.Sort		( ( key1, key2 ) => key1.index - key2.index );
			}
			else if( m_template_keys.Count >= 2 )
			{
				m_template_keys.Sort		( ( key1, key2 ) => key1.index - key2.index );
				m_pre_template_key			= m_template_keys[0];
				m_post_template_key			= m_template_keys[m_template_keys.Count - 1];
				m_template_keys.RemoveAt	( 0 );
				m_template_keys.RemoveAt	( m_template_keys.Count - 1 );
			}
			else if( m_template_keys.Count == 1 )
			{
				var key					= m_template_keys[0];
				m_pre_template_key		= key;
				m_template_keys.Clear	( );

				for( var i = key.index + 1; i < curve.keys.Count; ++i )
					m_template_keys.Add( curve.keys[i] );

				m_template_keys.Sort		( ( key1, key2 ) => key1.index - key2.index );
			}
		}
		private					void			window_closed		( Object sender, EventArgs e )				
		{
			if( !m_success )
				return_old_curve( );			
		}
		private					void			return_old_curve	( )											
		{
			while( curve.keys.Count > m_old_curve.keys.Count )
				curve.keys[ curve.keys.Count - 1 ].remove( );

			for( var i = 0; i < m_old_curve.keys.Count; ++i )
				set_key( i, m_old_curve.keys[i] );	
		}

		protected				void			set_key				( Int32 key_index, float_curve_key key )	
		{
			var visual_key		= key_index >= curve.keys.Count ? curve.add_key	( key.position_x ) : curve.keys[key_index];
			
			visual_key.type_of_key					= key.key_type;
			visual_key.range_control.range_delta	= key.range_delta;

			if( !visual_key.is_first_key )
				visual_key.left_tangent.position	= key.left_tangent;

			if( !visual_key.is_last_key )
				visual_key.right_tangent.position	= key.right_tangent;

			visual_key.move_to_without_swaping		( key.position );

			visual_key.is_selected					= false;
		}
		protected				void			cancel_click		( Object sender, RoutedEventArgs e )		
		{
			utils.get_parent_form( this ).Close		( );
		}
		protected				void			ok_click			( Object sender, RoutedEventArgs e )		
		{
			m_success	= true;
			utils.get_parent_form( this ).Close		( );
		}
		protected				void			set_key				( Int32 key_index, Double x, Double y )		
		{
			visual_curve_key key;
			if( key_index >= m_template_keys.Count )
			{
				key = ( x == m_left_limit )
					? curve.add_key	( x + math.epsilon_3 ) 
					: curve.add_key	( x );

				m_template_keys.Add	( key );
			}
			else
			{
				key = m_template_keys[key_index];
			}
			key.type_of_key		= float_curve_key_type.locked_manual;
			key.flat_tangents	( );
			key.move_to			( new Point( x, y ) );
		}
		protected	 			void			set_key				( Int32 key_index, Double x, Double y, Action<visual_curve_key> key_modifier )		
		{
			visual_curve_key key;
			if( key_index >= template_keys.Count )
			{
				key = ( x == m_left_limit )
					? curve.add_key	( x + math.epsilon_3 ) 
					: curve.add_key	( x );

				template_keys.Add( key );
			}
			else
			{
				key = template_keys[key_index];
			}
			key_modifier		( key );
			key.move_to			( new Point( x, y ) );
		}

		protected				void			trim_keys			( Int32 need_keys_count )					
		{
			while( template_keys.Count > need_keys_count )
			{
				template_keys[template_keys.Count-1].remove( );
				template_keys.RemoveAt( template_keys.Count-1 );
			}
		}

		public	virtual			void			commit				( )											
		{
		}
		public	virtual			void			show_settings		( )											
		{
			utils.get_parent_form( this ).Closed	+= window_closed;
		}
	}
}