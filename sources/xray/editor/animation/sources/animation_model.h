////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATED_MODEL_H_INCLUDED
#define ANIMATED_MODEL_H_INCLUDED

#pragma managed ( push, off )
#	include <xray/render/facade/model.h>
#	include <xray/animation/i_editor_mixer.h>
#	include <xray/animation/animation_callback.h>
#pragma managed ( push, on )

using namespace System;

namespace xray
{
	namespace	render { namespace editor { class renderer; } }
	namespace	animation
	{ 
		class		i_editor_mixer;
		namespace	mixing { class expression; class animation_lexeme; } 
	}
	namespace animation_editor
	{
		delegate void animation_callback ( animation::skeleton_animation_ptr const& anim, pcstr channel_id, u32 callback_time_in_ms, u8 domain_data );

		struct animation_managed_callback;

		ref class animation_model
		{
			typedef xray::render::skeleton_model_ptr skeleton_model_ptr;
		public:

			

								animation_model				(
									render::scene_ptr const&	scene,
									render::scene_renderer&		scene_renderer,
									render::debug::renderer&	debug_renderer,
									System::String^				n,
									System::String^				mn,
									float4x4&					m,
									query_result_delegate*		cb
								);
								~animation_model				( );

		public:
			property			System::String^				name;
			property			System::String^				model_name;
			property			bool						camera_follow_me;
			property			float4x4&					transform
			{
				float4x4&		get							( );
				void			set							( float4x4& val );
			}

			event animation_callback^	animation_end;

		public:
			skeleton_model_ptr	model						( );
			void				render						( );
			void				set_target					( animation::mixing::expression const& expression, u32 const current_time_in_ms );
			void				tick						( u32 const current_time_in_ms );
			void				push_expression				( animation::mixing::expression const& expression, u32 const expression_time );
			void				calculate_animations_events	( vectora<animation::editor_animations_event>& events );
			void				get_current_anim_states		( vectora<animation::editor_animation_state>& result, u32 current_time_in_ms );
			animation::camera_follower*	get_camera_follower			( );
			void				subscribe_footsteps			( animation::mixing::animation_lexeme& l );
			void				reset						( );
			void				add_to_render				( );
			void				remove_from_render			( );
			float4x4			get_object_matrix			( );

		private:
			void				model_loaded				( xray::resources::queries_result& result );
			void				animation_ended				( animation::skeleton_animation_ptr const& animation, pcstr channel_id, u32 callback_time_in_ms, u8 domain_data );

		private:
			animation_managed_callback*		m_animation_callback;
			render::skeleton_model_ptr*		m_model;
			query_result_delegate*			m_callback;
			render::scene_renderer&			m_scene_renderer;
			render::debug::renderer&		m_debug_renderer;
			animation::animation_player*	m_animation_player;
			animation::i_editor_mixer*		m_mixer;
			render::scene_ptr*				m_scene;
			float4x4*						m_transform;
			bool							m_added_to_render;

		}; // class animation_model
	} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATED_MODEL_H_INCLUDED