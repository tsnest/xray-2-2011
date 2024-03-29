// ==========================================================
// Bitmap rotation by means of 3 shears.
//
// Design and implementation by
// - Herv� Drolon (drolon@infonie.fr)
// - Thorsten Radde (support@IdealSoftware.com)
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

/* 
 ============================================================
 References : 
 [1] Paeth A., A Fast Algorithm for General Raster Rotation. 
 Graphics Gems, p. 179, Andrew Glassner editor, Academic Press, 1990. 
 [2] Yariv E., High quality image rotation (rotate by shear). 
 [Online] http://www.codeproject.com/bitmap/rotatebyshear.asp
 [3] Treskunov A., Fast and high quality true-color bitmap rotation function.
 [Online] http://anton.treskunov.net/Software/doc/fast_and_high_quality_true_color_bitmap_rotation_function.html
 ============================================================
*/

#include "FreeImage.h"
#include "Utilities.h"

#define ROTATE_PI	double (3.1415926535897932384626433832795)

#define RBLOCK		64	// image blocks of RBLOCK*RBLOCK pixels

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototypes definition

static void HorizontalSkew(FIBITMAP *src, FIBITMAP *dst, int row, int iOffset, double dWeight);
static void VerticalSkew(FIBITMAP *src, FIBITMAP *dst, int col, int iOffset, double dWeight);
static FIBITMAP* Rotate90(FIBITMAP *src);
static FIBITMAP* Rotate180(FIBITMAP *src);
static FIBITMAP* Rotate270(FIBITMAP *src);
static FIBITMAP* Rotate45(FIBITMAP *src, double dAngle);
static FIBITMAP* RotateAny(FIBITMAP *src, double dAngle);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Skews a row horizontally (with filtered weights). 
Limited to 45 degree skewing only. Filters two adjacent pixels.
@param src Pointer to source image to rotate
@param dst Pointer to destination image
@param row Row index
@param iOffset Skew offset
@param dWeight Relative weight of right pixel
*/
static void 
HorizontalSkew(FIBITMAP *src, FIBITMAP *dst, int row, int iOffset, BYTE Weight) {
	int i, j;
	int iXPos;

	int src_width  = FreeImage_GetWidth(src);
	int dst_width  = FreeImage_GetWidth(dst);

	switch(FreeImage_GetBPP(src)) {
		case 8:
		case 24:
		case 32:
		{
			BYTE pxlSrc[4], pxlLeft[4], pxlOldLeft[4];	// 4 = 32-bit max

			// calculate the number of bytes per pixel (1 for 8-bit, 3 for 24-bit or 4 for 32-bit)
			int bytespp = FreeImage_GetLine(src) / FreeImage_GetWidth(src);

			BYTE *src_bits = FreeImage_GetScanLine(src, row);
			BYTE *dst_bits = FreeImage_GetScanLine(dst, row);

			// fill gap left of skew with background
			if(iOffset > 0) {
				memset(dst_bits, 0, iOffset * bytespp);
			}
			memset(&pxlOldLeft[0], 0, bytespp);
			
			for(i = 0; i < src_width; i++) {
				// loop through row pixels
				memcpy(&pxlSrc[0], src_bits, bytespp);
				// calculate weights
				for(j = 0; j < bytespp; j++) {
					pxlLeft[j] = BYTE(((WORD)pxlSrc[j] * Weight) / 256);
				}
				// check boundaries 
				iXPos = i + iOffset;
				if((iXPos >= 0) && (iXPos < dst_width)) {
					// update left over on source
					for(j = 0; j < bytespp; j++) {
						pxlSrc[j] = pxlSrc[j] - (pxlLeft[j] - pxlOldLeft[j]);
					}
					memcpy(&dst_bits[iXPos*bytespp], &pxlSrc[0], bytespp);
				}
				// save leftover for next pixel in scan
				memcpy(&pxlOldLeft[0], &pxlLeft[0], bytespp);

				// next pixel in scan
				src_bits += bytespp;
			}			

			// go to rightmost point of skew
			iXPos = src_width + iOffset; 
			
			if(iXPos < dst_width) {
				dst_bits = FreeImage_GetScanLine(dst, row) + iXPos * bytespp;

				// If still in image bounds, put leftovers there
				memcpy(dst_bits, &pxlOldLeft[0], bytespp);

				// clear to the right of the skewed line with background
				dst_bits += bytespp;
				memset(dst_bits, 0, bytespp * (dst_width - iXPos - 1));
			}
		}
		break;

	}
}
/**
Skews a column vertically (with filtered weights). 
Limited to 45 degree skewing only. Filters two adjacent pixels.
@param src Pointer to source image to rotate
@param dst Pointer to destination image
@param col Column index
@param iOffset Skew offset
@param dWeight Relative weight of upper pixel
*/
static void 
VerticalSkew(FIBITMAP *src, FIBITMAP *dst, int col, int iOffset, BYTE Weight) {
	int i, j, iYPos;

	int src_height = FreeImage_GetHeight(src);
	int dst_height = FreeImage_GetHeight(dst);

	switch(FreeImage_GetBPP(src)) {
		case 8:
		case 24:
		case 32:
		{
			BYTE pxlSrc[4], pxlLeft[4], pxlOldLeft[4];	// 4 = 32-bit max

			// calculate the number of bytes per pixel (1 for 8-bit, 3 for 24-bit or 4 for 32-bit)
			int bytespp = FreeImage_GetLine(src) / FreeImage_GetWidth(src);

			unsigned src_pitch = FreeImage_GetPitch(src);
			unsigned dst_pitch = FreeImage_GetPitch(dst);
			unsigned index = col * bytespp;

			BYTE *src_bits = FreeImage_GetBits(src) + index;
			BYTE *dst_bits = FreeImage_GetBits(dst) + index;

			// fill gap above skew with background
			if(iOffset > 0) {
				for(i = 0; i < iOffset; i++) {
					memset(dst_bits, 0, bytespp);
					dst_bits += dst_pitch;
				}
			}
			memset(&pxlOldLeft[0], 0, bytespp);

			for(i = 0; i < src_height; i++) {
				// loop through column pixels
				memcpy(&pxlSrc[0], src_bits, bytespp);				
				// calculate weights
				for(j = 0; j < bytespp; j++) {
					pxlLeft[j] = BYTE(((WORD)pxlSrc[j] * Weight) / 256);
				}
				// check boundaries
				iYPos = i + iOffset;
				if((iYPos >= 0) && (iYPos < dst_height)) {
					// update left over on source
					for(j = 0; j < bytespp; j++) {
						pxlSrc[j] = pxlSrc[j] - (pxlLeft[j] - pxlOldLeft[j]);
					}
					dst_bits = FreeImage_GetScanLine(dst, iYPos) + index;
					memcpy(dst_bits, &pxlSrc[0], bytespp);
				}
				// save leftover for next pixel in scan
				memcpy(&pxlOldLeft[0], &pxlLeft[0], bytespp);

				// next pixel in scan
				src_bits += src_pitch;
			}
			// go to bottom point of skew
			iYPos = src_height + iOffset;

			if(iYPos < dst_height) {
				dst_bits = FreeImage_GetScanLine(dst, iYPos) + index;

				// if still in image bounds, put leftovers there				
				memcpy(dst_bits, &pxlOldLeft[0], bytespp);

				// clear below skewed line with background
				while(++iYPos < dst_height) {					
					dst_bits += dst_pitch;
					memset(dst_bits, 0, bytespp);
				}
			}
		}
		break;

	}
} 

/**
Rotates an image by 90 degrees (counter clockwise). 
Precise rotation, no filters required.<br>
Code adapted from CxImage (http://www.xdp.it/cximage.htm)
@param src Pointer to source image to rotate
@return Returns a pointer to a newly allocated rotated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
Rotate90(FIBITMAP *src) {
	int x, y, y2;

	int bpp = FreeImage_GetBPP(src);

	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);	
	int dst_width  = src_height;
	int dst_height = src_width;

	// allocate and clear dst image
	FIBITMAP *dst = FreeImage_Allocate(dst_width, dst_height, bpp);
	if(NULL == dst) return NULL;

	// get src and dst scan width
	int src_pitch  = FreeImage_GetPitch(src);
	int dst_pitch  = FreeImage_GetPitch(dst);

	if(bpp == 1) {
		// speedy rotate for BW images

		BYTE *sbits, *dbits, *dbitsmax, bitpos, *nrow, *srcdisp;
		div_t div_r;

		BYTE *bsrc  = FreeImage_GetBits(src); 
		BYTE *bdest = FreeImage_GetBits(dst);
		dbitsmax = bdest + dst_height * dst_pitch - 1;

		for(y = 0; y < src_height; y++) {
			// figure out the column we are going to be copying to
			div_r = div(y, 8);
			// set bit pos of src column byte
			bitpos = (BYTE)(128 >> div_r.rem);
			srcdisp = bsrc + y * src_pitch;
			for (x = 0; x < src_pitch; x++) {
				// get source bits
				sbits = srcdisp + x;
				// get destination column
				nrow = bdest + (dst_height - 1 - (x * 8)) * dst_pitch + div_r.quot;
				for (int z = 0; z < 8; z++) {
				   // get destination byte
					dbits = nrow - z * dst_pitch;
					if ((dbits < bdest) || (dbits > dbitsmax)) break;
					if (*sbits & (128 >> z)) *dbits |= bitpos;
				}
			}
		}
	}
	else if((bpp == 8) || (bpp == 24) || (bpp == 32)) {
		// anything other than BW :
		// This optimized version of rotation rotates image by smaller blocks. It is quite
		// a bit faster than obvious algorithm, because it produces much less CPU cache misses.
		// This optimization can be tuned by changing block size (RBLOCK). 96 is good value for current
		// CPUs (tested on Athlon XP and Celeron D). Larger value (if CPU has enough cache) will increase
		// speed somehow, but once you drop out of CPU's cache, things will slow down drastically.
		// For older CPUs with less cache, lower value would yield better results.

		int xs, ys;                            // x-segment and y-segment
		BYTE *bsrc  = FreeImage_GetBits(src);  // source pixels
		BYTE *bdest = FreeImage_GetBits(dst);  // destination pixels

		// calculate the number of bytes per pixel (1 for 8-bit, 3 for 24-bit or 4 for 32-bit)
		int bytespp = FreeImage_GetLine(src) / FreeImage_GetWidth(src);

		for(xs = 0; xs < dst_width; xs += RBLOCK) {    // for all image blocks of RBLOCK*RBLOCK pixels
			for(ys = 0; ys < dst_height; ys += RBLOCK) {
				for(y = ys; y < MIN(dst_height, ys + RBLOCK); y++) {    // do rotation
					y2 = dst_height - y - 1;
					// point to src pixel at (y2, xs)
					BYTE *src_bits = bsrc + (xs * src_pitch) + (y2 * bytespp);
					// point to dst pixel at (xs, y)
					BYTE *dst_bits = bdest + (y * dst_pitch) + (xs * bytespp);
					for (x = xs; x < MIN(dst_width, xs + RBLOCK); x++) {
						// dst.SetPixel(x, y, src.GetPixel(y2, x));
						for(int j = 0; j < bytespp; j++) {
							dst_bits[j] = src_bits[j];
						}
						dst_bits += bytespp;
						src_bits += src_pitch;
					}
				}
			}
		}
	}

	return dst;
}

/**
Rotates an image by 180 degrees (counter clockwise). 
Precise rotation, no filters required.
@param src Pointer to source image to rotate
@return Returns a pointer to a newly allocated rotated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
Rotate180(FIBITMAP *src) {
	int x, y, k, pos;

	int bpp = FreeImage_GetBPP(src);

	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);
	int dst_width  = src_width;
	int dst_height = src_height;

	FIBITMAP *dst = FreeImage_Allocate(dst_width, dst_height, bpp);
	if(NULL == dst) return NULL;

	if(bpp == 1) {
		for(int y = 0; y < src_height; y++) {
			BYTE *src_bits = FreeImage_GetScanLine(src, y);
			BYTE *dst_bits = FreeImage_GetScanLine(dst, dst_height - y - 1);
			for(int x = 0; x < src_width; x++) {
				// get bit at (x, y)
				k = (src_bits[x >> 3] & (0x80 >> (x & 0x07))) != 0;
				// set bit at (dst_width - x - 1, dst_height - y - 1)
				pos = dst_width - x - 1;
				k ? dst_bits[pos >> 3] |= (0x80 >> (pos & 0x7)) : dst_bits[pos >> 3] &= (0xFF7F >> (pos & 0x7));
			}
		}
	}
	else if((bpp == 8) || (bpp == 24) || (bpp == 32)) {
		 // Calculate the number of bytes per pixel (1 for 8-bit, 3 for 24-bit or 4 for 32-bit)
		int bytespp = FreeImage_GetLine(src) / FreeImage_GetWidth(src);

		for(y = 0; y < src_height; y++) {
			BYTE *src_bits = FreeImage_GetScanLine(src, y);
			BYTE *dst_bits = FreeImage_GetScanLine(dst, dst_height - y - 1) + (dst_width - 1) * bytespp;
			for(x = 0; x < src_width; x++) {
				// get pixel at (x, y)
				// set pixel at (dst_width - x - 1, dst_height - y - 1)
				for(k = 0; k < bytespp; k++) {
					dst_bits[k] = src_bits[k];
				}					
				src_bits += bytespp;
				dst_bits -= bytespp;
			}
		}
	}

	return dst;
}

/**
Rotates an image by 270 degrees (counter clockwise). 
Precise rotation, no filters required.<br>
Code adapted from CxImage (http://www.xdp.it/cximage.htm)
@param src Pointer to source image to rotate
@return Returns a pointer to a newly allocated rotated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
Rotate270(FIBITMAP *src) {
	int x, x2, y, dlineup;

	int bpp = FreeImage_GetBPP(src);

	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);	
	int dst_width  = src_height;
	int dst_height = src_width;

	// allocate and clear dst image
	FIBITMAP *dst = FreeImage_Allocate(dst_width, dst_height, bpp);
	if(NULL == dst) return NULL;

	// get src and dst scan width
	int src_pitch  = FreeImage_GetPitch(src);
	int dst_pitch  = FreeImage_GetPitch(dst);
	
	if(bpp == 1) {
		// speedy rotate for BW images
		
		BYTE *sbits, *dbits, *dbitsmax, bitpos, *nrow, *srcdisp;
		div_t div_r;

		BYTE *bsrc  = FreeImage_GetBits(src); 
		BYTE *bdest = FreeImage_GetBits(dst);
		dbitsmax = bdest + dst_height * dst_pitch - 1;
		dlineup = 8 * dst_pitch - dst_width;

		for(y = 0; y < src_height; y++) {
			// figure out the column we are going to be copying to
			div_r = div(y + dlineup, 8);
			// set bit pos of src column byte
			bitpos = (BYTE)(1 << div_r.rem);
			srcdisp = bsrc + y * src_pitch;
			for (x = 0; x < src_pitch; x++) {
				// get source bits
				sbits = srcdisp + x;
				// get destination column
				nrow = bdest + (x * 8) * dst_pitch + dst_pitch - 1 - div_r.quot;
				for (int z = 0; z < 8; z++) {
				   // get destination byte
					dbits = nrow + z * dst_pitch;
					if ((dbits < bdest) || (dbits > dbitsmax)) break;
					if (*sbits & (128 >> z)) *dbits |= bitpos;
				}
			}
		}

	} 
	else if((bpp == 8) || (bpp == 24) || (bpp == 32)) {
		// anything other than BW :
		// This optimized version of rotation rotates image by smaller blocks. It is quite
		// a bit faster than obvious algorithm, because it produces much less CPU cache misses.
		// This optimization can be tuned by changing block size (RBLOCK). 96 is good value for current
		// CPUs (tested on Athlon XP and Celeron D). Larger value (if CPU has enough cache) will increase
		// speed somehow, but once you drop out of CPU's cache, things will slow down drastically.
		// For older CPUs with less cache, lower value would yield better results.

		int xs, ys;                            // x-segment and y-segment
		BYTE *bsrc  = FreeImage_GetBits(src);  // source pixels
		BYTE *bdest = FreeImage_GetBits(dst);  // destination pixels

		// Calculate the number of bytes per pixel (1 for 8-bit, 3 for 24-bit or 4 for 32-bit)
		int bytespp = FreeImage_GetLine(src) / FreeImage_GetWidth(src);

		for(xs = 0; xs < dst_width; xs += RBLOCK) {    // for all image blocks of RBLOCK*RBLOCK pixels
			for(ys = 0; ys < dst_height; ys += RBLOCK) {
				for(x = xs; x < MIN(dst_width, xs + RBLOCK); x++) {    // do rotation
					x2 = dst_width - x - 1;
					// point to src pixel at (ys, x2)
					BYTE *src_bits = bsrc + (x2 * src_pitch) + (ys * bytespp);
					// point to dst pixel at (x, ys)
					BYTE *dst_bits = bdest + (ys * dst_pitch) + (x * bytespp);
					for (y = ys; y < MIN(dst_height, ys + RBLOCK); y++) {
						// dst.SetPixel(x, y, src.GetPixel(y, x2));
						for(int j = 0; j < bytespp; j++) {
							dst_bits[j] = src_bits[j];
						}
						src_bits += bytespp;
						dst_bits += dst_pitch;
					}
				}
			}
		}
	}

	return dst;
}

/**
Rotates an image by a given degree in range [-45 .. +45] (counter clockwise) 
using the 3-shear technique.
@param src Pointer to source image to rotate
@param dAngle Rotation angle
@return Returns a pointer to a newly allocated rotated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
Rotate45(FIBITMAP *src, double dAngle) {
	int u;

	int bpp = FreeImage_GetBPP(src);

	double dRadAngle = dAngle * ROTATE_PI / double(180); // Angle in radians
	double dSinE = sin(dRadAngle);
	double dTan = tan(dRadAngle / 2);

	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);

	// Calc first shear (horizontal) destination image dimensions 
	int width_1  = src_width + int((double)src_height * fabs(dTan) + 0.5);
	int height_1 = src_height; 

	/******* Perform 1st shear (horizontal) ******/

	// Allocate image for 1st shear
	FIBITMAP *dst1 = FreeImage_Allocate(width_1, height_1, bpp);
	if(NULL == dst1) {
		return NULL;
	}
	
	for(u = 0; u < height_1; u++) {  
		double dShear;

		if(dTan >= 0)	{
			// Positive angle
			dShear = (double(u) + 0.5) * dTan;
		}
		else {
			// Negative angle
			dShear = (double(int(u) - height_1) + 0.5) * dTan;
		}
		int iShear = int(floor(dShear));
		HorizontalSkew(src, dst1, u, iShear, BYTE(255 * (dShear - double(iShear)) + 1));
	}

	/******* Perform 2nd shear  (vertical) ******/

	// Calc 2nd shear (vertical) destination image dimensions
	int width_2  = width_1;
	int height_2 = int((double)src_width * fabs(dSinE) + (double)src_height * cos(dRadAngle) + 0.5) + 1;

	// Allocate image for 2nd shear
	FIBITMAP *dst2 = FreeImage_Allocate(width_2, height_2, bpp);
	if(NULL == dst2) {
		FreeImage_Unload(dst1);
		return NULL;
	}

	double dOffset;     // Variable skew offset
	if(dSinE > 0)	{   
		// Positive angle
		dOffset = double(src_width - 1) * dSinE;
	}
	else {
		// Negative angle
		dOffset = -dSinE * double (src_width - width_2);
	}

	for(u = 0; u < width_2; u++, dOffset -= dSinE) {
		int iShear = int(floor(dOffset));
		VerticalSkew(dst1, dst2, u, iShear, BYTE(255 * (dOffset - double(iShear)) + 1));
	}

	/******* Perform 3rd shear (horizontal) ******/

	// Free result of 1st shear
	FreeImage_Unload(dst1);

	// Calc 3rd shear (horizontal) destination image dimensions
	int width_3  = int(double(src_height) * fabs(dSinE) + double(src_width) * cos(dRadAngle) + 0.5) + 1;
	int height_3 = height_2;

	// Allocate image for 3rd shear
	FIBITMAP *dst3 = FreeImage_Allocate(width_3, height_3, bpp);
	if(NULL == dst3) {
		FreeImage_Unload(dst2);
		return NULL;
	}

	if(dSinE >= 0) {
		// Positive angle
		dOffset = double(src_width - 1) * dSinE * -dTan;
	}
	else {
		// Negative angle
		dOffset = dTan * (double(src_width - 1) * -dSinE + double(1 - height_3));
	}
	for(u = 0; u < height_3; u++, dOffset += dTan) {
		int iShear = int(floor(dOffset));
		HorizontalSkew(dst2, dst3, u, iShear, BYTE(255 * (dOffset - double (iShear)) + 1));
	}
	// Free result of 2nd shear    
	FreeImage_Unload(dst2);

	// Return result of 3rd shear
	return dst3;      
}

/**
Rotates a 1-, 8-, 24- or 32-bit image by a given angle (given in degree). 
Angle is unlimited, except for 1-bit images (limited to integer multiples of 90 degree). 
3-shears technique is used.
@param src Pointer to source image to rotate
@param dAngle Rotation angle
@return Returns a pointer to a newly allocated rotated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
RotateAny(FIBITMAP *src, double dAngle) {
	if(NULL == src) {
		return NULL;
	}

	FIBITMAP *image = src;

	while(dAngle >= 360) {
		// Bring angle to range of (-INF .. 360)
		dAngle -= 360;
	}
	while(dAngle < 0) {
		// Bring angle to range of [0 .. 360) 
		dAngle += 360;
	}
	if((dAngle > 45) && (dAngle <= 135)) {
		// Angle in (45 .. 135] 
		// Rotate image by 90 degrees into temporary image,
		// so it requires only an extra rotation angle 
		// of -45 .. +45 to complete rotation.
		image = Rotate90(src);
		dAngle -= 90;
	}
	else if((dAngle > 135) && (dAngle <= 225)) { 
		// Angle in (135 .. 225] 
		// Rotate image by 180 degrees into temporary image,
		// so it requires only an extra rotation angle 
		// of -45 .. +45 to complete rotation.
		image = Rotate180(src);
		dAngle -= 180;
	}
	else if((dAngle > 225) && (dAngle <= 315)) { 
		// Angle in (225 .. 315] 
		// Rotate image by 270 degrees into temporary image,
		// so it requires only an extra rotation angle 
		// of -45 .. +45 to complete rotation.
		image = Rotate270(src);
		dAngle -= 270;
	}

	// If we got here, angle is in (-45 .. +45]

	if(NULL == image)	{
		// Failed to allocate middle image
		return NULL;
	}

	if(0 == dAngle) {
		if(image == src) {
			// Nothing to do ...
			return FreeImage_Clone(src);
		} else {
			// No more rotation needed
			return image;
		}
	}
	else {
		// Perform last rotation
		FIBITMAP *dst = Rotate45(image, dAngle);

		if(src != image) {
			// Middle image was required, free it now.
			FreeImage_Unload(image);
		}

		return dst;
	}
}

// ==========================================================

FIBITMAP *DLL_CALLCONV 
FreeImage_RotateClassic(FIBITMAP *dib, double angle) {

	if(!dib) return NULL;	

	if(0 == angle) {
		return FreeImage_Clone(dib);
	}
	// DIB are stored upside down ...
	angle *= -1;

	try {

		int bpp = FreeImage_GetBPP(dib);

		if(bpp == 1) {
			// only rotate for integer multiples of 90 degree
			if(fmod(angle, 90) != 0)
				return NULL;

			// perform the rotation
			FIBITMAP *dst = RotateAny(dib, angle);
			if(!dst) throw(1);

			// build a greyscale palette
			RGBQUAD *dst_pal = FreeImage_GetPalette(dst);
			if(FreeImage_GetColorType(dib) == FIC_MINISBLACK) {
				dst_pal[0].rgbRed = dst_pal[0].rgbGreen = dst_pal[0].rgbBlue = 0;
				dst_pal[1].rgbRed = dst_pal[1].rgbGreen = dst_pal[1].rgbBlue = 255;			
			} else {
				dst_pal[0].rgbRed = dst_pal[0].rgbGreen = dst_pal[0].rgbBlue = 255;
				dst_pal[1].rgbRed = dst_pal[1].rgbGreen = dst_pal[1].rgbBlue = 0;			
			}

			// copy metadata from src to dst
			FreeImage_CloneMetadata(dst, dib);

			return dst;
		}

		if((bpp == 8) || (bpp == 24) || (bpp == 32)) {
			FIBITMAP *dst = RotateAny(dib, angle);
			if(!dst) throw(1);
			
			if(bpp == 8) {
				// copy original palette to rotated bitmap
				RGBQUAD *src_pal = FreeImage_GetPalette(dib);
				RGBQUAD *dst_pal = FreeImage_GetPalette(dst);
				memcpy(&dst_pal[0], &src_pal[0], 256 * sizeof(RGBQUAD));
			}

			// copy metadata from src to dst
			FreeImage_CloneMetadata(dst, dib);

			return dst;
		}

	} catch(int) {
		return NULL;
	}

	return NULL;
}

