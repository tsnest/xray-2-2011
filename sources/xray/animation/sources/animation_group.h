////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_GROUP_H_INCLUDED
#define ANIMATION_GROUP_H_INCLUDED

#include "animation_layer.h"
#include "fixed_bone.h"

#include <xray/fs_path_string.h>


namespace xray {

namespace configs
{
	class lua_config_value;
} // namespace configs

namespace resources
{
	class queries_result;
} // namespace resources



namespace animation {

class skeleton_animation;
class skeleton_animation_data;
class animation_layer;
class skeleton;
struct frame;



typedef					boost::function< void ( ) >	loaded_callback;

class animation_group {
public:
							animation_group			( configs::lua_config_value const &cfg, const skeleton *skel, const fixed_bones &fixed_bones  );
							~animation_group		( );
	
	void					play					( animation_vector_type &anims, const buffer_vector< float > &weights, float time )const;

	void					get_displacement		( frame &f, float3 &foot_displacement, u32 bone, const fixed_bones &fixed_bones, const buffer_vector< float > &weights )const;
	void					get_end_matrix			( float4x4 &m, u32 bone, const buffer_vector< float > &weights )const;
	u32						bone_index				( pcstr bone_name )const;
	
	u32						fixed_bone				( float time, const buffer_vector< float > &weights )const;

	inline float			time					()const	{ return m_time; }

	inline float			tail_time				()const { return time() - m_play_time; }
	inline float			play_time				()const { return m_play_time; }
	inline u32				gait					()const { return m_gait; }

	inline u32				animation_count			()const { return m_animations.size();}
	inline void				set_callback_on_loaded	( const loaded_callback &cb  ){ m_callback_on_loaded = cb; }
	inline bool				loaded					()const	{ return b_loaded; }
	inline void				set_name				( pcstr name ){ m_path = name; }
	inline pcstr			name					()const { return m_path.c_str(); }
	
	inline const foot_steps	&foots					()const { return m_foot_steps; }				

private:
	vector< skeleton_animation* >		m_animations;
	vector< skeleton_animation_data* >	m_animation_data;
	const skeleton						*m_skeleton;

	float								m_time;
	float								m_mix_in_time;
	float								m_play_time;
	
	u32									m_gait;

	bool								b_loaded;
	u32									m_size;
	loaded_callback						m_callback_on_loaded;
	fs::path_string						m_path;

	foot_steps							m_foot_steps;
	

private:
	void			create_animations	( );
	void			add_animation		( skeleton_animation_data &data  );
	void			on_anim_data_loaded	( xray::resources::queries_result& resource );
	

}; // class animation_group

} // namespace animation
} // namespace xray

#endif // #ifndef ANIMATION_GROUP_H_INCLUDED