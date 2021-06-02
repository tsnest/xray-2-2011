////////////////////////////////////////////////////////////////////////////
//	Created		: 04.10.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PHYSICS_SOFT_BODY_H_INCLUDED
#define XRAY_PHYSICS_SOFT_BODY_H_INCLUDED

// temporary (by Andy)
#pragma warning (push)
#pragma warning (disable:4251)

#include <xray/physics/api.h>

class btSoftBody;

namespace xray {
namespace physics {

class XRAY_PHYSICS_API bt_soft_body_rope
{
	friend class bullet_physics_world;
public:
					bt_soft_body_rope			( btSoftBody* body );
	bool			is_active					( ) const;
	u32				get_fragments_count			( ) const;
	void			get_fragment				( u32 idx, float3& pt0, float3& pt1 ) const;
	u32				get_nodes_count				( ) const;
	void			get_node					( u32 idx, float3& pt ) const;
private:
	btSoftBody*		m_bt_body;
}; // class rigid_body

struct XRAY_PHYSICS_API rope_construction_info
{
		rope_construction_info::rope_construction_info();
		float				kVCF;			// Velocities correction factor (Baumgarte)
		float				kDP;			// Damping coefficient [0,1]
		float				kDG;			// Drag coefficient [0,+inf]
		float				kLF;			// Lift coefficient [0,+inf]
		float				kPR;			// Pressure coefficient [-inf,+inf]
		float				kVC;			// Volume conversation coefficient [0,+inf]
		float				kDF;			// Dynamic friction coefficient [0,1]
		float				kMT;			// Pose matching coefficient [0,1]		
		float				kCHR;			// Rigid contacts hardness [0,1]
		float				kKHR;			// Kinetic contacts hardness [0,1]
		float				kSHR;			// Soft contacts hardness [0,1]
		float				kAHR;			// Anchors hardness [0,1]
		float				timescale;		// Time scale

		int					fragments_count;
		float				margin;
		int					iterations;
		float3				p0;
		float3				p1;
		float				stiftness;		//[0,1]
};

XRAY_PHYSICS_API bt_soft_body_rope* create_soft_body_rope	( world* world, rope_construction_info const& construction_info );
XRAY_PHYSICS_API void				destroy_soft_body_rope	(  world* world, bt_soft_body_rope* body );

#pragma warning (pop)
} // namespace physics
} // namespace xray

#endif // #ifndef XRAY_PHYSICS_SOFT_BODY_H_INCLUDED