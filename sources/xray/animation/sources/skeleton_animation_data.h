////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SKELETON_ANIMATION_DATA_H_INCLUDED
#define SKELETON_ANIMATION_DATA_H_INCLUDED
#include <xray/animation/i_skeleton_animation_data.h>
#include "bone_animation_data.h"

namespace xray {
namespace animation {

class skeleton_animation;
class bone_names;
class skeleton;

class skeleton_animation_data:
	public i_skeleton_animation_data
{

public:
		skeleton_animation_data	( );
virtual	~skeleton_animation_data( );

public:
	virtual bone_animation_data			&bone				( u32 id )		{ return m_bone_data[id]; }
	virtual const bone_animation_data	&bone				( u32 id )const	{ return m_bone_data[id]; } 
	virtual	u32							num_bones			( )const		{ return m_bone_data.size(); }
	virtual	void						set_num_bones		( u32 size );
	virtual	void						save				( xray::configs::lua_config_value &cfg )const;
	virtual	void						load				( xray::configs::lua_config_value const &cfg );
	virtual	bool						compare				( xray::animation::i_skeleton_animation_data const & skel )const;

	//virtual	void						recalculate_memory_usage_impl ( ) { m_memory_usage_self.unmanaged = get_size(); }

public:
			bool						compare				( skeleton_animation_data const & data ) const;
//			void						build_animation		( skeleton_animation &data)const;
			void						set_skeleton		( const skeleton *skel );
	const	vector< u32 >				&skeleton_bone_index( )const;

private:
	vector< bone_animation_data >		m_bone_data;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	
virtual	i_bone_names 					*get_bone_names	( );

const	animation::bone_names			*bone_names	( )const{ return m_bone_names; }

private:
	animation::bone_names				*m_bone_names;

	vector< u32 >						m_skeleton_bone_index;
	const skeleton						*m_skeleton;

}; // class skeleton_animation_data

} // namespace animation
} // namespace xray

#endif // #ifndef SKELETON_ANIMATION_DATA_H_INCLUDED