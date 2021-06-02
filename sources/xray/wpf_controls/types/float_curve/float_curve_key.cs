////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;

namespace xray.editor.wpf_controls
{
	public class float_curve_key
	{

		#region | Initialize |


		public		float_curve_key		( ): this( new Point( 0, 0 ) )					
		{
		}
		public		float_curve_key		( Point position ): this( position, new Point( position.X - math.epsilon_5, position.Y ), new Point( position.X + math.epsilon_5, position.Y ) )	
		{
		}
		public		float_curve_key		( Point position, float_curve_key_type key_type ): this( position, new Point( position.X - math.epsilon_5, position.Y ), new Point( position.X + math.epsilon_5, position.Y ) )	
		{
			m_key_type		= key_type;
		}
		public		float_curve_key		( Point position, Point left_tangent, Point right_tangent )
		{
			m_position		= position;
			m_right_tangent = right_tangent;
			m_left_tangent	= left_tangent;
			m_key_type		= float_curve_key_type.linear;
		}
		public		float_curve_key		( Point position, float_curve_key_type key_type, Point left_tangent, Point right_tangent )
		{
			m_position		= position;
			m_right_tangent = right_tangent;
			m_left_tangent	= left_tangent;
			m_key_type		= key_type;
		}

		public		float_curve_key		( Point position, float_curve_key_type key_type, Point left_tangent, Point right_tangent, Single range_delta )
		{
			m_position		= position;
			m_right_tangent = right_tangent;
			m_left_tangent	= left_tangent;
			m_key_type		= key_type;
			m_range_delta	= range_delta;
		}


		#endregion

		#region |   Fields   |

		
		private			Point					m_position;
		private			Point					m_right_tangent;
		private			Point					m_left_tangent;
		private			Single					m_range_delta;
		private			float_curve_key_type	m_key_type;


		#endregion

		#region | Properties |


		public		Point					position					
		{
			get { return m_position; }
			set { m_position = value; on_key_changed( ); }
		}
		public		Point					left_tangent				
		{
			get { return m_left_tangent; }
			set { m_left_tangent = value; on_key_changed(); }
		}
		public		Point					right_tangent				
		{
			get { return m_right_tangent; }
			set { m_right_tangent = value; on_key_changed( ); }
		}
		public		Single					range_delta					
		{
			get
			{
				return m_range_delta;
			}
			set
			{
				m_range_delta  = value;
			}
		}
		public		Single					range_up_delta				
		{
			get
			{
				return m_range_delta;
			}
			set
			{
				m_range_delta  = value;
			}
		}
		public		Single					range_down_delta			
		{
			get
			{
				return m_range_delta;
			}
			set
			{
				m_range_delta  = value;
			}
		}
		public		float_curve_key_type	key_type					
		{
			get { return m_key_type; }
			set { m_key_type = value; on_key_changed(); }
		}

		public		Double					position_x					
		{
			get { return m_position.X; }
			set { m_position.X = value; }
		}
		public		Double					position_y					
		{
			get { return m_position.Y; }
			set { m_position.Y = value; }
		}
		

		#endregion

		#region |   Events   |


		public event		Action		key_changed;


		#endregion

		#region |   Methods  |


		protected			void		on_key_changed				( )							
		{
			if(key_changed != null)
				key_changed();
		}

		public				void		fire_key_changed			( )							
		{
			on_key_changed( );
		}
		public				void		assign						( float_curve_key new_key )	
		{
			m_position		= new_key.m_position;
			m_left_tangent	= new_key.m_left_tangent;
			m_right_tangent	= new_key.m_right_tangent;
			m_range_delta	= new_key.m_range_delta;
			on_key_changed	( );
		}
		public override		String		ToString					( )							
		{
			return "pos: "+m_position.X+";"+m_position.Y+", left: "+m_left_tangent.X+";"+m_left_tangent.Y+", right: "+m_right_tangent.X+";"+m_right_tangent.Y;
		}
		public				void		set_identity_right_tangent	( )							
		{
			right_tangent = new Point( position_x + math.epsilon_3, position_y );
		}
		public				void		set_identity_left_tangent	( )							
		{
			left_tangent = new Point( position_x - math.epsilon_3, position_y );
		}


		#endregion

	}
}
