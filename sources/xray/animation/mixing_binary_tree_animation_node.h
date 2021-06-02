////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_BINARY_TREE_ANIMATION_NODE_H_INCLUDED
#define MIXING_BINARY_TREE_ANIMATION_NODE_H_INCLUDED

#include <xray/animation/mixing_binary_tree_base_node.h>
#include <xray/animation/mixing.h>
#include <xray/animation/instant_interpolator.h>
#include <xray/animation/mixing_animation_interval.h>

namespace xray {
namespace animation {
namespace mixing {

class channel_event_callback;
class binary_tree_animation_node;
class n_ary_tree_animation_node;

} // namespace mixing
} // namespace animation

template class XRAY_ANIMATION_API
	intrusive_ptr<
		animation::mixing::binary_tree_animation_node,
		animation::mixing::binary_tree_base_node,
		threading::single_threading_policy
	>;

namespace animation {
namespace mixing {

class animation_lexeme_parameters;
class animation_interval;

typedef intrusive_ptr<
	binary_tree_animation_node,
	binary_tree_base_node,
	threading::single_threading_policy
> binary_tree_animation_node_ptr;

class XRAY_ANIMATION_API binary_tree_animation_node : public binary_tree_base_node {

public:
			explicit				binary_tree_animation_node	( animation_lexeme_parameters const& parameters );
	inline							binary_tree_animation_node	( binary_tree_animation_node const& other );

	inline	mutable_buffer&			buffer						( ) const { return *m_buffer; }
	inline	float					time_scale					( ) const;
	inline	playing_type_enum		playing_type				( ) const;
	inline	u32						synchronization_group_id	( ) const;
	inline	u32						additivity_priority			( ) const;
	inline	binary_tree_animation_node_ptr driving_animation	( ) const;
	inline	n_ary_tree_animation_node* n_ary_driving_animation	( ) const;
	inline	void					set_driving_animation		( binary_tree_animation_node* animation_to_syynchronize_with );
	inline	void					set_n_ary_driving_animation	( n_ary_tree_animation_node* animation );
	inline	base_interpolator const& weight_interpolator		( ) const { R_ASSERT(m_weight_interpolator); return *m_weight_interpolator; }
	inline	base_interpolator const* time_scale_interpolator	( ) const { return m_time_scale_interpolator; }
#ifndef MASTER_GOLD
	inline	pcstr					identifier					( ) const { return m_identifier; }
#endif
	inline	u32						start_animation_interval_id	( ) const { return m_start_animation_interval_id; }
	inline	float				start_animation_interval_time	( ) const { return m_start_animation_interval_time; }
	inline	bool				override_existing_animation( ) const { return m_override_existing_animation; }

public:
	inline	animation_interval const* animation_intervals_begin	( )	const { return m_animation_intervals; }
	inline	animation_interval const* animation_intervals_end	( )	const { return m_animation_intervals + m_animation_intervals_count; }
	inline	u32						animation_intervals_count	( ) const { return m_animation_intervals_count; }
	inline	u32				start_cycle_animation_interval_id	( ) const { return m_start_cycle_animation_interval_id; }

public:
	virtual	void					accept						( binary_tree_visitor& visitor );

protected:
			void					destroy_animation_intervals	( );

private:
	inline	binary_tree_animation_node& operator=				( binary_tree_animation_node const& other );
	inline	void				assign_uninitialized_user_data	( );
	static	animation_interval const* clone						(
										mutable_buffer& buffer,
										animation_interval const* animation_intervals_begin,
										animation_interval const* animation_intervals_end
									);
	static	animation_interval const* clone						(
										mutable_buffer& buffer,
										skeleton_animation_ptr const& animation
									);
#ifndef MASTER_GOLD
	static	pcstr					clone						(
										mutable_buffer& buffer,
										pcstr identifier
									);
#endif // #ifndef MASTER_GOLD
	static	u32						animation_intervals_count	( skeleton_animation_ptr const& animation );
	static	u32						get_animation_interval_id	( skeleton_animation_ptr const& animation, float start_animation_time );
	static	float					get_animation_interval_time	( skeleton_animation_ptr const& animation, u32 start_interval_id, float start_animation_time );

private:
	mutable_buffer* const			m_buffer;
#ifndef MASTER_GOLD
	pcstr const						m_identifier;
#endif // #ifndef MASTER_GOLD
	animation_interval const* const	m_animation_intervals;
	base_interpolator const* const	m_weight_interpolator;
	base_interpolator const* const	m_time_scale_interpolator;
	binary_tree_animation_node*		m_driving_animation;
	n_ary_tree_animation_node*		m_n_ary_driving_animation;

public:
	binary_tree_animation_node_ptr	m_next_animation;
	u32								m_unique_weights_count;
#ifndef MASTER_GOLD
	u32								user_data;
#endif // #ifndef MASTER_GOLD
	
private:
	u32 const						m_animation_intervals_count;
	u32 const						m_start_animation_interval_id;
	float const						m_start_animation_interval_time;
	u32 const						m_start_cycle_animation_interval_id;
	float const						m_time_scale;
	playing_type_enum const			m_playing_type;
	u32 const						m_synchronization_group_id;
	u32 const						m_additivity_priority;
	bool const						m_override_existing_animation;

public:
	bool							m_null_weight_found;
}; // class binary_tree_animation_node

} // namespace mixing
} // namespace animation
} // namespace xray

#include <xray/animation/mixing_binary_tree_animation_node_inline.h>

#endif // #ifndef MIXING_BINARY_TREE_ANIMATION_NODE_H_INCLUDED