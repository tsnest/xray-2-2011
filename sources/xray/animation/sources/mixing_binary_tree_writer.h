////////////////////////////////////////////////////////////////////////////
//	Created		: 21.03.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_BINARY_TREE_WRITER_H_INCLUDED
#define MIXING_BINARY_TREE_WRITER_H_INCLUDED

#ifndef MASTER_GOLD

#include <xray/animation/mixing_binary_tree_visitor.h>
#include <xray/animation/interpolator_visitor.h>

namespace xray {
namespace animation {
namespace mixing {

class XRAY_ANIMATION_API binary_tree_writer : public binary_tree_visitor, public interpolator_visitor 
{
public:
	enum node_types {
		animation_node_type = 1,
		addition_node_type = 2,
		weight_node_type = 3,
		substraction_node_type = 4,
		multiplication_node_type = 5
	};
	enum interpolator_types {
		instant = 0,
		linear = 1,
		fermi = 2
	};
public:
					binary_tree_writer	(xray::configs::lua_config_value& cfg, float2& start_pos);

private:
					binary_tree_writer	(binary_tree_writer const& other);
		binary_tree_writer& operator =	(binary_tree_writer const& other);
	virtual	void				visit	(binary_tree_animation_node& node);
	virtual	void				visit	(binary_tree_weight_node& node);
	virtual	void				visit	(binary_tree_addition_node& node);
	virtual	void				visit	(binary_tree_subtraction_node& node);
	virtual	void				visit	(binary_tree_multiplication_node& node);
	virtual	void				visit	(instant_interpolator const& interpolator);
	virtual	void				visit	(linear_interpolator const& interpolator);
	virtual	void				visit	(fermi_interpolator const& interpolator);

private:
	xray::configs::lua_config_value&	m_config;
	u32									m_last_id;
	float2&								m_position;
}; // class binary_tree_null_weight_searcher

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MASTER_GOLD

#endif // #ifndef MIXING_BINARY_TREE_WRITER_H_INCLUDED