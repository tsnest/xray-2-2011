////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.02.2008
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PHYSICS_SHELL_H_INCLUDED
#define XRAY_PHYSICS_SHELL_H_INCLUDED

namespace xray {
namespace physics {

struct world;

struct XRAY_NOVTABLE update_object_interface {

	virtual void	physics_update_transform( float4x4 const* transforms, u32 transfrorms_count ) = 0;

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( update_object_interface )
}; // struct shell


struct XRAY_NOVTABLE shell {

	virtual void	set_update_object	( xray::physics::update_object_interface *object ) = 0;
	virtual void	deactivate			( world& w ) = 0;
	virtual void	move				( xray::math::float3 const &delta ) = 0;

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( shell )
}; // struct shell

} // namespace physics
} // namespace xray

#endif // #ifndef XRAY_PHYSICS_SHELL_H_INCLUDED