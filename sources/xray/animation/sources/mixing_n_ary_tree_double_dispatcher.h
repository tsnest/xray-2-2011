////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef N_ARY_TREE_DOUBLE_DISPATCHER_H_INCLUDED
#define N_ARY_TREE_DOUBLE_DISPATCHER_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

struct n_ary_tree_base_node;
class n_ary_tree_animation_node;
class n_ary_tree_weight_node;
class n_ary_tree_time_scale_node;
class n_ary_tree_addition_node;
class n_ary_tree_subtraction_node;
class n_ary_tree_multiplication_node;
class n_ary_tree_weight_transition_node;
class n_ary_tree_time_scale_transition_node;

struct XRAY_NOVTABLE n_ary_tree_double_dispatcher {
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_animation_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_weight_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_time_scale_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_weight_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_time_scale_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_addition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_subtraction_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_animation_node& left,				n_ary_tree_multiplication_node& right ) = 0;

	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,		n_ary_tree_animation_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,		n_ary_tree_weight_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,		n_ary_tree_time_scale_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,		n_ary_tree_weight_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,		n_ary_tree_time_scale_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,		n_ary_tree_addition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,		n_ary_tree_subtraction_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_transition_node& left,		n_ary_tree_multiplication_node& right ) = 0;

	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_animation_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_weight_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_time_scale_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_weight_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_time_scale_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_addition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_subtraction_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_transition_node& left,	n_ary_tree_multiplication_node& right ) = 0;

	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_animation_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_weight_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_time_scale_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_weight_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_time_scale_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_addition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_subtraction_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_weight_node& left,					n_ary_tree_multiplication_node& right ) = 0;

	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_animation_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_weight_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_time_scale_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_weight_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_time_scale_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_addition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_subtraction_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_addition_node& left,				n_ary_tree_multiplication_node& right ) = 0;

	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_animation_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_weight_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_time_scale_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_weight_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_time_scale_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_addition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_subtraction_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_subtraction_node& left,			n_ary_tree_multiplication_node& right ) = 0;

	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_animation_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_weight_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_time_scale_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_weight_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_time_scale_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_addition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_subtraction_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_multiplication_node& left,			n_ary_tree_multiplication_node& right ) = 0;

	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_animation_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_weight_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_time_scale_transition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_weight_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_time_scale_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_addition_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_subtraction_node& right ) = 0;
	virtual	void	dispatch	( n_ary_tree_time_scale_node& left,				n_ary_tree_multiplication_node& right ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( n_ary_tree_double_dispatcher )
}; // class n_ary_tree_double_dispatcher

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef N_ARY_TREE_DOUBLE_DISPATCHER_H_INCLUDED