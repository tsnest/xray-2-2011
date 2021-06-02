////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.02.2008
//	Author		: Konstantin Slipchenko
//	Description : shell
////////////////////////////////////////////////////////////////////////////
#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED

#include <xray/physics/shell.h>

#include "dynamic_object.h"
//#include "collide_dynamics.h"

class shell_element;
class collide_element;
class joint;

class shell:
	public	physics::shell,
	private dynamic_object,
	private boost::noncopyable
{
public:
					shell				( memory::base_allocator& allocator );
	virtual			~shell				( );

public:
	virtual	void			set_update_object	( xray::physics::update_object_interface *object );
	
	virtual void			move				( float3 const &delta );

public:
			void				add_element		( non_null<shell_element>::ptr e ){ m_elements.push_back(&(*e)); }
			void				add_joint		( non_null<joint>::ptr j ){ m_joints.push_back(&(*j)); }
	inline	shell_element		&get_element	(u32 index ){ return *m_elements[index]; }
	inline	const shell_element	&get_element	(u32 index ) const { return *m_elements[index]; }
			void				space_insert	( xray::collision::space_partitioning_tree &space );
			void				wake_up			( scheduler &s );
			void				remove			( scheduler &s );

protected:
	virtual	void		on_step				( scheduler &s );

private:
	virtual void		deactivate			( xray::physics::world& w );
	virtual	bool		collide_detail		( island& i,   space_object& o, bool reverted_order );
	virtual	bool		collide				( island& i,   collide_element_interface& e, bool reverted_order );

			void		force_integrate		( float time_delta );
			void		position_integrate	( island	&i, float time_delta );
	virtual void		island_step_start	( island	&i, float time_delta );
	virtual void		island_step_end		( island	&i, float time_delta );
	virtual	void		on_island_connect	( island	&i );
	virtual	void		aabb				( float3 &center, float3 &radius )const;
	virtual	math::aabb&	aabb				( math::aabb& aabb )const;
	virtual	bool		aabb_test			( math::aabb const& aabb ) const;
	virtual	void		space_check_validity( )const;
	virtual	void		render				( render::scene_ptr const& scene, render::debug::renderer& renderer )const;

private:
	typedef vectora<shell_element*>	element_vec;
	typedef vectora<joint*>	joint_vec;

	element_vec					m_elements;
	joint_vec					m_joints;
	memory::base_allocator&		m_allocator;
	xray::physics::update_object_interface*	m_update_object;

}; // class shell

#endif