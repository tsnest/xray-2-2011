////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.Windows.Media;
using System.Windows.Media.Media3D;
using xray.editor.wpf_controls.property_editors.attributes;
using xray.editor.wpf_controls.property_editors.item;
using xray.editor.wpf_controls.property_editors.value;

namespace xray.editor.wpf_controls.property_editors
{
	internal static class default_editors_lists
	{
		internal static			void					initialize( )
		{
			m_item_editors		= new List<editor_picker>
	  		{
				new attribute_editor_picker( typeof( control_container_attribute ), typeof( control_container_editor ) ),

	  			new attribute_editor_picker( typeof( data_source_attribute ), typeof( data_source_editor ) ),
				new attribute_editor_picker( typeof( custom_property_attribute ), typeof( custom_item_editor ) ),
	  			new attribute_editor_picker( typeof( special_attribute ),o => ( (special_attribute)o ).value == "material_stage_holder",typeof( material_stage_editor ) ),
	  			new attribute_editor_picker( typeof( material_stage_effect_holder_attribute ),typeof( material_stage_effect_editor ) ),
	  		};

			m_value_editors		= new List<editor_picker>
		  	{
				new attribute_editor_picker( typeof( text_box_extension_attribute ), typeof( text_box_editor_extension ) ),
				new attribute_editor_picker( typeof( control_attribute ), typeof( control_editor ) ),
				new attribute_editor_picker( typeof( external_editor_attribute ), typeof( external_editor ) ),
				new attribute_editor_picker( typeof( combo_box_items_attribute ), typeof( combo_box_editor ) ),
				
				new type_editor_picker( typeof( float_curve ), typeof( float_curve_editor ) ),
				new type_editor_picker( typeof( List<float_curve> ), typeof( float_curve_editor ) ),

				new attribute_editor_picker( typeof( collection_attribute ), typeof( properties_collection_editor ) ),
				new assignable_type_editor_picker( typeof( ICollection ), typeof( IList_collection_editor ), true ),
				new assignable_type_editor_picker( typeof( property_container_base ), typeof( property_container_editor ) ),
		  		new attribute_editor_picker( typeof( string_select_file_editor_attribute ),typeof( string_select_file_editor ) ),
				new type_editor_picker( typeof( Vector3D ), typeof( vector3_editor ) ),
		  		new type_editor_picker( typeof( Boolean ), typeof( Boolean_editor ) ),
				new type_editor_picker( typeof( Color ), typeof( color_editor ) ),
				new type_editor_picker( typeof( color_rgb ), typeof( color_editor ) ),
				new type_editor_picker( typeof( color_matrix ), typeof( value.color_matrix_editor ) ),
				new type_editor_picker( typeof( color_curve ), typeof( color_curve_editor ) ),
		  		new type_editor_picker( typeof( Int32 ), typeof( int32_editor ) ),
		  		new type_editor_picker( typeof( UInt32 ), typeof( int32_editor ) ),
		  		new type_editor_picker( typeof( String ), typeof( string_editor ) ),
		  		new type_editor_picker( typeof( Single ), typeof( single_editor ) ),
		  		new type_editor_picker( typeof( Double ), typeof( single_editor ) ),
		  		new assignable_type_editor_picker( typeof( Enum ), typeof( Enum_editor ) )
		  	};
		}

		private static			List<editor_picker>		m_item_editors;
		private static			List<editor_picker>		m_value_editors;

		public static			List<editor_picker>		item_editors		
		{
			get{ return m_item_editors; }
		}
		public static			List<editor_picker>		value_editors		
		{
			get{ return m_value_editors; }
		}
	}
} // xray.editor.wpf_controls.property_editors
