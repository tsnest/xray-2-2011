////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef VALUE_FUNCTION_H_INCLUDED
#define VALUE_FUNCTION_H_INCLUDED

#include "action_value.h"
#include "action.h"
#include <xray/math_randoms_generator.h>
#include "rtp_world.h"

//#include "dbg_sample_check_res.h"

namespace xray {

namespace configs{
	class lua_config_value;
} // namespace configs

namespace math{
 class random32_with_count;
} // namespace math

namespace rtp {





template< class regression_tree, class action >
class value_function:
	private boost::noncopyable
{
	typedef	regression_tree								regression_tree_type;

protected:
	typedef	action										action_type;
	
private:
	typedef	typename action_type::space_param_type				space_param_type;
	
	typedef	typename regression_tree_type::node_type			node_type;
	typedef	action_value< node_type, space_param_type >			action_value_type;

	typedef value_function< regression_tree, action >			self_type;


protected:
	
	static const u32	separate_dimensions_count	=	space_param_type::separate_dimensions_count;
	static const u32	discrete_dimension_size		=	space_param_type::discrete_dimension_size;

	typedef	typename	space_param_type::tree_space_param_type	tree_space_param_type;
	typedef	typename	space_param_type::separate_reward_type	separate_reward_type;

public:
		value_function		( const action_base< action_type >& ab );
virtual	~value_function		( ){}

public:
	separate_reward_type			core_pi		( const	action_type &afrom, u32 from_blend_id, const action_type &ato, u32 &blend_id, const space_param_type &from_param , 
																									space_param_type &to_param, 
																									//float &max_v, 
																									math::random32_with_count &rnd ) const;
	separate_reward_type			nu			( const	action_type &afrom, u32 from_blend_id, const space_param_type &from_param, math::random32_with_count &rnd ) const;

public:
const action_type	*pi			(	const action_type &from, u32 from_blend_id, const space_param_type &from_param, u32 &blend_id,  space_param_type &to_params, 
									separate_reward_type	&r, 
									//float &v,  
									math::random32_with_count &rnd ) const;
const action_type	*pi			( const action_type &from, u32 from_blend_id, const space_param_type &from_param, u32 &blend_id ) const;

public:
	separate_reward_type			value		(   const	action_type &ato, const space_param_type &to_param )const;

public:
	virtual	void		render		( xray::render::scene_ptr const& scene , xray::render::debug::renderer& renderer ) const;

public:
#ifndef MASTER_GOLD
	void			save		( xray::configs::lua_config_value cfg, bool ) const;
#endif // MASTER_GOLD

	void			load		( const xray::configs::binary_config_value &cfg );
	bool			empty		( ) const;
	
	bool			compare		( const self_type& value ) const;

protected:
	void			clear		( );

private:
	virtual	void	init_trees	( );

private:
	struct core_pi_result {
		space_param_type	to_param;
		u32					blend_id;
	}; // struct core_pi_result

	struct pi_result {
		space_param_type	to_param;
		action const*		ret;
		u32					blend_id;
		//float				reward[separate_dimensions_count];
	}; // struct pi_result

protected:
	typedef rtp::vector< action_value_type* >			regression_trees_type;

protected:
	action_base< action_type >const						&m_action_base;

	//rtp::vector< regression_tree_type* >				m_regression_trees;
	regression_trees_type								m_regression_trees;

}; // class value_function


template< class action >
class walking_value_function:
	public value_function< regression_tree< node< typename action::tree_space_param_type > >, action >
{
	typedef value_function< regression_tree< node< typename action::tree_space_param_type > >, action > super;
	

public:
	walking_value_function		( const action_base< action >& ab ): super( ab ){};


}; // class walking_value_function

} // namespace rtp

} // namespace xray

#include "value_function_inline.h"

#endif // #ifndef VALUE_FUNCTION_H_INCLUDED