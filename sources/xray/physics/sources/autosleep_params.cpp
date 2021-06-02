////////////////////////////////////////////////////////////////////////////
//	Created		: 01.06.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "autosleep_params.h"

autosleep_params_world	world_autosleep_params	=	
	{
	//object
		{
			{0.001f	,	0.0001f	}		,	//translational		vel	,	accel
			{0.001f	,	0.0001f	}		,	//rotational		vel	,	accel
			64								//level2 frames 2^
		}								,								
		1.5f								//reanable factor
	};



void autosleep_params::mul(float v)
{
	velocity			*=	v			;
	acceleration		*=	v			;
}

void autosleep_params_full::reset()
{
	*this=world_autosleep_params.objects_params;
}

//void autosleep_params_full::load(CInifile* ini)
//{
//	reset()										;
//	if(!ini)							return	;
//	if(!ini->section_exist("disable"))  return	;
//	if(ini->line_exist("disable","linear_factor"))	translational	.Mul(ini->r_float("disable","linear_factor"))		;
//	if(ini->line_exist("disable","angular_factor"))	rotational		.Mul(ini->r_float("disable","angular_factor"))		;
//	if(ini->line_exist("disable","change_count"))
//	{
//		int ch_cnt=ini->r_s8("disable","change_count");
//		if(ch_cnt<0)L2frames=L2frames >> u16(-ch_cnt);
//		else L2frames=L2frames	<< u16(ch_cnt);
//		VERIFY(ch_cnt<4 && L2frames!=0 );
//	}
//}
