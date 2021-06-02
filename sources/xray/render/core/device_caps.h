////////////////////////////////////////////////////////////////////////////
//	Created		: 06.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEVICE_CAPS_H_INCLUDED
#define DEVICE_CAPS_H_INCLUDED

namespace xray {
namespace render {

class device_caps
{
public:
	enum
	{
		max_gpus	= 8	//	MAX_GPUS
	};

public:
	struct		caps_geometry	//	caps_Geometry
	{
		u32	registers		: 16;	//	dwRegisters
		u32 instructions	: 16;	//	instructions
		u32	software		: 1;	//	bSoftware
		u32	point_sprites	: 1;	//	bPointSprites
		u32	vtf				: 1;	//	bVTF	//	vertex-texture-fetch
		u32	npatches		: 1;	//	bNPatches
		u32 clip_planes		: 4;	//	dwClipPlanes
		u32 vertex_cache	: 8;	//	dwVertexCache
	};
	struct		caps_raster		//	caps_Raster
	{
		u32	registers		: 16;	//	dwRegisters
		u32 instructions	: 16;	//	instructions
		u32	stages			: 4;	//	dwStages	// number of tex-stages
		u32	mrt_count		: 4;	//	dwMRT_count
		u32 b_mrt_mixdepth	: 1;	//	b_MRT_mixdepth
		u32	b_non_pow2		: 1;	//	bNonPow2
		u32	b_cubemap		: 1;	//	bCubemap
	};
public:
	device_caps();

	void	update(void);

public:
	// force flags
	bool		force_gpu_ref;		//	bForceGPU_REF
	bool		force_gpu_sw;		//	bForceGPU_SW
	bool		force_gpu_non_pure;	//	bForceGPU_NonPure
	//bool		scene_mode;			//	SceneMode

	u32			gpu_num;			//	iGPUNum

	// device format
	DXGI_FORMAT	target_fmt;			//	fTarget
	DXGI_FORMAT	depth_fmt;			//	fDepth
	u32			refresh_rate;		//	dwRefreshRate

	// caps itself
	u16				geometry_major;
	u16				geometry_minor;
	caps_geometry	geometry;
	u16				raster_major;
	u16				raster_minor;
	caps_raster		raster;

	u32				id_vendor;
	u32				id_device;

	//BOOL			bStencil;			// stencil buffer present
	//BOOL			bScissor;			// scissor rect supported
	//BOOL			bTableFog;			//

	// some precalculated values
	//D3DSTENCILOP	soDec, soInc;		// best stencil OPs for shadows
	//u32				dwMaxStencilValue;  // maximum value the stencil buffer can hold
}; // class device_caps

inline device_caps::device_caps()
{
	memory::zero(this, sizeof(*this));
}

} // namespace render 
} // namespace xray 


#endif // #ifndef DEVICE_CAPS_H_INCLUDED