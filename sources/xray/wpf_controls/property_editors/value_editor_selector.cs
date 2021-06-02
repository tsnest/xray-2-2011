////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using xray.editor.wpf_controls.property_editors.value;

namespace xray.editor.wpf_controls.property_editors
{
	public class value_editor_selector
	{
		public		value_editor_selector		( List<editor_picker> editors )		
		{
			m_editors = editors;
		}

		readonly				List<editor_picker>				m_editors;

		public					value_editor_base			select_editor	( property prop )								
		{
			Debug.Assert( prop != null );

			foreach ( var prop_editor in m_editors.Where( prop_editor => prop_editor.can_edit( prop ) ) )
				return (value_editor_base)Activator.CreateInstance( prop_editor.editor_type );

			return Activator.CreateInstance<default_editor>( );
		}
		public					value_editor_base			select_editor_without_extendion	( property prop )								
		{
			Debug.Assert( prop != null );

			foreach ( var prop_editor in m_editors.Where( prop_editor => prop_editor.can_edit( prop ) && !typeof( value_editor_extension_base ).IsAssignableFrom( prop_editor.editor_type ) ) )
				return (value_editor_base)Activator.CreateInstance( prop_editor.editor_type );

			return Activator.CreateInstance<default_editor>( );
		}
		public				void							select_sub_editors	( ItemsControl itemsCtrl, List<property> sub_properties )	
		{
			if( sub_properties == null )
				return;

			foreach( var property in sub_properties )
			{
				var editor				= select_editor( property );
				itemsCtrl.DataContext	= property;
				itemsCtrl.Items.Add		( editor );
				editor.DataContext		= property;
			}
		}
	}
}
