////////////////////////////////////////////////////////////////////////////
//	Created		: 18.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_FILE_STRUCT_H_INCLUDED
#define SOUND_FILE_STRUCT_H_INCLUDED

using  namespace System;
using  namespace System::ComponentModel;

namespace xray
{
	namespace editor
	{
		ref struct raw_file_property_struct;
		ref struct sound_options_struct;

		public ref struct sound_file_struct
		{
		public:
			sound_file_struct			(String^ set_name);
			sound_file_struct			(String^ set_name, raw_file_property_struct^ set_raw_options);

		private:
			Action<sound_file_struct^>^			m_loaded_callback;
			Action<raw_file_property_struct^>^	m_raw_loaded_callback;

		public:
			String^						m_name;
			raw_file_property_struct^	m_raw_options;
			String^						m_sound_object;
			sound_options_struct^		m_sound_options;

			[BrowsableAttribute(false)]
			Boolean						m_is_loaded;

		private:
			void						on_sound_file_loaded	(Boolean sucess);
			void						load_raw_options				();
			void						raw_options_loaded				(raw_file_property_struct^ raw_properties);
			void						load_sound_file					();
			void						sound_file_loaded				(xray::resources::queries_result& data);

		public:
			void						load					(Action<sound_file_struct^>^ on_load_callback);
			void						load					(Action<sound_file_struct^>^ on_load_callback, Action<raw_file_property_struct^>^ raw_options_loaded);
			void						save					();
		}; // class sound_file_struct
	}//namespace editor
}//namespace xray

#endif // #ifndef SOUND_FILE_STRUCT_H_INCLUDED