////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_BINARY_TREE_EXPRESSION_SIMPLIFIER_H_INCLUDED
#define MIXING_BINARY_TREE_EXPRESSION_SIMPLIFIER_H_INCLUDED

#include <xray/animation/mixing_binary_tree_visitor.h>
#include <xray/animation/mixing_binary_tree_weight_node.h>

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

class binary_tree_binary_operation_node;

class binary_tree_expression_simplifier :
	public binary_tree_visitor,
	private boost::noncopyable
{
public:
			binary_tree_expression_simplifier	( binary_tree_base_node& expression_node, mutable_buffer& buffer );
	inline	binary_tree_base_node* result		( ) const { R_ASSERT(m_result); return m_result.c_ptr(); }

private:
	inline	explicit binary_tree_expression_simplifier	( mutable_buffer& buffer ) : m_buffer(buffer), m_result(0), m_result_weight(0) { }

	virtual	void	visit			( binary_tree_animation_node& node );
	virtual	void	visit			( binary_tree_weight_node& node );
	virtual	void	visit			( binary_tree_addition_node& node );
	virtual	void	visit			( binary_tree_subtraction_node& node );
	virtual	void	visit			( binary_tree_multiplication_node& node );

private:
	binary_tree_weight_node* new_weight	( float weight, base_interpolator const* interpolator ) const;

	template < typename Operation >
	inline	void	process_null_weight	(
						Operation const& operation,
						binary_tree_expression_simplifier const& left_simplifier,
						binary_tree_expression_simplifier const& right_simplifier
					);
	inline	void	process_null_weight	(
						std::multiplies<float> const& operation,
						binary_tree_expression_simplifier const& left_simplifier,
						binary_tree_expression_simplifier const& right_simplifier
					);
	template < typename T, typename Operation >
			void	process			( T& node, Operation const& operation );

private:
	mutable_buffer&					m_buffer;
	binary_tree_base_node_ptr		m_result;
	binary_tree_weight_node_ptr		m_result_weight;
}; // class binary_tree_expression_simplifier

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_BINARY_TREE_EXPRESSION_SIMPLIFIER_H_INCLUDED