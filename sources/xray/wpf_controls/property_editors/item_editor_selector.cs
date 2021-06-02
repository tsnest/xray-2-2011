////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using xray.editor.wpf_controls.property_editors.item;

namespace xray.editor.wpf_controls.property_editors
{
	public class item_editor_selector
	{
		public		item_editor_selector			( List<editor_picker> item_editors )										
		{
			m_item_editors = item_editors;
		}
		public		item_editor_selector			( List<editor_picker> item_editors, Type default_editor_type )				
		{
			m_item_editors			= item_editors;
			m_default_editor_type	= default_editor_type;
		}
		
		private readonly	List<editor_picker>		m_item_editors;
		private readonly	Type					m_default_editor_type;

		public				item_editor_base		select_editor		( Object item )											
		{
			var prop = (property) item;
			
			item_editor_base ret_editor;

			foreach ( var item_editor in m_item_editors.Where( item_editor => item_editor.can_edit( prop ) ) )
			{
				ret_editor = (item_editor_base)Activator.CreateInstance( item_editor.editor_type );
				ret_editor.item_editor_selector = this;
				return ret_editor;
			}

			ret_editor = (item_editor_base)Activator.CreateInstance( m_default_editor_type ?? typeof( named_item_editor ) );
			ret_editor.item_editor_selector = this;
			return ret_editor;
		}
		public				void					select_sub_editors	( ItemsControl itemsCtrl, List<property> sub_properties )	
		{
			foreach( var property in sub_properties )
			{
				var editor				= select_editor( property );
				editor.DataContext		= null;
				itemsCtrl.Items.Add		( editor );
				editor.DataContext		= property;
			}
		}
		public				List<property_editor_base>	select_sub_editors	( List<property> sub_properties )	
		{
			var list = new List<property_editor_base>( );

			foreach( var property in sub_properties )
			{
				var editor				= select_editor( property );
				list.Add				( editor );
			}

			return list;
		}
	}
}
