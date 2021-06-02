////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_ANIMATION_PLAYER_H_INCLUDED
#define XRAY_ANIMATION_ANIMATION_PLAYER_H_INCLUDED

#include <xray/animation/animation_callback.h>
#include <xray/animation/mixing_n_ary_tree.h>

namespace xray {

namespace render {
namespace debug {
	class renderer;
} // namespace render
} // namespace xray

namespace animation {

struct editor_animation_state;

namespace mixing {
	class expression;
	struct n_ary_tree_base_node;
} // namespace mixing

template class XRAY_ANIMATION_API boost::function< void (xray::animation::mixing::expression const&, u32) >;

enum reserved_channel_ids_enum {
	channel_id_on_animation_end				= 1,
	channel_id_on_animation_interval_end,
//	channel_id_on_animation_lexeme_end,
	
	channel_id_max,
}; // enum reserved_channel_ids_enum

class XRAY_ANIMATION_API animation_player : public resources::unmanaged_resource
{
public:
							animation_player		( );
			bool			set_target_and_tick		( mixing::expression const& expression, u32 const current_time_in_ms );
			void			tick					( u32 current_time_in_ms );

			void			subscribe				( pcstr channel_id, callback_type const& callback, u8 callback_id );
			void			subscribe				( reserved_channel_ids_enum channel_id, callback_type const& callback, u8 callback_id );
			void			unsubscribe				( pcstr channel_id, u8 callback_id );
			void			unsubscribe				( reserved_channel_ids_enum channel_id, u8 callback_id );

			void			set_object_transform	( float4x4 const& transform );
			float4x4		get_object_transform	( ) const;
			void			compute_bones_matrices	( skeleton const& skeleton, float4x4* begin, float4x4* end ) const;

	inline	bool			are_there_any_animations( ) const { return m_mixing_tree.are_there_any_animations(); }

public:
#ifndef MASTER_GOLD
			void			set_controller_callback	( boost::function<void (mixing::expression const&, u32)> const& callback );
			void			reset					( bool clear_callbacks = false );
			void			fill_animation_states	( vectora< editor_animation_state >& result );
#endif // #ifndef MASTER_GOLD

private:
			bool			set_target				( mixing::expression const& expression, u32 const current_time_in_ms );
	inline	pvoid			get_next_buffer			( u32 buffer_size );
			void			compact_callbacks		( );
	static	void			destroy_subscriptions	( subscribed_channel const* channels_head );

public:
	enum {
		stack_buffer_size		= 1024*sizeof( pvoid ),
		callbacks_buffer_size	= 64*sizeof( pvoid ),
	};

private:
	typedef char			buffer_type[ stack_buffer_size ];

private:
	buffer_type					m_tree_buffers[ 2 ];
	char						m_callbacks_buffer_raw[ callbacks_buffer_size ];
	mixing::n_ary_tree			m_mixing_tree;
	buffer_type*				m_current_buffer;
	subscribed_channel*			m_first_subscribed_channel;
	mutable_buffer				m_callbacks_buffer;
	u32							m_subscribed_channels_count;;
	u16							m_in_tick;
	bool						m_callbacks_are_actual;

#ifndef MASTER_GOLD
private:
	boost::function< void ( mixing::expression const&, u32 ) >	m_controller_callback;
#endif // #ifndef MASTER_GOLD
}; // class animation_player

typedef	resources::resource_ptr <
	animation_player,
	resources::unmanaged_intrusive_base
> animation_player_ptr;

			void		test_animation_player_new	(
								skeleton_animation_ptr skeleton_animation0,
								skeleton_animation_ptr skeleton_animation1,
								skeleton const& skeleton
							);

} // namespace animation
} // namespace xray

#include <xray/animation/animation_player_inline.h>

#endif // #ifndef XRAY_ANIMATION_ANIMATION_PLAYER_H_INCLUDED
