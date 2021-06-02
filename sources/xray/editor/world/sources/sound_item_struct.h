////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_ITEM_STRUCT_H_INCLUDED
#define SOUND_ITEM_STRUCT_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
#include "dependency_editor_type.h"

namespace xray
{
	namespace editor
	{
		public ref struct sound_item_struct
		{

		#pragma region | InnerTypes |
		
		public:
			ref struct sound;
			ref struct sound_property;
			ref struct property_dependency;

		public:

			enum class sound_item_properties
			{
				precondition,
				volume
			};

			enum class sound_item_dependencies
			{
				day_time,
				distance,
				self
			};

			ref struct sound
			{
			public:
				sound();

			private:
				String^											m_name;
				Collections::Generic::List<sound_property^>^	m_properties;

			public:
				property	String^							name
				{
					String^						get	()									{ return m_name; }
					void						set	(String^ value)						{ m_name = value; }
				}

				property	Collections::Generic::List<sound_property^>^			properties
				{
					Collections::Generic::List<sound_property^>^		get	()									{ return m_properties; }
					void						set	(Collections::Generic::List<sound_property^>^ value)		{ m_properties = value; }
				}
			};

			ref struct sound_property
			{
			public:
				sound_property();

			private:
				sound_item_properties			m_type;
				Collections::Generic::List<property_dependency^>^		m_dependencies;

			public:
				property	sound_item_properties			type
				{
					sound_item_properties		get	()									{ return m_type; }
					void						set	(sound_item_properties value)		{ m_type = value; }
				}

				property	Collections::Generic::List<property_dependency^>^		dependencies
				{
					Collections::Generic::List<property_dependency^>^	get	()									{ return m_dependencies; }
					void						set	(Collections::Generic::List<property_dependency^>^ value)	{ m_dependencies = value; }
				}
			};

			ref struct property_dependency
			{
			public:
				property_dependency();

			private:
				sound_item_dependencies					m_type;
				list_of_float^							m_values;

			public:
				property	sound_item_dependencies							type
				{
					sound_item_dependencies	get	()										{ return m_type; }
					void						set	(sound_item_dependencies value)		{ m_type = value; }
				}

				property	list_of_float^					values
				{
					list_of_float^	get	()									{ return m_values; }
					void			set	(list_of_float^ value)				{ m_values = value; }
				}
			};

		#pragma endregion 
		
		#pragma region | Initialize |
		
		public:
				sound_item_struct(String^ file_path);
				sound_item_struct(configs::lua_config_ptr config);

		#pragma endregion
		
		#pragma region |   Fields   |

		private:
			String^								m_name;
			Collections::Generic::List<sound^>^	m_sounds;
			Boolean								m_loaded;

			Action<sound_item_struct^>^ loaded_callback;

		public:
			String^					m_file_path;

		#pragma endregion 

		#pragma region | Properties |

		public:
			property			String^					name
			{
				String^				get	()							{ return m_name; }
				void				set	(String^ value)				{ m_name = value; }
			}

			property Collections::Generic::List<sound^>^			sounds
			{
				Collections::Generic::List<sound^>^		get	()							{ return m_sounds; }
				void				set	(Collections::Generic::List<sound^>^ value)		{ m_sounds = value; }
			}

			property			Boolean					loaded
			{
				Boolean				get	()							{ return m_loaded; }
				void				set	(Boolean value)				{ m_loaded = value; }
			}

		#pragma endregion 

		#pragma region |  Methods   |

		private:
			void	on_file_property_loaded		(xray::resources::queries_result& data);

		public:
			void	load						(Action<sound_item_struct^>^ on_load_callback);
			void	load						(Action<sound_item_struct^>^ on_load_callback, memory::base_allocator* allocator);
			void	save						();

		#pragma endregion

		}; //struct sound_item_struct
	}//namespace editor
}//namespace xray

#endif // #ifndef SOUND_ITEM_STRUCT_H_INCLUDED