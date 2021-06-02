// ATI_Compress_Test_Helpers.h : Defines the entry point for the console application.
//

#include "../ATI_Compress.h"

ATI_TC_FORMAT GetFormat(DWORD dwFourCC);
DWORD GetFourCC(ATI_TC_FORMAT nFormat);
bool IsDXT5SwizzledFormat(ATI_TC_FORMAT nFormat);
ATI_TC_FORMAT ParseFormat(char const* pszFormat);
TCHAR* GetFormatDesc(ATI_TC_FORMAT nFormat);

bool LoadDDSFile(TCHAR* pszFile, ATI_TC_Texture& texture);
void SaveDDSFile(TCHAR* pszFile, ATI_TC_Texture& texture);


#ifdef _WIN64
#	define POINTER_64 __ptr64

//#pragma pack(4)
#pragma pack(push,4)

typedef struct _DDSURFACEDESC2_64
{
   DWORD               dwSize;                 // size of the DDSURFACEDESC structure
   DWORD               dwFlags;                // determines what fields are valid
   DWORD               dwHeight;               // height of surface to be created
   DWORD               dwWidth;                // width of input surface
   union
   {
      LONG            lPitch;                 // distance to start of next line (return value only)
      DWORD           dwLinearSize;           // Formless late-allocated optimized surface size
   } DUMMYUNIONNAMEN(1);
   union
   {
      DWORD           dwBackBufferCount;      // number of back buffers requested
      DWORD           dwDepth;                // the depth if this is a volume texture 
   } DUMMYUNIONNAMEN(5);
   union
   {
      DWORD           dwMipMapCount;          // number of mip-map levels requestde
      // dwZBufferBitDepth removed, use ddpfPixelFormat one instead
      DWORD           dwRefreshRate;          // refresh rate (used when display mode is described)
      DWORD           dwSrcVBHandle;          // The source used in VB::Optimize
   } DUMMYUNIONNAMEN(2);
   DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
   DWORD               dwReserved;             // reserved
   void* __ptr32       lpSurface;              // pointer to the associated surface memory
   union
   {
      DDCOLORKEY      ddckCKDestOverlay;      // color key for destination overlay use
      DWORD           dwEmptyFaceColor;       // Physical color for empty cubemap faces
   } DUMMYUNIONNAMEN(3);
   DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
   DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
   DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
   union
   {
      DDPIXELFORMAT   ddpfPixelFormat;        // pixel format description of the surface
      DWORD           dwFVF;                  // vertex format description of vertex buffers
   } DUMMYUNIONNAMEN(4);
   DDSCAPS2            ddsCaps;                // direct draw surface capabilities
   DWORD               dwTextureStage;         // stage in multitexture cascade
} DDSURFACEDESC2_64;

#define DDSD2 DDSURFACEDESC2_64
#pragma pack(pop)

#else
#define DDSD2 DDSURFACEDESC2
#endif

static const DWORD DDS_HEADER = MAKEFOURCC('D', 'D', 'S', ' ');