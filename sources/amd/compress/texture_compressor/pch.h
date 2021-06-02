////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef AMD_COMPRESS_TEXTURE_COMPRESSOR_PCH_H_INCLUDED
#define AMD_COMPRESS_TEXTURE_COMPRESSOR_PCH_H_INCLUDED

#define	XRAY_TEXTURE_COMPRESSOR_BUILDING
#include <xray/os_preinclude.h>
#undef NOKERNEL
#undef NOD3D
#undef NOGDI
#undef NOUSER
#undef NOMSG
#include <xray/os_include.h>

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	uint;


#include <amd/compress/texture_compressor/api.h>
#include <ddraw.h>
#include <d3d9types.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

using xray::texture_compressor::malloc_function_ptr_type;
using xray::texture_compressor::free_function_ptr_type;

#include <freeimage/FreeImage.h>
#include <freeimage/FreeImageIO.h>

#include <nvidia/nvt/inc/nvtt.h>

//#include <nvidia\DDSUtilities\inc\dxtlib\dxtlib.h>
//#ifdef WIN64
//#	pragma comment (lib, "../../../nvidia/DDSUtilities/lib/nvDXTlibMTDLL.vc8.x64.lib" )
//#else // #ifdef WIN64
//#	pragma comment (lib, "../../../nvidia/DDSUtilities/lib/nvDXTlibMTDLL.vc8.lib" )
//#endif // #ifdef WIN64

#endif // #ifndef AMD_COMPRESS_TEXTURE_COMPRESSOR_PCH_H_INCLUDED