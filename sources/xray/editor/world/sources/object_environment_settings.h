////////////////////////////////////////////////////////////////////////////
//	Created		: 05.12.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_ENVIRONMENT_SETTINGS_H_INCLUDED
#define OBJECT_ENVIRONMENT_SETTINGS_H_INCLUDED

#include "object_base.h"

using namespace System::Collections::Generic;
using namespace System::ComponentModel;

namespace xray
{
	namespace editor
	{
		ref class tool_base;
		ref class object_environment_settings_key;

		typedef object_environment_settings_key key;

		using xray::editor::wpf_controls::property_container;

		ref class object_environment_settings: public object_base
		{
			typedef object_base super;
		public:
			object_environment_settings	( tool_base^ t );
			~object_environment_settings( );

		public:
			[DisplayNameAttribute("keys")]
			property List<key^>^		keys;

		public:
			virtual void				save					( configs::lua_config_value t )			override;
			virtual void				load_props				( configs::lua_config_value const& t )	override;
			virtual void				load_contents			( )										override;
			virtual void				unload_contents			( bool )								override;
			virtual property_container^	get_property_container	( )										override;

		private:

		}; // class object_environment_settings

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_ENVIRONMENT_SETTINGS_H_INCLUDED