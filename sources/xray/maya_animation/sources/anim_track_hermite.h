////////////////////////////////////////////////////////////////////////////
//	Created		: 10.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ANIM_TRACK_HERMITE_H_INCLUDED
#define ANIM_TRACK_HERMITE_H_INCLUDED

#include <xray/animation/anim_track_common.h>

namespace xray {
namespace maya_animation {

struct anim_track;
struct animation_curve_data_header;
struct EtKey;
struct EtCurve;

struct hermite_key_interval
{
	float		x1;			/* start x of the segment						*/
	float4		coeff;		/* bezier x parameters (only used for weighted curves */
};

inline float evaluate (const hermite_key_interval &ki, float time)
{
	float t = time - ki.x1;
	return t * (t * (t * ki.coeff.x + ki.coeff.y) + ki.coeff.z) + ki.coeff.w;
}


class anim_curve_hermite 
{
public:
			anim_curve_hermite		();
	void	build					( const EtKey *keys, u32 num_keys );	
	void	build					( const EtCurve *anim_curve );	
	void	save					(FILE* /*f*/)const{};
	void	load					(FILE* /*f*/){};
	float	evaluate				( float time )const;

private:
	vector < hermite_key_interval >	m_keys;
	float							m_time_end;
	mutable u32						m_last_interval;
	bool							b_static;

}; // class anim_track_hermite


class anim_track_hermite
{

public:
		void			build						( const anim_track &anim_curve );	

public:
		void			bone_pos					(xray::float4x4	&pose, float time) const;
static	void			setup_header				( animation_curve_data_header & ){};

private:
	inline	float		evaluate					( animation::enum_channel_id id, float time ) const;

private:
	anim_curve_hermite	m_channels[ animation::channel_max];
	
};


}//maya_animation
}//xray



#endif // #ifndef ANIM_TRACK_HERMITE_H_INCLUDED