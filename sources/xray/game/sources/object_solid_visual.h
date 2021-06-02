////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_SOLID_VISUAL_H_INCLUDED
#define OBJECT_SOLID_VISUAL_H_INCLUDED

#include "object.h"
#include <xray/render/facade/model.h>
#include <xray/render/facade/particles.h>
#include <xray/particle/world.h>
#include <xray/render/engine/base_classes.h>
#include <xray/physics/model.h>
#include <xray/physics/world.h>

//namespace xray {
//	namespace collision {
//		class collision_object_geometry;
//	} // namespace collision
//} // namespace xray

namespace stalker2 {

class object_visual : public game_object_static
{
	typedef game_object_static	super;
public:
						object_visual	( game_world& w );
	virtual				~object_visual	( );
	virtual void		load			( configs::binary_config_value const& t );

protected:
	fs::path_string		m_visual_name;

};

class object_solid_visual : public object_visual
{
	typedef object_visual	super;
public:
						object_solid_visual		( game_world& w );

	virtual void		load_contents			( );
	virtual void		unload_contents			( );
protected:
	void				on_resources_ready		( resources::queries_result& data );

protected:
	render::static_model_ptr				m_model;
//	collision::collision_object_geometry*	m_collision;
	physics::bt_collision_shape_ptr			m_collision_shape;
	physics::bt_rigid_body*					m_physics_rigid_body;
}; // class object_solid_visual

class object_dynamic_visual : public object_visual
{
	typedef object_visual	super;
public:
						object_dynamic_visual	( game_world& w );

	virtual void		load_contents			( );
	virtual void		unload_contents			( );
	void				physics_update_transform( );
	void				set_transform			( float4x4 const& m );
protected:
	void				on_resources_ready		( resources::queries_result& data );

protected:
	render::static_model_ptr				m_model;
	physics::bt_collision_shape_ptr			m_collision_shape;
	physics::bt_rigid_body*					m_physics_rigid_body;
}; // class object_dynamic_visual

class object_skeleton_visual : public object_visual 
{
	typedef object_visual	super;
public:
						object_skeleton_visual	( game_world& w );

	virtual void		load_contents			( );
	virtual void		unload_contents			( );

protected:
	void				on_visual_ready			( resources::queries_result& data );

protected:
	render::skeleton_model_ptr	m_model;
}; // class object_skeleton_visual

class object_particle_visual : public object_visual 
{
	typedef object_visual	super;
public:
						object_particle_visual	( game_world& w );

	virtual void		load_contents			( );
	virtual void		unload_contents			( );

protected:
	void				on_visual_ready			( resources::queries_result& data );

protected:
	xray::particle::particle_system_instance_ptr	m_particle_system_instance_ptr;
	bool											m_playing;
}; // class object_particle_visual

class object_speedtree_visual : public object_visual 
{
	typedef object_visual	super;
public:
						object_speedtree_visual	( game_world& w );
	
	virtual void		load_contents			( );
	virtual void		unload_contents			( );
	
protected:
	void				on_visual_ready			( resources::queries_result& data );
	xray::render::speedtree_instance_ptr		m_speedtree_instance_ptr;
}; // class object_particle_visual

} // namespace stalker2

#endif // #ifndef OBJECT_SOLID_VISUAL_H_INCLUDED
