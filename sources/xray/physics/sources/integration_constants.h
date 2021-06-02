////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.02.2008
//	Author		: Konstantin Slipchenko
//	Description : integration constants
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_INTEGRATION_CONSTANTS_H_INCLUDED
#define XRAY_PHYSICS_INTEGRATION_CONSTANTS_H_INCLUDED

#include "math_utils.h"

//const float 		default_w_limit									= 9.8174770f;//(M_PI/16.f/(fixed_step=0.02f));
//const float 		default_l_limit									= 150.f;//(3.f/fixed_step=0.02f);
//const float 		default_l_scale									= 1.01f;
//const float 		default_w_scale									= 1.01f;
//const float			default_k_l										= 0.0002f;//square resistance !!
//const float			default_k_w										= 0.05f;


const u16		max_joint_allowed_for_exeact_integration		= 30;

//base	params
const float		base_fixed_step									=	0.02f				;
const float		base_erp										=	0.54545456f			;
const float		base_cfm										=	1.1363636e-006f		;

//base params
const float 	default_integration_step						=	0.01f;

const float 	default_cfm										=	cfm( spring( base_cfm,base_erp, base_fixed_step ), damping( base_cfm, base_erp ), default_integration_step );
const float 	default_erp										=	erp( spring( base_cfm,base_erp, base_fixed_step ), damping( base_cfm, base_erp ), default_integration_step );

const float		default_spring									=	spring( default_cfm, default_erp, default_integration_step );
const float		default_damping									=	damping( default_cfm,default_erp );




//const float base_fixed_step											=	0.02f				;
//const float base_erp												=	0.54545456f			;
//const float base_cfm												=	1.1363636e-006f		;
////base params
//float 			fixed_step											=	0.01f;
//float 			world_cfm											=	CFM(SPRING_S(base_cfm,base_erp,base_fixed_step),DAMPING(base_cfm,base_erp));
//float 			world_erp											=	ERP(SPRING_S(base_cfm,base_erp,base_fixed_step),DAMPING(base_cfm,base_erp));
//float			world_spring										=	1.0f*SPRING	(world_cfm,world_erp);
//float			world_damping										=	1.0f*DAMPING(world_cfm,world_erp);





const float			default_gravity									=	2*9.81f;


/////////////////////////////////////////////////////

const u32			default_quick_step_interaton_number				=	18;


#endif