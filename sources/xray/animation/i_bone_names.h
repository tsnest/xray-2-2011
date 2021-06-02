////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_BONE_NAMES_H_INCLUDED
#define I_BONE_NAMES_H_INCLUDED

#include <xray/animation/type_definitions.h>

namespace xray {

namespace configs{
	class lua_config_value;
}

namespace animation {

class i_bone_names {

public:
	virtual	void			set_bones_number( bone_index_type size )							=0;
	virtual void			set_name		( bone_index_type bone_index, pcstr name )			=0;
	
public:
	virtual void			read			( xray::configs::lua_config_value const &cfg )		=0;
	virtual void			write			( xray::configs::lua_config_value		&cfg )const	=0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( i_bone_names )
};

} // namespace animation
} // namespace xray

#endif // #ifndef I_BONE_NAMES_H_INCLUDED