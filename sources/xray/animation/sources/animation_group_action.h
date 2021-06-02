////////////////////////////////////////////////////////////////////////////
//	Created		: 22.03.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_GROUP_ACTION_H_INCLUDED
#define ANIMATION_GROUP_ACTION_H_INCLUDED

#include <xray/animation/i_animation_action.h>

namespace xray {
namespace animation {

class	animation_group;
class	animation_controller_set;
struct	frame;

class animation_group_action:
	public i_animation_action
{

public:
	animation_group_action	( animation_group const *group, animation_controller_set *set );
	

public:
	inline	u32	 id								( )const{ return m_id; }
	inline	animation_group	const&	animation	( )const{ return *m_group; }

public:
	void			get_displacement			( frame &f, float3 &foot_displacement, const buffer_vector< float > &weights )const;

	virtual void	get_displacement_transform	( float4x4 &m,  const buffer_vector< float > &weights )const;
	virtual	u32		animations_number			( ) const;
	virtual float	duration					( )const;
	virtual	u32		get_id						( )const { return id(); } 
	virtual	u32		gait						( )const;

public:

	virtual pcstr	name						( )const;

public:
	virtual	u32		bone_index					( pcstr bone_name )const;
	virtual void	local_bone_matrix			( float4x4 &m,  u32 bone_idx,  const buffer_vector< float > &weights )const;

private:
	virtual	float run	( i_animation_controller_set	*set, const buffer_vector< float > &from_weights, const buffer_vector< float > &weights, u32 step, float time )const;




	animation_group					const	*m_group;
	animation_controller_set				*m_set;
	u32										m_id;

}; // class animation_group_action






} // namespace animation
} // namespace xray

#endif // #ifndef ANIMATION_GROUP_ACTION_H_INCLUDED