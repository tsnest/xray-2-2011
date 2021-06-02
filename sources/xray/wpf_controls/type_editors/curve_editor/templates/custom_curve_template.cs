////////////////////////////////////////////////////////////////////////////
//	Created		: 11.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using xray.editor.wpf_controls.curve_editor.templates;

namespace xray.editor.wpf_controls.type_editors.curve_editor.templates
{
	internal class custom_curve_template: curve_template_base
	{
		public	custom_curve_template	( float_curve template_curve )	
		{
			m_template_curve	= template_curve;
			m_success			= true;
		}

		private		float_curve			m_template_curve;

		protected override		void	initialize		( )		
		{
		}
		public override			void	commit			( )		
		{
			while( curve.keys.Count > m_template_curve.keys.Count )
				curve.keys[ curve.keys.Count - 1 ].remove( );

			for( var i = 0; i < m_template_curve.keys.Count; ++i )
				set_key( i, m_template_curve.keys[i] );	
		}
	}
}