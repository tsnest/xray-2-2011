namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for Enum_editor.xaml
	/// </summary>
	public partial class Enum_editor
	{
		public				Enum_editor		( )		
		{
			InitializeComponent( );

			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;

				m_property = (property)DataContext;

				combo_box.ItemsSource = System.Enum.GetNames( m_property.type );
				
				if( m_property.is_read_only )
					combo_box.IsEnabled = false;
			};
		}

		public override		void	update	( )		
		{
			m_property.invalidate_value( );
		}
	}
}
