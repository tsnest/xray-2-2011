////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class combo_box_items_attribute : Attribute
	{
		public combo_box_items_attribute    ( params String[] items )   
		{
			this.items = new ArrayList( );
			foreach ( var item in items )
				this.items.Add( item );
		}
		public combo_box_items_attribute    ( params Object[] items )   
		{
			is_associative	= true;
			this.items		= new ArrayList( );
			this.values		= new ArrayList( );
			var even		= true;

			foreach ( var item in items )
			{
				if( even )
					this.items.Add	( item );
				else
					this.values.Add	( item );

				even			= !even;
			}
		}
		public combo_box_items_attribute    ( ArrayList items )
		{
			this.items = items;
		}
		public combo_box_items_attribute    ( String argument, Func<String, IEnumerable<String>> get_items )         
		{
			this.get_items	= get_items;
			this.argument	= argument;
		}
		public combo_box_items_attribute    ( Func<Int32> items_count_func, Func<Int32, String> get_item_func )
		{
			this.items_count_func = items_count_func;
			this.get_item_func = get_item_func;
		}

		public readonly	String								argument;
		public readonly ArrayList							items;
		public readonly ArrayList							values;
		public readonly Func<Int32>							items_count_func;
		public readonly Func<Int32, String>					get_item_func;
		public readonly Func<String, IEnumerable<String>>	get_items;
		public readonly	Boolean								is_associative;
	}
}
