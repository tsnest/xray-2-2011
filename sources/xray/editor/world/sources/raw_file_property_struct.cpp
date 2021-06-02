////////////////////////////////////////////////////////////////////////////
//	Created		: 12.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "raw_file_property_struct.h"
#include <xray/editor/base/managed_delegate.h>
#include "sound_editor.h"

using namespace System::Reflection;

using namespace System::IO;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace xray::configs;

using xray::editor::raw_file_property_struct;
using xray::editor::sound_editor;

namespace xray {
namespace editor {

		raw_file_property_struct::raw_file_property_struct		(String^ file_path):
	m_file_path(file_path)
{
	m_name					= gcnew String		("sound");
	m_bits_per_sample		=		16;
	m_number_of_chanels		=		1;
	m_samples_per_second	=		44100;
	m_output_bitrate		=		256;
}

		raw_file_property_struct::raw_file_property_struct		(configs::lua_config_ptr config)
{
	configs::lua_config_value const& config_root = config->get_root();

	configs::lua_config_value value = config_root["options"]; 

	//Getting properties
	m_name					= gcnew String		(value["name"]);
	m_bits_per_sample		=		Int32		(value["bits_per_sample"]);
	m_number_of_chanels		=		Int32		(value["number_of_chanels"]);
	m_samples_per_second	=		Int32		(value["samples_per_second"]);
	m_output_bitrate		=		Int32		(value["output_bitrate"]);
}

void	raw_file_property_struct::load(Action<raw_file_property_struct^>^ on_load_callback)
{
	load(on_load_callback, g_allocator);
}			

void	raw_file_property_struct::load(Action<raw_file_property_struct^>^ on_load_callback, memory::base_allocator* allocator)
{
	loaded_callback = on_load_callback;

	query_result_delegate* rq = XRAY_NEW_IMPL(allocator, query_result_delegate)(gcnew query_result_delegate::Delegate(this, &raw_file_property_struct::on_file_property_loaded));
	xray::resources::query_resource(
		unmanaged_string(sound_editor::sound_resources_path+"sources/"+m_file_path).c_str(),
		xray::resources::config_lua_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		allocator
	);
}

void	raw_file_property_struct::on_file_property_loaded		(xray::resources::queries_result& data)
{
	//R_ASSERT	(!data.is_failed());	

	if( data.is_failed())
	{
		if(loaded_callback != nullptr)
			loaded_callback(nullptr);
		return;
	}
	
	if( data.size() > 0 )
	{
		//get object of config reader
		configs::lua_config_ptr config = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());

		//get config root
		configs::lua_config_value const& config_root = config->get_root();

		configs::lua_config_value value = config_root["options"]; 

		//Getting properties
		m_name					= gcnew String		(value["name"]);
		m_bits_per_sample		=		Int32		(value["bits_per_sample"]);
		m_number_of_chanels		=		Int32		(value["number_of_chanels"]);
		m_samples_per_second	=		Int32		(value["samples_per_second"]);
		m_output_bitrate		=		Int32		(value["output_bitrate"]);

		if(loaded_callback != nullptr)
			loaded_callback(this);
	}
	else
	{
		if(loaded_callback != nullptr)
			loaded_callback(nullptr);
	}
}

void	raw_file_property_struct::save							()
{
	//create correct path
	String^ file_path = sound_editor::absolute_sound_resources_path+"sources/"+m_file_path;
	
	//open config file and get root of it
	configs::lua_config_ptr	const&		config	= configs::create_lua_config(unmanaged_string(file_path).c_str());
	configs::lua_config_value config_options	= config->get_root()["options"];

	//store data to config file
	
	config_options["name"]						= unmanaged_string(m_name).c_str();
	config_options["bits_per_sample"]			= m_bits_per_sample;
	config_options["number_of_chanels"]			= m_number_of_chanels;
	config_options["samples_per_second"]		= m_samples_per_second;
	config_options["output_bitrate"]			= m_output_bitrate;

	//save config file to disk
	config->save();
}
}//namespace editor
}//namespace xray
