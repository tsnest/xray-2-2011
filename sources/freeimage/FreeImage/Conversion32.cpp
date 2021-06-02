// ==========================================================
// Bitmap conversion routines
//
// Design and implementation by
// - Floris van den Berg (flvdberg@wxs.nl)
// - Herv� Drolon (drolon@infonie.fr)
// - Jani Kajala (janik@remedy.fi)
// - Detlev Vendt (detlev.vendt@brillit.de)
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

#include "FreeImage.h"
#include "Utilities.h"

// ----------------------------------------------------------
//  internal conversions X to 32 bits
// ----------------------------------------------------------

void DLL_CALLCONV
FreeImage_ConvertLine1To32(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette) {
	for (int cols = 0; cols < width_in_pixels; cols++) {
		int index = (source[cols>>3] & (0x80 >> (cols & 0x07))) != 0 ? 1 : 0;

		target[FI_RGBA_BLUE]	= palette[index].rgbBlue;
		target[FI_RGBA_GREEN]	= palette[index].rgbGreen;
		target[FI_RGBA_RED]		= palette[index].rgbRed;
		target[FI_RGBA_ALPHA]	= 0xFF;
		target += 4;
	}	
}

void DLL_CALLCONV
FreeImage_ConvertLine4To32(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette) {
	BOOL low_nibble = FALSE;
	int x = 0;

	for (int cols = 0 ; cols < width_in_pixels ; ++cols) {
		if (low_nibble) {
			target[FI_RGBA_BLUE]	= palette[LOWNIBBLE(source[x])].rgbBlue;
			target[FI_RGBA_GREEN]	= palette[LOWNIBBLE(source[x])].rgbGreen;
			target[FI_RGBA_RED]		= palette[LOWNIBBLE(source[x])].rgbRed;

			x++;
		} else {
			target[FI_RGBA_BLUE]	= palette[HINIBBLE(source[x]) >> 4].rgbBlue;
			target[FI_RGBA_GREEN]	= palette[HINIBBLE(source[x]) >> 4].rgbGreen;
			target[FI_RGBA_RED]		= palette[HINIBBLE(source[x]) >> 4].rgbRed;
		}

		low_nibble = !low_nibble;

		target[FI_RGBA_ALPHA] = 0xFF;
		target += 4;
	}
}

void DLL_CALLCONV
FreeImage_ConvertLine8To32(BYTE *target, BYTE *source, int width_in_pixels, RGBQUAD *palette) {
	for (int cols = 0; cols < width_in_pixels; cols++) {
		target[FI_RGBA_BLUE]	= palette[source[cols]].rgbBlue;
		target[FI_RGBA_GREEN]	= palette[source[cols]].rgbGreen;
		target[FI_RGBA_RED]		= palette[source[cols]].rgbRed;
		target[FI_RGBA_ALPHA]	= 0xFF;
		target += 4;
	}
}

void DLL_CALLCONV
FreeImage_ConvertLine16To32_555(BYTE *target, BYTE *source, int width_in_pixels) {
	WORD *bits = (WORD *)source;

	for (int cols = 0; cols < width_in_pixels; cols++) {
		target[FI_RGBA_RED]   = (BYTE)((((bits[cols] & FI16_555_RED_MASK) >> FI16_555_RED_SHIFT) * 0xFF) / 0x1F);
		target[FI_RGBA_GREEN] = (BYTE)((((bits[cols] & FI16_555_GREEN_MASK) >> FI16_555_GREEN_SHIFT) * 0xFF) / 0x1F);
		target[FI_RGBA_BLUE]  = (BYTE)((((bits[cols] & FI16_555_BLUE_MASK) >> FI16_555_BLUE_SHIFT) * 0xFF) / 0x1F);
		target[FI_RGBA_ALPHA] = 0xFF;
		target += 4;
	}
}

void DLL_CALLCONV
FreeImage_ConvertLine16To32_565(BYTE *target, BYTE *source, int width_in_pixels) {
	WORD *bits = (WORD *)source;

	for (int cols = 0; cols < width_in_pixels; cols++) {
		target[FI_RGBA_RED]   = (BYTE)((((bits[cols] & FI16_565_RED_MASK) >> FI16_565_RED_SHIFT) * 0xFF) / 0x1F);
		target[FI_RGBA_GREEN] = (BYTE)((((bits[cols] & FI16_565_GREEN_MASK) >> FI16_565_GREEN_SHIFT) * 0xFF) / 0x3F);
		target[FI_RGBA_BLUE]  = (BYTE)((((bits[cols] & FI16_565_BLUE_MASK) >> FI16_565_BLUE_SHIFT) * 0xFF) / 0x1F);
		target[FI_RGBA_ALPHA] = 0xFF;
		target += 4;
	}
}
/*
void DLL_CALLCONV
FreeImage_ConvertLine24To32(BYTE *target, BYTE *source, int width_in_pixels) {
	for (int cols = 0; cols < width_in_pixels; cols++) {
		*(DWORD *)target = (*(DWORD *) source & FI_RGBA_RGB_MASK) | FI_RGBA_ALPHA_MASK;
		target += 4;
		source += 3;
	}
}
*/
/**
This unoptimized version of the conversion function avoid an undetermined bug with VC++ SP6. 
The bug occurs in release mode only, when the image height is equal to 537 
(try e.g. a size of 432x537 to reproduce the bug with the optimized function).
*/
void DLL_CALLCONV
FreeImage_ConvertLine24To32(BYTE *target, BYTE *source, int width_in_pixels) {
	for (int cols = 0; cols < width_in_pixels; cols++) {
		target[FI_RGBA_RED]   = source[FI_RGBA_RED];
		target[FI_RGBA_GREEN] = source[FI_RGBA_GREEN];
		target[FI_RGBA_BLUE]  = source[FI_RGBA_BLUE];
		target[FI_RGBA_ALPHA] = 0xFF;
		target += 4;
		source += 3;
	}
}
// ----------------------------------------------------------

static void DLL_CALLCONV
MapTransparentTableToAlpha1(RGBQUAD *target, BYTE *source, BYTE *table, int transparent_pixels, int width_in_pixels) {
	for (int cols = 0 ; cols < width_in_pixels ; cols++) {
		int index = (source[cols>>3] & (0x80 >> (cols & 0x07))) != 0 ? 1 : 0;
		target[cols].rgbReserved = (index < transparent_pixels) ? table[index] : 255;
    }
}

static void DLL_CALLCONV
MapTransparentTableToAlpha4(RGBQUAD *target, BYTE *source, BYTE *table, int transparent_pixels, int width_in_pixels) {
    BOOL low_nibble = FALSE;
    int x = 0;

    for (int cols = 0 ; cols < width_in_pixels ; ++cols) {
        int index;
        if (low_nibble) {
            index = LOWNIBBLE(source[x]);
            x++;
        } else {
            index = HINIBBLE(source[x]) >> 4;
        }
        target[cols].rgbReserved = (index < transparent_pixels) ? table[index] : 255;
        low_nibble = !low_nibble;
    }
}

static void DLL_CALLCONV
MapTransparentTableToAlpha8(RGBQUAD *target, BYTE *source, BYTE *table, int transparent_pixels, int width_in_pixels) {
    for (int cols = 0; cols < width_in_pixels; cols++) {
		target[cols].rgbReserved = (source[cols] < transparent_pixels) ? table[source[cols]] : 255;
	}
} 

// ----------------------------------------------------------

FIBITMAP * DLL_CALLCONV
FreeImage_ConvertTo32Bits(FIBITMAP *dib) {
	if(!dib) return NULL;

	const int bpp = FreeImage_GetBPP(dib);
	const FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(dib);
	
	if((image_type != FIT_BITMAP) && (image_type != FIT_RGB16) && (image_type != FIT_RGBA16)) {
		return NULL;
	}
	
	const int width = FreeImage_GetWidth(dib);
	const int height = FreeImage_GetHeight(dib);

	if(image_type == FIT_BITMAP) {

		if(bpp == 32) {
			return FreeImage_Clone(dib);
		}

		FIBITMAP *new_dib = FreeImage_Allocate(width, height, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
		if(new_dib == NULL) {
			return NULL;
		}

		// copy metadata from src to dst
		FreeImage_CloneMetadata(new_dib, dib);

		switch(bpp) {
			case 1:
			{
				for (int rows = 0; rows < height; rows++) {
					FreeImage_ConvertLine1To32(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width, FreeImage_GetPalette(dib));
					
					if (FreeImage_IsTransparent(dib)) {
						MapTransparentTableToAlpha1((RGBQUAD *)FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), FreeImage_GetTransparencyTable(dib), FreeImage_GetTransparencyCount(dib), width);
					}
				}

				return new_dib;
			}

			case 4:
			{
				for (int rows = 0; rows < height; rows++) {
					FreeImage_ConvertLine4To32(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width, FreeImage_GetPalette(dib));
			
					if (FreeImage_IsTransparent(dib)) {
						MapTransparentTableToAlpha4((RGBQUAD *)FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), FreeImage_GetTransparencyTable(dib), FreeImage_GetTransparencyCount(dib), width);
					}
				}

				return new_dib;
			}
				
			case 8:
			{
				for (int rows = 0; rows < height; rows++) {
					FreeImage_ConvertLine8To32(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width, FreeImage_GetPalette(dib));

					if (FreeImage_IsTransparent(dib)) {
						MapTransparentTableToAlpha8((RGBQUAD *)FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), FreeImage_GetTransparencyTable(dib), FreeImage_GetTransparencyCount(dib), width);
					}
				}

				return new_dib;
			}

			case 16:
			{
				for (int rows = 0; rows < height; rows++) {
					if ((FreeImage_GetRedMask(dib) == FI16_565_RED_MASK) && (FreeImage_GetGreenMask(dib) == FI16_565_GREEN_MASK) && (FreeImage_GetBlueMask(dib) == FI16_565_BLUE_MASK)) {
						FreeImage_ConvertLine16To32_565(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width);
					} else {
						// includes case where all the masks are 0
						FreeImage_ConvertLine16To32_555(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width);
					}
				}

				return new_dib;
			}

			case 24:
			{
				for (int rows = 0; rows < height; rows++) {
					FreeImage_ConvertLine24To32(FreeImage_GetScanLine(new_dib, rows), FreeImage_GetScanLine(dib, rows), width);
				}

				return new_dib;
			}
		}

	} else if(image_type == FIT_RGB16) {
		FIBITMAP *new_dib = FreeImage_Allocate(width, height, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
		if(new_dib == NULL) {
			return NULL;
		}

		// copy metadata from src to dst
		FreeImage_CloneMetadata(new_dib, dib);

		const unsigned src_pitch = FreeImage_GetPitch(dib);
		const unsigned dst_pitch = FreeImage_GetPitch(new_dib);
		const BYTE *src_bits = FreeImage_GetBits(dib);
		BYTE *dst_bits = FreeImage_GetBits(new_dib);
		for (int rows = 0; rows < height; rows++) {
			const FIRGB16 *src_pixel = (FIRGB16*)src_bits;
			RGBQUAD *dst_pixel = (RGBQUAD*)dst_bits;
			for(int cols = 0; cols < width; cols++) {
				dst_pixel[cols].rgbRed		= (BYTE)(src_pixel[cols].red   >> 8);
				dst_pixel[cols].rgbGreen	= (BYTE)(src_pixel[cols].green >> 8);
				dst_pixel[cols].rgbBlue		= (BYTE)(src_pixel[cols].blue  >> 8);
				dst_pixel[cols].rgbReserved = (BYTE)0xFF;
			}
			src_bits += src_pitch;
			dst_bits += dst_pitch;
		}

		return new_dib;

	} else if(image_type == FIT_RGBA16) {
		FIBITMAP *new_dib = FreeImage_Allocate(width, height, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
		if(new_dib == NULL) {
			return NULL;
		}

		// copy metadata from src to dst
		FreeImage_CloneMetadata(new_dib, dib);

		const unsigned src_pitch = FreeImage_GetPitch(dib);
		const unsigned dst_pitch = FreeImage_GetPitch(new_dib);
		const BYTE *src_bits = FreeImage_GetBits(dib);
		BYTE *dst_bits = FreeImage_GetBits(new_dib);
		for (int rows = 0; rows < height; rows++) {
			const FIRGBA16 *src_pixel = (FIRGBA16*)src_bits;
			RGBQUAD *dst_pixel = (RGBQUAD*)dst_bits;
			for(int cols = 0; cols < width; cols++) {
				dst_pixel[cols].rgbRed		= (BYTE)(src_pixel[cols].red   >> 8);
				dst_pixel[cols].rgbGreen	= (BYTE)(src_pixel[cols].green >> 8);
				dst_pixel[cols].rgbBlue		= (BYTE)(src_pixel[cols].blue  >> 8);
				dst_pixel[cols].rgbReserved = (BYTE)(src_pixel[cols].alpha >> 8);
			}
			src_bits += src_pitch;
			dst_bits += dst_pitch;
		}		

		return new_dib;
	}
	
	return NULL;
}

static void get_min_max_components(FIBITMAP *dib, 
								   WORD& out_min_r, WORD& out_min_g, WORD& out_min_b, WORD& out_min_a, 
								   WORD& out_max_r, WORD& out_max_g, WORD& out_max_b, WORD& out_max_a)
{
	out_min_r = out_min_g = out_min_b = out_min_a = (WORD)-1;
	out_max_r = out_max_g = out_max_b = out_max_a = 0;
	
	const int width = FreeImage_GetWidth(dib);
	const int height = FreeImage_GetHeight(dib);
	const FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(dib);
	
	const unsigned src_pitch = FreeImage_GetPitch(dib);
	
	const int bpp = FreeImage_GetBPP(dib);
	
	const BYTE *src_bits = FreeImage_GetBits(dib);
	
	for (int rows = 0; rows < height; rows++) 
	{
		const RGBTRIPLE*	src_pixel_rgb8  = (RGBTRIPLE*)src_bits;
		const RGBQUAD*		src_pixel_rgba8	= (RGBQUAD*)src_bits;
		const FIRGB16*		src_pixel_rgb   = (FIRGB16*)src_bits;
		const FIRGBA16*		src_pixel_rgba  = (FIRGBA16*)src_bits;
		
		for(int cols = 0; cols < width; cols++) 
		{
			WORD r = 0xFFFF, g = 0xFFFF, b = 0xFFFF, a = 0xFFFF;
			if (image_type == FIT_RGB16)
			{
				r = src_pixel_rgb[cols].red;
				g = src_pixel_rgb[cols].green;
				b = src_pixel_rgb[cols].blue;
				a = 0xFFFF;
			}
			else if (image_type == FIT_RGBA16)
			{
				r = src_pixel_rgba[cols].red;
				g = src_pixel_rgba[cols].green;
				b = src_pixel_rgba[cols].blue;
				a = src_pixel_rgba[cols].alpha;
			}
			else if (image_type == FIT_BITMAP)
			{
				if (bpp==24)
				{
					r = src_pixel_rgb8[cols].rgbtRed;
					g = src_pixel_rgb8[cols].rgbtGreen;
					b = src_pixel_rgb8[cols].rgbtBlue;
				}
				else if (bpp==32)
				{
					r = src_pixel_rgba8[cols].rgbRed;
					g = src_pixel_rgba8[cols].rgbGreen;
					b = src_pixel_rgba8[cols].rgbBlue;
					a = src_pixel_rgba8[cols].rgbReserved;
				}				
			}
			
			if (r > out_max_r) out_max_r = r;
			if (g > out_max_g) out_max_g = g;
			if (b > out_max_b) out_max_b = b;
			if (a > out_max_a) out_max_a = a;
			
			if (r < out_min_r) out_min_r = r;
			if (g < out_min_g) out_min_g = g;
			if (b < out_min_b) out_min_b = b;
			if (a < out_min_a) out_min_a = a;
		}
		src_bits += src_pitch;
	}
}

void DLL_CALLCONV
FreeImage_Normalize(FIBITMAP *dib, 
					WORD& out_min_r, WORD& out_min_g, WORD& out_min_b, WORD& out_min_a, 
					WORD& out_max_r, WORD& out_max_g, WORD& out_max_b, WORD& out_max_a)
{
	if(!dib) return;
	
	const FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(dib);
	
	const int bpp = FreeImage_GetBPP(dib);
	
	const int width = FreeImage_GetWidth(dib);
	const int height = FreeImage_GetHeight(dib);
	
	get_min_max_components(dib, out_min_r, out_min_g, out_min_b, out_min_a, out_max_r, out_max_g, out_max_b, out_max_a);
	
	const unsigned src_pitch = FreeImage_GetPitch(dib);
	
	BYTE *src_bits = FreeImage_GetBits(dib);
	
	for (int rows = 0; rows < height; rows++) 
	{
		RGBTRIPLE*	src_pixel_rgb8  = (RGBTRIPLE*)src_bits;
		RGBQUAD*	src_pixel_rgba8	= (RGBQUAD*)src_bits;
		FIRGB16*	src_pixel_rgb	= (FIRGB16*)src_bits;
		FIRGBA16*	src_pixel_rgba	= (FIRGBA16*)src_bits;
		
		for(int cols = 0; cols < width; cols++) 
		{
			switch(image_type)
			{
				case FIT_RGB16:
					src_pixel_rgb[cols].red   = WORD((float(src_pixel_rgb[cols].red   - out_min_r) / float(out_max_r - out_min_r)) * ((WORD)-1));
					src_pixel_rgb[cols].green = WORD((float(src_pixel_rgb[cols].green - out_min_g) / float(out_max_g - out_min_g)) * ((WORD)-1));
					src_pixel_rgb[cols].blue  = WORD((float(src_pixel_rgb[cols].blue  - out_min_b) / float(out_max_b - out_min_b)) * ((WORD)-1));
				break;
				case FIT_RGBA16:
					src_pixel_rgba[cols].red   = WORD((float(src_pixel_rgba[cols].red   - out_min_r) / float(out_max_r - out_min_r)) * ((WORD)-1));
					src_pixel_rgba[cols].green = WORD((float(src_pixel_rgba[cols].green - out_min_g) / float(out_max_g - out_min_g)) * ((WORD)-1));
					src_pixel_rgba[cols].blue  = WORD((float(src_pixel_rgba[cols].blue  - out_min_b) / float(out_max_b - out_min_b)) * ((WORD)-1));
					src_pixel_rgba[cols].alpha = WORD((float(src_pixel_rgba[cols].alpha - out_min_a) / float(out_max_a - out_min_a)) * ((WORD)-1));
				break;
				case FIT_BITMAP:
					if (bpp==24)
					{
						src_pixel_rgb8[cols].rgbtRed   = BYTE((float(src_pixel_rgb8[cols].rgbtRed   - out_min_r) / float(out_max_r - out_min_r)) * ((BYTE)-1));
						src_pixel_rgb8[cols].rgbtGreen = BYTE((float(src_pixel_rgb8[cols].rgbtGreen - out_min_g) / float(out_max_g - out_min_g)) * ((BYTE)-1));
						src_pixel_rgb8[cols].rgbtBlue  = BYTE((float(src_pixel_rgb8[cols].rgbtBlue  - out_min_b) / float(out_max_b - out_min_b)) * ((BYTE)-1));
					}
					else if (bpp==32)
					{
						src_pixel_rgba8[cols].rgbRed    = BYTE((float(src_pixel_rgba8[cols].rgbRed   - out_min_r) / float(out_max_r - out_min_r)) * ((BYTE)-1));
						src_pixel_rgba8[cols].rgbGreen  = BYTE((float(src_pixel_rgba8[cols].rgbGreen - out_min_g) / float(out_max_g - out_min_g)) * ((BYTE)-1));
						src_pixel_rgba8[cols].rgbBlue   = BYTE((float(src_pixel_rgba8[cols].rgbBlue  - out_min_b) / float(out_max_b - out_min_b)) * ((BYTE)-1));
						src_pixel_rgba8[cols].rgbReserved  = BYTE((float(src_pixel_rgba8[cols].rgbReserved - out_min_a) / float(out_max_a - out_min_a)) * ((BYTE)-1));
					}
				break;
			}
		}
		src_bits += src_pitch;
	}
}
