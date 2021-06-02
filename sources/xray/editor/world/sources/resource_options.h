////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_OPTIONS_H_INCLUDED
#define RESOURCE_OPTIONS_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;

namespace xray
{
	namespace editor
	{

		public struct resource_options: public resources::unmanaged_resource
		{
		public:
			resource_options(fs::path_string absolute_resource_path):
				m_absolute_resource_path(absolute_resource_path){}
			resource_options(fs::path_string absolute_resource_path, xray::fs::path_string file_path):
				m_absolute_resource_path(absolute_resource_path), m_resource_name(file_path){}

			fs::path_string				m_resource_name;
			fs::path_string				m_absolute_resource_path;

			virtual void				load						(configs::lua_config_ptr config)=0;
			virtual void				save						()=0;

		}; // struct resource_struct
	}//namesapce editor
}//namespace editor

#endif // #ifndef RESOURCE_OPTIONS_H_INCLUDED