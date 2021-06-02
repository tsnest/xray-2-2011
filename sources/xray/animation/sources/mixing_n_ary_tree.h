////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_H_INCLUDED
#define N_ARY_TREE_H_INCLUDED

//#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_intrusive_base.h"
#include "mixing_animation_state.h"

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

class n_ary_tree_animation_node;

class n_ary_tree {
public:
	typedef	intrusive_ptr	<
				n_ary_tree_intrusive_base,
				n_ary_tree_intrusive_base,
				threading::single_threading_policy
			>						n_ary_tree_ptr;

public:
	inline							n_ary_tree				( );
	inline							n_ary_tree				(
										n_ary_tree_animation_node* const root,
										memory::base_allocator* const allocator,
										base_interpolator const** const interpolators,
										animation_state* const animations_states,
										n_ary_tree_intrusive_base* const reference_counter,
										u32 const animations_count,
										u32 const interpolators_count,
										u32 const transitions_count
									);
	inline							n_ary_tree				( n_ary_tree const& other );
	inline	n_ary_tree&				operator =				( n_ary_tree const& other );
									~n_ary_tree				( );
			void					tick					( u32 const current_time_in_ms );
			void					dump					( ) const;
	inline	animation_state const*	animation_states		( ) const;
	inline	u32				significant_animations_count	( ) const;
	inline	n_ary_tree_animation_node* root					( ) const;
	inline	base_interpolator const* const*	interpolators	( ) const;
	inline	u32						interpolators_count		( ) const;

private:
	inline	void					assign					( n_ary_tree const& other );
			void					destroy					( );

private:
	n_ary_tree_animation_node*		m_root;
	memory::base_allocator*			m_allocator;
	base_interpolator const**		m_interpolators;
	animation_state*				m_animation_states;
	n_ary_tree_ptr					m_reference_counter;
	u32								m_animations_count;
	u32								m_significant_animations_count;
	u32								m_interpolators_count;
	u32								m_transitions_count;
}; // class n_ary_tree

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_inline.h"

#endif // #ifndef N_ARY_TREE_H_INCLUDED