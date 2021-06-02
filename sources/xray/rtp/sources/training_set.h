////////////////////////////////////////////////////////////////////////////
//	Created		: 27.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRAINING_SET_H_INCLUDED
#define TRAINING_SET_H_INCLUDED

namespace xray {

namespace configs{
class lua_config_value;
} // namespace configs

namespace rtp {

template< class ActionType >
struct space_param_training_data;

template< class tree_space_param >
struct training_sample
{
	typedef	tree_space_param							tree_space_param_type;
	typedef	typename tree_space_param::action_type		action_type;

	typedef space_param_training_data< action_type >	space_param_training_data_type;

	training_sample			( tree_space_param_type const	&param, float reward, space_param_training_data_type const&  training_data ):
							param( param ), reward( reward ), space_param_training_data( training_data ){}
	
							training_sample( ): 
							reward( memory::uninitialized_value<float>() ){}

	tree_space_param_type	param;
	float					reward;

	space_param_training_data_type		space_param_training_data;

};

template < class training_sample_type >
class training_sub_set
{
private:
	typedef	typename training_sample_type::tree_space_param_type  space_param_type;

public:
												training_sub_set	( );
	void										add					( training_sample_type const& sample );

	rtp::vector< training_sample_type >			&samples			( ){ return m_samples; }
	const rtp::vector< training_sample_type >	&samples			( )const{ return m_samples; }
	rtp::vector< const training_sample_type* >	&p_samples			( ){ return m_p_samples; }

	void										randomize			( );

	u32											&min_in_leaf_samples( ){ return m_min_in_leaf_samples ;}

public:
#ifndef MASTER_GOLD
	void										save				( xray::configs::lua_config_value cfg )const;
#endif //MASTER_GOLD

	void										load				( const xray::configs::binary_config_value &cfg );

public:
	void										synchronize			();

private:
	u32											m_min_in_leaf_samples;							
	rtp::vector< training_sample_type >			m_samples;
	rtp::vector< const training_sample_type* >	m_p_samples;
	rtp::vector< training_sample_type >			m_add_samples;

	static const u32							min_in_leaf_samples_default = 5;

}; // class training_set



template < class space_param, class training_sample_type >
class training_set
{

	typedef			 space_param								space_param_type;
	typedef	typename space_param_type::tree_space_param_type	tree_space_param_type;
	typedef	typename space_param_type::separate_reward_type		separate_reward_type;

	static const	u32	separate_dimensions_count = space_param_type::separate_dimensions_count;
	static const	u32	discrete_dimension_size	=	space_param_type::discrete_dimension_size;

	typedef	training_sub_set<training_sample_type>		training_sub_set_type;


public:
	static const u32 subset_count	= separate_dimensions_count * discrete_dimension_size;

public:
	//											training_set		( );
	void										add					(  const space_param_type& param, separate_reward_type const& reward );

	//rtp::vector< training_sample_type >			&samples			( u32 discrete_dim )		;
	//const rtp::vector< training_sample_type >	&samples			( u32 discrete_dim  )const	;

	
	u32											samples_number		()const;

	//inline u32									subsets_number		()const{ return discrete_dimension_size; }

private:
	training_sub_set_type						&subset				( u32 idx  );
	const training_sub_set_type					&subset				( u32 idx  )const;


public:
	training_sub_set_type						&subset				( u32 separate_dim, u32 discrete_dim );
	const training_sub_set_type					&subset				( u32 separate_dim, u32 discrete_dim )const;

	space_param_type							get_param			( u32 idx, u32 discrete_dim )const;
	u32											get_samples_count	( u32 discrete_dim )const;
	void										set_value			(u32 idx, u32 discrete_dim, separate_reward_type const& v);


	u32											index				( u32 separate_dim, u32 discrete_dim )const;

	void										randomize			( );

	
public:
#ifndef MASTER_GOLD
	void										save				( xray::configs::lua_config_value cfg )const;
#endif // MASTER_GOLD

	void										load				( const xray::configs::binary_config_value &cfg );

public:
	void										synchronize			();

private:
						
	training_sub_set<training_sample_type>		m_sub_sets[ subset_count ];
	


}; // class training_set





} // namespace rtp
} // namespace xray

#include "training_set_inline.h"

#endif // #ifndef TRAINING_SET_H_INCLUDED