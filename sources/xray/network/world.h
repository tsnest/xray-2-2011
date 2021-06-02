////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_NETWORK_WORLD_H_INCLUDED
#define XRAY_NETWORK_WORLD_H_INCLUDED

namespace xray {
namespace network {

struct handler;

struct XRAY_NOVTABLE world {
	virtual	void	tick			( u32 const logic_frame_id ) = 0;
	virtual	void	clear_resources	( ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world )
}; // class world

} // namespace network
} // namespace xray

#endif // #ifndef XRAY_NETWORK_WORLD_H_INCLUDED