////////////////////////////////////////////////////////////////////////////
//	Created		: 06.06.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "curves_serializer.h"

namespace xray {
namespace editor_base {

void curves_serializer::synchronize_float_curve_config ( float_curve^ curve, configs::lua_config_value& curve_keys_config )
{
	curve_keys_config.clear();
	curve_keys_config.create_table();

	System::Int32 i = 0;

	for each(float_curve_key^ p in curve->keys){
		xray::configs::lua_config_value key = curve_keys_config[unmanaged_string("key" + i).c_str()];

		float position_x		= (float)p->position.X;
		float left_tangent_x	= (float)p->left_tangent.X;
		float right_tangent_x	= (float)p->right_tangent.X;

		R_ASSERT( ( position_x >= left_tangent_x ) && ( right_tangent_x >= position_x ),"Wrong curve [%s] settings!!! " );

		key["position"]			= float2((float)p->position.X, (float)p->position.Y);
		key["left_tangent"]		= float2((float)p->left_tangent.X, (float)p->left_tangent.Y);
		key["right_tangent"]	= float2((float)p->right_tangent.X, (float)p->right_tangent.Y);
		key["key_type"]			= (int)p->key_type;
		key["delta"]			= p->range_delta;
		i++;
	}	

	//	(*m_changes_config_value)["keys"].assign_lua_value(data_source_config.copy());
	//	m_parent_node->parent_document->particle_system_property_changed(m_parent_node, *m_changes_config);
}


float_curve^ curves_serializer::load_float_curve_from_config ( configs::lua_config_value const& curve_keys_config )
{
	System::Collections::Generic::List<float_curve_key^>^ lst = gcnew System::Collections::Generic::List<float_curve_key^>();

	int index = 0;

	while ( curve_keys_config.value_exists( unmanaged_string("key" + index).c_str() ) )
	{
		configs::lua_config_value key_config = (curve_keys_config[unmanaged_string("key" + index).c_str()]);
		System::Windows::Point	position		= System::Windows::Point(
			((float2)(key_config["position"])).x,
			((float2)(key_config["position"])).y
			);

		System::Windows::Point	left_tangent	= System::Windows::Point(
			((float2)(key_config["left_tangent"])).x,
			((float2)(key_config["left_tangent"])).y
			);

		System::Windows::Point	right_tangent	= System::Windows::Point(
			((float2)(key_config["right_tangent"])).x,
			((float2)(key_config["right_tangent"])).y
			);

		float					delta			= 0.f;

		if ( key_config.value_exists("delta") )
			delta = (float)key_config["delta"];


		float_curve_key_type key_type = float_curve_key_type::linear;

		if (key_config.value_exists("key_type"))
			key_type = (float_curve_key_type)(int)(key_config["key_type"]);

		float_curve_key^ key = gcnew float_curve_key(position, key_type, left_tangent, right_tangent, delta );

		lst->Add(key);

		index++;
		float position_x		= (float)position.X;
		float left_tangent_x	= (float)left_tangent.X;
		float right_tangent_x	= (float)right_tangent.X;			

		R_ASSERT( ( position_x >= left_tangent_x ) && ( right_tangent_x >= position_x ),"Wrong curve settings!!! " );			 		
	}			 

	R_ASSERT( lst->Count != 0, "Keys list couldn't be empty!" );

	return gcnew float_curve(lst);

}


void curves_serializer::synchronize_color_curve_config( color_curve^ color_curve, configs::lua_config_value& color_curve_config )
{
	System::Int32 i = 0;
	configs::lua_config_value	data_source_config	= color_curve_config;
	data_source_config.clear();
	data_source_config.create_table();
	for each(color_curve_key^ key in color_curve->keys)
	{
		xray::configs::lua_config_value key_config = data_source_config[unmanaged_string("key" + i).c_str()];
		key_config["key"] = key->position;
		color_rgb key_color = key->color;
		key_config["value"] = math::float4( (float)key_color.r, (float)key_color.g, (float)key_color.b, (float)key_color.a );
		i++;
	}	

}


color_curve^ curves_serializer::load_color_curve_from_config ( configs::lua_config_value const& curve_keys_config )
{	
	System::Collections::Generic::List<color_curve_key^>^ lst = gcnew System::Collections::Generic::List<color_curve_key^>();

	configs::lua_config_value::iterator begin		= curve_keys_config.begin();
	configs::lua_config_value::iterator end			= curve_keys_config.end();

	for(;begin != end;++begin)
	{
		System::Single			pos = (*begin)["key"];
		math::float4 key_color_source = (math::float4)(*begin)["value"];
		color_rgb	color = color_rgb( key_color_source.x, key_color_source.y, key_color_source.z, key_color_source.w );
		lst->Add(gcnew color_curve_key(pos, color));
	}

	return gcnew color_curve(lst);
}


color_matrix^ curves_serializer::load_color_matrix_from_config( configs::lua_config_value const& color_matrix_config )
{
	color_matrix^ ret_matrix = gcnew color_matrix( System::Windows::Size( 1,1 ) );
	int row_index = 0;
	while (color_matrix_config.value_exists( unmanaged_string( "row" + row_index ).c_str( ) ) )
	{
		configs::lua_config_value elements_config = color_matrix_config[ unmanaged_string( "row" + row_index ).c_str( ) ];

		System::Collections::Generic::List<color_matrix_element^>^ row_elements = 
			gcnew System::Collections::Generic::List<color_matrix_element^>();

		int element_index = 0;
		while ( elements_config.value_exists( unmanaged_string( "element" + element_index ).c_str( ) ) )
		{
			color_matrix_element^ matrix_element = gcnew color_matrix_element();

			math::float2 position_source	= (math::float2)elements_config[unmanaged_string("element" + element_index).c_str()]["position" ];
			math::float4 color_source		= (math::float4)elements_config[unmanaged_string("element" + element_index).c_str()]["color"];

			matrix_element->position		= System::Windows::Point( position_source.x, position_source.y );
			matrix_element->color			= color_rgb( color_source.x, color_source.y, color_source.z, color_source.w);;

			row_elements->Add( matrix_element );

			element_index++;
		}
		ret_matrix->add_row( row_elements );

		row_index++;
	}

	return ret_matrix;

}

void curves_serializer::synchronize_color_matrix_config( color_matrix^ color_matrix, configs::lua_config_value& color_matrix_config )
{
	color_matrix_config.clear();
	color_matrix_config.create_table();

	System::Int32 row_index = 0;

	for ( ;row_index < color_matrix->height; ++row_index )
	{
		System::Int32 element_index = 0;

		configs::lua_config_value elements_config = color_matrix_config[ unmanaged_string( "row" + row_index ).c_str() ].create_table( );

		for ( ;element_index < color_matrix->width; ++element_index )
		{
			configs::lua_config_value matrix_element_config = 
				elements_config[ unmanaged_string( "element" + element_index ).c_str() ].create_table( );

			color_matrix_element^ matrix_element = color_matrix[element_index,row_index];

			matrix_element_config[ "position" ]	= math::float2( (float)matrix_element->position.X, (float)matrix_element->position.Y );

			matrix_element_config[ "color" ]		= math::float4(
				(float)matrix_element->color.r,
				(float)matrix_element->color.g,
				(float)matrix_element->color.b,
				(float)matrix_element->color.a 
				);
		}
	}	
}

} // namespace editor_base
} // namespace xray