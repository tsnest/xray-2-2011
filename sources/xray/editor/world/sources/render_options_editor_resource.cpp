////////////////////////////////////////////////////////////////////////////
//	Created		: 02.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_world.h"
#include "render_options_editor_resource.h"

#include "render_options_editor_source.h"
#include "render_options_editor.h"

using xray::editor::wpf_controls::property_editors::attributes::combo_box_items_attribute;
using xray::editor::wpf_controls::property_editors::attributes::external_editor_attribute;
using xray::editor::wpf_controls::property_editors::attributes::external_editor_event_handler;
using xray::editor::wpf_controls::i_property_value;

#pragma managed( push, off )
#	include <xray/render/engine/base_classes.h>
#	include <xray/render/facade/editor_renderer.h>
#	include <xray/resources.h>
#	include <xray/resources_classes.h>
#pragma managed( pop )

namespace xray {
namespace editor {

render_options_editor_resource::render_options_editor_resource( editor_world& in_editor_world, System::String^ resource_name, resource_changed_delegate^ changed_delegate, resource_load_delegate^ loaded_delegate ):
	m_editor_world(in_editor_world)
{
	m_name					= resource_name;
	
	subscribe_on_changed	( changed_delegate );
	subscribe_on_loaded		( loaded_delegate );
	m_options_config		= NEW(configs::lua_config_ptr)();
	m_options_config_backup	= NEW(configs::lua_config_ptr)(configs::create_lua_config());
	m_options_template_config = NEW(configs::lua_config_ptr)();
	
	init					( );
	
	m_load_query			= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate( this, &render_options_editor_resource::options_loaded), g_allocator);
	
	System::String^	req_name = "resources/render/" + resource_name + ".cfg";
	
	unmanaged_string		temp_string0(req_name);
	unmanaged_string		temp_string1("resources/render/base.cfg");
	
	xray::resources::request requests[2] = 
	{
		{temp_string0.c_str(), resources::lua_config_class},
		{temp_string1.c_str(), resources::lua_config_class}
	};
	
	resources::query_resources(
		requests,
		boost::bind(&query_result_delegate::callback, m_load_query, _1),
		g_allocator
	);
}

void render_options_editor_resource::init( )
{
	if(m_type_list.Count!=0)
		return;
}

void render_options_editor_resource::load_internal( )
{
	ASSERT_CMP(m_options_template_config->c_ptr(), !=, NULL);
	
	m_load_query											= nullptr;
	loaded													= true;
	
	(*m_options_config_backup)->get_root().assign_lua_value	((*m_options_config)->get_root().copy());
	
	configs::lua_config_value options_table				= (*m_options_config)->get_root()["options"];
	
 	xray::configs::lua_config_value const& tempalate_options_table = m_options_template_config->c_ptr()->get_root()["options"];
 	
 	for (configs::lua_config_value::iterator it = tempalate_options_table.begin(); 
											 it != tempalate_options_table.end(); 
											 ++it)
 	{
 		pcstr	property_id									= it.key();
 		
 		if (!options_table.value_exists(property_id))
 		{
 			options_table[property_id].assign_lua_value		((*it)["value"]);
			properties_changed								();
 		}
 	}
 	
	u32 index = 0;
	for (configs::lua_config_value::iterator it = options_table.begin(); it != options_table.end(); ++it)
	{
		pcstr	property_id									= it.key();
		pcstr	show_id										= tempalate_options_table.value_exists(property_id) ? 
															  pcstr(tempalate_options_table[property_id]["show_id"]) : property_id;
		configs::lua_config_value value						= (*it);
		
		xray::configs::enum_types property_type				= (*it).get_type();
		
		switch (property_type)
		{
		case xray::configs::t_boolean:
			{
				lua_property_value_bool^					prop;
				System::String^ property_name				= gcnew System::String(show_id);//"booleans/" + gcnew System::String(property_id);
				prop										= gcnew lua_property_value_bool		( value, "" );
				m_properties.Add							( property_name, prop );
			}
			break;
		case xray::configs::t_float:
		case xray::configs::t_integer:
			{
				lua_property_value_float^					prop;
				System::String^ property_name				= gcnew System::String(show_id);//"numerics/" + gcnew System::String(property_id);
				prop										= gcnew lua_property_value_float	( value, "" );
				m_properties.Add							( property_name, prop );
			}
			break;
		default:
			NODEFAULT();
			break;
		}
		
		index++;
	}
	
	//properties_changed		( );
	m_loaded_delegate		( this, true );
}

void render_options_editor_resource::options_loaded( resources::queries_result& data )
{
 	if (data[1].is_successful())
 	{
 		*m_options_template_config = static_cast_resource_ptr<configs::lua_config_ptr>(data[1].get_unmanaged_resource());
 	}
 	
	if (!data[0].is_successful())
		return;
	
	(*m_options_config)		= static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());
	
	load_internal			();
}

render_options_editor_resource::~render_options_editor_resource( )
{
	DELETE			( m_options_config );
	DELETE			( m_options_config_backup );
	DELETE			( m_options_template_config );
	
	if(m_load_query)
		m_load_query->m_rejected = true;
	for each (xray::editor::wpf_controls::i_property_value^ p in m_properties.Values)
		delete p;
}

void render_options_editor_resource::apply_changes( )
{
	if(loaded)
		(*m_options_config_backup)->get_root().assign_lua_value( (*m_options_config)->get_root().copy() );
}

void render_options_editor_resource::copy_from( resource_editor_resource^ other )
{
	render_options_editor_resource^ other_resource			= safe_cast<render_options_editor_resource^>( other );
	m_options_config					= NEW( configs::lua_config_ptr )( configs::create_lua_config() );
	
	*m_options_template_config			= *other_resource->m_options_template_config;
	
	( *m_options_config )->get_root( )	= ( *other_resource->m_options_config )->get_root( ).copy( );
	store_backup_config					( );
	load_internal						( );
}

void render_options_editor_resource::store_backup_config( )
{
	//save default config
	(*m_options_config_backup)->get_root().assign_lua_value( (*m_options_config)->get_root().copy() );
}

void render_options_editor_resource::reset_to_default( )
{
	if(loaded)
		(*m_options_config)->get_root().assign_lua_value( (*m_options_config_backup)->get_root().copy() );
}

void render_options_editor_resource::save( )
{
//	if(loaded)
//		(*m_options_config)->save();
	
	if (loaded)
	{
		configs::lua_config_ptr config_ptr		= configs::create_lua_config( unmanaged_string( resource_name(m_name)).c_str() );
		config_ptr->get_root().assign_lua_value	( ( *m_options_config )->get_root() );
		config_ptr->save						( configs::target_sources );	
		
		m_old_name					= m_name;	
		if (m_editor_world.get_selected_renderer_configuration() == unmanaged_string(name).c_str())
			m_editor_world.engine().get_renderer_world().editor_renderer().set_renderer_configuration(unmanaged_string(name).c_str());
		
		m_editor_world.updated_render_options_list();
	}
};

wpf_property_container^ render_options_editor_resource::get_property_container( bool )
{
	unmanaged_string s(m_name);
	
	wpf_property_container^ cont	= gcnew wpf_property_container;
	
//	property_descriptor^ desc		= nullptr;
	
	cont->properties->add_from_obj( this, "name" );
	
	if( m_properties.Count !=0 )
	{
		for each( KeyValuePair<String^, i_property_value^>^ p in m_properties )
		{
			System::String^					full_key	= p->Key;
			
			cli::array<System::String^>^	tokens		= full_key->Split('/');
			
			if (tokens->Length == 2)
			{
				System::String^				category	= tokens[0];
				System::String^				key			= tokens[1];
				
				cont->properties->add( gcnew property_descriptor( key, category, "", nullptr, p->Value ) );
			}
			else
			{
				cont->properties->add( gcnew property_descriptor( p->Key, p->Value ) );
			}
		}
	}
	
	return cont;
}

System::String^ render_options_editor_resource::name::get( )
{
	return m_name;
}

} // namespace editor
} // namespace xray
