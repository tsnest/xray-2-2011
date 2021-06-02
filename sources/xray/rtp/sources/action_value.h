////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_VALUE_H_INCLUDED
#define ACTION_VALUE_H_INCLUDED


#include "training_regression_tree.h"

namespace xray {

namespace configs{
	class lua_config_value;
} // namespace configs

namespace render {
namespace debug{

	class renderer;

} // namespace debug{
} // namespace render 


namespace rtp {

template< class space_param >
class node_save_load_data;

template < class space_param, class training_sample_type >
class training_set;

template< class space_param, class training_sample_type  >
class training_action_value_save_load_data;

template< class space_param, class training_sample_type  >
class action_value_save_load_data
{
public:
	virtual training_action_value_save_load_data< space_param, training_sample_type >* get_training_save_load_data() { return 0; }

public:
	virtual const training_action_value_save_load_data< space_param, training_sample_type >* get_training_save_load_data()const { return 0; }
	virtual	~action_value_save_load_data	() { }
}; // class action_value_save_load_data


template< class space_param, class training_sample_type  >
class training_action_value_save_load_data:
	public action_value_save_load_data< space_param, training_sample_type >,
	private boost::noncopyable
{

public:
	training_action_value_save_load_data( const training_set<space_param, training_sample_type> &set ):  m_training_set_const( &set ){}
	training_action_value_save_load_data( training_set<space_param, training_sample_type> &set ):  m_training_set( &set ),  m_training_set_const( &set ){}

public:
	virtual training_action_value_save_load_data<space_param, training_sample_type>* get_training_save_load_data() { return this; }

public:
	virtual const training_action_value_save_load_data<space_param, training_sample_type>* get_training_save_load_data()const { return this; }
	virtual	~training_action_value_save_load_data	() { }

	inline const	training_set<space_param, training_sample_type>		*set	()const	{ return m_training_set_const; }
	inline			training_set<space_param, training_sample_type>		*set	()		{ return m_training_set; }

private:
	const training_set<space_param, training_sample_type>	*m_training_set_const;
	training_set<space_param, training_sample_type>			*m_training_set;

}; // class node_save_load_data


template< class regression_tree, class space_param >
class action_value_base
{

private:
	typedef typename	regression_tree::node_type			node_type; 

protected:
	typedef				space_param							space_param_type;

public:
	typedef typename	space_param_type::tree_space_param_type	tree_space_param_type;
	//typedef	std::pair< tree_space_param_type, float >			training_sample_type;
	typedef training_sample< tree_space_param_type >						training_sample_type;

public:
	typedef	training_set< space_param_type, training_sample_type >		training_set_type;

protected:
	static const u32 separate_dimensions_count	= space_param_type::separate_dimensions_count;
	static const u32 discrete_dimension_size	= space_param_type::discrete_dimension_size;
	typedef	typename	space_param_type::separate_reward_type	separate_reward_type;

public:
		action_value_base	( );
		action_value_base	( training_set_type &set );
virtual	~action_value_base	( );

private:
	float				reward			( u32 separate_dimension, u32 discrete_dimension, const tree_space_param_type &params  ) const;

public:
	separate_reward_type				reward			(  const space_param_type &params  ) const;

public:
	bool				compare			( const action_value_base< regression_tree, space_param  > &value ) const;

public:
	void				render			( xray::render::debug::renderer& renderer, const iaction< space_param_type > &a ) const;



regression_tree* const	&tree			( u32 separate_dim, u32 discrete_dim )const;
regression_tree*		&tree			( u32 separate_dim, u32 discrete_dim );


public:
#ifndef	MASTER_GOLD
virtual	void	save		( xray::configs::lua_config_value cfg,  const action_value_save_load_data< space_param_type, training_sample_type >& data )const;
#endif	// MASTER_GOLD

virtual	void	load		( const xray::configs::binary_config_value &cfg, action_value_save_load_data< space_param_type, training_sample_type >& data );

private:
	regression_tree*		trees[ separate_dimensions_count ][ discrete_dimension_size ];

}; // class action_value_base


template< class node, class space_param > 
class action_value;

template< class  space_param > 
class action_value < training_node< typename space_param::tree_space_param_type >, space_param >: public action_value_base< training_regression_tree< training_node< typename space_param::tree_space_param_type > >, space_param >
{

typedef action_value_base< training_regression_tree< training_node< typename space_param::tree_space_param_type > >, space_param  >	super;

typedef training_regression_tree< training_node< typename space_param::tree_space_param_type > > regression_tree_type;

using super::discrete_dimension_size;
using typename	super::space_param_type;
using typename	super::tree_space_param_type;


using  super::separate_dimensions_count;
using typename 	super::separate_reward_type;

public:
using typename super::training_sample_type;

public:
using super::tree;

public:
		action_value	( ): super(){};
		action_value	( typename super::training_set_type &set ): super( set ){};

public:
#ifndef	MASTER_GOLD
virtual	void	save		( xray::configs::lua_config_value cfg,  const action_value_save_load_data< space_param_type, training_sample_type >& data )const;
#endif	// MASTER_GOLD

virtual	void	load		( const xray::configs::binary_config_value &cfg, action_value_save_load_data< space_param_type, training_sample_type >& data );


public:
		void	build			();
		void	recalculate		();

}; // class action_value


template< class  space_param > 
class action_value < node< typename space_param::tree_space_param_type >, space_param >: public action_value_base< regression_tree< node< typename space_param::tree_space_param_type > >, space_param >
{

typedef action_value_base< regression_tree< node< typename space_param::tree_space_param_type > >, space_param > super;

//using super::training_set_type;

using super::discrete_dimension_size;
using typename	super::space_param_type;
using typename	super::tree_space_param_type;


using  super::separate_dimensions_count;
using typename 	super::separate_reward_type;


public:
		action_value	( ): super(){};
		action_value	( typename super::training_set_type &set ): super( set ){};

}; // class action_value;


} // namespace rtp
} // namespace xray

#include "action_value_inline.h"

#endif // #ifndef ACTION_VALUE_H_INCLUDED