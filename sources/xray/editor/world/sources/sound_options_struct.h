////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_OPTIONS_STRUCT_H_INCLUDED
#define SOUND_OPTIONS_STRUCT_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;

namespace xray
{
	namespace editor
	{
		public ref struct sound_options_struct
		{
		
		#pragma region | Initialize |
		
		public:
			sound_options_struct(String^ file_path);
			sound_options_struct(configs::lua_config_ptr config);

		#pragma endregion
		
		#pragma region |   Fields   |

		private:
			String^					m_name;
			array<Single>^			m_rms_data;
			int						m_rms_discretization;

			Action<sound_options_struct^>^ loaded_callback;

		public:
			String^					m_file_path;

		#pragma endregion 

		#pragma region | Properties |

		public:
			///<summary>
			///song name - summary
			///</summary>
			[CategoryAttribute("Sound Options"),	DefaultValueAttribute("sound"),					DisplayNameAttribute("name"),					DescriptionAttribute("object position")]
			property			String^					name
			{
				String^				get	()							{ return m_name; }
				void				set	(String^ value)				{ m_name = value; }
			}

			///<summary>
			///song compression - summary
			///</summary>
			[CategoryAttribute("Sound Options"),	DefaultValueAttribute(true),					DisplayNameAttribute("Bits per sample"),			DescriptionAttribute("object position")]
			property			array<Single>^			rms_data
			{
				array<Single>^		get	()							{ return m_rms_data; }
				void				set	(array<Single>^ value)		{ m_rms_data = value; }
			}

			///<summary>
			///song compression - summary
			///</summary>
			[CategoryAttribute("Sound Options"),	DefaultValueAttribute(2),						DisplayNameAttribute("Number of chanels"),			DescriptionAttribute("object position")]
			property			int						rms_discretization
			{
				int					get	()							{ return m_rms_discretization; }
				void				set	(int value)					{ m_rms_discretization = value; }
			}

		#pragma endregion 

		#pragma region |  Methods   |

		private:
			void	on_file_property_loaded		(xray::resources::queries_result& data);

		public:
			void	load						(Action<sound_options_struct^>^ on_load_callback);
			void	load						(Action<sound_options_struct^>^ on_load_callback, memory::base_allocator* allocator);
			void	save						();

		#pragma endregion

		}; // class raw_file_proprty_struct
	}//namespace editor
}//namespace xray
#endif // #ifndef RAW_FILE_PROPERTY_STRUCT_H_INCLUDED