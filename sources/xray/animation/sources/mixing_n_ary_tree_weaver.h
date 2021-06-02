////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_WEAVER_H_INCLUDED
#define N_ARY_TREE_WEAVER_H_INCLUDED

#include <xray/animation/mixing_binary_tree_visitor.h>

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

struct binary_tree_base_node;

class n_ary_tree_weaver :
	public binary_tree_visitor,
	private boost::noncopyable
{
public:
	explicit		n_ary_tree_weaver	( mutable_buffer& buffer );
	inline	binary_tree_animation_node*	animations_root	( ) const { return m_animations_root; }
	inline	binary_tree_base_node*	interpolators_root	( ) const { return m_interpolators_root; }
	inline	u32		interpolators_count	( ) const { return m_interpolators_count; }

private:
	virtual	void	visit				( binary_tree_animation_node& node );
	virtual	void	visit				( binary_tree_weight_node& node );
	virtual	void	visit				( binary_tree_addition_node& node );
	virtual	void	visit				( binary_tree_subtraction_node& node );
	virtual	void	visit				( binary_tree_multiplication_node& node );

private:
			void	join_animations		( n_ary_tree_weaver const& other );

	template < typename T >
	inline	void	propagate			( T& node, n_ary_tree_weaver& weaver );

	template < typename T >
	inline	void	propagate			( T& node, n_ary_tree_weaver& left, n_ary_tree_weaver& right );

private:
			void	add_interpolator	( binary_tree_base_node& node, base_interpolator const& interpolator );

private:
	mutable_buffer&				m_buffer;
	binary_tree_base_node*		m_new_node;
	binary_tree_animation_node*	m_animations_root;
	binary_tree_base_node*		m_weights_root;
	binary_tree_base_node*		m_interpolators_root;
	binary_tree_animation_node*	m_current_animations_root;
	u32							m_interpolators_count;
}; // class n_ary_tree_weaver

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_WEAVER_H_INCLUDED