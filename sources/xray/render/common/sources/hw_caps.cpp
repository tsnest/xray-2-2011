////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "hw_caps.h"

#include "hw_wrapper.h"

static u32 cap_version( u32 a, u32 b)	
{
	return a*10 + b;
}

namespace
{
/*
	u32 GetNVGpuNum()
	{
		NvLogicalGpuHandle  logicalGPUs[NVAPI_MAX_LOGICAL_GPUS];
		NvU32               logicalGPUCount;
		NvPhysicalGpuHandle physicalGPUs[NVAPI_MAX_PHYSICAL_GPUS];
		NvU32               physicalGPUCount;

		//	int result = NVAPI_OK;

		int iGpuNum = 0;

		NvAPI_Status	status;
		status = NvAPI_Initialize();

		if (status != NVAPI_OK)
		{
			Msg("* NVAPI is missing.");
			return iGpuNum;
		}

		// enumerate logical gpus
		status = NvAPI_EnumLogicalGPUs(logicalGPUs, &logicalGPUCount);
		if (status != NVAPI_OK)
		{
			Msg("* NvAPI_EnumLogicalGPUs failed!");
			return iGpuNum;
			// error
		}

		// enumerate physical gpus
		status = NvAPI_EnumPhysicalGPUs(physicalGPUs, &physicalGPUCount);
		if (status != NVAPI_OK)
		{
			Msg("* NvAPI_EnumPhysicalGPUs failed!");
			return iGpuNum;
			// error
		}

		Msg	("* NVidia MGPU: Logical(%d), Physical(%d)", physicalGPUCount, logicalGPUCount);

		//	Assume that we are running on logical GPU with most physical GPUs connected.
		for ( u32 i = 0; i<logicalGPUCount; ++i )
		{
			status = NvAPI_GetPhysicalGPUsFromLogicalGPU( logicalGPUs[i], physicalGPUs, &physicalGPUCount);
			if (status == NVAPI_OK)
				iGpuNum = _max( iGpuNum, physicalGPUCount);
		}

		if (iGpuNum>1)
		{
			Msg	("* NVidia MGPU: %d-Way SLI detected.", iGpuNum);
		}

		return iGpuNum;
	}

	u32 GetATIGpuNum()
	{
		int iGpuNum = AtiMultiGPUAdapters();
		//int iGpuNum = 1;

		if (iGpuNum>1)
		{
			Msg	("* ATI MGPU: %d-Way CrossFire detected.", iGpuNum);
		}

		return iGpuNum;
	}

	u32 get_gpu_num()
	{
		u32 res = GetNVGpuNum();

		res = _max( res, GetATIGpuNum() );

		res = _max( res, 2 );

		res = _min( res, hw_caps::MAX_GPUS );

		//	It's vital to have at least one GPU, else
		//	code will fail.
		VERIFY(res>0);

		Msg("* Starting rendering as %d-GPU.", res);

		return res;
	}
*/
	u32 get_gpu_num()
	{
		return 2;
	}
}

namespace xray {
namespace render {


#ifndef	USE_DX10
void hw_caps::update()
{
	//pdevice
	hw_wrapper	&hw = hw_wrapper::get_ref();
	ID3DDevice	*pdevice = hw.device();
	D3DCAPS9	caps;
	pdevice->GetDeviceCaps	(&caps);

	// ***************** GEOMETRY
	geometry_major				= u16 ( (u32(caps.VertexShaderVersion)&(0xf << 8ul))>>8 );
	geometry_minor				= u16 ( (u32(caps.VertexShaderVersion)&0xf) );
	geometry.software			= (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)==0;
	geometry.point_sprites		= FALSE;
	geometry.npatches			= (caps.DevCaps & D3DDEVCAPS_NPATCHES)!=0;
	DWORD cnt					= (caps.MaxVertexShaderConst);
	math::clamp<DWORD>(cnt,0,256);
	geometry.registers			= cnt;
	geometry.instructions		= 256;
	geometry.clip_planes		= math::min(caps.MaxUserClipPlanes,15);
	geometry.vtf				= (geometry_major>=3) && hw.support(D3DFMT_R32F,D3DRTYPE_TEXTURE,D3DUSAGE_QUERY_VERTEXTEXTURE);

	// ***************** PIXEL processing
	raster_major				= u16 ( u32(u32(caps.PixelShaderVersion)&u32(0xf << 8ul))>>8 );
	raster_minor				= u16 ( u32(u32(caps.PixelShaderVersion)&u32(0xf)) );
	raster.stages				= caps.MaxSimultaneousTextures;
	raster.b_non_pow2			= ((caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)!=0)  || ((caps.TextureCaps & D3DPTEXTURECAPS_POW2)==0);
	raster.b_cubemap			= (caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP)!=0;
	raster.mrt_count			= (caps.NumSimultaneousRTs);
	raster.b_mrt_mixdepth		= (caps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS)!=0;
	raster.instructions			= (caps.PS20Caps.NumInstructionSlots);

	// ***************** Info
	LOG_INFO("* GPU shading: vs(%x/%d.%d/%d), ps(%x/%d.%d/%d)",
		caps.VertexShaderVersion,	geometry_major, geometry_minor, cap_version(geometry_major,	geometry_minor),
		caps.PixelShaderVersion,	raster_major,	raster_minor,	cap_version(raster_major,	raster_minor)
		);

	// *******1********** Vertex cache
	ID3DQuery*	q_vc;
	D3DDEVINFO_VCACHE	vc;
	HRESULT _hr	= pdevice->CreateQuery(D3DQUERYTYPE_VCACHE,&q_vc);
	if (FAILED(_hr))
	{
		vc.OptMethod			= 0;
		vc.CacheSize			= 16;
		geometry.vertex_cache	= 16;
	} 
	else 
	{
		q_vc->Issue(D3DISSUE_END);
		q_vc->GetData(&vc,sizeof(vc),D3DGETDATA_FLUSH);
		safe_release(q_vc);
		if (1==vc.OptMethod	)
			geometry.vertex_cache	= vc.CacheSize;
		else
			geometry.vertex_cache	= 16;
	}
	LOG_INFO("* GPU vertex cache: %s, %d",(1==vc.OptMethod)?"recognized":"unrecognized",u32(geometry.vertex_cache));

	// *******1********** Compatibility : vertex shader
	if (0==raster_major)		geometry_major=0;		// Disable VS if no PS

	/*
	bTableFog			=	FALSE;	//BOOL	(caps.RasterCaps&D3DPRASTERCAPS_FOGTABLE);

	// Detect if stencil available
	bStencil			=	FALSE;
	IDirect3DSurface9*	surfZS=0;
	D3DSURFACE_DESC		surfDESC;
	CHK_DX		(pdevice->GetDepthStencilSurface(&surfZS));
	R_ASSERT	(surfZS);
	CHK_DX		(surfZS->GetDesc(&surfDESC));
	_RELEASE	(surfZS);

	switch		(surfDESC.Format)
	{
	case D3DFMT_D15S1:		bStencil = TRUE;	break;
	case D3DFMT_D24S8:		bStencil = TRUE;	break;
	case D3DFMT_D24X4S4:	bStencil = TRUE;	break;
	}

	// Scissoring
	if (caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST)	bScissor	= TRUE;
	else												bScissor	= FALSE;

	// Stencil relative caps
	u32 dwStencilCaps = caps.StencilCaps;
	if( (!(dwStencilCaps & D3DSTENCILCAPS_INCR) && !(dwStencilCaps & D3DSTENCILCAPS_INCRSAT))
		||(!(dwStencilCaps & D3DSTENCILCAPS_DECR) && !(dwStencilCaps & D3DSTENCILCAPS_DECRSAT)))
	{
		soDec = soInc = D3DSTENCILOP_KEEP;
		dwMaxStencilValue = 0;
	}
	else
	{
		// Prefer sat ops that cap at 0/max, but can use other ones as long as enough stencil bits
		soInc=(dwStencilCaps & D3DSTENCILCAPS_INCRSAT)? D3DSTENCILOP_INCRSAT:D3DSTENCILOP_INCR;
		soDec=(dwStencilCaps & D3DSTENCILCAPS_DECRSAT)? D3DSTENCILOP_DECRSAT:D3DSTENCILOP_DECR;
		dwMaxStencilValue=(1<<8)-1;
	}
	*/

	// DEV INFO
	gpu_num = get_gpu_num();
}
#else	//	USE_DX10
void hw_caps::update()
{
	// ***************** GEOMETRY
	geometry_major				= 4;
	geometry_minor				= 0;
	geometry.software			= FALSE;
	geometry.point_sprites		= FALSE;
	geometry.npatches			= FALSE;
	DWORD cnt					= 256;
	clamp<DWORD>(cnt,0,256);
	geometry.registers		= cnt;
	geometry.instructions		= 256;
	geometry.clip_planes		= _min(6,15);
	geometry.vtf				= TRUE;

	// ***************** PIXEL processing
	raster_major				= 4;
	raster_minor				= 0;
	raster.stages				= 16;
	raster.b_non_pow2				= TRUE;
	raster.b_cubemap				= TRUE;
	raster.mrt_count			= 4;
	//raster.b_mrt_mixdepth		= FALSE;
	raster.b_mrt_mixdepth		= TRUE;
	raster.instructions		= 256;

	// ***************** Info
	Msg							("* GPU shading: vs(%x/%d.%d/%d), ps(%x/%d.%d/%d)",
		0,	geometry_major, geometry_minor, CAP_VERSION(geometry_major,	geometry_minor),
		0,	raster_major,	raster_minor,	CAP_VERSION(raster_major,	raster_minor)
		);

	// *******1********** Vertex cache
	//	TODO: DX10: Find a way to detect cache size
	geometry.vertex_cache = 24;
	Msg					("* GPU vertex cache: %s, %d","unrecognized",u32(geometry.vertex_cache));

	// *******1********** Compatibility : vertex shader
	if (0==raster_major)		geometry_major=0;		// Disable VS if no PS
/*
	//
	bTableFog			=	FALSE;	//BOOL	(caps.RasterCaps&D3DPRASTERCAPS_FOGTABLE);

	// Detect if stencil available
	bStencil			=	TRUE;

	// Scissoring
	bScissor	= TRUE;

	// Stencil relative caps
	soInc=D3DSTENCILOP_INCRSAT;
	soDec=D3DSTENCILOP_DECRSAT;
	dwMaxStencilValue=(1<<8)-1;
*/

	// DEV INFO
	gpu_num = get_gpu_num();
}

#endif	//	USE_DX10

} // namespace render 
} // namespace xray 
