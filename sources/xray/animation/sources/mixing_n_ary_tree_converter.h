////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_CONVERTER_H_INCLUDED
#define N_ARY_TREE_CONVERTER_H_INCLUDED

#include <xray/animation/mixing_n_ary_tree.h>

namespace xray {
namespace animation {
namespace mixing {

class expression;
class n_ary_tree_animation_node;
class binary_tree_animation_node;
struct binary_tree_base_node;

typedef intrusive_ptr<
			binary_tree_base_node,
			binary_tree_base_node,
			threading::single_threading_policy
		> binary_tree_base_node_ptr;

class n_ary_tree_converter : private boost::noncopyable {
public:
				n_ary_tree_converter	( expression const& expression );
				~n_ary_tree_converter	( );
		u32		needed_buffer_size		( );
	n_ary_tree	constructed_n_ary_tree	(
					xray::mutable_buffer& buffer,
					bool const is_final_tree,
					u32 const current_time_in_ms,
					float4x4 const& object_transform,
					subscribed_channel*& channels_head
				);

private:
	binary_tree_base_node* create_binary_multipliers	(
					mutable_buffer& buffer,
					binary_tree_base_node* const start_weight
				);
		void	sort_animations			( mutable_buffer& buffer );
		void	simplify_weights		( );
		void	compute_buffer_size		( );
		void	process_interpolators	( binary_tree_base_node* const interpolators_root, u32 const interpolators_count, xray::mutable_buffer& buffer );

private:
	binary_tree_base_node_ptr			m_root;
	binary_tree_animation_node*			m_animations_root;
	base_interpolator const**			m_binary_interpolators;
	base_interpolator const**			m_interpolators;
	animation_state*					m_animation_states;
	animation_state**					m_animation_events;
	n_ary_tree_intrusive_base*			m_reference_counter;
	binary_tree_base_node*				m_nodes_to_destroy_manually;
	u32									m_animations_count;
	u32									m_interpolators_count;
	u32									m_buffer_size;
}; // struct n_ary_tree_converter

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_CONVERTER_H_INCLUDED