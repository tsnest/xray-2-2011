////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/device_caps.h>

static inline u32 cap_version( u32 a, u32 b)	
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

		res = _min( res, device_caps::MAX_GPUS );

		//	It's vital to have at least one GPU, else
		//	code will fail.
		ASSERT(res>0);

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


// Needs some reactor
void device_caps::update()
{
	// ***************** GEOMETRY
	geometry_major				= 4;
	geometry_minor				= 0;
	geometry.software			= false;
	geometry.point_sprites		= false;
	geometry.npatches			= false;
	DWORD cnt					= 256;
	math::clamp<DWORD>			(cnt,0,256);
	geometry.registers			= cnt;
	geometry.instructions		= 256;
	geometry.clip_planes		= math::min(6,15);
	geometry.vtf				= true;

	// ***************** PIXEL processing
	raster_major				= 4;
	raster_minor				= 0;
	raster.stages				= 16;
	raster.b_non_pow2			= true;
	raster.b_cubemap			= true;
	raster.mrt_count			= 4;
	//raster.b_mrt_mixdepth		= false;
	raster.b_mrt_mixdepth		= true;
	raster.instructions		= 256;

	// ***************** Info
	LOG_INFO					("* GPU shading: vs(%x/%d.%d/%d), ps(%x/%d.%d/%d)",
		0,	geometry_major, geometry_minor, cap_version(geometry_major,	geometry_minor),
		0,	raster_major,	raster_minor,	cap_version(raster_major,	raster_minor)
		);

	// *******1********** Vertex cache
	//	TODO: DX10: Find a way to detect cache size
	geometry.vertex_cache = 24;
	LOG_INFO					("* GPU vertex cache: %s, %d","unrecognized",u32(geometry.vertex_cache));

	// *******1********** Compatibility : vertex shader
	if (0==raster_major)		geometry_major=0;		// Disable VS if no PS
/*
	//
	bTableFog			=	false;	//BOOL	(caps.RasterCaps&D3DPRASTERCAPS_FOGTABLE);

	// Detect if stencil available
	bStencil			=	true;

	// Scissoring
	bScissor	= true;

	// Stencil relative caps
	soInc=D3DSTENCILOP_INCRSAT;
	soDec=D3DSTENCILOP_DECRSAT;
	dwMaxStencilValue=(1<<8)-1;
*/

	// DEV INFO
	gpu_num = get_gpu_num();
}


} // namespace render 
} // namespace xray 
