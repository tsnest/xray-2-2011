////////////////////////////////////////////////////////////////////////////
//	Created 	: 02.02.2008
//	Author		: Konstantin Slipchenko
//	Description : space_object
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_SPACE_OBJECT_H_INCLUDED
#define XRAY_PHYSICS_SPACE_OBJECT_H_INCLUDED

#include <xray/render/engine/base_classes.h>

namespace xray {
namespace render {
	enum debug_user_id;
} // namespace render
} // namespace xray

namespace xray {

namespace collision {
	struct space_partitioning_tree;
} // namespace collision

namespace render{

class world;

} // namespace base

namespace physics {
	class space_item;
} // namespace physics

} // namespace xray


class island;
class collision_space;
class collide_element_interface;

class	space_object
{
public:
	
			void			space_insert		( xray::collision::space_partitioning_tree & space );
			void			space_remove		( );

public:
	virtual	void			collide				( island& i, space_object& o, bool reverted_order  )						= 0;

	virtual	bool			collide_detail		( island& i, space_object& o, bool reverted_order  )						= 0;
	virtual	bool			collide				( island& i, collide_element_interface& e, bool reverted_order  )			= 0;
	virtual	void			aabb				( float3 &center, float3 &radius )const										= 0;
			void			move				( )																			;
	virtual	bool			aabb_test			( math::aabb const& aabb ) const											= 0;
	virtual	void			render				( render::scene_ptr const& scene, render::debug::renderer& renderer )const	= 0;

private:
	virtual	void			space_check_validity( )const																	= 0;

public:
	inline	xray::physics::space_item* get_space_item		( )			{ return m_space_item; }
	inline	xray::physics::space_item const* get_space_item	( ) const	{ return m_space_item; }

protected:
	inline					space_object		( ) : m_space_item( 0 ) { }
							~space_object		( );
private:
	xray::physics::space_item*	m_space_item;

}; // class	space_object

#endif