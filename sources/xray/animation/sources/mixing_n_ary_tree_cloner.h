////////////////////////////////////////////////////////////////////////////
//	Created		: 02.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_CLONER_H_INCLUDED
#define MIXING_N_ARY_TREE_CLONER_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

struct n_ary_tree_base_node;
class n_ary_tree;

class n_ary_tree_cloner :
	private n_ary_tree_visitor,
	private boost::noncopyable
{
public:
	explicit		n_ary_tree_cloner	( mutable_buffer& buffer, u32 start_time_in_ms );
			void	initialize			(
						n_ary_tree const& from,
						n_ary_tree const& to
					);

public:
	inline n_ary_tree_base_node* clone	( n_ary_tree_base_node& node_to_clone ) { return clone(node_to_clone, (base_interpolator*)0); }
	n_ary_tree_base_node*		clone	( n_ary_tree_base_node& node_to_clone, base_interpolator const* animation_interpolator );
	n_ary_tree_base_node*		clone	( n_ary_tree_base_node& node_to_clone, float time_scale_factor, float animation_interval_time );
	n_ary_tree_base_node*		clone	( n_ary_tree_base_node& node_to_clone, float time_scale_factor );
	base_interpolator const*	clone	( base_interpolator const& interpolator, bool assert_on_failure = true ) const;

public:
	inline	base_interpolator const** interpolators	( ) const { return m_interpolators; }
	inline	u32		interpolators_count	( ) const { return m_interpolators_count; }

private:
	virtual	void	visit				( n_ary_tree_animation_node& node );
	virtual	void	visit				( n_ary_tree_weight_node& node );
	virtual	void	visit				( n_ary_tree_time_scale_node& node );
	virtual	void	visit				( n_ary_tree_addition_node& node );
	virtual	void	visit				( n_ary_tree_subtraction_node& node );
	virtual	void	visit				( n_ary_tree_multiplication_node& node );
	virtual	void	visit				( n_ary_tree_weight_transition_node& node );
	virtual	void	visit				( n_ary_tree_time_scale_transition_node& node );

private:
	template < typename T >
	inline	void	propagate			( T& node );
	template < typename T >
	inline	T*		new_constructed		( T& node );

	inline	n_ary_tree_animation_node* new_constructed	( n_ary_tree_animation_node& node );

	template < typename T >
	inline	void	process				( T& left, T& right );

private:
	n_ary_tree_base_node*				m_result;
	mutable_buffer&						m_buffer;
	base_interpolator const*			m_animation_interpolator;
	base_interpolator const**			m_interpolators;
	float const*						m_animation_interval_time;
	u32 const							m_start_time_in_ms;
	u32									m_interpolators_count;
	float								m_time_scale_factor;
}; // class mixing_n_ary_tree_cloner

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_CLONER_H_INCLUDED