////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for Boolean_editor.xaml
	/// </summary>
	public partial class Boolean_editor
	{
		public					Boolean_editor		( )		
		{
			InitializeComponent( );

			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;

				m_property =  (property)DataContext;

				m_check_box.IsEnabled = !m_property.is_read_only;
			};
		}

		public override			void	update		( )		
		{
			m_property.invalidate_value( );
		}
	}
}
