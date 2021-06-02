////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SPACE_PARAMS_H_INCLUDED
#define SPACE_PARAMS_H_INCLUDED

#include <xray/math_randoms_generator.h>
#include "rtp_world.h"

namespace xray {

namespace configs{
	class lua_config_value;
} // namespace configs

namespace rtp {

inline double random( math::random32& rnd )
{
	return ( double( rnd.random( u32(-1)) )/double (u32(-1)) );
}

inline float random( float min, float max, math::random32 &rnd )
{
	const float range = max - min;
	return min + float( random( rnd ) * range ) ;
}

inline bool in_range(float v, float min, float max )
{
	return v > min && v < max;
}

template< u32 dimension, class FinalType >
class tree_space_param;

template< u32 dimension,  class FinalType >
class tree_space_param
{
	typedef tree_space_param<dimension,FinalType> self_type;
public: 
	typedef FinalType final_type;

public:
							tree_space_param		( ){}
						
	void					set_random		( self_type const &min, self_type const &max, math::random32_with_count &r = rtp_world::random() );

	bool					equal			( const tree_space_param& l )const;


	inline bool in_range		( self_type const& min, self_type const& max  )const;
	inline bool in_area			( )const;
	inline void clamp_to_range	( self_type const& min, self_type const& max );
	inline void clamp_to_area	(  );

protected:
	bool					similar			( const tree_space_param& l )const;
	
public:

#ifndef MASTER_GOLD
	void					save			( xray::configs::lua_config_value cfg )const;
#endif //MASTER_GOLD

	void					load			( const xray::configs::binary_config_value &cfg );

public:
	inline float	operator[]		( u32 d )	const;
	inline float&	operator []		( u32 d )	;


public:
//	static	const final_type		min, max;
	static	const final_type		area_min, area_max;

public:
	static	const u32		dimensions_number			= dimension;
	

protected:
	float	params[dimensions_number];


};

template<u32 separate_dimensions>
struct	separate_reward;

template<u32 separate_dimensions>
struct	separate_reward
{
		inline	separate_reward();

		inline	separate_reward&		set			( float v );

		inline	separate_reward		operator +	( const separate_reward& right )const;
		inline	separate_reward		operator -	( const separate_reward& right )const;

		inline	separate_reward		operator *	( float v )const;
		inline	bool				operator <	( separate_reward<separate_dimensions> const &left ) const
		{
			if((*this )[0] == -math::infinity)
				return true;
			return sum_reward() < left.sum_reward();
		}
		inline	bool				operator >	( separate_reward<separate_dimensions> const &left ) const
		{
			if((*this )[0] == -math::infinity)
				return false;
			return sum_reward() > left.sum_reward();
		}
		inline	bool				operator ==	( separate_reward<separate_dimensions> const &left ) const
		{	
			ASSERT( (*this )[0] != -math::infinity );
			//if((*this )[0] == -math::infinity)
			//	return false;
			return sum_reward() == left.sum_reward();
		}
		inline	float&	operator []				( u32 i );
		inline	float	operator []				( u32 i )const;
		inline	float	sum_reward				( )const;
		inline	float	square_sum_reward		( )const;
		inline	void	set_negative_infinity	( ){ (*this )[0] = -math::infinity; }
		inline	bool	valid					( );

//		inline	float max_reward()const;

		float	value[separate_dimensions];
};


template< class ActionType >
struct space_param_training_data
{
	space_param_training_data(): blend_id( u32(-1) ), previos_action( 0 ), previous_blend_id( u32(-1) ){}
	
	u32				blend_id;

	ActionType const	*previos_action;
	u32					previous_blend_id;
};


template< u32 separate_dimensions, u32 discrete_size,  class TreeParamType, class FinalType >
class space_param{


	typedef	space_param<separate_dimensions, discrete_size, TreeParamType, FinalType> self_type;

	typedef	typename TreeParamType::action_type	action_type;

	typedef space_param_training_data< action_type >	training_data_type;

public:
	typedef TreeParamType	tree_space_param_type;
	static	const u32		separate_dimensions_count		= separate_dimensions;
	static	const u32		discrete_dimension_size			= discrete_size;
	typedef	separate_reward<separate_dimensions_count>		separate_reward_type;

public:

	space_param( ):	m_discrete_dimension_value( u32(-1) ){}

	void					set_random		( math::random32_with_count &r = rtp_world::random() );
	bool					equal			( const self_type& l )const;

	inline bool 	in_range		( )const;
	inline bool 	in_area			( )const;
	inline void 	clamp_to_range	( );
	inline void 	clamp_to_area	( );
	inline u32&		discrete_value	( )			{ return m_discrete_dimension_value; }
	inline u32		discrete_value	( ) const	{ return m_discrete_dimension_value; }

	inline u32&		blend_id		( ) 		{ return m_training_data.blend_id; }
	inline u32		blend_id		( ) const	{ return m_training_data.blend_id; }
	
	inline training_data_type&			training_data()		 { return m_training_data; }
	inline training_data_type const&	training_data()const { return m_training_data; }

static inline u32 trees_count	( ) 
	{
		return
			separate_dimensions_count	( ) *
			discrete_dimension_size;
	}

protected:
	bool					similar			( const self_type& l )const;
	
public:

#ifndef MASTER_GOLD
	void					save			( xray::configs::lua_config_value cfg )const;
#endif //MASTER_GOLD

	void					load			( const xray::configs::binary_config_value &cfg );

public:
	inline tree_space_param_type &separate_params( u32 id ){
		ASSERT( id < separate_dimensions );
		return m_separate_params[ id ];
	}
	inline tree_space_param_type const &separate_params( u32 id ) const {
		ASSERT( id < separate_dimensions );
		return m_separate_params[ id ];
	}
	
protected:
	static const FinalType		min ,max;	

	tree_space_param_type	m_separate_params[separate_dimensions];
	u32						m_discrete_dimension_value;
	
	training_data_type			m_training_data;
	//u32						m_blend_id; //it is not a part of state param it is an action param and used in sample update
										//move it to sample structure ?
};

inline float3 render_pos(const float2& pos )
{
	const float3 gpos	= float3( 0, 10, 0 );
	const float3 p = gpos + float3( 0, pos.x, pos.y );
	return p;
}

//template< class space_param_type > 
//void sort_by_dimension( vector< std::pair< space_param_type, u32 > > &samples, u32 dimension )
//{
//	
//	struct cmp
//	{
//		u32 m_dimension;
//		cmp( u32 dimension ): m_dimension( dimension ){}
//		bool operator () ( const std::pair< space_param_type, u32 >& l, const std::pair< space_param_type, u32 >& r )
//		{
//			return l->first[m_dimension] < r->first[m_dimension];
//		}
//	};
//	std::sort( samples.begin(), samples.end(), cmp(dimension) );
//}


} // namespace rtp
} // namespace xray

#include "space_params_inline.h"

#endif // #ifndef SPACE_PARAMS_H_INCLUDED