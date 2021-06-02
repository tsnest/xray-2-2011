////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_OPTIONS_H_INCLUDED
#define TEXTURE_OPTIONS_H_INCLUDED

#include "resource_options.h"

using namespace System;
using namespace System::Collections;
using namespace System::ComponentModel;

namespace xray {
	namespace editor {

		public struct texture_options: public resource_options
		{

		#pragma region | InnerTypes |
			
			enum ETType{
    			sky = 0,
				bump,
				normal_map,
				terrain,
				terrain_bump,
				terrain_color,
				type_2d
			};
			enum ETBumpMode{
				paralax			= 0,
				stip_paralax,
				displacement,
			};
			enum ETAlphaBlendMode{
				none			= 0,
				reference,
				blend,
			};
			enum ETPackMode{
				packed			= 0,
				unpacked,
			};

		#pragma endregion

		#pragma region | Initialize |
		
		public:
			texture_options(fs::path_string absolute_resource_path, xray::fs::path_string file_path);
			texture_options(fs::path_string absolute_resource_path, configs::lua_config_ptr config);

		#pragma endregion
		
		#pragma region |   Fields   |

		public:
			//type
			ETType						type;

			//dimensions
			int							width;
			int							height;	

			// texture part
			ETAlphaBlendMode			alpha_blend_mode;
			ETPackMode					pack_mode;
			bool						has_mip_levels;

			// detail ext
			xray::fs::path_string		normal_name;
			xray::fs::path_string		color_name;
			float						detail_scale;

			// bump	
			ETBumpMode					bump_mode;
			xray::fs::path_string		bump_name;

			//terrain
			int							tile;

			bool						m_is_default_values;

		#pragma endregion 

		#pragma region |  Methods   |

		private:
			void			on_file_property_loaded		(xray::resources::queries_result& data);

		public:
			void			load						(configs::lua_config_ptr config);
			void			save						();
			static ETType	get_type_from_name			(pcstr name);

		#pragma endregion

			//virtual	void	recalculate_memory_usage_impl ( ) { m_memory_usage_self.unmanaged = get_size(); }

		}; // struct texture_options

		typedef	xray::resources::resource_ptr<texture_options, resources::unmanaged_intrusive_base>	texture_options_ptr;

	}//namespace editor
}//namespace xray

#endif // #ifndef TEXTURE_RAW_OPTIONS_H_INCLUDED