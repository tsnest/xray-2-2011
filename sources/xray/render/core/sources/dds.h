////////////////////////////////////////////////////////////////////////////
//	Created		: 14.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DDS_H_INCLUDED
#define XRAY_DDS_H_INCLUDED

//////////////////////////////////////////////////////////////////////////
// DDS defines
#define DDSCAPS_COMPLEX					0x8 
#define DDSCAPS_MIPMAP					0x400000 
#define DDSCAPS_TEXTURE					0x1000 

#define DDSCAPS2_CUBEMAP				0x200 
#define DDSCAPS2_CUBEMAP_POSITIVEX		0x400 
#define DDSCAPS2_CUBEMAP_NEGATIVEX		0x800 
#define DDSCAPS2_CUBEMAP_POSITIVEY		0x1000 
#define DDSCAPS2_CUBEMAP_NEGATIVEY		0x2000 
#define DDSCAPS2_CUBEMAP_POSITIVEZ		0x4000 
#define DDSCAPS2_CUBEMAP_NEGATIVEZ		0x8000 
#define DDSCAPS2_VOLUME					0x200000 


#define DDSD_CAPS						0x1
#define DDSD_HEIGHT						0x2
#define DDSD_WIDTH						0x4
#define DDSD_PITCH						0x8
#define DDSD_PIXELFORMAT				0x1000
#define DDSD_MIPMAPCOUNT				0x20000
#define DDSD_LINEARSIZE					0x80000
#define DDSD_DEPTH						0x800000

#define DDPF_ALPHAPIXELS				0x1
#define DDPF_ALPHA						0x2
#define DDPF_FOURCC						0x4
#define DDPF_RGB						0x40
#define DDPF_YUV						0x200
#define DDPF_LUMINANCE					0x20000

namespace xray {
namespace render {

//////////////////////////////////////////////////////////////////////////
// DDS system structure
typedef struct  {
	UINT dwSize;
	UINT dwFlags;
	UINT dwFourCC;
	UINT dwRGBBitCount;
	UINT dwRBitMask;
	UINT dwGBitMask;
	UINT dwBBitMask;
	UINT dwABitMask;
} DDS_PIXELFORMAT;

typedef struct {
	DWORD           dwSize;
	DWORD           dwFlags;
	DWORD           dwHeight;
	DWORD           dwWidth;
	DWORD           dwLinearSize;
	DWORD           dwDepth;
	DWORD           dwMipMapCount;
	DWORD           dwReserved1[11];
	DDS_PIXELFORMAT ddpf;
	DWORD           dwCaps;
	DWORD           dwCaps2;
	DWORD           dwCaps3;
	DWORD           dwCaps4;
	DWORD           dwReserved2;
} DDS_HEADER;

typedef struct {
	DXGI_FORMAT             dxgiFormat;
	D3D_RESOURCE_DIMENSION	resourceDimension;
	UINT                    miscFlag;
	UINT                    arraySize;
	UINT                    reserved;
} DDS_HEADER_DXT10;

//////////////////////////////////////////////////////////////////////////
// DDS xray structures
struct dds_info
{
	u32				width;
	u32				height;
	u32				mips_count;
	u32				array_size;
	DXGI_FORMAT		format;
	D3D_RESOURCE_DIMENSION    resource_dimension;
	u8 const*		data_ptr;
	u32				data_size;
};

class res_texture;

//////////////////////////////////////////////////////////////////////////
// DDS utility functions
bool	load_dds_info		( u8 const * buffer, u32 size, dds_info & dds_info);
bool	load_dds_to_texture	( dds_info const & dds_info, res_texture* staging_texture, u32 surf_min_pitch = 256, u32 row_min_pitch = 64);

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_DDS_H_INCLUDED