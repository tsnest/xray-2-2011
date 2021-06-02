////////////////////////////////////////////////////////////////////////////
//	Created		: 07.10.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef EDITOR_MIXER_H_INCLUDED
#define EDITOR_MIXER_H_INCLUDED

//#include "fixed_bone.h"

#include <xray/animation/i_editor_mixer.h>
#include <xray/animation/mixing_expression.h>
#include <xray/animation/animation_player.h>

namespace xray
{
	namespace animation
	{
		class animation_mixer;
		class bone_mixer;
		class mixer;
		class skeleton;
		class footsteps_position_modifier;

		struct editor_animation_state;
		namespace mixing { class expression; }

		class editor_mixer : public i_editor_mixer, private boost::noncopyable
		{
		public:
										editor_mixer				( memory::base_allocator& allocator, animation_player& animation_player );
			virtual 					~editor_mixer				( );

		public:
			virtual void				render						(
											render::scene_ptr const&	scene,
											render::scene_renderer&		scene_renderer,
											render::debug::renderer&	debug_renderer,
											render::skeleton_model_ptr	model
										);
			virtual void				set_target_and_tick			( mixing::expression const& expression, u32 const current_time_in_ms );
			virtual void				tick						( u32 const current_time_in_ms );
			virtual void				push_expression				( mixing::expression const& expression );
			virtual void				push_expression_time		( u32 const expression_time );
			virtual void				calculate_animations_events	( vectora<editor_animations_event>& events );
			virtual void				get_current_anim_states		( vectora<editor_animation_state>& result, u32 current_time_in_ms );
			virtual	camera_follower&	get_camera_follower			( ) { return m_camera_follower; }
			virtual	float4x4			get_object_matrix_for_camera( ) const;
			virtual void				subscribe_footsteps			( mixing::animation_lexeme& anim );
			virtual void				reset						( );
			virtual void				set_model_transform			( float4x4& m );

		private:	
					void	calculate_animations_events_callback	( vectora< editor_animations_event >* events, editor_animations_event const& e );

		private:
			animation_player&				m_animation_player;
			memory::base_allocator&			m_allocator;
			vectora<mixing::expression>		m_expressions;
			vectora<u32>					m_expression_times;
			camera_follower					m_camera_follower;

			//animation_mixer*				m_mixer;
			//bone_mixer*					m_bone_mixer;
			//footsteps_position_modifier*	m_footsteps_position_modifier;
			//fixed_bones					m_fixed_bones;

		}; // class mixer

	} // namespace editor_mixer
} // namespace xray

#endif // #ifndef EDITOR_MIXER_H_INCLUDED
