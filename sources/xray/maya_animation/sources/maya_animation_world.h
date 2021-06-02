////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.01.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MAYA_ANIMATION_WORLD_H_INCLUDED
#define MAYA_ANIMATION_WORLD_H_INCLUDED

#include <xray/maya_animation/world.h>

namespace xray {

namespace animation {
	class bi_spline_skeleton_animation;
} // namespace animation

namespace maya_animation {
struct engine;
class discrete_data;
class maya_animation_world :
	public maya_animation::world,
	private boost::noncopyable
{
public:
									maya_animation_world			( maya_animation::engine& engine );
	virtual							~maya_animation_world			( );
	virtual	void					tick					( );
	virtual raw_bi_spline<float,1>	*create_1d_bi_spline	( )const;
	virtual raw_bi_spline<float3,3>	*create_3d_bi_spline	( )const;
	virtual discrete_data			*create_discrete_data	( )const;

	virtual void					build_animation_data	( const maya_animation::discrete_data &data, animation::bi_spline_skeleton_animation &animation_data   )const;

	virtual void					destroy					( discrete_data* &p )const;
	virtual void					destroy					( raw_bi_spline<float,1>	* &p )const;
	virtual void					destroy					(  raw_bi_spline<float3,3>	* &p )const;

private:
	engine&	m_engine;
}; // class maya_animation_world

} // namespace maya_animation
} // namespace xray

#endif // #ifndef MAYA_ANIMATION_WORLD_H_INCLUDED