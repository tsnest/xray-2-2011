////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_NODE_COMPARER_H_INCLUDED
#define N_ARY_TREE_NODE_COMPARER_H_INCLUDED

#include "mixing_n_ary_tree_double_dispatcher.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_node_comparer : public n_ary_tree_double_dispatcher {
public:
	enum enum_result {
		equal,
		less,
		more,
	}; // enum enum_result

public:
	inline	n_ary_tree_node_comparer	( ) : result(equal) { }
		enum_result	compare		( n_ary_tree_base_node& left,					n_ary_tree_base_node& right );
	inline	bool	operator ( )( n_ary_tree_base_node& left,					n_ary_tree_base_node& right ) { return compare( left, right ) == less; }
	inline	bool	operator ( )( n_ary_tree_base_node* left,					n_ary_tree_base_node* right ) { return compare( *left, *right ) == less; }

private:
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_animation_node& right );
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_weight_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_time_scale_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_weight_node& right );
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_time_scale_node& right );
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_addition_node& right );
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_subtraction_node& right );
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_multiplication_node& right );

	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,				n_ary_tree_animation_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,				n_ary_tree_weight_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,				n_ary_tree_time_scale_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,				n_ary_tree_weight_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,				n_ary_tree_time_scale_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,				n_ary_tree_addition_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,				n_ary_tree_subtraction_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,				n_ary_tree_multiplication_node& right );

	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_animation_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_weight_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_time_scale_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_weight_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_time_scale_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_addition_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_subtraction_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_multiplication_node& right );

	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_animation_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_weight_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_time_scale_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_weight_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_time_scale_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_addition_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_subtraction_node& right );
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_multiplication_node& right );

	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_animation_node& right );
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_weight_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_time_scale_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_weight_node& right );
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_time_scale_node& right );
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_addition_node& right );
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_subtraction_node& right );
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_multiplication_node& right );

	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_animation_node& right );
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_weight_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_time_scale_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_weight_node& right );
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_time_scale_node& right );
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_addition_node& right );
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_subtraction_node& right );
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_multiplication_node& right );

	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_animation_node& right );
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_weight_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_time_scale_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_weight_node& right );
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_time_scale_node& right );
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_addition_node& right );
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_subtraction_node& right );
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_multiplication_node& right );

	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_animation_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_weight_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_time_scale_transition_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_weight_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_time_scale_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_addition_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_subtraction_node& right );
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_multiplication_node& right );

private:
	template <typename T>
	inline void		propagate	( T& left, T& right );

public:
	enum_result		result;
}; // class n_ary_tree_node_comparer

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_NODE_COMPARER_H_INCLUDED