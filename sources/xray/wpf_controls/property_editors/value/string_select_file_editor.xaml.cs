////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Linq;
using System.Windows;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for string_select_file_editor.xaml
	/// </summary>
	public partial class string_select_file_editor
	{
		public				string_select_file_editor	( )		
		{
			InitializeComponent();

			DataContextChanged += delegate 
			{
				if( DataContext == null )
					return;
				if( ( (property)DataContext ).is_read_only )
					Browse.IsEnabled		= false;
			};
		}

		private				void	browse_click		( Object sender, RoutedEventArgs e )		
		{
			m_property			= (property)DataContext;
			var attributes		= m_property.descriptors[0].Attributes;
			foreach ( var attribute in attributes.OfType<string_select_file_editor_attribute>( ) )
			{
				// Configure open file dialog box
				var dlg = new Microsoft.Win32.OpenFileDialog
	          	{
	          		Title				= attribute.caption,
	          		InitialDirectory	= attribute.default_folder,
	          		DefaultExt			= attribute.default_extension,
	          		Filter				= attribute.file_mask
	          	};

				// Show open file dialog box
				var result = dlg.ShowDialog( );

				// Process open file dialog box results
				if ( result == true )
					m_property.value = dlg.FileName;

				break;
			}

			file_path.Text = (String)m_property.value;
		}
		public override		void	update				( )											
		{
			file_path.Text = (String)m_property.value;
		}
	}
}
