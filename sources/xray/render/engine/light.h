////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include <xray/render/base/light_props.h>
#include <xray/render/core/res_common.h>
#include <xray/render/core/res_effect.h>

namespace xray {
namespace render_dx10 {

class light: public res_base
{
public:
	light();
	virtual ~light(); // class light

	void _free() const;

	void	set_type( render::light_type type) {flags.type = type;}
	u32		get_type() {return flags.type;}

	virtual void	set_active(bool b);
	virtual bool	get_active() {return flags.bActive;}
	virtual void	set_shadow(bool b) {flags.bShadow=b;}
	virtual void	set_volumetric(bool b) {flags.bVolumetric=b;}

	virtual void	set_volumetric_quality(float fValue) {m_volumetric_quality = fValue;}
	virtual void	set_volumetric_intensity(float fValue) {m_volumetric_intensity = fValue;}
	virtual void	set_volumetric_distance(float fValue) {m_volumetric_distance = fValue;}
	
	virtual void	set_position	(const float3& P);
	virtual void	set_orientation	(const float3& D, const float3& R);
	virtual void	set_cone		(float angle);
	virtual void	set_range		(float R);
	virtual void	set_virtual_size(float R) {	XRAY_UNREFERENCED_PARAMETER	(R); };
	virtual void	set_color		(const math::color& C) {color = C;}
	virtual void	set_color		(float r, float g, float b) {set_color( math::color(r,g,b,1) );}
	virtual void	set_texture		(LPCSTR name);
	virtual void	set_hud_mode	(bool b) {flags.bHudMode=b;}
	virtual bool	get_hud_mode	() {return flags.bHudMode;};

	//virtual	void	spatial_move			();
	//virtual	float3	spatial_sector_point	();

	//virtual IRender_Light*	dcast_Light		()	{ return this; }

	//vis_data&		get_homdata				();
	void			gi_generate				();
	void			xform_calc				();
	void			vis_prepare				();
	void			vis_update				();
	//void			export 					(light_Package& dest);
	void			set_attenuation_params	(float a0, float a1, float a2, float fo);

	float			get_LOD					();

public:
	struct
	{
		u32			type	:	4;
		u32			bStatic	:	1;
		u32			bActive	:	1;
		u32			bShadow	:	1;
		u32			bVolumetric:1;
		u32			bHudMode:	1;

	}	flags;

	float3			position	;
	float3			direction	;
	float3			right		;
	float			range		;
	float			cone		;
	math::color		color		;

	//vis_data		hom			;
	u32				frame_render;

	float			m_volumetric_quality;
	float			m_volumetric_intensity;
	float			m_volumetric_distance;

	float			falloff;			// precalc to make light equal to zero at light range
	float	        attenuation0;		// Constant attenuation		
	float	        attenuation1;		// Linear attenuation		
	float	        attenuation2;		// Quadratic attenuation	

	light*						omnipart	[6]	;
	//xr_vector<light_indirect>	indirect		;
	u32							indirect_photons;

	//smapvis			svis;		// used for 6-cubemap faces

	ref_effect		s_spot;
	ref_effect		s_point;
	ref_effect		s_volumetric;

#if RENDER==R_R3
	ref_effect		s_spot_msaa[8];
	ref_effect		s_point_msaa[8];
	ref_effect		s_volumetric_msaa[8];
#endif	//	RENDER==R_R3

	u32				m_xform_frame;
	float4x4		m_xform;

	struct _vis
	{
		u32			frame2test;		// frame the test is sheduled to
		u32			query_id;		// ID of occlusion query
		u32			query_order;	// order of occlusion query
		bool		visible;		// visible/invisible
		bool		pending;		// test is still pending
		u16			smap_ID;
	}				vis;

	struct	_xform
	{
		struct		_D
		{
			float4x4						combine	;
			s32							minX,maxX	;
			s32							minY,maxY	;
			BOOL						transluent	;
		}	D;
		struct		_P
		{
			float4x4						world		;
			float4x4						view		;
			float4x4						project		;
			float4x4						combine		;
		}	P;
		struct		_S
		{
			float4x4						view		;
			float4x4						project		;
			float4x4						combine		;
			u32							size		;
			u32							posX		;
			u32							posY		;
			BOOL						transluent	;
		}	S;
	}	X;
};

typedef intrusive_ptr<light, res_base, threading::single_threading_policy> ref_light;

} // namespace render 
} // namespace xray 


#endif // #ifndef LIGHT_H_INCLUDED