////////////////////////////////////////////////////////////////////////////
//	Created		: 18.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_file_struct.h"
#include <xray/editor/base/managed_delegate.h>
#include "sound_object_cook.h"
#include "raw_file_property_struct.h"
#include "sound_options_struct.h"
#include "sound_editor.h"

using namespace System;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Windows::Forms;

using xray::resources::sound_object_resource;
using xray::editor::sound_editor;

namespace xray
{
	namespace editor
	{
		sound_file_struct::sound_file_struct(String^ set_name)
		{
			m_name		= set_name;
			m_is_loaded = false;
		}

		sound_file_struct::sound_file_struct(String^ set_name, raw_file_property_struct^ set_raw_options):
			m_raw_options(set_raw_options)
		{
			m_name		= set_name;
			m_is_loaded = false;
		}

		void		sound_file_struct::load(Action<sound_file_struct^>^ on_load_callback)
		{
			//save callback
			m_loaded_callback = on_load_callback;

			load_raw_options();
		}

		void		sound_file_struct::load(Action<sound_file_struct^>^ on_load_callback, Action<raw_file_property_struct^>^ raw_options_loaded_callback)
		{
			m_raw_loaded_callback = raw_options_loaded_callback;
			load(on_load_callback);
		}
	
		void		sound_file_struct::load_raw_options()
		{
			//load raw file properties
			m_raw_options			= gcnew raw_file_property_struct(m_name+".options");
			m_raw_options->load		(gcnew Action<raw_file_property_struct^>(this, &sound_file_struct::raw_options_loaded));
		}

		void		sound_file_struct::raw_options_loaded(raw_file_property_struct^ properties)
		{
			//chech loaded properties
			if(properties == nullptr)
			{
				m_raw_options->save();
			}

			m_raw_loaded_callback(m_raw_options);

			load_sound_file();
		}

		void		sound_file_struct::load_sound_file()
		{
			//load sound file width sound properties
			query_result_delegate* rq			= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &sound_file_struct::sound_file_loaded));
			unmanaged_string	request_name	("resources/sounds/converted_local/"+m_name);
			resources::request	requests[]		=	{ request_name.c_str(), xray::resources::ogg_sound_wrapper_class};
			resources::query_resources			(
				requests,
				boost::bind(&query_result_delegate::callback, rq, _1),
				g_allocator
			);
		}

		void		sound_file_struct::sound_file_loaded(xray::resources::queries_result& data)
		{
			//check loaded encoded files
			//R_ASSERT	(!data.is_failed());	

			if( data.is_failed() )
			{
				on_sound_file_loaded					(false);
				return;
			}

			if( data.size() > 0 )
			{
				//get object of config reader
				sound_object_resource* sound_object		= static_cast_checked<sound_object_resource*>(data[0].get_unmanaged_resource().c_ptr());
				this->m_sound_object					= "has object";

				//if sound options has no loaded
				if(sound_object->m_options_resource_ptr.c_ptr() != NULL)
				{
					configs::lua_config* conf = (static_cast_checked<configs::lua_config*>(sound_object->m_options_resource_ptr.c_ptr()));
					configs::lua_config_ptr config(conf);
					m_sound_options = gcnew sound_options_struct(config);
					m_sound_options->name = m_name+".options";
				}

				on_sound_file_loaded						(true);
				return;
			}
			on_sound_file_loaded					(false);
		}

		void sound_file_struct::on_sound_file_loaded(Boolean success)
		{
			XRAY_UNREFERENCED_PARAMETER(success);

			m_is_loaded			= true;
			m_loaded_callback	(this);
		}

		void sound_file_struct::save()
		{

		}
	}
}