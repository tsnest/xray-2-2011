////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;

namespace xray.editor.wpf_controls.curve_editor
{
	/// <summary>
	/// Interaction logic for curve_control_key.xaml
	/// </summary>
	public partial class visual_curve_key
	{

		#region | Initialize |


		internal	visual_curve_key		( visual_curve parent_curve, float_curve_key key, Int32 index )			
		{
			
			InitializeComponent		( );

			this.index				= index;
			m_parent_curve			= parent_curve;
			m_key					= key;

			visual_position			= parent_curve.logical_to_visual_point( key.position );
			m_range_control			= new visual_curve_key_range_control( this );
			Children.Add			( m_range_control );

			SetValue( ZIndexProperty, 20 );
		}
		

		#endregion

		#region |   Fields   |


		internal const				Double				c_key_weight		= 1 / 3.0;
		internal static readonly	Vector				half_key_size		= new Vector( 5.5, 5.5 );
		public static				Double				key_width			= half_key_size.X * 2;
		public static				Double				key_height			= half_key_size.Y * 2;

		private				visual_curve				m_parent_curve;

		private				Int32						m_index;
		private				visual_curve_segment		m_left_segment;
		private				visual_curve_segment		m_right_segment;
		private				float_curve_key				m_key;
		private				Boolean						m_is_selected;

		private				visual_curve_key_range_control	m_range_control;
		private				visual_curve_key_tangent		m_left_tangent;
		private				visual_curve_key_tangent		m_right_tangent;


		#endregion

		#region | Properties |


		internal	visual_curve_segment		left_segment				
		{
			get		
			{
				return m_left_segment;
			}
			set
			{
				m_left_segment			= value;
			}
		}
		internal	visual_curve_segment		right_segment				
		{
			get
			{
				return m_right_segment;
			}
			set
			{
				m_right_segment			= value;
			}
		}
		internal	visual_curve_key_tangent	right_tangent				
		{
			get { 
				if( m_right_tangent == null )
					create_right_tangent( );

				return m_right_tangent; 
			}
		}
		internal	visual_curve_key_tangent	left_tangent				
		{
			get { 
				if( m_left_tangent == null )
					create_left_tangent( );

				return m_left_tangent; 
			}
		}

		internal	visual_curve_key_range_control	range_control			
		{
			get
			{
				return m_range_control;
			}
		}
		
		public		visual_curve				parent_curve				
		{
			get
			{
				return m_parent_curve; 
			}
			internal set
			{
				m_parent_curve = value;
			}
		}
		public		float_curve_key				key							
		{
			get 
			{
				return m_key; 
			}
		}
		public		visual_curve_key			prev_key					
		{
			get
			{
				return ( is_first_key ) ? null : m_parent_curve.keys[index - 1];
			}
		}
		public		visual_curve_key			next_key					
		{
			get
			{
				return ( is_last_key ) ? null : m_parent_curve.keys[index + 1];
			}
		}
		
		public		float_curve_key_type		type_of_key					
		{
			get 
			{
				return m_key.key_type; 
			}
			set 
			{
				m_key.key_type		= value; 
				compute_tangents	( );
				update_visual		( );
				on_key_changed		( );
			}
		}
		public		Int32						index						
		{
			get
			{
				return m_index;
			}
			internal set
			{
				m_index = value;
			}
		}

		public		Boolean						is_selected					
		{
			get	
			{
				return m_is_selected;
			}
			set 
			{
				m_is_selected = value;
				if ( value )
				{
					parent_curve.selected_keys.Add( this );
					selection_ellipse.Visibility = Visibility.Visible;
					show_tangents( );
					if( m_parent_curve.is_range )
						m_range_control.Visibility = Visibility.Visible;
				}
				else
				{
					parent_curve.selected_keys.Remove( this );
					selection_ellipse.Visibility = Visibility.Hidden;
					hide_tangents( );
					m_range_control.Visibility = Visibility.Hidden;
				}
			}
		}
		
		public		Boolean						is_first_key				
		{
			get
			{
				return parent_curve.keys[0] == this;
			}
		}
		public		Boolean						is_last_key					
		{
			get
			{
				return parent_curve.keys[parent_curve.keys.Count-1] == this;
			}
		}
		public		Visibility					range_control_visibility	
		{
			get
			{
				return m_range_control.Visibility;
			}
			set
			{
				m_range_control.Visibility = value;
			}
		}
		
		public		Point						position					
		{
			get
			{
				return m_key.position;
			}
			//set
			//{
			//    key.position		= value;
			//    visual_position		= parent_curve.logical_to_visual_point( value );

			//    on_key_changed( );
			//}
		}
		
		public		Double						position_x					
		{
			get
			{
				return m_key.position.X;
			}
			set
			{
				var pt = m_key.position;
				pt.X = value;
				move_to( pt );
			}
		}
		public		Double						position_y					
		{
			get
			{
				return m_key.position.Y;
			}
			set
			{
				var pt	= m_key.position;
				pt.Y	= value;
				move_to	( pt );
			}
		}
		public		Point						visual_position				
		{
			get
			{
				return parent_curve.logical_to_visual_point( m_key.position ); 
			}
			private set
			{
				SetValue( LeftProperty, value.X-Width/2);
				SetValue( TopProperty, value.Y-Height/2);
			}
		}
		
		public		Point						left_tangent_visual_point	
		{
			get
			{
				return parent_curve.logical_to_visual_point( m_key.left_tangent ); 
			}
		}
		public		Point						right_tangent_visual_point	
		{
			get
			{
				return parent_curve.logical_to_visual_point( m_key.right_tangent ); 
			}
		}


		#endregion

		#region |  Methods   |


		private				void		mouse_enter					( Object sender, MouseEventArgs e )			
		{
			dot_ellipse.Fill = Brushes.Yellow;
		}
		private				void		mouse_leave					( Object sender, MouseEventArgs e )			
		{
			dot_ellipse.Fill = Brushes.Red;
		}

		private				void		create_left_tangent			( )											
		{
			m_left_tangent					= new visual_curve_key_tangent( this, m_key.left_tangent, false );
			m_left_tangent.SetValue			( VisibilityProperty, Visibility.Hidden );
			Children.Add					( m_left_tangent );
		}
		private				void		create_right_tangent		( )											
		{
			m_right_tangent					= new visual_curve_key_tangent( this, m_key.right_tangent, true );
			m_right_tangent.SetValue		( VisibilityProperty, Visibility.Hidden );
			Children.Add					( m_right_tangent );
		}
		private				void		compute_tangents			( )											
		{
			//Расчет всех возможных тангентов
			switch (type_of_key)
			{
			case float_curve_key_type.locked_auto:
			case float_curve_key_type.locked_manual:
				{
					if ( !is_first_key && !is_last_key )
					{
						var vector = ( type_of_key == float_curve_key_type.locked_auto )?
							parent_curve.keys[index + 1].position - parent_curve.keys[index - 1].position:
							right_tangent.position - left_tangent.position;

						left_tangent.compute_tangent( -vector );
						right_tangent.compute_tangent( vector );
					}
					else
					{
						if ( !is_first_key )
							left_tangent.compute_tangent( );

						if ( !is_last_key )
							right_tangent.compute_tangent( );
					}
				}
				break;
			case float_curve_key_type.breaked:
				{
					if (!is_first_key)
						left_tangent.compute_tangent( );

					if (!is_last_key)
						right_tangent.compute_tangent( );
				}
				break;
			case float_curve_key_type.linear:
				{
					if ( !is_first_key )
						left_tangent.compute_tangent( (Vector)parent_curve.keys[m_index - 1].position - (Vector)position );

					if ( !is_last_key )
						right_tangent.compute_tangent( (Vector)parent_curve.keys[m_index + 1].position - (Vector)position );
				}
				break;
			case float_curve_key_type.step:
				{
					if ( !is_first_key )
						left_tangent.compute_tangent( new Vector( -1, 0 ) );

					if ( !is_last_key )
						right_tangent.compute_tangent( new Vector( 1, 0 ) );
				}
                m_key.key_type = float_curve_key_type.locked_manual;            
				break;
			default:
				throw new ArgumentOutOfRangeException();
			}

			on_key_changed( );
		}
		
		protected			void		on_key_changed				( )											
		{
			parent_curve.fire_key_changed( this );
		}
		protected			void		on_keys_swaped				( Int32 first_index, Int32 second_index )	
		{
			parent_curve.fire_key_swapped( first_index, second_index );
		}

		internal			void		set_key_type				( float_curve_key_type new_type )			
		{
			m_key.key_type = new_type;
		}
		internal			void		compute_key					( )											
		{
			compute_tangents	( );

			if ( !is_first_key )
			{
			    parent_curve.keys[m_index - 1].compute_tangents	( );
				parent_curve.keys[m_index - 1].update_visual	( );
			}

			if ( !is_last_key )
			{
			    parent_curve.keys[m_index + 1].compute_tangents	( );
				parent_curve.keys[m_index + 1].update_visual	( );
			}

			update_visual		( );
		}
		internal			void		show_tangents				( )											
		{
			if ( !is_first_key )
				left_tangent.Visibility		= Visibility.Visible;

			if ( !is_last_key )
				right_tangent.Visibility	= Visibility.Visible;
		}
		internal			void		hide_tangents				( )											
		{
			if (!is_first_key)
				left_tangent.Visibility		= Visibility.Hidden;

			if (!is_last_key)
				right_tangent.Visibility	= Visibility.Hidden;
		}
		public				void		hide_key_range_control		( )											
		{
			m_range_control.Visibility = Visibility.Collapsed;
		}
		internal			void		right_tangent_changed		( )											
		{
			m_right_segment.update_left( );
			on_key_changed( );
		}
		internal			void		left_tangent_changed		( )											
		{
			m_left_segment.update_right( );
			on_key_changed( );
		}
		internal			void		rage_changed				( )											
		{
			on_key_changed	( );
			if( is_first_key )
				parent_curve.update_first_segment_visual( );
			else if( m_parent_curve.range != null )
				m_parent_curve.range.update_right	( m_index - 1 );

			if( is_last_key )
				parent_curve.update_last_segment_visual( );
			else if( m_parent_curve.range != null )
				m_parent_curve.range.update_left	( m_index + 1 );
		}
		internal			void		remove						( )											
		{
			if( parent_curve != null )
			{
				parent_curve.remove_key( this );
			}
		}
		internal static		void		swap						( visual_curve_key left_key, visual_curve_key right_key )
		{
			left_key.hide_tangents		( );
			right_key.hide_tangents		( );

			var tmp_key					= new float_curve_key( );
			tmp_key.assign				( left_key.key );
			left_key.key.assign			( right_key.key );
			right_key.key.assign		( tmp_key );

			visual_curve_segment.swap_keys	( left_key.right_segment );
			utils.swap_values				( ref left_key.m_key,			ref right_key.m_key );
			utils.swap_values				( ref left_key.m_left_segment,	ref right_key.m_left_segment );
			utils.swap_values				( ref left_key.m_right_segment,	ref right_key.m_right_segment );
			utils.swap_values				( ref left_key.m_index,			ref right_key.m_index );

			left_key.parent_curve.keys[left_key.index]	= left_key;
			left_key.parent_curve.keys[right_key.index]	= right_key;

			left_key.compute_key	( );
			right_key.compute_key	( );

			if( left_key.is_last_key || right_key.is_last_key )
				left_key.parent_curve.update_last_segment_visual( );

			if( left_key.is_first_key || right_key.is_first_key )
				left_key.parent_curve.update_first_segment_visual( );

			if( left_key.is_selected )
				left_key.show_tangents	( );

			if( right_key.is_selected )
				right_key.show_tangents	( );
			
			left_key.on_keys_swaped( left_key.index, right_key.index );
		}
		internal			void		move_to_without_swaping		( Point new_point )							
		{
			var left_tangent_tmp	= m_key.left_tangent - m_key.position;
			var right_tangent_tmp	= m_key.right_tangent - m_key.position;

			m_key.position			= new Point( new_point.X, new_point.Y);
			m_key.left_tangent		= m_key.position + left_tangent_tmp;
			m_key.right_tangent		= m_key.position + right_tangent_tmp;

			if( !is_first_key && key.position.X == parent_curve.keys[index - 1].position.X )
				key.position_x += math.epsilon_5;
			
			if( !is_last_key && key.position.X == parent_curve.keys[index + 1].position.X )
				key.position_x -= math.epsilon_5;

			compute_key	( );
		}

		public				void		flat_tangents				( )											
		{
			if( !is_first_key ) 
				left_tangent.compute_tangent	( new Vector( -1, 0 ) );

			if( !is_last_key )
				right_tangent.compute_tangent	( new Vector( 1, 0 ) );
		}
		public				void		raise_key_changed			( )											
		{
			on_key_changed( );
		}
		public				void		process_selection			( )											
		{
			var shift_pressed	= Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.LeftShift);
			var ctrl_pressed	= Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.LeftCtrl);

			if (shift_pressed && ctrl_pressed)
				is_selected = true;
			else if (ctrl_pressed)
				is_selected = false;
			else if (shift_pressed)
				is_selected = !is_selected;
			else
				is_selected = true;

			return;
		}
		public				void		move_to						( Point new_point )							
		{
			var left_tangent_tmp	= m_key.left_tangent - m_key.position;
			var right_tangent_tmp	= m_key.right_tangent - m_key.position;

			m_key.position			= new Point( new_point.X, new_point.Y);
			m_key.left_tangent		= m_key.position + left_tangent_tmp;
			m_key.right_tangent		= m_key.position + right_tangent_tmp;

			//swap left
			while( !is_first_key && key.position.X < parent_curve.keys[index - 1].position.X )
				swap( parent_curve.keys[index - 1], this );

			//swap right
			while( !is_last_key && key.position.X > parent_curve.keys[index + 1].position.X )
				swap( this, parent_curve.keys[index + 1] );

			if( !is_first_key && key.position.X == parent_curve.keys[index - 1].position.X )
				key.position_x += math.epsilon_5;
			
			if( !is_last_key && key.position.X == parent_curve.keys[index + 1].position.X )
				key.position_x -= math.epsilon_5;

			compute_key	( );
		}
		public				void		update_visual				( )											
		{
			visual_position						= parent_curve.logical_to_visual_point( m_key.position );
			
			if ( !is_first_key )
			{
				left_tangent.update_visual		( );
				m_left_segment.update_right		( );
			}
			if ( !is_last_key )
			{
				right_tangent.update_visual		( );
				m_right_segment.update_left		( );
			}
			if( m_range_control != null )
				m_range_control.update_visual	( );

			if( is_first_key )
				parent_curve.update_first_segment_visual( );
			if( is_last_key )
				parent_curve.update_last_segment_visual( );
		}
		public override		String		ToString					( )											
		{
			return parent_curve + " key: " + index;
		}


		#endregion

	}
}