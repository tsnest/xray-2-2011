using System;

namespace xray.editor.wpf_controls
{
	public class color_curve_key
	{

		#region |   Events   |


		public event Action key_changed;

		
		#endregion

		#region | Initialize |

		
		public		color_curve_key		( Single position, color_rgb color )	
		{
			m_position		= position;
			m_color			= color;
		}

		
		#endregion

		#region |   Fields   |

		
		private		Single				m_position;
		private		color_rgb			m_color;


		#endregion

		#region | Properties |

		
		public		Single				position						
		{
			get { return m_position; }
			set { m_position = value; on_key_changed(); }
		}
		public		color_rgb			color							
		{
			get { return m_color; }
			set { m_color = value; on_key_changed(); }
		}


		#endregion

		#region |   Methods  |


		protected			void		on_key_changed	()							
		{
			if(key_changed != null)
				key_changed();
		}
		public				void		assign			(color_curve_key new_key)	
		{
			position		= new_key.position;
			color			= new_key.color;
		}
		public override		String		ToString		()							
		{
			return "p: "+m_position+" c: " + m_color;
		}


		#endregion

	}
}
