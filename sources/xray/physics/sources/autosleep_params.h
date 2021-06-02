////////////////////////////////////////////////////////////////////////////
//	Created		: 01.06.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef AUTOSLEEP_PARAMS_H_INCLUDED
#define AUTOSLEEP_PARAMS_H_INCLUDED

struct	autosleep_params
{
	void				mul					( float v )	;
	float				velocity						;
	float				acceleration					;
};

struct	autosleep_params_full
{
	void				reset						( )	;
	//void				load			(CInifile* ini)	;
	autosleep_params	translational					;
	autosleep_params	rotational						;
	u16					l2_frames						;
};

struct autosleep_params_world
{
	autosleep_params_full	objects_params					;
	float					reanable_factor					;
};

class step_counter_ref;
struct autosleep_update_params:
	boost::noncopyable
{
	//const	bool				sleep_state;
	const	float3				&linear_velocity;
	const	float3				&angular_velocity;
	const	float3				&angular_rotation;
	const	float4x4			&transform;
	const	step_counter_ref	&step_counter;
	autosleep_update_params(	//		bool				sleep_state_,
								const	float3				&linear_velocity_,
								const	float3				&angular_velocity_,
								const	float3				&angular_rotation_,
						   		const	float4x4			&transform_,
								const	step_counter_ref	&step_counter_
							):	//sleep_state		(sleep_state_),
								linear_velocity	( linear_velocity_ ),
								angular_velocity( angular_velocity_ ),
								angular_rotation( angular_rotation_ ),
								transform		(transform_),
								step_counter	(step_counter_){}


};
extern autosleep_params_world	world_autosleep_params;

#endif // #ifndef AUTOSLEEP_PARAMS_H_INCLUDED