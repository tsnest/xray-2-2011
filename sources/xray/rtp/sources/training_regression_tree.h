////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRAINING_REGRESSION_TREE_H_INCLUDED
#define TRAINING_REGRESSION_TREE_H_INCLUDED

#include "regression_tree.h"
#include "space_params.h"
#include "training_set.h"
#include <xray/math_randoms_generator.h>

namespace xray {

namespace configs{
	class lua_config_value;
} // namespace configs

namespace rtp {

template< class space_param_type >
class training_node:
	public node< space_param_type >
{
		typedef	node< space_param_type > super;
		using super::m_left;
		using super::m_right;
		using super::m_split_value;
		using super::m_split_dimension;
		using super::m_reward_value;
		using super::is_leaf;

public:
		//typedef	std::pair< space_param_type, float >				training_sample_type;
		typedef training_sample< space_param_type >						training_sample_type;


public:
		training_node	(	u32 level, const training_sample_type** samples_begin, const training_sample_type** samples_end, 
							const space_param_type &min, const space_param_type &max, u32 min_samples );

		training_node	( u32 level, u32 min_samples );
		training_node	( u32 level );

public:
virtual	~training_node	( );

public:
virtual	bool	build		( );
virtual	bool	recalculate ( );

public:
		void	init_build	( );
		void	set_samples	( const training_sample_type **begin, const training_sample_type **end );
//		void	render		( xray::render::debug::renderer& renderer, const iaction_type &a ) const;

private:
virtual	super*	create			( u32 level );
virtual	u32		level			( ) { return m_level; };
		bool	divide			( );
		void	compute_reward	( );

private:
		void	dbg_check_validity	( ) const;

public:
#ifndef MASTER_GOLD
virtual		void	save		( xray::configs::lua_config_value cfg, const node_save_load_data< space_param_type >& data )const;
#endif // MASTER_GOLD

virtual		void	load		( const xray::configs::binary_config_value &cfg, node_save_load_data< space_param_type >& data );

public:
	static const u32							min_samples_max			= 50;
	static const u32							max_tree_depth			= 3000;

private:
	bool	samples_empty		( ) const {return m_samples_begin == m_samples_end; } 
	u32		samples_size		( ) const {return u32( m_samples_end - m_samples_begin ); } 

private:
	space_param_type								m_min, m_max;

	const training_sample_type						**m_samples_begin,  **m_samples_end;

	u32												m_level;
	u32												m_min_samples;

};


template< class training_node >
class training_regression_tree :
	public regression_tree< training_node >
{
	
	typedef	regression_tree< training_node >			super;

	typedef	typename training_node::space_param_type	space_param_type;

	using super::m_root;

public:
	typedef typename super::node_type::training_sample_type		training_sample_type;


public:
		training_regression_tree	(  ){};
		training_regression_tree	(  training_sub_set< training_sample_type >& set );
		training_regression_tree	(  u32 min_samples, buffer_vector< const training_sample_type* >	&samples );

virtual	~training_regression_tree	( ){}

public:

		void							recalculate	( )								 { m_root.recalculate()		; }	
		void							build		( );

public:
#ifndef MASTER_GOLD
virtual	void							save		( xray::configs::lua_config_value cfg, const node_save_load_data< space_param_type >& data  )const;
#endif // MASTER_GOLD

virtual	void							load		( const xray::configs::binary_config_value &cfg, node_save_load_data< space_param_type >& data );

public:
//		void							render		( xray::render::debug::renderer& renderer, space_param_type const &to ) const;

public:
	const training_sample_type*		const &p_sample	( u32 i )const;
	const training_sample_type*		&p_sample		( u32 i );
	const	u32						num_samples		( )const{ return m_num_samples; }

private:

	const training_sample_type**				m_training_samples_buff;
	u32											m_num_samples;

}; // class regression_tree

template< class tree_space_param >
class training_save_load_data :
	public node_save_load_data< tree_space_param >,
	private boost::noncopyable
{

	typedef tree_space_param	tree_space_param_type;
	//typedef	std::pair< tree_space_param_type, float >			training_sample_type;
	typedef training_sample< tree_space_param >					training_sample_type;
	typedef	training_node< tree_space_param_type >				training_node_type;
	typedef	training_regression_tree< training_node_type >		training_regression_tree_type;
public:
					training_save_load_data( const rtp::vector< training_sample_type > &samples, training_regression_tree_type &tree ): m_samples( &samples ), m_tree( &tree ), 
																																				 m_training_sub_set( 0 ){}
				
					training_save_load_data( const training_sub_set<training_sample_type> &set ): m_samples( 0 ), m_tree( 0 ),  m_training_sub_set_const( &set ){}
					training_save_load_data( training_sub_set<training_sample_type> &set ): m_samples( 0 ), m_tree( 0 ),  m_training_sub_set( &set ),  m_training_sub_set_const( &set ){}

public:
	virtual const	training_save_load_data< tree_space_param >*	get_training_save_load_data()const		{ return this; }
	virtual			training_save_load_data< tree_space_param >*	get_training_save_load_data()			{ return this; }

public:
	inline const	rtp::vector< training_sample_type >					*samples()const	{ return m_samples; }
	inline const	training_regression_tree_type						*tree	()const { return m_tree; }
	inline 			training_regression_tree_type						*tree	()		{ return m_tree; }

	inline const	training_sub_set<training_sample_type>		*sub_set	()const	{ return m_training_sub_set_const; }
	inline			training_sub_set<training_sample_type>		*sub_set	()		{ return m_training_sub_set; }

private:
	const rtp::vector< training_sample_type >	*m_samples;

	training_regression_tree_type				*m_tree;


	const training_sub_set<training_sample_type>	*m_training_sub_set_const;
		 training_sub_set<training_sample_type>		*m_training_sub_set;

}; // class training_save_load_data

} // namespace rtp
} // namespace xray

#include "training_regression_tree_inline.h"

#endif // #ifndef TRAINING_REGRESSION_TREE_H_INCLUDED