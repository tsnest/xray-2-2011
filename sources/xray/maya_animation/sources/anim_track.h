////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ANIM_TRACK_H_INCLUDED
#define ANIM_TRACK_H_INCLUDED


#include <xray/animation/anim_track_common.h>

namespace xray {
namespace maya_animation {

struct EtReadKey 
{
	float			time;
	float			value;
	MFnAnimCurve::TangentType	inTangentType;
	MFnAnimCurve::TangentType	outTangentType;
	float			inAngle;
	float			inWeightX;
	float			inWeightY;
	float			outAngle;
	float			outWeightX;
	float			outWeightY;
	struct EtReadKey *	next;
}; // struct EtReadKey 

struct EtKey 
{
	float	time;			/* key time (in seconds)						*/
	float	value;			/* key value (in internal units)				*/
	float	inTanX;			/* key in-tangent x value						*/
	float	inTanY;			/* key in-tangent y value						*/
	float	outTanX;		/* key out-tangent x value						*/
	float	outTanY;		/* key out-tangent y value						*/
}; // struct EtKey 

enum EtInfinityType 
{
	kInfinityConstant,
	kInfinityLinear,
	kInfinityCycle,
	kInfinityCycleRelative,
	kInfinityOscillate
};

struct  EtCurve 
{
	int		numKeys;	/* The number of keys in the anim curve			*/
	bool	isWeighted;	/* whether or not this curve has weighted tangents */
	bool	isStatic;	/* whether or not all the keys have the same value */
	EtInfinityType preInfinity;		/* how to evaluate pre-infinity			*/
	EtInfinityType postInfinity;	/* how to evaluate post-infinity		*/

	/* evaluate cache */
	EtKey *		lastKey;	/* lastKey evaluated							*/
	int			lastIndex;	/* last index evaluated							*/
	int			lastInterval;	/* last interval evaluated					*/
	bool		isStep;		/* whether or not this interval is a step interval */
	bool		isStepNext;		/* whether or not this interval is a step interval */
	bool		isLinear;	/* whether or not this interval is linear		*/
	float		fX1;		/* start x of the segment						*/
	float		fX4;		/* end x of the segment							*/
	float		fCoeff[4];	/* bezier x parameters (only used for weighted curves */
	float		fPolyY[4];	/* bezier y parameters							*/

	EtKey*		keyListPtr;	/* This must be the last element and contains	*/
							/* an array of keys sorted in ascending order	*/
							/* by time										*/
	float		evaluate	(float time);
	void		create_keys	(int count);
				~EtCurve	();
				EtCurve		();
};



struct    anim_track
{
							anim_track					( );
							~anim_track					( );
							EtCurve*		get_channel	( animation::enum_channel_id id, bool b_create );
	EtCurve*				get_channel					( const char* name, bool b_create );
animation::enum_channel_id	get_channel_id				( const char* name );
	void					save						( FILE* f );
	void					save						( const char* fn );
	void					load						( FILE* f );
	void					load						( const char* fn );

	EtCurve*		m_channels[animation::channel_max];

	float			evaluate					( animation::enum_channel_id id, float time );
	void			evaluate					( xray::float4x4	&pose, float time );
	float			min_time					();
	float			max_time					();


//profile
	void			reset_profile				();
	u32				m_num_evaluate;
	float			m_evaluate_time;
	
}; // struct    anim_track

  void				engineHermiteCreate ( EtCurve *animCurve, float x[4], float y[4] );
  float				engineAnimEvaluate	( EtCurve *animCurve, float time );

} // namespace animation
} // namespace xray

#endif // #ifndef ANIM_TRACK_H_INCLUDED