/*  SCE CONFIDENTIAL
 *  PlayStation(R)3 Programmer Tool Runtime Library 300.001
 *  Copyright (C) 2008 Sony Computer Entertainment Inc.
 *  All Rights Reserved.
 */

#ifndef __CELL_GCMUTIL_H__
#define __CELL_GCMUTIL_H__

// libgcm
#include <cell/gcm.h>
#include <sysutil/sysutil_sysparam.h>

#if defined(__cplusplus)

// cgb
#include <cell/cgb.h>

// vector math
#include <vectormath/cpp/vectormath_aos.h>

// gcmutil definition
#include "gcmutil_type.h"
#include "gcmutil_enum.h"
#include "gcmutil_inline.h"
#include "gcmutil_class.h"

#endif

// old interface
#include "gcmutil_old.h"

#if defined(__cplusplus)

#include "padutil.h"
#include "gtfloader.h"

namespace CellGcmUtil{


/* memory.cpp */
bool cellGcmUtilInitLocal(void* local_base_addr, size_t local_size);
bool cellGcmUtilInitMain(void* main_base_addr, size_t main_size);
bool cellGcmUtilAllocateLocal(uint32_t size, uint32_t alignment, Memory_t *memory);
bool cellGcmUtilAllocateMain(uint32_t size, uint32_t alignment, Memory_t *memory);
bool cellGcmUtilAllocate(uint32_t size, uint32_t alignment, uint8_t location, Memory_t *memory);
bool cellGcmUtilReallocate(Memory_t *memory, uint8_t location, uint32_t alignment);
bool cellGcmUtilAllocateUnmappedMain(uint32_t size, uint32_t alignment, Memory_t *memory);
void cellGcmUtilFree(Memory_t *memory);
void cellGcmUtilFinalizeMemory(void);


/* file.cpp */
bool cellGcmUtilReadFile(const char *fname, uint8_t **buffer, uint32_t *buf_size, uint32_t offset = 0, uint32_t size = 0xFFFFFFFFUL);
bool cellGcmUtilWriteFile(const char *fname, uint8_t *buffer, uint32_t buf_size);
uint32_t cellGcmUtilGetFileList(const char *path, const char *ext, FileList_t *filelist);
void cellGcmUtilFreeFileList(FileList_t *filelist);
bool cellGcmUtilIsFileExsits(const char *fname);


/* display */
bool cellGcmUtilInitDisplay(uint8_t color_format, uint8_t depth_format, uint8_t buffer_number, Memory_t *buffer, CellGcmSurface *surface);
bool cellGcmUtilSetDisplayBuffer(uint8_t number, CellGcmTexture *texture_array);


/* dbgfont.cpp */
bool cellGcmUtilDbgfontInit(int32_t word_count, void* addr, uint32_t size, uint8_t location);
void cellGcmUtilDbgfontEnd(void);
uint32_t cellGcmUtilDbgfontGetRequireSize(int32_t word_count);
void cellGcmUtilPuts(const char* str);
void cellGcmUtilPrintf(const char* format, ...);
void cellGcmUtilPrintfColor(uint32_t color, const char* format, ...);
void cellGcmUtilSetPrintPos(float x, float y);
void cellGcmUtilSetPrintSize(float size);
void cellGcmUtilSetPrintColor(uint32_t color);
void cellGcmUtilGetPrintPos(float *x, float *y);


/* utility.cpp */
bool cellGcmUtilInit(int32_t cb_size, int32_t main_size);
void cellGcmUtilEnd();
CellVideoOutResolution cellGcmUtilGetResolution(void);
float cellGcmUtilGetAspectRatio();
Viewport_t cellGcmUtilGetViewport2D(uint16_t width, uint16_t height);
Viewport_t cellGcmUtilGetViewportGL(uint32_t surface_height, uint32_t vpX, uint32_t vpY, uint32_t vpWidth, uint32_t vpHeight, float vpZMin, float vpZMax);
Viewport_t cellGcmUtilGetViewportDX(uint32_t vpX, uint32_t vpY, uint32_t vpWidth, uint32_t vpHeight, float vpZMin, float vpZMax, bool bPixelCenterInteger = true);
bool cellGcmUtilOffsetToAddress(uint8_t location, uint32_t offset, void **address);

/* shader.cpp */
bool cellGcmUtilLoadShader(const char *fname, Shader_t *shader);
bool cellGcmUtilLoadShaderFromMemory(const unsigned char *buffer, uint32_t size, Shader_t *shader);
void cellGcmUtilDestroyShader(Shader_t *shader);
bool cellGcmUtilGetVertexUCode(const Shader_t *shader, Memory_t *ucode);
bool cellGcmUtilGetFragmentUCode(const Shader_t *shader, uint8_t location, Memory_t *ucode);


/* texture.cpp */
bool cellGcmUtilCreateTexture(CellGcmTexture *texture, Memory_t *image);
bool cellGcmUtilCreateSimpleTexture(uint32_t width, uint32_t height, uint8_t location, CellGcmTexture *texture, Memory_t *image);
bool cellGcmUtilLoadTexture(const char* fname, uint32_t location, CellGcmTexture *tex, Memory_t *image);
uint32_t cellGcmUtilLoadPackedTexture(const char* fname, uint32_t index, uint32_t location, CellGcmTexture *texture, Memory_t *image);
void cellGcmUtilSetTextureUnit(uint32_t tex_unit, const CellGcmTexture *tex);
void cellGcmUtilInvalidateTextureUnit(uint32_t tex_unit);
uint32_t cellGcmUtilGetTextureRequireSize(const CellGcmTexture tex);
uint8_t cellGcmUtilGetRawFormat(uint8_t texture_format);
const char* cellGcmUtilGetFormatName(uint8_t texture_format);
uint16_t cellGcmUtilStr2Remap(const char *str);
const char* cellGcmUtilRemap2Str(uint16_t remap);
bool cellGcmUtilSaveTexture(const char* fname, const CellGcmTexture *texture, Memory_t *image);
CellGcmTexture cellGcmUtilSurfaceToTexture(const CellGcmSurface *surface, uint32_t index);
CellGcmTexture cellGcmUtilDepthToTexture(const CellGcmSurface *surface, bool bAsColor);
CellGcmSurface cellGcmUtilTextureToSurface(const CellGcmTexture *color, const CellGcmTexture *depth);
bool cellGcmUtilCreateTiledTexture(uint32_t width, uint32_t height, uint8_t format, uint8_t location, uint32_t comp_mode, uint32_t number, CellGcmTexture *texture_array, Memory_t *buffer);
bool cellGcmUtilCreateRenderTexture(uint32_t width, uint32_t height, uint8_t format, uint8_t location, uint32_t comp_mode, CellGcmTexture *texture, Memory_t *image);
bool cellGcmUtilCreateDepthTexture(uint32_t width, uint32_t height, uint8_t format, uint8_t location, uint32_t comp_mode, bool bZCull, bool bTile, CellGcmTexture *texture, Memory_t *image);
uint8_t cellGcmUtilBindTile(CellGcmTexture *texture, uint32_t comp_mode);
uint8_t cellGcmUtilBindSharedTile(CellGcmTexture *texture, uint32_t comp_mode, uint32_t size);
uint32_t cellGcmUtilBindZCull(CellGcmTexture *texture);
void cellGcmUtilUnbindTile(CellGcmTexture *texture);
void cellGcmUtilUnbindZCull(CellGcmTexture *texture);

/* camera.cpp */
void cellGcmUtilSimpleCameraInit(Vectormath::Aos::Vector3 eye, Vectormath::Aos::Vector3 at, Vectormath::Aos::Vector3 up);
void cellGcmUtilSimpleCameraUpdate(void);
void cellGcmUtilSimpleCameraRotate(CellPadUtilAxis angle, bool bReverseX, bool bReverseY, bool bReset);
void cellGcmUtilSimpleCameraMove(int32_t dx, int32_t dy, int32_t dz, bool bReset);
Vectormath::Aos::Matrix4 cellGcmUtilSimpleCameraGetMatrix(void);
void cellGcmUtilSimpleCameraGetInitParam(Vectormath::Aos::Vector3 *eye, Vectormath::Aos::Vector3 *at, Vectormath::Aos::Vector3 *up);
void cellGcmUtilSimpleCameraGetStatus(Vectormath::Aos::Vector3 *eye, Vectormath::Aos::Vector3 *at, Vectormath::Aos::Vector3 *up);
void cellGcmUtilSimpleCameraGetParam(Vectormath::Aos::Vector3 *trans, float *yaw, float *pitch);
void cellGcmUtilSimpleCameraSetParam(Vectormath::Aos::Vector3 trans, float yaw, float pitch);


} // namespace CellGcmUtil
#endif

#endif /* __CELL_GCMUTIL_H__ */

