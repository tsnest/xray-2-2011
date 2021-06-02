// ATI_Compress_Test_Helpers.cpp
//
#include "pch.h"

#include "../ATI_Compress.h"
#include "ATI_Compress_Test_Helpers.h"

#define FOURCC_ATI1N		        MAKEFOURCC('A', 'T', 'I', '1')
#define FOURCC_ATI2N		        MAKEFOURCC('A', 'T', 'I', '2')
#define FOURCC_ATI2N_XY		        MAKEFOURCC('A', '2', 'X', 'Y')
#define FOURCC_ATI2N_DXT5	        MAKEFOURCC('A', '2', 'D', '5')
#define FOURCC_DXT5_xGBR	        MAKEFOURCC('x', 'G', 'B', 'R')
#define FOURCC_DXT5_RxBG	        MAKEFOURCC('R', 'x', 'B', 'G')
#define FOURCC_DXT5_RBxG	        MAKEFOURCC('R', 'B', 'x', 'G')
#define FOURCC_DXT5_xRBG	        MAKEFOURCC('x', 'R', 'B', 'G')
#define FOURCC_DXT5_RGxB	        MAKEFOURCC('R', 'G', 'x', 'B')
#define FOURCC_DXT5_xGxR	        MAKEFOURCC('x', 'G', 'x', 'R')
#define FOURCC_APC1			        MAKEFOURCC('A', 'P', 'C', '1')
#define FOURCC_APC2			        MAKEFOURCC('A', 'P', 'C', '2')
#define FOURCC_APC3			        MAKEFOURCC('A', 'P', 'C', '3')
#define FOURCC_APC4			        MAKEFOURCC('A', 'P', 'C', '4')
#define FOURCC_APC5			        MAKEFOURCC('A', 'P', 'C', '5')
#define FOURCC_APC6			        MAKEFOURCC('A', 'P', 'C', '6')
#define FOURCC_ATC_RGB			    MAKEFOURCC('A', 'T', 'C', ' ')
#define FOURCC_ATC_RGBA_EXPLICIT    MAKEFOURCC('A', 'T', 'C', 'A')
#define FOURCC_ATC_RGBA_INTERP	    MAKEFOURCC('A', 'T', 'C', 'I')
#define FOURCC_ETC_RGB			    MAKEFOURCC('E', 'T', 'C', ' ')
#define FOURCC_BC1                  MAKEFOURCC('B', 'C', '1', ' ')
#define FOURCC_BC2                  MAKEFOURCC('B', 'C', '2', ' ')
#define FOURCC_BC3                  MAKEFOURCC('B', 'C', '3', ' ')
#define FOURCC_BC4                  MAKEFOURCC('B', 'C', '4', ' ')
#define FOURCC_BC4S                 MAKEFOURCC('B', 'C', '4', 'S')
#define FOURCC_BC4U                 MAKEFOURCC('B', 'C', '4', 'U')
#define FOURCC_BC5                  MAKEFOURCC('B', 'C', '5', ' ')
#define FOURCC_BC5S                 MAKEFOURCC('B', 'C', '5', 'S')
// Deprecated but still supported for decompression
#define FOURCC_DXT5_GXRB            MAKEFOURCC('G', 'X', 'R', 'B')
#define FOURCC_DXT5_GRXB            MAKEFOURCC('G', 'R', 'X', 'B')
#define FOURCC_DXT5_RXGB            MAKEFOURCC('R', 'X', 'G', 'B')
#define FOURCC_DXT5_BRGX            MAKEFOURCC('B', 'R', 'G', 'X')

typedef struct 
{
   DWORD dwFourCC;
   ATI_TC_FORMAT nFormat;
} ATI_TC_FourCC;

ATI_TC_FourCC g_FourCCs[] = 
{
   {FOURCC_DXT1,               ATI_TC_FORMAT_DXT1},
   {FOURCC_DXT3,               ATI_TC_FORMAT_DXT3},
   {FOURCC_DXT5,               ATI_TC_FORMAT_DXT5},
   {FOURCC_DXT5_xGBR,          ATI_TC_FORMAT_DXT5_xGBR},
   {FOURCC_DXT5_RxBG,          ATI_TC_FORMAT_DXT5_RxBG},
   {FOURCC_DXT5_RBxG,          ATI_TC_FORMAT_DXT5_RBxG},
   {FOURCC_DXT5_xRBG,          ATI_TC_FORMAT_DXT5_xRBG},
   {FOURCC_DXT5_RGxB,          ATI_TC_FORMAT_DXT5_RGxB},
   {FOURCC_DXT5_xGxR,          ATI_TC_FORMAT_DXT5_xGxR},
   {FOURCC_DXT5_GXRB,          ATI_TC_FORMAT_DXT5_xRBG},
   {FOURCC_DXT5_GRXB,          ATI_TC_FORMAT_DXT5_RxBG},
   {FOURCC_DXT5_RXGB,          ATI_TC_FORMAT_DXT5_xGBR},
   {FOURCC_DXT5_BRGX,          ATI_TC_FORMAT_DXT5_RGxB},
   {FOURCC_ATI1N,              ATI_TC_FORMAT_ATI1N},
   {FOURCC_ATI2N,              ATI_TC_FORMAT_ATI2N},
   {FOURCC_ATI2N_XY,           ATI_TC_FORMAT_ATI2N_XY},
   {FOURCC_ATI2N_DXT5,         ATI_TC_FORMAT_ATI2N_DXT5},
   {FOURCC_BC1,                ATI_TC_FORMAT_BC1},
   {FOURCC_BC2,                ATI_TC_FORMAT_BC2},
   {FOURCC_BC3,                ATI_TC_FORMAT_BC3},
   {FOURCC_BC4,                ATI_TC_FORMAT_BC4},
   {FOURCC_BC4S,               ATI_TC_FORMAT_BC4},
   {FOURCC_BC4U,               ATI_TC_FORMAT_BC4},
   {FOURCC_BC5,                ATI_TC_FORMAT_BC5},
   {FOURCC_BC5S,               ATI_TC_FORMAT_BC5},
   {FOURCC_ATC_RGB,            ATI_TC_FORMAT_ATC_RGB},
   {FOURCC_ATC_RGBA_EXPLICIT,  ATI_TC_FORMAT_ATC_RGBA_Explicit},
   {FOURCC_ATC_RGBA_INTERP,    ATI_TC_FORMAT_ATC_RGBA_Interpolated},
   {FOURCC_ETC_RGB,            ATI_TC_FORMAT_ETC_RGB},
};
DWORD g_dwFourCCCount = sizeof(g_FourCCs) / sizeof(g_FourCCs[0]);

ATI_TC_FORMAT GetFormat(DWORD dwFourCC)
{
   for(DWORD i = 0; i < g_dwFourCCCount; i++)
      if(g_FourCCs[i].dwFourCC == dwFourCC)
         return g_FourCCs[i].nFormat;

   return ATI_TC_FORMAT_Unknown;
}

DWORD GetFourCC(ATI_TC_FORMAT nFormat)
{
   for(DWORD i = 0; i < g_dwFourCCCount; i++)
      if(g_FourCCs[i].nFormat == nFormat)
         return g_FourCCs[i].dwFourCC;

   return 0;
}

bool IsDXT5SwizzledFormat(ATI_TC_FORMAT nFormat)
{
   if(nFormat == ATI_TC_FORMAT_DXT5_xGBR || nFormat == ATI_TC_FORMAT_DXT5_RxBG || nFormat == ATI_TC_FORMAT_DXT5_RBxG || 
      nFormat == ATI_TC_FORMAT_DXT5_xRBG || nFormat == ATI_TC_FORMAT_DXT5_RGxB || nFormat == ATI_TC_FORMAT_DXT5_xGxR || 
      nFormat == ATI_TC_FORMAT_ATI2N_DXT5)
      return true;
   else
      return false;
}

typedef struct 
{
   ATI_TC_FORMAT nFormat;
   TCHAR* pszFormatDesc;
} ATI_TC_FormatDesc;

ATI_TC_FormatDesc g_FormatDesc[] = 
{
   {ATI_TC_FORMAT_Unknown,                 ("Unknown")},
   {ATI_TC_FORMAT_ARGB_8888,               ("ARGB_8888")},
   {ATI_TC_FORMAT_RGB_888,                 ("RGB_888")},
   {ATI_TC_FORMAT_RG_8,                    ("RG_8")},
   {ATI_TC_FORMAT_R_8,                     ("R_8")},
   {ATI_TC_FORMAT_ARGB_2101010,            ("ARGB_2101010")},
   {ATI_TC_FORMAT_ARGB_16,                 ("ARGB_16")},
   {ATI_TC_FORMAT_RG_16,                   ("RG_16")},
   {ATI_TC_FORMAT_R_16,                    ("R_16")},
   {ATI_TC_FORMAT_ARGB_16F,                ("ARGB_16F")},
   {ATI_TC_FORMAT_RG_16F,                  ("RG_16F")},
   {ATI_TC_FORMAT_R_16F,                   ("R_16F")},
   {ATI_TC_FORMAT_ARGB_32F,                ("ARGB_32F")},
   {ATI_TC_FORMAT_RG_32F,                  ("RG_32F")},
   {ATI_TC_FORMAT_R_32F,                   ("R_32F")},
   {ATI_TC_FORMAT_DXT1,                    ("DXT1")},
   {ATI_TC_FORMAT_DXT3,                    ("DXT3")},
   {ATI_TC_FORMAT_DXT5,                    ("DXT5")},
   {ATI_TC_FORMAT_DXT5_xGBR,               ("DXT5_xGBR")},
   {ATI_TC_FORMAT_DXT5_RxBG,               ("DXT5_RxBG")},
   {ATI_TC_FORMAT_DXT5_RBxG,               ("DXT5_RBxG")},
   {ATI_TC_FORMAT_DXT5_xRBG,               ("DXT5_xRBG")},
   {ATI_TC_FORMAT_DXT5_RGxB,               ("DXT5_RGxB")},
   {ATI_TC_FORMAT_DXT5_xGxR,               ("DXT5_xGxR")},
   {ATI_TC_FORMAT_ATI1N,                   ("ATI1N")},
   {ATI_TC_FORMAT_ATI2N,                   ("ATI2N")},
   {ATI_TC_FORMAT_ATI2N_XY,                ("ATI2N_XY")},
   {ATI_TC_FORMAT_ATI2N_DXT5,              ("ATI2N_DXT5")},
   {ATI_TC_FORMAT_BC1,                     ("BC1")},
   {ATI_TC_FORMAT_BC2,                     ("BC2")},
   {ATI_TC_FORMAT_BC3,                     ("BC3")},
   {ATI_TC_FORMAT_BC4,                     ("BC4")},
   {ATI_TC_FORMAT_BC5,                     ("BC5")},
   {ATI_TC_FORMAT_ATC_RGB,                 ("ATC_RGB")},
   {ATI_TC_FORMAT_ATC_RGBA_Explicit,       ("ATC_RGBA_Explicit")},
   {ATI_TC_FORMAT_ATC_RGBA_Interpolated,   ("ATC_RGBA_Interpolated")},
   {ATI_TC_FORMAT_ETC_RGB,                 ("ETC_RGB")},
};
DWORD g_dwFormatDescCount = sizeof(g_FormatDesc) / sizeof(g_FormatDesc[0]);

ATI_TC_FORMAT ParseFormat(char const* pszFormat)
{
   if(pszFormat == NULL)
      return ATI_TC_FORMAT_Unknown;

   for(DWORD i = 0; i < g_dwFormatDescCount; i++)
      if(strcmp(pszFormat, g_FormatDesc[i].pszFormatDesc) == 0)
         return g_FormatDesc[i].nFormat;

   return ATI_TC_FORMAT_Unknown;
}

TCHAR* GetFormatDesc(ATI_TC_FORMAT nFormat)
{
   for(DWORD i = 0; i < g_dwFormatDescCount; i++)
      if(nFormat == g_FormatDesc[i].nFormat)
         return g_FormatDesc[i].pszFormatDesc;

   return g_FormatDesc[0].pszFormatDesc;
}

bool LoadDDSFile(TCHAR* pszFile, ATI_TC_Texture& texture)
{
   FILE* pSourceFile = 0;
   fopen_s( &pSourceFile, pszFile, ("rb"));

   DWORD dwFileHeader;
   fread(&dwFileHeader, sizeof(DWORD), 1, pSourceFile);
   if(dwFileHeader != DDS_HEADER)
   {
      printf(("Source file is not a valid DDS.\n"));
      fclose(pSourceFile);
      return false;
   }

   DDSD2 ddsd;
   fread(&ddsd, sizeof(DDSD2), 1, pSourceFile);

   memset(&texture, 0, sizeof(texture));
   texture.dwSize = sizeof(texture);
   texture.dwWidth = ddsd.dwWidth;
   texture.dwHeight = ddsd.dwHeight;
   texture.dwPitch = ddsd.lPitch;

   if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
      texture.format = ATI_TC_FORMAT_ARGB_8888;
   else if(ddsd.ddpfPixelFormat.dwRGBBitCount==24)
      texture.format = ATI_TC_FORMAT_RGB_888;
   else if(GetFormat(ddsd.ddpfPixelFormat.dwPrivateFormatBitCount) != ATI_TC_FORMAT_Unknown)
      texture.format = GetFormat(ddsd.ddpfPixelFormat.dwPrivateFormatBitCount);
   else if(GetFormat(ddsd.ddpfPixelFormat.dwFourCC) != ATI_TC_FORMAT_Unknown)
      texture.format = GetFormat(ddsd.ddpfPixelFormat.dwFourCC);
   else
   {
      printf(("Unsupported source format.\n"));
      fclose(pSourceFile);
      return false;
   }

   // Init source texture
   texture.dwDataSize = ATI_TC_CalculateBufferSize(&texture);
   texture.pData = (ATI_TC_BYTE*) malloc(texture.dwDataSize);

   fread(texture.pData, texture.dwDataSize, 1, pSourceFile);
   fclose(pSourceFile);

   return true;
}

void SaveDDSFile(TCHAR* pszFile, ATI_TC_Texture& texture)
{
   FILE* pFile = 0;
   errno_t const error	= fopen_s( &pFile, pszFile, ("wb"));
   if ( error )
      return;

   fwrite(&DDS_HEADER, sizeof(DWORD), 1, pFile);

   DDSD2 ddsd;
   memset(&ddsd, 0, sizeof(DDSD2));
   ddsd.dwSize = sizeof(DDSD2);
   ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT|DDSD_MIPMAPCOUNT|DDSD_LINEARSIZE;
   ddsd.dwWidth = texture.dwWidth;
   ddsd.dwHeight = texture.dwHeight;
   ddsd.dwMipMapCount = 1;

   ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
   ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE|DDSCAPS_COMPLEX|DDSCAPS_MIPMAP;

   ddsd.ddpfPixelFormat.dwFourCC = GetFourCC(texture.format);
   if(ddsd.ddpfPixelFormat.dwFourCC)
   {
      ddsd.dwLinearSize = texture.dwDataSize;
      ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
      if(IsDXT5SwizzledFormat(texture.format))
      {
         ddsd.ddpfPixelFormat.dwPrivateFormatBitCount = ddsd.ddpfPixelFormat.dwFourCC;
         ddsd.ddpfPixelFormat.dwFourCC = FOURCC_DXT5;
      }
   }
   else
   {
      switch(texture.format)
      {
      case ATI_TC_FORMAT_ARGB_8888:
         ddsd.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
         ddsd.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
         ddsd.ddpfPixelFormat.dwBBitMask = 0x000000ff;
         ddsd.lPitch = texture.dwPitch;
         ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
         ddsd.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_RGB;
         ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
         break;

      case ATI_TC_FORMAT_RGB_888:
         ddsd.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
         ddsd.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
         ddsd.ddpfPixelFormat.dwBBitMask = 0x000000ff;
         ddsd.lPitch = texture.dwPitch;
         ddsd.ddpfPixelFormat.dwRGBBitCount = 24;
         ddsd.ddpfPixelFormat.dwFlags=DDPF_RGB;
         break;

      case ATI_TC_FORMAT_RG_8:
         ddsd.ddpfPixelFormat.dwRBitMask = 0x0000ff00;
         ddsd.ddpfPixelFormat.dwGBitMask = 0x000000ff;
         ddsd.lPitch = texture.dwPitch;
         ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
         ddsd.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_LUMINANCE;
         ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
         break;

      case ATI_TC_FORMAT_R_8:
         ddsd.ddpfPixelFormat.dwRBitMask = 0x000000ff;
         ddsd.lPitch = texture.dwPitch;
         ddsd.ddpfPixelFormat.dwRGBBitCount = 8;
         ddsd.ddpfPixelFormat.dwFlags= DDPF_LUMINANCE;
         break;

      case ATI_TC_FORMAT_ARGB_2101010:
         ddsd.ddpfPixelFormat.dwRBitMask = 0x000003ff;
         ddsd.ddpfPixelFormat.dwGBitMask = 0x000ffc00;
         ddsd.ddpfPixelFormat.dwBBitMask = 0x3ff00000;
         ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xc0000000;
         ddsd.lPitch = texture.dwPitch;
         ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
         ddsd.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_RGB;
         break;

      case ATI_TC_FORMAT_ARGB_16:
         ddsd.dwLinearSize = texture.dwDataSize;
         ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
         ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_A16B16G16R16;
         break;

      case ATI_TC_FORMAT_RG_16:
         ddsd.dwLinearSize = texture.dwDataSize;
         ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
         ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_G16R16;
         break;

      case ATI_TC_FORMAT_R_16:
         ddsd.dwLinearSize = texture.dwDataSize;
         ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
         ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_L16;
         break;

      case ATI_TC_FORMAT_ARGB_16F:
         ddsd.dwLinearSize = texture.dwDataSize;
         ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
         ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_A16B16G16R16F;
         break;

      case ATI_TC_FORMAT_RG_16F:
         ddsd.dwLinearSize = texture.dwDataSize;
         ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
         ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_G16R16F;
         break;

      case ATI_TC_FORMAT_R_16F:
         ddsd.dwLinearSize = texture.dwDataSize;
         ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
         ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_R16F;
         break;

      case ATI_TC_FORMAT_ARGB_32F:
         ddsd.dwLinearSize = texture.dwDataSize;
         ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
         ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_A32B32G32R32F;
         break;

      case ATI_TC_FORMAT_RG_32F:
         ddsd.dwLinearSize = texture.dwDataSize;
         ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
         ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_G32R32F;
         break;

      case ATI_TC_FORMAT_R_32F:
         ddsd.dwLinearSize = texture.dwDataSize;
         ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
         ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_R32F;
         break;

      default:
         assert(0);
         break;
      }
   }

   fwrite(&ddsd, sizeof(DDSD2), 1, pFile);
   fwrite(texture.pData, texture.dwDataSize, 1, pFile);

   fclose(pFile);
}
