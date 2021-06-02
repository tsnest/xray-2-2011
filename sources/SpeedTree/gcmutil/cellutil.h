/*   SCE CONFIDENTIAL                                       */
/*   PlayStation(R)3 Programmer Tool Runtime Library 300.001 */
/*   Copyright (C) 2006 Sony Computer Entertainment Inc.    */
/*   All Rights Reserved.                                   */

#ifndef __CELL_UTILS_H__
#define __CELL_UTILS_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

static inline uint32_t cellUtilCountLeadingZero(uint32_t x)
{
//#ifdef __PPU__
//	uint32_t result;
//    __asm__ volatile ("cntlzw %0, %1" : "=r"(result) : "r"(x));
//	return result;
//#else
	uint32_t y;
	uint32_t n = 32;

	y = x >> 16; if (y != 0) { n = n - 16; x = y; }
	y = x >>  8; if (y != 0) { n = n -  8; x = y; }
	y = x >>  4; if (y != 0) { n = n -  4; x = y; }
	y = x >>  2; if (y != 0) { n = n -  2; x = y; }
	y = x >>  1; if (y != 0) { return n - 2; }
	return n - x;
//#endif
}

static inline uint32_t cellUtilLog2(uint32_t x)
{
	return 31 - cellUtilCountLeadingZero(x);
}

static inline int cellUtilIsPowerOfTwo(uint32_t x)
{
	return ((x & -x) == x);
}

static inline uint32_t cellUtilRoundDownToPowOf2(uint32_t x)
{
	if(cellUtilIsPowerOfTwo(x))
		return x;
	
	int i = 0;
	while( (x >> i) != 0 ) i++;
	return (1 << (i-1));
}

static inline uint32_t cellUtilRoundUpToPowOf2(uint32_t x)
{
	if(cellUtilIsPowerOfTwo(x))
		return x;
	
	int i = 0;
	while( (x >> i) != 0 ) i++;
	return (1 << (i));
}

extern void cellUtilConvertLinearToSwizzle(uint8_t *dst, uint8_t *src,
										   const uint32_t width,
										   const uint32_t height,
										   const uint32_t depth);

extern uint16_t cellUtilFloatToHalf(const float val);
extern float cellUtilHalfToFloat(const uint16_t val);

extern void cellUtilGenerateGradationTexture(uint8_t *texture,
											 const uint32_t width,
											 const uint32_t height,
											 const uint32_t depth);

extern void cellUtilGenerateSwizzledGradationTexture(uint8_t *texture,
													 const uint32_t width,
													 const uint32_t height,
													 const uint32_t depth);
extern void cellUtilGenerateGridTexture(uint8_t *texture,
										const uint32_t width,
										const uint32_t height,
										const uint32_t depth,
										const uint32_t linewidth,
										const uint32_t linefreq);
extern void cellUtilGenerateMipmap(uint8_t *texture,
								   const uint32_t width, const uint32_t height,
								   const uint32_t depth);

extern void cellUtilGenerateCubeMap(uint8_t *texture,
									const uint32_t width,
									const uint32_t height,
									const uint32_t depth,
									const uint32_t swizzled);

extern void cellUtilGenerateGradationTextureFP16(uint8_t *texture,
												 const uint32_t width,
												 const uint32_t height);
extern void cellUtilGenerateSwizzledGradationTextureFP16(uint8_t *texture,
														 const uint32_t width,
														 const uint32_t height);
extern void cellUtilGenerateGradationTextureFP32(uint8_t *texture,
												 const uint32_t width,
												 const uint32_t height);
extern void cellUtilGenerateSwizzledGradationTextureFP32(uint8_t *texture,
														 const uint32_t width,
														 const uint32_t height);

#if defined(__cplusplus)
}
#endif

/* for C++ linkage */

#if defined(__cplusplus)

#include <vectormath/cpp/vectormath_aos.h>

struct VertexData3D {
	Vectormath::Aos::Point3 pos;
	float u, v;
};

struct VertexData2D {
	float x,y;
	float u,v;
};

struct PointSpriteData {
	Vectormath::Aos::Point3 pos;
	float size;
};

extern uint32_t cellUtilGenerateCube(VertexData3D *V, float size);
extern uint32_t cellUtilGenerateQuad(VertexData2D *point, const float size);
extern uint32_t cellUtilGenerateRECTQuad(VertexData2D *point,
										 const uint32_t width,
										 const uint32_t height);
extern void cellUtilMatrixToFloatArray(Vectormath::Aos::Matrix4 &matrix,
									   float array[]);
extern uint32_t cellUtilGeneratePlane(VertexData3D *point, const float size,
									  const uint32_t row, const uint32_t col);
extern uint32_t cellUtilGenerateSphere(const uint32_t meridians,
									   const uint32_t parallels,
									   const float size,
									   float* vertices, uint16_t* indices);
extern uint32_t cellUtilGenerateWall(const uint32_t col, const uint32_t row,
									 const float size,
									 float *vertices, float *uv,
									 uint16_t *indices);
#endif /* __cplusplus */

#endif /* __CELL_UTILS_H__ */
