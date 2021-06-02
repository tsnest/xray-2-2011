////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2010
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
	/// Interaction logic for visual_curve_key_tangent.xaml
	/// </summary>
	internal partial class visual_curve_key_tangent
	{

		#region | Initialize |


		public visual_curve_key_tangent( visual_curve_key key, Point position, Boolean is_right )	
		{
			InitializeComponent();

			m_is_right					= is_right;
			parent_key					= key;
			this.position				= position;
			m_path_line_segment.Point	= (Vector)parent_key.visual_position - (Vector)visual_position + new Point( Width / 2, Height / 2 );

			SetValue( ZIndexProperty, 100 );
		}
		

		#endregion

		#region |   Fields   |


		private const		Int32				c_visual_tangent_length		= 40;
		private const		Double				c_epsilon					= 0.0001;

		private				Boolean				m_is_selected;
		private readonly	Boolean				m_is_right;


		#endregion

		#region | Properties |


		private		Vector				relative_tangent	
		{
			get
			{
				if( m_is_right )
					return parent_key.key.right_tangent - parent_key.position;
				
				return parent_key.key.left_tangent - parent_key.position;
			}
		}

		internal	Boolean				is_selected			
		{
			get	
			{
				return m_is_selected;
			}
			set 
			{
				m_is_selected = value;
				if (value)
				{
					m_tangent_ellipse.Fill	= Brushes.Yellow;
					m_tangent_line.Stroke	= Brushes.Yellow;
					parent_key.parent_curve.selected_tangents.Add( this );
				}
				else
				{
					m_tangent_ellipse.Fill	= Brushes.Brown;
					m_tangent_line.Stroke	= Brushes.Brown;
					parent_key.parent_curve.selected_tangents.Remove( this );
				}
			}
		}
		internal	visual_curve_key	parent_key			
		{
			get;
			private set;
		}
		internal	Point				position			
		{
			get
			{
				return m_is_right ? parent_key.key.right_tangent : parent_key.key.left_tangent;
			}
			set
			{
				if( m_is_right )
					parent_key.key.right_tangent = value;
				else
					parent_key.key.left_tangent = value;

				visual_position = parent_key.parent_curve.logical_to_visual_point( value );
			}
		}
		internal	Point				visual_position		
		{
			get
			{
				return new Point(
					(Double)GetValue( LeftProperty) + Width / 2, 
					(Double)GetValue( TopProperty) + Height / 2);
			}
			set
			{
				SetValue( LeftProperty, value.X - Width / 2);
				SetValue( TopProperty, value.Y - Height / 2);
			}
		}
		internal	Double				x_offset			
		{
			get
			{
				if( m_is_right )
				{
					var keys = parent_key.parent_curve.keys;
					return ( keys[parent_key.index + 1].position.X - parent_key.position.X) * visual_curve_key.c_key_weight;
				}
				else
				{
					var keys = parent_key.parent_curve.keys;
					return ( parent_key.position.X - keys[parent_key.index - 1].position.X ) * visual_curve_key.c_key_weight;
				}
			}
		}
	

		#endregion

		#region |   Methods  |


		private		void		tangent_mouse_enter				( Object sender, MouseEventArgs e )						
		{
			m_tangent_ellipse.Fill = Brushes.Yellow;
			m_tangent_line.Stroke = Brushes.Yellow;
		}
		private		void		tangent_mouse_leave				( Object sender, MouseEventArgs e )						
		{
			if (!m_is_selected)
			{
				m_tangent_ellipse.Fill = Brushes.Brown;
				m_tangent_line.Stroke = Brushes.Brown;
			}
		}
		private		void		tangent_is_visible_changed		( Object sender, DependencyPropertyChangedEventArgs e )	
		{
			if (Visibility == Visibility.Hidden)
				is_selected = false;
		}

		internal	void		compute_tangent_as				( float_curve_key_type type_of_key )						
		{
			
			if( !( parent_key.type_of_key == float_curve_key_type.linear && type_of_key == float_curve_key_type.linear ) )
				parent_key.type_of_key = float_curve_key_type.breaked;

			switch ( type_of_key )
			{
			case float_curve_key_type.locked_auto:
				{
					if( parent_key.is_first_key || parent_key.is_last_key )
						break;

					var vector = parent_key.prev_key.position - parent_key.next_key.position;
					if( m_is_right )
						vector = -vector;

					compute_tangent( vector );
				}
				break;
			case float_curve_key_type.linear:
				{
					if( m_is_right )
						compute_tangent	( parent_key.next_key.position - parent_key.position );
					else
						compute_tangent	( parent_key.prev_key.position - parent_key.position );
				}
				break;
			case float_curve_key_type.step:
				{
					compute_tangent( new Vector( -1, 0 ) );
				}
				break;
			default:
				throw new ArgumentOutOfRangeException();
			}

			parent_key.update_visual( );
			parent_key.raise_key_changed( );
		}	
		internal	void		compute_tangent					( )														
		{
			var tangent_pos		= position;
			var tangent_vector	= (Vector) tangent_pos - (Vector) parent_key.position;
			compute_tangent		( tangent_vector );
		}
		internal	void		compute_tangent					( Vector new_tangent_vector )							
		{
			if( m_is_right && new_tangent_vector.X <= c_epsilon )
				new_tangent_vector.X = c_epsilon;
			else if( !m_is_right && new_tangent_vector.X >= -c_epsilon )
				new_tangent_vector.X = -c_epsilon;

			new_tangent_vector		= new_tangent_vector * Math.Abs( x_offset / new_tangent_vector.X );
			
			if( m_is_right )
				parent_key.key.right_tangent			= new_tangent_vector + parent_key.position;
			else
				parent_key.key.left_tangent				= new_tangent_vector + parent_key.position;

		}
		internal	void		move_to							( Point new_position )									
		{
			if(	parent_key.type_of_key == float_curve_key_type.locked_auto )
				parent_key.set_key_type( float_curve_key_type.locked_manual );

			if(	parent_key.type_of_key == float_curve_key_type.linear )
				parent_key.set_key_type( float_curve_key_type.breaked );

			var tangent_vector		= (Vector)new_position - (Vector)parent_key.position;

			compute_tangent		( tangent_vector );
			update_visual		( );

			if ( parent_key.type_of_key == float_curve_key_type.locked_manual )
			{
				if ( m_is_right && !parent_key.is_first_key )
					parent_key.left_tangent.compute_tangent( -tangent_vector );

				else if ( !m_is_right && !parent_key.is_last_key )
					parent_key.right_tangent.compute_tangent( -tangent_vector );

				parent_key.update_visual( );
			}

			if( m_is_right )
				parent_key.right_tangent_changed( );
			else
				parent_key.left_tangent_changed	( );
		}

		internal	void		process_selection				( )														
		{
			parent_key.show_tangents( );
			var shift_pressed	= Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift);
			var ctrl_pressed	= Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl);

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
		internal	void		update_visual					( )														
		{
			var tangent_vector	= relative_tangent;

			tangent_vector.X	*= parent_key.parent_curve.parent_panel.scale.X;
			tangent_vector.Y	*= parent_key.parent_curve.parent_panel.scale.Y;

			tangent_vector.Normalize( );
			var new_this_pos	= tangent_vector * c_visual_tangent_length;
			new_this_pos.Y		= -new_this_pos.Y;

			visual_position						= (Point)new_this_pos + visual_curve_key.half_key_size;
			m_path_line_segment.Point			= -(Vector)visual_position + new Point( Width / 2, Height / 2 ) + visual_curve_key.half_key_size;
		}


		#endregion

		
	}
}
