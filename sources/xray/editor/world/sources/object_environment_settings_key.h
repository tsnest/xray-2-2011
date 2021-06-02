////////////////////////////////////////////////////////////////////////////
//	Created		: 06.12.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_ENVIRONMENT_SETTINGS_KEY_H_INCLUDED
#define OBJECT_ENVIRONMENT_SETTINGS_KEY_H_INCLUDED

namespace xray
{
	namespace editor
	{
		ref class object_environment_settings;

		public ref class object_environment_settings_key
		{
		public:
			object_environment_settings_key	( object_environment_settings^ parent );
			
		private:
			object_environment_settings^		m_parent;

		}; // class object_environment_settings_key

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_ENVIRONMENT_SETTINGS_KEY_H_INCLUDED