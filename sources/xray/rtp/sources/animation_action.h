////////////////////////////////////////////////////////////////////////////
//	Created		: 31.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_ACTION_H_INCLUDED
#define ANIMATION_ACTION_H_INCLUDED

#include <xray/animation/i_animation_action.h>
#include <xray/animation/i_animation_controller_set.h>

#include "animation_action_params.h"
#include "action.h"

namespace xray {

namespace animation {
class i_animation_action;
class i_animation_controller_set;
} // namespace animation


namespace rtp {

struct cache_blend_id_task_range;

template< class space_params, class world_space_params, class action >
class animation_action:
	public abstract_action< space_params, world_space_params, action >
{
	
	typedef abstract_action< space_params, world_space_params, action >	super;

public:
		animation_action( action_base<action> &ab, animation::i_animation_action  const *animation );
		animation_action( action_base<action> &ab );
virtual	~animation_action();
public:

	virtual		void	render					( const world_space_params&,  xray::render::debug::renderer&  ) const{}
	virtual		void	render					( const space_params& ,  xray::render::debug::renderer& ) const{};

#ifndef MASTER_GOLD
	virtual		void	save					( xray::configs::lua_config_value & cfg )const;
	virtual		void	save_cached_data		( configs::lua_config_value cfg ) const;
	virtual		void	save_cached_data		( ) const;
#endif // MASTER_GOLD

	virtual		void	load					( const xray::configs::binary_config_value &cfg );
	virtual		void	load_cached_data		( const configs::binary_config_value &cfg );

	inline		void	load_config				( );

				void	cached_data_query		( const boost::function< void ( ) >	 &cb  );

	virtual		void	construct				( );

public:
						void	get_weights				( u32 blend, buffer_vector< float > &weights )const;

inline		animation::e_gait	gait					( )const { ASSERT( m_animation_action );	return m_animation_action->gait(); 	}
inline		u32					discrete_param_value	( )const ;

inline		u32					flags					( )const { ASSERT( m_animation_action );	return m_animation_action->flags(); }
inline		u32					animation_action_id		( )const { ASSERT( m_animation_action );	return m_animation_action->get_id(); } 

inline		bool				has_motion_data				( )const { return m_has_motion_data; }
inline		bool				motion_data_query_returned	( )const { return m_motion_data_query_returned; }

public:
	virtual	void				cache_transforms		( );

protected:
	virtual		void		on_cache_transform		( u32 id_transform, float4x4 const &transform );
	virtual		void		cache_blend_transform	( u32 id_transform, u32 blend, buffer_vector< float > const &weights, u32 from_action_id, buffer_vector< float > const &from_weights );


protected:
				float4x4	calculate_transform		( u32 blend, buffer_vector< float > const &weights, u32 from_action_id, buffer_vector< float > const &from_weights )const;
				float4x4	calculate_transform		( const action& af, u32 from_sample, u32 blend_sample  )const;

public:
				float	run						( world_space_params& wfrom, const space_params& lfrom, const action& afrom, u32 from_sample, u32 blend_sample, u32 step, float time )const	;
				void	run_simulate			( world_space_params& wfrom, const space_params& lfrom, const action& afrom, u32 from_sample, u32 blend_sample )const	;

				void	run						( space_params& , const action& , u32 , u32  )const{}

protected:
	virtual		void	on_walk_run				( world_space_params& wfrom, const space_params& lfrom, const float4x4 &disp, buffer_vector< float > &weights )const;

public:
				u32		weights_combinations_count		()const;

public:
				u32		anim_count				()const;
				float	duration				()const;

public:
	virtual		pcstr	name					()const;


				
protected:
				void	weight_mean				( buffer_vector< float > &weights, u32 blend[4], float factors[2] )const;
				void	calc_samples_per_weight	();

inline			u32		cached_transform_id			(  u32 action_id, u32 blend_id, u32 from_blend_id )const;
inline			u32		transforms_per_blend		( )const;
inline			u32		blend_transform_ids_start	( u32 blend_id )const;


private:
inline			void	cache_blend_transform_internal	( u32 blend_id, u32 from_action_id, u32 from_blend_id,   buffer_vector< float > const &weights );
inline			void	cache_blend_transform_range		( cache_blend_id_task_range const& range );
inline			void	cache_blend_transform_internal	( u32 blend_id );

inline			void	generate_all_weights_combinations		( );
inline			void	calculate_weights_combinations_count	( );
private:
				fs_new::virtual_path_string		cached_data_file_path( )const;

protected:
	animation::i_animation_action		const *m_animation_action;

	static 			u32					max_samples	;

					u32					m_samples_per_weight;
					bool				m_depend_on_previous_action;



private:
					void			on_cached_data_loaded	( resources::queries_result& data );

boost::function< void ( ) >			m_cached_data_loaded_callback;

bool								m_has_motion_data;
bool								m_motion_data_query_returned;

float								*m_weights;
u32									m_weights_count;
u32									m_weights_combinations_count;

}; // class animation_action







} // namespace rtp
} // namespace xray

#include "animation_action_inline.h"

#endif // #ifndef ANIMATION_ACTION_H_INCLUDED