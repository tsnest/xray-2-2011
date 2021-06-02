////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows.Data;
using System.Windows;

namespace xray.editor.wpf_controls.property_editors
{
	class empty_collection_checher: IValueConverter
	{
		#region IValueConverter Members

		public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			var properties = ((property)value).sub_properties;

			if(properties != null)
				return (properties.Count != 0)?Visibility.Visible:Visibility.Collapsed;
			else
				return Visibility.Visible;
		}

		public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			throw new NotImplementedException();
		}

		#endregion
	}
}
