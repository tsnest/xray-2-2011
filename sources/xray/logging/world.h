////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOGGING_WORLD_H_INCLUDED
#define XRAY_LOGGING_WORLD_H_INCLUDED

namespace xray {
namespace logging {

struct XRAY_NOVTABLE world {
	virtual	void	tick		( ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world );
}; // class world

} // namespace logging
} // namespace xray

#endif // #ifndef XRAY_LOGGING_WORLD_H_INCLUDED