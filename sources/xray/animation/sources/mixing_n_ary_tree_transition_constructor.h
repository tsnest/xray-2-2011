////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_TRANSITION_CONSTRUCTOR_H_INCLUDED
#define N_ARY_TREE_TRANSITION_CONSTRUCTOR_H_INCLUDED

#include "mixing_n_ary_tree_double_dispatcher.h"
#include "mixing_n_ary_tree_visitor.h"

namespace xray {
namespace animation {

struct base_interpolator;
class bone_mixer;

namespace mixing {

class n_ary_tree;
class n_ary_tree_intrusive_base;
struct animation_state;

enum interpolation_direction;

class n_ary_tree_transition_constructor : public n_ary_tree_visitor {
public:
			n_ary_tree_transition_constructor	(
						mutable_buffer& buffer,
						bone_mixer& bone_mixer,
						n_ary_tree const& from,
						n_ary_tree const& to,
						base_interpolator const* additive_interpolator,
						base_interpolator const* non_additive_interpolator,
						u32 const animations_count,
						u32 const current_time_in_ms
					);
	n_ary_tree		computed_tree				( memory::base_allocator* allocator );

private:
	n_ary_tree_transition_constructor			( n_ary_tree_transition_constructor const& );
	n_ary_tree_transition_constructor& operator=( n_ary_tree_transition_constructor const& );

private:
	virtual	void	visit						( n_ary_tree_animation_node& node );
	virtual	void	visit						( n_ary_tree_weight_node& node );
	virtual	void	visit						( n_ary_tree_addition_node& node );
	virtual	void	visit						( n_ary_tree_subtraction_node& node );
	virtual	void	visit						( n_ary_tree_multiplication_node& node );
	virtual	void	visit						( n_ary_tree_transition_node& node );

private:
	template <typename T>
			void	propagate					( T& left, T& right );
	template < typename T >
	inline	T*		new_constructed				( T& node );
	template < typename T >
	inline	void	process						( T& left, T& right );

private:
			void	process_interpolators		(
						mutable_buffer& buffer,
						n_ary_tree const& from,
						n_ary_tree const& to,
						base_interpolator const* additive_interpolator,
						base_interpolator const* non_additive_interpolator
					);

private:
	enum interpolation_direction {
		interpolation_increase,
		interpolation_decrease,
		interpolation_no_interpolation,
	}; // enum interpolation_direction

			void	add_animation				(
						n_ary_tree_animation_node** left_animation,
						n_ary_tree_animation_node** right_animation,
						n_ary_tree_animation_node*& previous_animation
					);

	n_ary_tree_animation_node* add_animation	(
						n_ary_tree_animation_node& animation,
						base_interpolator const& interpolator,
						interpolation_direction interpolation_direction
					);

	void			compute_operands_count		(
						n_ary_tree_animation_node& from_animation,
						n_ary_tree_animation_node& to_animation,
						u32& operands_count,
						u32& left_unique_count,
						u32& right_unique_count
					) const;

	void			fill_operands				(
						n_ary_tree_animation_node& from_animation,
						n_ary_tree_animation_node& to_animation,
						n_ary_tree_base_node**& operands,
						n_ary_tree_base_node** left_unique_nodes,
						n_ary_tree_base_node** right_unique_nodes
					);

	void			create_transition_node		(
						n_ary_tree_base_node**& operands,
						u32 const left_unique_count,
						u32 const right_unique_count,
						n_ary_tree_base_node** left_unique_nodes_begin,
						n_ary_tree_base_node** right_unique_nodes_begin,
						base_interpolator const& interpolator
					);

	n_ary_tree_animation_node* add_animation	(
						n_ary_tree_animation_node& from_animation,
						n_ary_tree_animation_node& to_animation,
						base_interpolator const& interpolator
					);
	template < typename T >
	inline	void	propagate					( T& node );
	base_interpolator const& cloned_interpolator( base_interpolator const& interpolator ) const;

private:
	mutable_buffer&				m_buffer;
	bone_mixer&					m_bone_mixer;
	n_ary_tree_animation_node&	m_from;
	n_ary_tree_animation_node&	m_to;
	n_ary_tree_base_node*		m_cloned_tree;
	n_ary_tree_animation_node*	m_result;
	base_interpolator const**	m_interpolators;
	animation_state*			m_animation_states;
	base_interpolator const*	m_additive_interpolator;
	base_interpolator const*	m_non_additive_interpolator;
	n_ary_tree_intrusive_base*	m_reference_counter;
	u32							m_animations_count;
	u32							m_interpolators_count;
	u32							m_transitions_count;
	u32							m_current_time_in_ms;
}; // class n_ary_tree_transition_constructor

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_TRANSITION_CONSTRUCTOR_H_INCLUDED