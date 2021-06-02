////////////////////////////////////////////////////////////////////////////
//	Created		: 24.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_CAMERA_H_INCLUDED
#define OBJECT_CAMERA_H_INCLUDED

#include "object_base.h"

namespace xray{
namespace editor{

ref class tool_misc;

public ref class object_camera :public object_base
{
	typedef object_base	super;
public:
							object_camera			( tool_misc^ tool );
	virtual void			load_contents			( ) override				{ };
	virtual void			unload_contents			( bool ) override			{ };
	virtual void			save					( configs::lua_config_value t ) override;

private:
			tool_misc^		m_tool_misc;
}; // class object_camera

public ref class object_timer :public object_base
{
	typedef object_base	super;
public:
							object_timer			( tool_misc^ tool );
	virtual void			load_contents			( ) override				{ };
	virtual void			unload_contents			( bool ) override			{ };
	virtual void			save					( configs::lua_config_value t ) override;
}; // class object_timer

}// namespace editor
}// namespace xray

#endif // #ifndef OBJECT_CAMERA_H_INCLUDED