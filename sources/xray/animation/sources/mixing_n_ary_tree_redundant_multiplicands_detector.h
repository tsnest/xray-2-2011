////////////////////////////////////////////////////////////////////////////
//	Created		: 19.10.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_REDUNDANT_MULTIPLICANDS_DETECTOR_H_INCLUDED
#define MIXING_N_ARY_TREE_REDUNDANT_MULTIPLICANDS_DETECTOR_H_INCLUDED

#include <xray/animation/mixing_binary_tree_visitor.h>

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

class n_ary_tree_redundant_multiplicands_detector : public binary_tree_visitor {
public:
	explicit	n_ary_tree_redundant_multiplicands_detector	( base_interpolator const& interpolator );
	inline	bool				result						( ) const { return m_result; }

private:
	n_ary_tree_redundant_multiplicands_detector				( n_ary_tree_redundant_multiplicands_detector const& other );
	n_ary_tree_redundant_multiplicands_detector& operator =	( n_ary_tree_redundant_multiplicands_detector const& other );

private:
	virtual	void				visit						( binary_tree_animation_node& node );
	virtual	void				visit						( binary_tree_weight_node& node );
	virtual	void				visit						( binary_tree_addition_node& node );
	virtual	void				visit						( binary_tree_subtraction_node& node );
	virtual	void				visit						( binary_tree_multiplication_node& node );

private:
	base_interpolator const&	m_interpolator;
	bool						m_result;
}; // class n_ary_tree_redundant_multiplicands_detector

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_REDUNDANT_MULTIPLICANDS_DETECTOR_H_INCLUDED