////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef AMD_COMPRESS_TEXTURE_COMPRESSOR_UTILS_H_INCLUDED
#define AMD_COMPRESS_TEXTURE_COMPRESSOR_UTILS_H_INCLUDED

#include "ATI_Compress_Test_Helpers.h"


#define Max(a,b)            (((a) > (b)) ? (a) : (b))
#define Min(a,b)            (((a) < (b)) ? (a) : (b))

#pragma pack(push,4)
struct dds_header {
	u32		signature;
	DDSD2	header;
}; // struct dds_header
#pragma pack(pop)

enum TGAType {
    TGA_TYPE_INDEXED		= 1,
    TGA_TYPE_RGB			= 2,
    TGA_TYPE_GREY			= 3,
    TGA_TYPE_RLE_INDEXED	= 9,
    TGA_TYPE_RLE_RGB		= 10,
    TGA_TYPE_RLE_GREY		= 11
};

#define TGA_INTERLEAVE_MASK	0xc0
#define TGA_INTERLEAVE_NONE	0x00
#define TGA_INTERLEAVE_2WAY	0x40
#define TGA_INTERLEAVE_4WAY	0x80

#define TGA_ORIGIN_MASK		0x30
#define TGA_ORIGIN_LEFT		0x00
#define TGA_ORIGIN_RIGHT	0x10
#define TGA_ORIGIN_LOWER	0x00
#define TGA_ORIGIN_UPPER	0x20

#define TGA_HAS_ALPHA		0x0F


static inline void setBGRA	( u32& output, u8 const blue, u8 const green, u8 const red, u8 const alpha )
{
	union bgra {
#pragma warning(push)
#pragma warning(disable:4201)
		struct {
			u8 b, g, r, a;
		};
#pragma warning(pop)
		u32 u;
	};
	bgra	temp;
	temp.b	= blue;
	temp.g	= green;
	temp.r	= red;
	temp.a	= alpha;
	output	= temp.u;
}

static inline void setBGRA_16	( u64& output, u16 const blue, u16 const green, u16 const red, u16 const alpha )
{
	union bgra {
#pragma warning(push)
#pragma warning(disable:4201)
		struct {
			u16 b, g, r, a;
		};
#pragma warning(pop)
		u64 u;
	};
	bgra	temp;
	temp.b	= blue;
	temp.g	= green;
	temp.r	= red;
	temp.a	= alpha;
	output	= temp.u;
}

struct Color555 {
	u16 b : 5;
	u16 g : 5;
	u16 r : 5;
};

void		fill_dds_header	(	dds_header& header, 
								ATI_TC_DWORD width, 
								ATI_TC_DWORD height, 
								ATI_TC_DWORD size, 
								ATI_TC_DWORD pitch, 
								ATI_TC_FORMAT format, 
								bool is_cubemap, 
								u32 mip_map_count );

u32* cook_dds_body_from_fb	(	 u32& out_body_size,
								 malloc_function_ptr_type allocator, 
								 free_function_ptr_type deallocator, 
								 ATI_TC_CompressOptions options, 
								 ATI_TC_FORMAT format, 
								 FIBITMAP* fb, 
								 s32 cubemap_face );

FIBITMAP*	get_freeimage_from_memory	(	u8 const* const tga_file_data, 
											u32 const tga_file_data_size );

void output_log( pcstr string );
namespace extreme_dxt
{
	void CompressImageDXT1				( const BYTE* argb, BYTE* dxt1, int width, int height );
	void CompressImageDXT5				( const BYTE* argb, BYTE* dxt5, int width, int height );
}

namespace realtime_dxt{
	void CompressImageDXT1( const byte *inBuf, byte *outBuf, int width, int height, int &outputBytes );
	void CompressImageDXT5( const byte *inBuf, byte *outBuf, int width, int height, int &outputBytes );
};

#endif // #ifndef AMD_COMPRESS_TEXTURE_COMPRESSOR_UTILS_H_INCLUDED