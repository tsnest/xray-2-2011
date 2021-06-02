////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_item_struct.h"
#include <xray/editor/base/managed_delegate.h>
#include "sound_editor.h"

using namespace System::Reflection;

using namespace System::IO;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace xray::configs;

using xray::editor::sound_item_struct;
using xray::editor::sound_editor;

namespace xray
{
	namespace editor
	{
		sound_item_struct::sound::sound()
		{
			m_name			= "undefined";
			m_properties	= gcnew Collections::Generic::List<sound_property^>();
		}

		sound_item_struct::sound_property::sound_property()
		{
			m_dependencies	= gcnew Collections::Generic::List<property_dependency^>();
		}

		sound_item_struct::property_dependency::property_dependency()
		{
			m_values		= gcnew list_of_float();
		}

		sound_item_struct::sound_item_struct		(String^ file_path):
			m_file_path(file_path)
		{
			m_name					= Path::GetFileName(file_path);
			m_sounds				= gcnew Collections::Generic::List<sound^>();
		}

		sound_item_struct::sound_item_struct		(configs::lua_config_ptr config)
		{
			configs::lua_config_value const& config_root = config->get_root();

			configs::lua_config_value value = config_root["options"]; 

			//Getting properties
			m_name					= gcnew String		(value["name"]);
			
		}

		void	sound_item_struct::load(Action<sound_item_struct^>^ on_load_callback)
		{
			load(on_load_callback, g_allocator);
		}			

		void	sound_item_struct::load(Action<sound_item_struct^>^ on_load_callback, memory::base_allocator* allocator)
		{
			loaded_callback = on_load_callback;
			m_loaded = false;

			query_result_delegate* rq = XRAY_NEW_IMPL(allocator, query_result_delegate)(gcnew query_result_delegate::Delegate(this, &sound_item_struct::on_file_property_loaded));
			xray::resources::query_resource(
				unmanaged_string(sound_editor::sound_resources_path+"items/"+m_file_path+".sound_item").c_str(),
				xray::resources::config_lua_class,
				boost::bind(&query_result_delegate::callback, rq, _1),
				allocator
			);
		}

		void	sound_item_struct::on_file_property_loaded		(xray::resources::queries_result& data)
		{
			//R_ASSERT	(!data.is_failed());	

			if( data.is_failed())
			{
				if(loaded_callback != nullptr)
					loaded_callback(this);
				return;
			}
			
			if( data.size() > 0 )
			{
				//get object of config reader
				configs::lua_config_ptr config = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
				//get config root
				configs::lua_config_value const& config_root = config->get_root();

				configs::lua_config_value item = config_root["item"]; 

				//Getting properties
				m_name					= gcnew String		(item["name"]);
			
				//Load all sounds
				lua_config_value::iterator sound_begin = item["sounds"].begin();
				lua_config_value::iterator sound_end = item["sounds"].end();
				
				for(;sound_begin != sound_end; ++sound_begin)
				{
					sound^ sound_one	= gcnew sound();
					sound_one->name		= gcnew String((*sound_begin)["name"]);


					//Load all properties
					lua_config_value::iterator prop_begin = (*sound_begin)["properties"].begin();
					lua_config_value::iterator prop_end = (*sound_begin)["properties"].end();

					for(;prop_begin != prop_end; ++prop_begin)
					{
						sound_property^ property	= gcnew sound_property();
						property->type				= safe_cast<sound_item_properties>((int)((*prop_begin)["type"]));

						//Load all dependencies
						lua_config_value::iterator dep_begin = (*prop_begin)["dependencies"].begin();
						lua_config_value::iterator dep_end = (*prop_begin)["dependencies"].end();

						for(;dep_begin != dep_end; ++dep_begin)
						{
							property_dependency^ dependency	= gcnew property_dependency();
							dependency->type				= safe_cast<sound_item_dependencies>((int)((*dep_begin)["type"]));

							//Load all values
							lua_config_value::iterator val_begin = (*dep_begin)["values"].begin();
							lua_config_value::iterator val_end = (*dep_begin)["values"].end();

							for(;val_begin != val_end; ++val_begin)
							{
								dependency->values->Add(*val_begin);
							}

							//save to property
							property->dependencies->Add(dependency);
						}

						//save to sound
						sound_one->properties->Add(property);
					}

					//save to sound_item
					this->sounds->Add(sound_one);
				}
				m_loaded = true;

				if(loaded_callback != nullptr)
					loaded_callback(this);
			}
			else
			{
				if(loaded_callback != nullptr)
					loaded_callback(this);
			}
		}

		void	sound_item_struct::save							()
		{
			//create correct path
			String^ file_path	= sound_editor::absolute_sound_resources_path+"items/"+m_file_path+".sound_item";
			String^ dir_path	= Path::GetDirectoryName(file_path);
			if(!Directory::Exists(dir_path))
			{
				Directory::CreateDirectory(dir_path);
			}
			
			//open config file and get root of it
			configs::lua_config_ptr	const&		config				= configs::create_lua_config(unmanaged_string(file_path).c_str());
			configs::lua_config_value config_item					= config->get_root()["item"];

			//store data to config file
			
			config_item["name"]										= unmanaged_string(m_name).c_str();
			
			configs::lua_config_value sounds = config_item["sounds"]; 
			
			//Save all sounds
			int sound_index		= 0;
			for each(sound^ sound_one in this->sounds)
			{
				sounds[sound_index]["name"] = unmanaged_string(sound_one->name).c_str();
				configs::lua_config_value properties = sounds[sound_index++]["properties"];

				//Save all properties
				int property_index		= 0;
				for each(sound_property^ prop in sound_one->properties)
				{
					properties[property_index]["type"] = (int)(prop->type);
					properties[property_index]["name"] = unmanaged_string(prop->type.ToString()).c_str();
					configs::lua_config_value dependencies = properties[property_index++]["dependencies"];

					//Save all dependencies
					int dependency_index		= 0;
					for each(property_dependency^ dep in prop->dependencies)
					{
						dependencies[dependency_index]["type"] = (int)(dep->type);
						dependencies[dependency_index]["name"] = unmanaged_string(dep->type.ToString()).c_str();
						configs::lua_config_value values = dependencies[dependency_index++]["values"];	

						//Save all values
						int value_index		= 0;
						for each(Single val in dep->values)
						{
							values[value_index++] = val;
						}
					}
				}
			}
			config->save();
		}
	}//namespace editor
}//namespace xray