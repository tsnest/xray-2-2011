////////////////////////////////////////////////////////////////////////////
//	Created		: 29.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef AUTO_SLEEP_H_INCLUDED
#define AUTO_SLEEP_H_INCLUDED


#include "autosleep_params.h"
#include "step_counter.h"


struct auto_sleep_vector
{
	float3						from																		;
	float3						to																			;
	float						get_difference_to_last		( const float3& new_vector )const				;
	float						update						( const float3& new_vector )					;
	float						update_add					( const float3& new_difference )				;
	void						reset						( const float3& new_vector )					;
	
	float						sum_length				( const float3& cur_vector )					;

	auto_sleep_vector										()												;
};


//struct auto_sleep_quaternion
//{
//	math::quaternion		from;
//	math::quaternion		to;
//	float						get_difference_to_last		( const math::quaternion& new_vector )const		;
//	float						update						( const math::quaternion& new_vector )			;
//	void						reset						( const math::quaternion& new_vector )			;
//	void						init						()												;
//	float						sum_length				( const math::quaternion& cur_vector )			;
//	auto_sleep_quaternion									()												;
//};

struct auto_sleep_update_state
{
	bool						m_disable																	;
	bool						m_enable																	;
	void						reset						()												;
	auto_sleep_update_state&	operator	&=				( auto_sleep_update_state& ltate )				;
								auto_sleep_update_state		()												;
};

struct base_auto_sleep_data
{
public:
				bool			update							( const autosleep_update_params &v )		;
protected:
								base_auto_sleep_data			()											;
	virtual						~base_auto_sleep_data			() {}

protected:
				u16							m_count															;
				u16							m_frames														;
				step_counter_ref			m_last_frame_updated											;
				auto_sleep_update_state		m_state_l1														;
				auto_sleep_update_state		m_state_l2														;
				bool						m_disabled														;
protected:

	inline			void			check_state					( const auto_sleep_update_state& state )
	{
		if(m_disabled)	m_disabled=!state.m_enable;
		else			m_disabled=state.m_disable;
	}

				void			reset						()												;
																			
	virtual		void			update_l1					( const autosleep_update_params &v )			=0;
	virtual		void			update_l2					( const autosleep_update_params &v )			=0;

};

class autosleep_base :
	public virtual base_auto_sleep_data
{
protected:

				void			update_values_l1			( const autosleep_update_params &v );
				void			update_values_l2			( const autosleep_update_params &v );
				bool			do_updating					();

	virtual		float			velocity_update				( const autosleep_update_params &v )=0;
	virtual		float			acceleration_update			( const autosleep_update_params &v )=0;
	virtual		float			velocity_last_difference	( const autosleep_update_params &v )const=0;
	virtual		float			acceleration_last_difference( const autosleep_update_params &v )const=0;
	virtual		float			velocity_sum				( const autosleep_update_params &v )=0;
	virtual		float			acceleration_sum			( const autosleep_update_params &v )=0;

				void			set_disable_params			( const autosleep_params& params );

					void			reset					( );
	inline			void			check_state				( auto_sleep_update_state& state, float vel, float	accel )
	{
		if	(vel	<	m_params.velocity	&&	accel	<	m_params.acceleration) 
																		state.m_disable=true	;
		if	(vel	>	m_params.velocity*world_autosleep_params.reanable_factor	||	
			accel	>	m_params.acceleration*world_autosleep_params.reanable_factor)
																		state.m_enable=true	;
	}
protected:


	autosleep_params			m_params																	;
};

class autosleep_rotational: 
	public autosleep_base
{
public:
								autosleep_rotational		()																;
				void			reset						( const float4x4 &transform, const float3 &a_vel )	;
				void			set_disable_params			( const autosleep_params_full& params );
protected:
	virtual		void			update_l1					( const autosleep_update_params &v );
	virtual		void			update_l2					( const autosleep_update_params &v );
	virtual		float			velocity_update				( const autosleep_update_params &v  );
	virtual		float			acceleration_update			( const autosleep_update_params &v  );
	virtual		float			velocity_last_difference	( const autosleep_update_params &v  )const;
	virtual		float			acceleration_last_difference( const autosleep_update_params &v  )const;
	virtual		float			velocity_sum				( const autosleep_update_params &v );
	virtual		float			acceleration_sum			( const autosleep_update_params &v );

private:
	auto_sleep_vector			m_mean_velocity;
	auto_sleep_vector			m_mean_acceleration;
};

class autosleep_translational:
	protected autosleep_base
{
public:
								autosleep_translational		()																;
				void			reset						( const float4x4 &transform, const float3 &l_vel )	;
				void			set_disable_params			( const autosleep_params_full& params )							;	
protected:
	virtual		void			update_l1					( const autosleep_update_params &v );
	virtual		void			update_l2					( const autosleep_update_params &v );
	virtual		float			velocity_update				( const autosleep_update_params &v );
	virtual		float			acceleration_update			( const autosleep_update_params &v );

	virtual		float			velocity_last_difference	( const autosleep_update_params &v )const;
	virtual		float			acceleration_last_difference( const autosleep_update_params &v )const;
	virtual		float			velocity_sum				( const autosleep_update_params &v );
	virtual		float			acceleration_sum			( const autosleep_update_params &v );

private:
	auto_sleep_vector			m_mean_velocity	;
	auto_sleep_vector			m_mean_acceleration;
};

class autosleep_full:
	public		autosleep_translational,
	public		autosleep_rotational
{
public:
				void			reset						( const float4x4 &transform, const float3 &l_vel, const float3 &a_vel )											;
				void			set_disable_params			( const autosleep_params_full& params )												;
protected:
	virtual		void			update_l1					( const autosleep_update_params &v  )												;
	virtual		void			update_l2					( const autosleep_update_params &v )												;
				
};

//class step_counter_ref;
//class auto_sleep
//{
//public:
//
//	bool update	( const step_counter_ref& sr, float4x4 &pose ) { return false; }
//	void reset	( float4x4 &pose ) { ; }
//	auto_sleep( ){};
//}; // class auto_sleep

#endif // #ifndef AUTO_SLEEP_H_INCLUDED