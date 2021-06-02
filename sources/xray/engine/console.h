////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ENGINE_CONSOLE_H_INCLUDED
#define XRAY_ENGINE_CONSOLE_H_INCLUDED

#include <xray/input/handler.h>

namespace xray {
namespace engine {

class console : public xray::input::handler
{
public:
	virtual					~console			( )			{};
	virtual bool			get_active			( ) const	= 0;
	virtual void			on_activate			( )			= 0;
	virtual void			on_deactivate		( )			= 0;
	virtual void			tick				( )			= 0;
}; // class console

} // namespace engine
} // namespace xray

#endif // #ifndef XRAY_ENGINE_CONSOLE_H_INCLUDED