////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "light.h"

namespace xray {
namespace render {


float deg2rad(float deg) {return deg/180 * math::pi;}

light::light(void)
{
	flags.type		= light_type_point;
	flags.bStatic	= false;
	flags.bActive	= false;
	flags.bShadow	= false;
	flags.bVolumetric = false;
	flags.bHudMode	= false;
	position.set	(0,-1000,0);
	direction.set	(0,-1,0);
	right.set		(0,0,0);
	range			= 8.f;
	cone			= deg2rad(60.f);
	color.set		(1,1,1,1);

	m_volumetric_quality	= 1;
	//m_volumetric_quality	= 0.5;
	m_volumetric_intensity	= 1;
	m_volumetric_distance	= 1;

	frame_render	= 0;

	ZeroMemory		(omnipart,sizeof(omnipart));
	s_spot			= NULL;
	s_point			= NULL;
	vis.frame2test	= 0;	// xffffffff;
	vis.query_id	= 0;
	vis.query_order	= 0;
	vis.visible		= true;
	vis.pending		= false;
}

light::~light	()
{
	//for (int f=0; f<6; f++)	DELETE(omnipart[f]);
	set_active		(false);

	// remove from Lights_LastFrame
	//for (u32 it=0; it<RImplementation.Lights_LastFrame.size(); it++)
	//	if (this==RImplementation.Lights_LastFrame[it])	RImplementation.Lights_LastFrame[it]=0;
}

void light::set_texture		(LPCSTR name)
{
	XRAY_UNREFERENCED_PARAMETERS	( name );

//	if ((0==name) || (0==name[0]))
//	{
//		// default shaders
//		s_spot = 0;
//		s_point = 0;
//		s_volumetric = 0;
//		return;
//	}
//
//	string256				temp;
//	
//	strconcat(sizeof(temp),temp,"r2\\accum_spot_",name);
//	//strconcat(sizeof(temp),temp,"_nomsaa",name);
//	//s_spot.create			(RImplementation.Target->b_accum_spot,temp,name);
//
////#if	RENDER!=R_R3
//	s_volumetric.create		("accum_volumetric", name);
//#else	//	RENDER!=R_R3
//	s_volumetric.create		("accum_volumetric_nomsaa", name);
//	if( RImplementation.o.dx10_msaa )
//	{
//		int bound = 1;
//
//		if( !RImplementation.o.dx10_msaa_opt )
//			bound = RImplementation.o.dx10_msaa_samples;
//
//		for( int i = 0; i < bound; ++i )
//		{
//			s_spot_msaa[i].create				(RImplementation.Target->b_accum_spot_msaa[i],strconcat(sizeof(temp),temp,"r2\\accum_spot_",name),name);
//			s_volumetric_msaa[i].create	(RImplementation.Target->b_accum_volumetric_msaa[i],strconcat(sizeof(temp),temp,"r2\\accum_volumetric_",name),name);
//		}
//	}
//#endif	//	RENDER!=R_R3
}


void light::set_active		(bool a)
{
	if (a)
	{
		if (flags.bActive)					return;
		flags.bActive						= true;
		//spatial_register					();
		//spatial_move						();
		//Msg								("!!! L-register: %X",u32(this));

#ifdef DEBUG
		float3	zero(0,-1000,0);
		if (position.similar(zero))			{
			LOG_INFO("- Uninitialized light position.");
		}
#endif // DEBUG
	}
	else
	{
		if (!flags.bActive)					return;
		flags.bActive						= false;
		//spatial_move						();
		//spatial_unregister					();
		//Msg								("!!! L-unregister: %X",u32(this));
	}
}

void	light::set_position		(const float3& P)
{
	float	eps					=	math::epsilon_7;	//_max	(range*0.001f,EPS_L);
	if (position.similar(P,eps))return	;
	position = (P);
	//spatial_move				();
}

void	light::set_range		(float R)			
{
	float	eps					=	std::max	(range*0.1f, math::epsilon_7);
	if (math::similar(range,R,eps))	return	;
	range						= R		;
	//spatial_move				();
}

void	light::set_cone			(float angle)		
{
	if (math::similar(cone,angle))	return	;
	ASSERT(cone < deg2rad(121.f));	// 120 is hard limit for lights
	cone						= angle;
	//spatial_move				();
}

void	light::set_orientation		(const float3& D, const float3& R)	
{ 
	XRAY_UNREFERENCED_PARAMETER	( R );

	float3	old_D		= direction;
	direction = D;
	direction.normalize	();
	//right = R;
	//right.normalize();
	//if (!fsimilar(1.f, old_D.dot_product(D)))	spatial_move	();
}

//void	light::spatial_move			()
//{
//	switch(flags.type)	{
//	case IRender_Light::REFLECTED	:	
//	case IRender_Light::POINT		:	
//		{
//			spatial.sphere.set		(position, range);
//		} 
//		break;
//	case IRender_Light::SPOT		:	
//		{
//			// minimal enclosing sphere around cone
//			VERIFY2						(cone < deg2rad(121.f), "Too large light-cone angle. Maybe you have passed it in 'degrees'?");
//			if (cone>=PI_DIV_2)			{
//				// obtused-angled
//				spatial.sphere.P.mad	(position,direction,range);
//				spatial.sphere.R		= range * tanf(cone/2.f);
//			} else {
//				// acute-angled
//				spatial.sphere.R		= range / (2.f * _sqr(_cos(cone/2.f)));
//				spatial.sphere.P.mad	(position,direction,spatial.sphere.R);
//			}
//		}
//		break;
//	case IRender_Light::OMNIPART	:
//		{
//			// is it optimal? seems to be...
//			//spatial.sphere.P.mad		(position,direction,range);
//			//spatial.sphere.R			= range;
//			// This is optimal.
//			const float fSphereR		= range*RSQRTDIV2;
//			spatial.sphere.P.mad		(position,direction,fSphereR);
//			spatial.sphere.R			= fSphereR;
//		}
//		break;
//	}
//
//	// update spatial DB
//	ISpatial::spatial_move			();
//
//#if (RENDER==R_R2) || (RENDER==R_R3)
//	if (flags.bActive) gi_generate	();
//	svis.invalidate					();
//#endif
//}

//vis_data&	light::get_homdata		()
//{
//	// commit vis-data
//	hom.sphere.set	(spatial.sphere.P,spatial.sphere.R);
//	hom.box.set		(spatial.sphere.P,spatial.sphere.P);
//	hom.box.grow	(spatial.sphere.R);
//	return			hom;
//};
//
//float3	light::spatial_sector_point	()	
//{ 
//	return position; 
//}
//
//////////////////////////////////////////////////////////////////////////
//#if (RENDER==R_R2) || (RENDER==R_R3)
//// Xforms
void	light::xform_calc			()
{
	if	(render_device::get_ref().get_frame() == m_xform_frame)	return;
	m_xform_frame = render_device::get_ref().get_frame();

	// build final rotation / translation
	float3					L_dir,L_up,L_right;

	// dir
	L_dir = direction;
	float l_dir_m = L_dir.magnitude();
	if (math::valid(l_dir_m) && l_dir_m>math::epsilon_3)	L_dir /= l_dir_m;
	else									L_dir.set(0,0,1);

	// R&N
	if (right.square_magnitude()>math::epsilon_5)
	{
		// use specified 'up' and 'right', just enshure ortho-normalization
		L_right = right; L_right.normalize();
		L_up = math::cross_product(L_dir,L_right); L_up.normalize();
		L_right = math::cross_product(L_up,L_dir);	L_right.normalize();
	} else {
		// auto find 'up' and 'right' vectors
		L_up.set(0,1,0); if (abs(math::dot_product(L_up, L_dir))>.99f)	L_up.set(0,0,1);
		L_right=math::cross_product(L_up,L_dir); L_right.normalize();
		L_up=math::cross_product(L_dir,L_right); L_up.normalize();
	}

	// matrix
	float4x4					mR;
	mR.i.x = L_right.x;	 mR.i.y = L_right.y;  mR.i.z = L_right.z;  mR.e03/*_14*/ = 0;
	mR.j.x = L_up.x;     mR.j.y = L_up.y;     mR.j.z = L_up.z;     mR.e13/*_24*/ = 0;
	mR.k.x = L_dir.x;    mR.k.y = L_dir.y;    mR.k.z = L_dir.z;    mR.e23/*_34*/ = 0;
	mR.c.x = position.x; mR.c.y = position.y; mR.c.z = position.z; mR.e33/*_44*/ = 1;

	// switch
	switch(flags.type)	{
	case light_type_reflected:
	case light_type_point:
		{
			// scale of identity sphere
			float		L_R			= range;
			float4x4 mScale = math::create_scale(float3(L_R,L_R,L_R));
			m_xform = math::mul4x3(mScale, mR);
		}
		break;
	case light_type_spot:
		{
			// scale to account range and angle
			float	s = 2.f*range*math::tan(cone/2.f);	
			float4x4 mScale = math::create_scale(float3(s,s,range));	// make range and radius
			m_xform = math::mul4x3(mScale, mR);
		}
		break;
	case light_type_omnipart:
		{
			float	L_R = 2*range;		// volume is half-radius
			float4x4 mScale = math::create_scale(float3(L_R,L_R,L_R));
			m_xform = math::mul4x3(mScale, mR);
		}
		break;
	default:
		m_xform.identity	();
		break;
	}
}
//
////								+X,				-X,				+Y,				-Y,			+Z,				-Z
//static	float3 cmNorm[6]	= {{0.f,1.f,0.f}, {0.f,1.f,0.f}, {0.f,0.f,-1.f},{0.f,0.f,1.f}, {0.f,1.f,0.f}, {0.f,1.f,0.f}};
//static	float3 cmDir[6]	= {{1.f,0.f,0.f}, {-1.f,0.f,0.f},{0.f,1.f,0.f}, {0.f,-1.f,0.f},{0.f,0.f,1.f}, {0.f,0.f,-1.f}};
//
//void	light::export		(light_Package& package)
//{
//	if (flags.bShadow)			{
//		switch (flags.type)	{
//			case IRender_Light::POINT:
//				{
//					// tough: create/update 6 shadowed lights
//					if (0==omnipart[0])	for (int f=0; f<6; f++)	omnipart[f] = xr_new<light> ();
//					for (int f=0; f<6; f++)	{
//						light*	L			= omnipart[f];
//						float3				R;
//						R.crossproduct		(cmNorm[f],cmDir[f]);
//						L->set_type			(IRender_Light::OMNIPART);
//						L->set_shadow		(true);
//						L->set_position		(position);
//						L->set_orientation		(cmDir[f],	R);
//						L->set_cone			(PI_DIV_2);
//						L->set_range		(range);
//						L->set_color		(color);
//						L->spatial.sector	= spatial.sector;	//. dangerous?
//						L->s_spot			= s_spot	;
//						L->s_point			= s_point	;
//						
//						// Holger - do we need to export msaa stuff as well ?
//#if	RENDER==R_R3
//						if( RImplementation.o.dx10_msaa )
//						{
//							int bound = 1;
//
//							if( !RImplementation.o.dx10_msaa_opt )
//								bound = RImplementation.o.dx10_msaa_samples;
//
//							for( int i = 0; i < bound; ++i )
//							{
//								L->s_point_msaa[i] = s_point_msaa[i];
//								L->s_spot_msaa[i] = s_spot_msaa[i];
//								//L->s_volumetric_msaa[i] = s_volumetric_msaa[i];
//							}
//						}
//#endif	//	RENDER!=R_R3
//
//						//	Igor: add volumetric support
//						L->set_volumetric(flags.bVolumetric);
//						L->set_volumetric_quality(m_volumetric_quality);
//						L->set_volumetric_intensity(m_volumetric_intensity);
//						L->set_volumetric_distance(m_volumetric_distance);
//
//						package.v_shadowed.push_back	(L);
//					}
//				}
//				break;
//			case IRender_Light::SPOT:
//				package.v_shadowed.push_back			(this);
//				break;
//		}
//	}	else	{
//		switch (flags.type)	{
//			case IRender_Light::POINT:		package.v_point.push_back	(this);	break;
//			case IRender_Light::SPOT:		package.v_spot.push_back	(this);	break;
//		}
//	}
//}

void	light::set_attenuation_params	(float a0, float a1, float a2, float fo)
{
	attenuation0 = a0;
	attenuation1 = a1;
	attenuation2 = a2;
	falloff      = fo;
}

//#endif

//extern float		r_ssaGLOD_start,	r_ssaGLOD_end;
//extern float		ps_r2_slight_fade;
//float	light::get_LOD					()
//{
//	if	(!flags.bShadow)	return 1;
//	float	distSQ			= Device.vCameraPosition.distance_to_sqr(spatial.sphere.P)+EPS;
//	float	ssa				= ps_r2_slight_fade * spatial.sphere.R/distSQ;
//	float	lod				= _sqrt(clampr((ssa - r_ssaGLOD_end)/(r_ssaGLOD_start-r_ssaGLOD_end),0.f,1.f));
//	return	lod	;
//}

} // namespace render 
} // namespace xray 

