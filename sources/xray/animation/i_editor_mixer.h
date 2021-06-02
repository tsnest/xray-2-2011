////////////////////////////////////////////////////////////////////////////
//	Created		: 07.10.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_I_EDITOR_MIXER_H_INCLUDED
#define XRAY_ANIMATION_I_EDITOR_MIXER_H_INCLUDED

#include <xray/render/facade/model.h>
#include <xray/animation/camera_follower.h>

namespace xray {
namespace render {

class scene_renderer;

namespace debug { class renderer; }

} // namespace render

namespace animation {
	namespace mixing { 
		class expression;
		class n_ary_tree_animation_node;
		class animation_lexeme;
	} // namespace mixing
	
	class	camera_follower;

	struct editor_animation_state
	{
		u32		user_data;
		pcstr	identifier;
		float	position;
		float	length;
		float	weight;
		float	time_scale;
		float	offset;
		float	time_scaled_length;
		bool	is_leading;
	}; // struct editor_animation_state

	struct editor_animations_event
	{
		enum event_type 
		{
			new_animation = 0,
			start_animation = 1,
			finish_animation = 2
		};

		editor_animations_event(event_type t, u32 time, u32 ud, u32 ad)
			:type(t),
			time_event_happened(time),
			user_data(ud),
			additional_data(ad)
		{};

		event_type	type;
		u32			time_event_happened;
		u32			user_data;
		u32			additional_data;
	}; // struct editor_animation_state

	class i_editor_mixer: public camera_follower_interface
	{
	public:
		virtual 			void	render						(
										render::scene_ptr const& scene,
										render::scene_renderer& scene_renderer,
										render::debug::renderer& debug_renderer,
										render::skeleton_model_ptr model
									) = 0;
		virtual 			void	set_target_and_tick			(mixing::expression const& expression, u32 const current_time_in_ms) = 0;
		virtual 			void	tick						(u32 const current_time_in_ms) = 0;
		virtual 			void	push_expression				(mixing::expression const& expression) = 0;
		virtual 			void	push_expression_time		(u32 const expression_time) = 0;
		virtual 			void	calculate_animations_events	(vectora<editor_animations_event>& events) = 0;
		virtual 			void	get_current_anim_states		(vectora<editor_animation_state>& cur_anim_states, u32 current_time_in_ms) = 0;
		virtual	camera_follower&	get_camera_follower			() = 0;	
		virtual				void	subscribe_footsteps			(mixing::animation_lexeme& anim) = 0;
		virtual				void	reset						() = 0;
		virtual				void	set_model_transform			(float4x4& m) = 0;
		virtual			float4x4	get_object_matrix_for_camera() const = 0;

	public:
		inline static void	call_user_callback		(editor_animations_event::event_type t, u32 time, mixing::n_ary_tree_animation_node* n, u32 ad);
		typedef boost::function<void (editor_animations_event)> editor_mixer_callback;
		static editor_mixer_callback*		callback;

	protected:
		XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( i_editor_mixer );
	}; // class i_editor_mixer
} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_I_EDITOR_MIXER_H_INCLUDED