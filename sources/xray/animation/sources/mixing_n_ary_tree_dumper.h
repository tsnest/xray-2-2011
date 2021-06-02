////////////////////////////////////////////////////////////////////////////
//	Created		: 07.10.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_DUMPER_H_INCLUDED
#define MIXING_N_ARY_TREE_DUMPER_H_INCLUDED

#include "mixing_n_ary_tree_visitor.h"
#include <xray/animation/interpolator_visitor.h>

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_n_ary_operation_node;

class n_ary_tree_dumper :
	public n_ary_tree_visitor,
	public interpolator_visitor,
	private boost::noncopyable
{
public:
	inline	n_ary_tree_dumper	( u32 const current_time_in_ms ) : m_offset(0), m_current_time_in_ms(current_time_in_ms) { }
	
private:
	virtual	void	visit		( n_ary_tree_animation_node& node );
	virtual	void	visit		( n_ary_tree_weight_node& node );
	virtual	void	visit		( n_ary_tree_time_scale_node& node );
	virtual	void	visit		( n_ary_tree_addition_node& node );
	virtual	void	visit		( n_ary_tree_subtraction_node& node );
	virtual	void	visit		( n_ary_tree_multiplication_node& node );
	virtual	void	visit		( n_ary_tree_weight_transition_node& node );
	virtual	void	visit		( n_ary_tree_time_scale_transition_node& node );

private:
	virtual	void	visit		( instant_interpolator const& other );
	virtual	void	visit		( linear_interpolator const& other );
	virtual	void	visit		( fermi_interpolator const& other );

private:
	template < typename T >
	inline	void	propagate	( T& node );

private:
	u32				m_offset;
	u32 const		m_current_time_in_ms;
}; // class n_ary_tree_dumper

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_DUMPER_H_INCLUDED