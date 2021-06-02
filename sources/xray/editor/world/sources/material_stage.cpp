////////////////////////////////////////////////////////////////////////////
//	Created		: 20.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_stage.h"
#include "material.h"
#include "material_effect.h"
#include "material_property.h"
#include "texture_editor.h"

using xray::editor::wpf_controls::property_editors::attributes::material_stage_effect_holder_attribute;
using xray::editor::wpf_controls::color_rgb;

namespace xray {
namespace editor {

material_stage::material_stage( material^ parent, System::String^ n, const configs::lua_config_value& config_value )
:m_parent( parent )
{
	name			= n;
	m_config		= NEW(configs::lua_config_value)(config_value);
	m_effect		= gcnew material_effect( this, (*m_config)["effect"] );
}

material_stage::~material_stage( )
{
	if(m_load_query)
		m_load_query->m_rejected = true;

	DELETE( m_config );

	for each( material_property_base^ p in m_properties )
		delete p;
}

void material_stage::properties_loaded( resources::queries_result& result )
{
	m_load_query				= NULL;

	ASSERT(result[0].is_successful());
	
	configs::lua_config_ptr config			= static_cast_resource_ptr<configs::lua_config_ptr>(result[0].get_unmanaged_resource());
	configs::lua_config_value config_root	= config->get_root()["properties"];

	configs::lua_config_value::iterator cur_prop_descriptor	= config_root.begin();
	configs::lua_config_value::iterator end					= config_root.end();
	
	List<System::String^> loaded_properties;
	for(; cur_prop_descriptor != end; ++cur_prop_descriptor )
	{
		if( !(bool)( *cur_prop_descriptor )["is_readonly"] )
		{
			R_ASSERT( ( *cur_prop_descriptor ).value_exists( "default_value" ), "property must contain default value" );

			pcstr	property_type		= ( *cur_prop_descriptor )[ "type" ];
			System::String^ type_id		= gcnew System::String( property_type );
			pcstr	property_id			= ( *cur_prop_descriptor )[ "shader_parameter_id" ];
			bool	is_model_property	= ( *cur_prop_descriptor )[ "is_model_parameter_by_default" ];
			
			//if is no such property in stage -> create default
			if( !(*m_config).value_exists( property_id ) || 
				!(*m_config)[property_id].value_exists("value")
			)
			{
				(*m_config)[property_id]["value"].assign_lua_value	( ( *cur_prop_descriptor )[ "default_value" ].copy() );
				(*m_config)[property_id]["is_model_parameter"]		= is_model_property;
				m_parent->properties_changed	();
			}

			material_property_base^		prop;

			if( type_id == "int" )
			{
				prop	= gcnew material_property<int>( gcnew System::String( property_id ), "value", (*m_config)[property_id] );

				System::String^ subtype_id	= ( (*cur_prop_descriptor).value_exists("subtype") ) ? gcnew System::String( (*cur_prop_descriptor)["subtype"] ) : "";

				if( subtype_id == "combo_box" )
				{
					ArrayList^ combo_box_strings				= gcnew ArrayList( );
					configs::lua_config_value combo_box_items	= (*cur_prop_descriptor)["combo_items"];
					int count									= combo_box_items.size( );

					for( int i = 0; i < count; ++i )
						combo_box_strings->Add( gcnew System::String( (pcstr)combo_box_items[i] ) );

					prop->property_attributes->Add( gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( combo_box_strings ) );
				}
			}
			else if( type_id == "float" )
			{
				prop	= gcnew material_property<float>( gcnew System::String( property_id ), "value", (*m_config)[property_id] );

				value_range_and_format_attribute^ attribute = gcnew value_range_and_format_attribute( Single::MinValue, Single::MaxValue, 0.01f );

				bool need_range				= false;
				if( (*cur_prop_descriptor).value_exists("min_value") )
				{
					need_range				= true;
					attribute->min_value	= (float)(*cur_prop_descriptor)["min_value"];
				}
				if( (*cur_prop_descriptor).value_exists("max_value") )
				{
					need_range				= true;
					attribute->max_value	= (float)(*cur_prop_descriptor)["max_value"];
				}
				if( (*cur_prop_descriptor).value_exists("change_step") )
				{
					need_range				= true;
					attribute->step_size	= (float)(*cur_prop_descriptor)["change_step"];
				}
				if( need_range )
					prop->property_attributes->Add( attribute );
			}
			else if( type_id == "bool" )
				prop	= gcnew material_property<bool>( gcnew System::String( property_id ), "value", (*m_config)[property_id] );
			else if( type_id == "float3" )
				prop	= gcnew material_property_converted<Vector3D>( gcnew System::String( property_id ), "value", (*m_config)[property_id], gcnew material_property_vector3_read_writer( ) );
			else if( type_id == "color" )
				prop	= gcnew material_property_converted<color_rgb>( gcnew System::String( property_id ), "value", (*m_config)[property_id], gcnew material_property_color_rgb_read_writer( ) );
			else if( type_id == "texture" )
			{
				if( !(*cur_prop_descriptor).value_exists("subtype") )
				{
					fail_loading	( "Effect file " + name + "/" + name + " corrupt. <subtype> parameter in " + gcnew System::String( property_id ) + " texture descriptor is not exists." );
					return;
				}
				System::String^ subtype = gcnew System::String( (pcstr)(*cur_prop_descriptor)["subtype"] );

				prop	= gcnew material_property_converted<System::String^>( gcnew System::String( property_id ), "value", (*m_config)[property_id], gcnew material_property_string_read_writer( ) );

				prop->subtype = subtype;

				//if( subtype == "_2d" )
				//	prop->value_changed += gcnew System::Action<material_property_base^>( this, &material_effect::diffuse_texture_value_chaged );

				int group_id = 0;
				if( (*cur_prop_descriptor).value_exists("group_id") )
					group_id = (int)(*cur_prop_descriptor)["group_id"];

				prop->group_id = group_id;

				prop->property_attributes->Add( gcnew external_editor_attribute( gcnew external_editor_event_handler( this, &material_stage::texture_selector_launch ), true, subtype ) );
			}
			else
				continue;

			pcstr	show_id			= ( *cur_prop_descriptor )[ "show_id" ];
			prop->show_id			= gcnew System::String(show_id);
			m_properties.Add		( prop );
			loaded_properties.Add	( gcnew System::String(property_id) );
		}
	}

	configs::lua_config_value::iterator prop_it		= (*m_config).begin();
	configs::lua_config_value::iterator prop_it_end	= (*m_config).end();

	for(; prop_it != prop_it_end; ++prop_it )
	{
		if( (*prop_it).value_exists("effect_id"))
			continue;

		pcstr prop_id = prop_it.key();
		System::String^ prop_id_m = gcnew System::String(prop_id);
		if(!loaded_properties.Contains(prop_id_m))
		{
			(*m_config).erase( prop_id );// old unused property
			m_parent->properties_changed	();
		}
	}

	m_properties_loaded = true;
	if( m_effect_loaded )
		m_parent->stage_loaded( );
}

void material_stage::fail_loading( System::String^ error_message )
{
	System::Windows::Forms::MessageBox::Show	( error_message, "Stage Loading Fail", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error );
	m_properties.Clear							( );
	m_parent->stage_loaded						( );
}

void material_stage::texture_selector_launch( wpf_controls::property_editors::property^ prop, Object^ filter )
{
	texture_editor_ptr selector( m_parent->m_editor_world, filter	);

	selector->selecting_entity = resource_editor::entity::resource_name;
	selector->selected_name = safe_cast<System::String^>(prop->value);

	System::Windows::Forms::DialogResult res = selector->ShowDialog();
	if( res == System::Windows::Forms::DialogResult::OK)
	{
		prop->value = selector->selected_name;
	}
}

void material_stage::effect_loaded( )
{
	m_effect_loaded		= true;

	if( m_properties_loaded )
		m_parent->stage_loaded( );
}

void material_stage::load( )
{
	fs_new::virtual_path_string path	= "resources/stages/";
	path.append				( unmanaged_string( name ).c_str( ) );
	path.append				( ".stage" );

	m_load_query = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &material_stage::properties_loaded), g_allocator);

	resources::query_resource(
		path.c_str( ), 
		resources::lua_config_class, 
		boost::bind(&query_result_delegate::callback, m_load_query, _1),
		g_allocator
	);

	m_effect->load( gcnew System::Action(this, &material_stage::effect_loaded));
}

wpf_property_container^ material_stage::get_property_container( bool material_editor_mode )
{
	wpf_property_container^ cont = gcnew wpf_property_container;
	
	for each (material_property_base^ p in m_properties )
	{
		unmanaged_string			n(p->name);
		bool can_override			= (*m_config)[n.c_str()]["is_model_parameter"];
		if(!material_editor_mode && !can_override)
			continue;

		property_descriptor^ desc		= gcnew property_descriptor( p->name, p );

		if(material_editor_mode)
		{
			property_descriptor^ idesc	= gcnew property_descriptor( p->name, gcnew lua_property_value_bool( (*m_config)[n.c_str()], "is_model_parameter" ) );
			desc->inner_properties->Add	( "asd", idesc );
		}

		for each( Attribute^ attr in p->property_attributes )
			desc->dynamic_attributes->add( attr );
		
		desc->display_name			= p->show_id;
		
		cont->properties->add		( desc );
	}
	
	property_descriptor^ desc		= gcnew property_descriptor( "effect", gcnew wpf_controls::object_property_value<i_property_container^>( m_effect->get_property_container(material_editor_mode) ) );
	desc->dynamic_attributes->add	( gcnew material_stage_effect_holder_attribute );
	cont->properties->add			( desc );
	
	return cont;
}

void material_stage::check_integrity( )
{
	//for each (material_property_base^ p in m_properties )
	//{
	//	// check properties
	//}
	//m_effect->check_integrity();
}

} // namespace editor
} // namespace xray