////////////////////////////////////////////////////////////////////////////
//	Created 	: 25.02.2008
//	Author		: Konstantin Slipchenko
//	Description : joint_base_impl
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_JOINT_BASE_IMPL_H_INCLUDED
#define XRAY_PHYSICS_JOINT_BASE_IMPL_H_INCLUDED

#include "integration_constants.h"
#include <xray/render/engine/base_classes.h>

namespace xray {
namespace render {

enum debug_user_id;

} // namespace render
} // namespace xray

class  body;
class  island;
struct dxJoint;

class joint_base_impl
{
private:	
	float	m_base_param_step;
	body	*m_body_0;
	body	*m_body_1;

protected:	
					~joint_base_impl	();
	inline			joint_base_impl		();

protected:
		void		connect			( dxJoint *j, body	*b0, body	*b1 );
		void		connect			( dxJoint *j, body	*b );

protected:
		void		integrate		( dxJoint *j, float	integration_step )				;
		void		fill_island 	( dxJoint *j, island	&i )						;
		void		render			( const dxJoint *j, render::scene_ptr const& scene, render::debug::renderer& renderer ) const	;
inline	body		*body0			( )		{ return m_body_0; }
inline	body		*body1			( )		{ return m_body_1; }
inline	const body	*body0			( )const{ return m_body_0; }
inline	const body	*body1			( )const{ return m_body_1; }
inline	float		base_param_step ( )const{ return m_base_param_step; }

protected:
	void		detach		( dxJoint *j);

}; // class joint_base_impl

inline		joint_base_impl::joint_base_impl( ): m_body_0(0), m_body_1(0), m_base_param_step( default_integration_step )
{
}

#endif
