////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_BUILDER_H_INCLUDED
#define N_ARY_TREE_BUILDER_H_INCLUDED

#include "mixing_n_ary_tree.h"

namespace xray {
namespace animation {
namespace mixing {

class expression;
class n_ary_tree_animation_node;
class binary_tree_animation_node;
struct binary_tree_base_node;

class n_ary_tree_builder : private boost::noncopyable {
public:
				n_ary_tree_builder		( expression const& expression );
		u32		needed_buffer_size		( );
		bool	need_transitions		( ) const;
	n_ary_tree	constructed_n_ary_tree	( xray::mutable_buffer& buffer, xray::memory::base_allocator* allocator, u32 const current_time_in_ms );

private:
		void	sort_animations			( );
		void	simplify_weights		( );
		void	compute_buffer_size		( );
		void	process_interpolators	( );

private:
	binary_tree_base_node&				m_root;
	binary_tree_animation_node*			m_animations_root;
	binary_tree_base_node*				m_interpolators_root;
	base_interpolator const**			m_interpolators;
	animation_state*					m_animation_states;
	n_ary_tree_intrusive_base*			m_reference_counter;
	u32									m_animations_count;
	u32									m_interpolators_count;
	u32									m_buffer_size;
}; // struct n_ary_tree_builder

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_BUILDER_H_INCLUDED