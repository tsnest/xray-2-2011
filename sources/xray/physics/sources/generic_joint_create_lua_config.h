////////////////////////////////////////////////////////////////////////////
//	Created		: 10.06.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GENERIC_JOINT_CREATE_LUA_CONFIG_H_INCLUDED
#define GENERIC_JOINT_CREATE_LUA_CONFIG_H_INCLUDED

class body;
class joint;
namespace xray {
namespace configs{
	class binary_config_value;
} // namespace configs
} //namespace xray 

//template<class ConfigValueType>
//inline joint* new_generic_joint(const ConfigValueType &cfg, body* b0, body* b1, const float4x4 &transform  );

#include "generic_joint_create_lua_config_inline.h"
#endif // #ifndef GENERIC_JOINT_CREATE_LUA_CONFIG_H_INCLUDED