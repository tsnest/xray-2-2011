////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SKELETON_ANIMATION_BLEND_H_INCLUDED
#define SKELETON_ANIMATION_BLEND_H_INCLUDED

#include "bone_animation.h"
#include "skeleton.h"

#include "blend_function.h"

namespace xray {
namespace animation {

class skeleton;
class skeleton_animation_data;

class skeleton_animation {

public:
			skeleton_animation( const skeleton_animation_data &data );			
			skeleton_animation( const skeleton_animation_data &data, vector< u32 > const& idx );

public:
inline		bone_animation	const	&bone				( u32 id )const	{ return m_bone_data[id]; }
inline		bone_animation			&bone				( u32 id )		{ return m_bone_data[id]; }

public:
			u32						bone_count			( )const { return m_bone_data.size(); }
inline		float					max_time			( )const { return m_bone_data[0].max_time(); }
inline		float					min_time			( )const { return m_bone_data[0].min_time(); }
public:
inline		float					blend_factor		( float time, float blending_time )const { return m_blend_function.factor( time, blending_time ); }

private:
			void					build				( const skeleton_animation_data &data, vector< u32 > const& idx );

private:
	vector< bone_animation >		m_bone_data;
	blend_function					m_blend_function;

}; // class skeleton_animation

} // namespace animation
} // namespace xray

#endif // #ifndef SKELETON_ANIMATION_BLEND_H_INCLUDED