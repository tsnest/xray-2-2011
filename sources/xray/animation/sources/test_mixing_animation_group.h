////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_MIXING_ANIMATION_GROUP_H_INCLUDED
#define TEST_MIXING_ANIMATION_GROUP_H_INCLUDED

namespace xray {
namespace animation {

class skeleton;
class mixer;
class bone_mixer;
class skeleton_animation;

namespace mixing {
	class animation_clip;
} // namespace mixing

struct test_mixing_animation_group
{
	typedef std::pair<mixing::animation_clip*, float>	animation_pair;

	
	
		test_mixing_animation_group	(  configs::lua_config_value cfg, skeleton const*	skeleton  );
		~test_mixing_animation_group( );
	void	on_animations_loaded	( xray::resources::queries_result& resource );
	void	add_animation			( configs::lua_config_value const &cfg, xray::resources::query_result_for_user const &anim, skeleton const*	skeleton );
	void	set_target				( mixer &mxr, const u32 time )const;
	float	play_time				( ) const ;
	float	max_time				( ) const { return m_max_time ;}
	bool	loaded					( ) const { return m_loaded; }

	float							m_max_mix_time;
	float							m_max_time;
	configs::lua_config_value		m_config;
	skeleton const*					m_skeleton;
	vector< skeleton_animation* >	m_p_animations;//
	vector< animation_pair >		m_clips;
	bool							m_loaded;

};

} // namespace animation
} // namespace xray

#endif // #ifndef TEST_MIXING_ANIMATION_GROUP_H_INCLUDED