////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.02.2008
//	Author		: Konstantin Slipchenko
//	Description : body - physics bone (no collision)
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_BODY_H_INCLUDED
#define XRAY_PHYSICS_BODY_H_INCLUDED
//#include "pose_anchor.h"
#include <xray/render/engine/base_classes.h>

namespace xray {
namespace collision {
	class geometry_instance;
} // namespace collision
} // namespace xray

class island_object;
class island;
struct dxBody;
class step_counter_ref;
class body:
	private boost::noncopyable

{

public:
virtual	void				fill_island				( island	&i )									=0;
virtual	bool				integrate				( const step_counter_ref& sr, float time_delta )	=0;
virtual	void				interpolate_pose		( float factor, float4x4 &transform )const			=0;
virtual void				on_step_start			( island	&i, float time_delta )					=0; 
virtual	void				set_position			( const float3 &p )									=0;
virtual	void				move_position			( const float3 &p )									=0;
virtual	void				set_pose				( const float4x4 &pose )							=0;

public:
	virtual	void				set_mass				( float density, const xray::collision::geometry_instance &og,  xray::math::float4x4 const &pose_in_body )	=0;

public:
virtual	float4x4			&get_pose				( float4x4 &pose )const								=0;
virtual	float4x4			get_pose				( )const											=0;
virtual	dxBody				*ode					( )													=0;
virtual	void				render					( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const	=0;
//inline	const pose_anchor	&pose_anchor			( )	const					{ return m_pose_anchor; }

							body					( )	{}	;
virtual						~body					( )	{}	;

};

#endif
