////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ANIM_TRACK_H_INCLUDED
#define ANIM_TRACK_H_INCLUDED

#include <xray/animation/api.h>
#include <xray/animation/anim_track_common.h>

namespace xray {
namespace animation {

struct XRAY_ANIMATION_API EtKey 
{
	float	time;			/* key time (in seconds)		*/
	float	value;			/* key value (in internal units)*/
	float	inTanX;			/* key in-tangent x value		*/
	float	inTanY;			/* key in-tangent y value		*/
	float	outTanX;		/* key out-tangent x value		*/
	float	outTanY;		/* key out-tangent y value		*/
	u8		type;			/* key type						*/
#ifndef MASTER_GOLD
	void	save			( xray::configs::lua_config_value t_root );
	void	load			( xray::configs::lua_config_value const& t_root );
#endif
	void	load			( xray::configs::binary_config_value const& t_root );
	bool operator <		(float t) const { return time<t;}
}; // struct EtKey 

enum EtInfinityType 
{
	kInfinityConstant,
	kInfinityLinear,
	kInfinityCycle,
	kInfinityCycleRelative,
	kInfinityOscillate
};

#pragma warning(push)
#pragma warning(disable:4251)

struct XRAY_ANIMATION_API EtCurve 
{
//	int		numKeys;				/* The number of keys in the anim curve			*/
	bool	isWeighted;				/* whether or not this curve has weighted tangents */
	bool	isStatic;				/* whether or not all the keys have the same value */
	EtInfinityType preInfinity;		/* how to evaluate pre-infinity			*/
	EtInfinityType postInfinity;	/* how to evaluate post-infinity		*/

	/* evaluate cache */
	EtKey *		lastKey;			/* lastKey evaluated						*/
	int			lastIndex;			/* last index evaluated						*/
	int			lastInterval;		/* last interval evaluated					*/
	bool		isStep;				/* whether or not this interval is a step interval */
	bool		isStepNext;			/* whether or not this interval is a step interval */
	bool		isLinear;			/* whether or not this interval is linear		*/
	float		fX1;				/* start x of the segment						*/
	float		fX4;				/* end x of the segment							*/
	float		fCoeff[4];			/* bezier x parameters (only used for weighted curves */
	float		fPolyY[4];			/* bezier y parameters							*/

	xray::vectora<EtKey>			keyList;
	//EtKey*		keyListPtr;		/* This must be the last element and contains	*/
	//								/* an array of keys sorted in ascending order	*/
	//								/* by time										*/
	float		evaluate			( float time );
				EtCurve				( xray::memory::base_allocator* a );
				~EtCurve			( );
	void		reset				( );
#ifndef MASTER_GOLD
	void		save				( xray::configs::lua_config_value t_root );
	void		load				( xray::configs::lua_config_value const& t_root );
#endif // #ifndef MASTER_GOLD
	void		load				( xray::configs::binary_config_value const& t_root );
	void		insert				( float time, float value );
	void		remove				( u32 idx );
	void		calc_smooth_tangents();
};

struct XRAY_ANIMATION_API anm_track
{
							anm_track				( xray::memory::base_allocator* a );
							anm_track				( xray::memory::base_allocator* a, int max_channels );
							~anm_track				( );
	EtCurve*				get_channel				( animation::enum_channel_id id, bool b_create );
	EtCurve*				get_channel				( const char* name, bool b_create );
animation::enum_channel_id	get_channel_id			( const char* name );

#ifndef MASTER_GOLD
	void					save						( xray::configs::lua_config_value t_root );
	void					load						( xray::configs::lua_config_value const& t_root );
#endif
	void					load						( xray::configs::binary_config_value const& t_root );

	EtCurve**		m_channels;

	float			evaluate					( animation::enum_channel_id id, float channel_position );
	void			evaluate					( float4x4& pose, float time, bool use_time, float& on_track_position );
	void			evaluate					( float4x4& pose, float time, bool use_time );
	void			evaluate					( math::float4x4& pose, float on_track_position );
	float			min_time					( );
	float			max_time					( );
	
	void			initialize_empty			( bool bposition, bool brotation, bool bscale );
	void			insert						( float on_track_position, float3 const& position, float3 const& rotation, float3 const& scale );
	void			remove						( u32 idx );
	void			calc_smooth_tangents		( );
//profile
	void			reset_profile				( );
	u32				m_num_evaluate;
	float			m_evaluate_time;
	static const int m_time_channel_id = 9;
private:
	xray::memory::base_allocator*	m_allocator;
	int								m_max_channels;
}; // struct    anim_track

#pragma warning(pop)

  void				engineHermiteCreate ( EtCurve *animCurve, float x[4], float y[4] );
  float				engineAnimEvaluate	( EtCurve *animCurve, float time );

} // namespace animation
} // namespace xray

#endif // #ifndef ANIM_TRACK_H_INCLUDED