/*   SCE CONFIDENTIAL                                       */
/*   PlayStation(R)3 Programmer Tool Runtime Library 300.001 */
/*   Copyright (C) 2006 Sony Computer Entertainment Inc.    */
/*   All Rights Reserved.                                   */

#ifndef __CELL_GCMUTIL_OLD_H__
#define __CELL_GCMUTIL_OLD_H__

#include <sys/cdefs.h>
#include <cell/gcm.h>

#define GCM_UTIL_COLOR_BUF_NUM 2
#define GCM_UTIL_USED_TILE_INDEX_NUM (GCM_UTIL_COLOR_BUF_NUM+1)
#define GCM_UTIL_LABEL_INDEX 255

#if defined(__cplusplus) 
extern "C" {
#endif  

enum {
	CELL_GCM_UTIL_RGBA8 = 0x8058,
	CELL_GCM_UTIL_ARGB8 = 0xff00,
	CELL_GCM_UTIL_BGRA8 = 0xff01,
	CELL_GCM_UTIL_DXT1  = 0x83f1,
	CELL_GCM_UTIL_DXT3  = 0x83f2,
	CELL_GCM_UTIL_DXT5  = 0x83f3,
	CELL_GCM_UTIL_FP16  = 0x881a,
	CELL_GCM_UTIL_FP32  = 0x8814,
};

//
// prototypes of function from memory.cpp
//
extern void cellGcmUtilInitializeLocalMemory(const size_t base,
											 const size_t size);
extern void *cellGcmUtilAllocateLocalMemory(const size_t size,
											const size_t alignment);

extern uint32_t cellGcmUtilGetOffsetOfMainMemory(void* ea);
extern void* cellGcmUtilGetMainMemoryBaseAddress(void);
extern void cellGcmUtilInitializeMainMemory(const size_t size);
extern void cellGcmUtilFreeMainMemory(void);
extern void cellGcmUtilResetMainMemory(void);
extern void *cellGcmUtilAllocateMainMemory(const size_t size,
										 const size_t alignment);
extern size_t cellGcmUtilGetAllocatedLocalMemorySize(void);
extern size_t cellGcmUtilGetAllocatedMainMemorySize(void);

// 
// prototypes of function from texture.cpp 
//
extern int cellGcmUtilGetTextureAttribute(const uint32_t glFormat,
										  uint32_t *gcmFormat, uint32_t *remap,
										  uint32_t swizzle, uint32_t normalize);

// 
// prototypes of function from display.cpp 
//
extern void      cellGcmUtilFinish(void);
extern void      cellGcmUtilWaitForIdle(void);
extern bool      cellGcmUtilInitDisplay(void);
extern void      cellGcmUtilSetTile(void);
extern void      cellGcmUtilSetInvalidateTile(void);
extern void      cellGcmUtilSetZcull(void);
extern void      cellGcmUtilUnsetZcull(void);
extern CellGcmSurface* cellGcmUtilGetCurrentSurface(void);
extern void      cellGcmUtilFlipDisplay(void);
extern void      cellGcmUtilResetRenderTarget(void);
extern void      cellGcmUtilSwapRenderTarget(void);
extern void*     cellGcmUtilGetColorBaseAddress(void);
extern void*     cellGcmUtilGetDepthBaseAddress(void);
extern uint32_t  cellGcmUtilGetDisplayWidth(void);
extern uint32_t  cellGcmUtilGetDisplayHeight(void);
extern uint32_t  cellGcmUtilGetColorBufferSize(void);
extern uint32_t  cellGcmUtilGetDepthBufferSize(void);
extern float     cellGcmUtilGetDisplayAspectRatio(void);
extern bool      cellGcmUtilEnableFP16Surface(void);
extern bool      cellGcmUtilResetDisplayBuffer(uint8_t gcm_format);
extern bool      cellGcmUtilIsFP16Supported(void);

extern void      cellGcmUtilSetClearColor(const uint32_t color);
extern void      cellGcmUtilSetClearSurface(const uint32_t mask);

// 
// prototypes of function from tiled.cpp
//
extern void cellGcmUtilLinearToTileAddress(unsigned long *adrOut, unsigned long adrIn);
extern int cellGcmUtilConvertLinearToTileFormat(unsigned int baseAddress, unsigned int tilePitch, unsigned int bankSense, unsigned int texSize);

//
// prototypes of function from exit_callback.cpp
//
extern int  cellGcmUtilInitCallback(void (*exit_callback)(void));
extern bool cellGcmUtilCheckCallback(void);

#if defined(__cplusplus) 
}
#endif 

#endif /* __CELL_GCMUTIL_OLD_H__ */
