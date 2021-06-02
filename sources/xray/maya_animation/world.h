////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.01.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEFAULT_WORLD_H_INCLUDED
#define XRAY_DEFAULT_WORLD_H_INCLUDED

#include <xray/maya_animation/raw_bi_spline.h>

namespace xray {

namespace animation {
	class bi_spline_skeleton_animation;
} // namespace animation

namespace maya_animation {

class	discrete_data;
struct	handler;

typedef	raw_bi_spline<float,1>	raw_bi_spline_1d;
typedef	raw_bi_spline<float3,3> raw_bi_spline_3d;

struct XRAY_NOVTABLE world {
	virtual	void					tick					( ) = 0;
	virtual void					build_animation_data	( const maya_animation::discrete_data &data, animation::bi_spline_skeleton_animation &animation_data   )const =0 ;
	virtual raw_bi_spline_1d		*create_1d_bi_spline	( )const = 0;
	virtual raw_bi_spline_3d		*create_3d_bi_spline	( )const = 0;
	virtual discrete_data			*create_discrete_data	( )const = 0;
	virtual void					destroy					( raw_bi_spline<float,1>	* &p )const = 0;
	virtual void					destroy					(  raw_bi_spline<float3,3>	* &p )const = 0;
	virtual void					destroy					(  maya_animation::discrete_data	* &p )const = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world )
}; // class world

} // namespace maya_animation
} // namespace xray

#endif // #ifndef XRAY_DEFAULT_WORLD_H_INCLUDED