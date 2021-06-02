////////////////////////////////////////////////////////////////////////////
//	Created		: 22.03.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_VALUE_BASE_H_INCLUDED
#define PROPERTY_VALUE_BASE_H_INCLUDED

namespace xray {
namespace editor_base {

public delegate void on_property_value_changed( System::String^ property_path, configs::lua_config_value const& property_value );

public interface class property_value_base {
public:
	virtual property on_property_value_changed^ on_property_value_changed;
	

}; // class property_value_base

} // namespace editor_base
} // namespace xray

#endif // #ifndef PROPERTY_VALUE_BASE_H_INCLUDED