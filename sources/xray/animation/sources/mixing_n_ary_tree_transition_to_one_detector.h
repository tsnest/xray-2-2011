////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TRANSITION_TO_ONE_DETECTOR_H_INCLUDED
#define MIXING_N_ARY_TREE_TRANSITION_TO_ONE_DETECTOR_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_transition_to_one_detector : public n_ary_tree_visitor {
public:
	inline	n_ary_tree_transition_to_one_detector	( ) : m_result( false ), m_has_transitions( false ) { }
	inline	bool	result	( ) const { return m_result; }
	inline	bool	has_transitions	( ) const { return m_has_transitions; }
	
private:
	virtual	void	visit	( n_ary_tree_animation_node& node );
	virtual	void	visit	( n_ary_tree_weight_transition_node& node );
	virtual	void	visit	( n_ary_tree_time_scale_transition_node& node );
	virtual	void	visit	( n_ary_tree_weight_node& node );
	virtual	void	visit	( n_ary_tree_time_scale_node& node );
	virtual	void	visit	( n_ary_tree_addition_node& node );
	virtual	void	visit	( n_ary_tree_subtraction_node& node );
	virtual	void	visit	( n_ary_tree_multiplication_node& node );

private:
	bool	m_result;
	bool	m_has_transitions;
}; // class n_ary_tree_transition_to_one_detector

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_TRANSITION_TO_ONE_DETECTOR_H_INCLUDED