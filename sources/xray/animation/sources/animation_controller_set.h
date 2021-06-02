////////////////////////////////////////////////////////////////////////////
//	Created		: 22.03.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_CONTROLLER_SET_H_INCLUDED
#define ANIMATION_CONTROLLER_SET_H_INCLUDED

#include "animation_group_action.h"
#include "animation_layer.h"

#include <xray/animation/i_animation_controller_set.h>

namespace xray {
namespace animation {



class skeleton;
class animation_mix;
class fixed_bones;


class animation_controller_set:
	public i_animation_controller_set
{

public:
											animation_controller_set		();
											~animation_controller_set		();
public:
	virtual i_animation_action	const		*taget_action					()const;
	virtual u32								num_actions						( )const;
	virtual i_animation_action	const		*action							( u32 id )const;
	virtual	void							get_taget_transform				( float4x4 &m )const;
	virtual void							set_callback_on_loaded			( const boost::function< void ( ) >	 &cb  ){ m_callback_on_loaded = cb; };
	virtual	float4x4			const		&position						()const	{ return m_sum_displacement; }
	virtual float							crrent_action_start_time		()const { return m_action_start_time; }
	virtual	bool							loaded							()const { return b_loaded; }
	virtual	void							init							( u32 id, const buffer_vector< float > &weights, float time_global );
	virtual	void							reset							( ){ m_id_current = u32(-1); }
	virtual	void							start_transition				( const animation_group_action& next_action, const buffer_vector< float > &weights, const buffer_vector< float > &new_weights, float time_global );
	virtual	void							update							( float time_global, const buffer_vector< float > &weights );
	virtual	void							get_bone_world_matrix			( float4x4 &m, u32 bone_id ) const;

public:
	inline	u32								count						( ){ return m_actions.size(); }
	inline	animation_group_action	const&	action						( u32 id ){ return m_actions[ id ]; }
	inline	skeleton				const*	get_skeleton				( )	{ return static_cast_checked<const skeleton*>( m_skeleton.c_ptr() ); }

			void							dbg_test_run				( const animation_group_action& action, const buffer_vector< float > &weights, float time );
			float							time_left					( const animation_group_action& action, float time_global );

			void							test_update					( float time_global );
	virtual	void							render						(  render::debug::renderer& renderer, float time_global ) const;
			u32								moving_moving_bone			( )const{ return m_moving_root_bone; }
	inline	fixed_bones				const&	get_fixed_bones				( )const { return *m_fixed_bones; }
			u32								id_current					( )const{ return m_id_current; }
			u32								&id_step					( ){ return m_id_step; }
private:
			void							on_config_loaded			( xray::resources::queries_result& resource );
			void							query_resources				(  configs::lua_config_value const& config );
			void							on_resources_loaded			( xray::resources::queries_result& resource );
			void							add_group					( animation_group *group );
			void							clear_groups				( );

private:
	inline	animation_group_action const&	current						( )const;

private:

	inline  bool						is_active						( )const			{ return ( m_id_current != u32(-1) ); }
			
			void						apply_transition				( float time_global );
			void						set_mix							( );

	inline	float						action_time						( float time )const;
			void						start_action_transition			( const animation_group_action& next_action, float time_global, const buffer_vector< float > &weights, const buffer_vector< float > &new_weights );
			void						start_action					( const animation_group_action& next_action, float time_global, float time_action, const buffer_vector< float > &weights  );
			void						foot_steps_update				( float time_global, const buffer_vector< float > &weights  );

			float4x4					object_position					( )const;
			void						calculate_bones					( )const;
			void						calculate_anim_fixed_vertex		( float3 &pos, float time_global )const;
			float3						fixed_bone_shift				( float time_global )const;
			void						fixed_bone_start				( u32 bone, float time_global, const buffer_vector< float > &weights );
			bool						check_loaded					();
			bool						ready							()const;							

private:
			void						callback_group_loaded			();

public:
	static const	u32					max_group_size = 5;

private:
	animation_mix						*m_mix;
	vector< animation_group_action >	m_actions;
	vector< animation_group* >			m_groups;

	u32									m_id_current;
	u32									m_id_step;

	float								m_action_start_time;
	float								m_action_start_local_time;

private:
	float								m_update_time;

private:
	animation_vector_type				m_animation_in;
	animation_vector_type				m_animation_out;

	u32									m_fixed_bone;
	float3								m_fixed_vetex_global;


	fixed_bones							*m_fixed_bones;

	float3								m_fixed_global_shift;

///////////////////////////////////////////////////////////////////////
	float3								m_dbg_shift_fixed_action;
///////////////////////////////////////////////////////////////////////////

	u32									m_moving_root_bone;
	float4x4							m_sum_displacement;

	resources::unmanaged_resource_ptr	m_skeleton;
	bool								b_loaded;
	boost::function< void ( ) >			m_callback_on_loaded;

}; // class animation_controller_set




} // namespace animation
} // namespace xray

#endif // #ifndef ANIMATION_CONTROLLER_SET_H_INCLUDED