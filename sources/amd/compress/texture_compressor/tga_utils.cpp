#include "pch.h"

#include "utils.h"

u32* xray::texture_compressor::load_tga	( malloc_function_ptr_type allocator, free_function_ptr_type deallocator, u8 const* buffer, ATI_TC_Texture& texture )
{
	TgaHeader* const tga	= (TgaHeader*)buffer;
	buffer					+= TgaHeader::Size + tga->id_length;

	texture.dwSize			= sizeof(texture);
	texture.dwWidth			= tga->width;
	texture.dwHeight		= tga->height;
	texture.dwPitch			= 0;
	texture.format			= ATI_TC_FORMAT_ARGB_8888;

	// Get header info.
	bool rle = false;
	bool pal = false;
	bool rgb = false;
	bool grey = false;

	switch( tga->image_type ) {
		case TGA_TYPE_RLE_INDEXED:
			rle = true;
			// no break is intended!
		case TGA_TYPE_INDEXED:
			if( tga->colormap_type!=1 || tga->colormap_size!=24 || tga->colormap_length>256 ) {
				return false;
			}
			pal = true;
			break;

		case TGA_TYPE_RLE_RGB:
			rle = true;
			// no break is intended!
		case TGA_TYPE_RGB:
			rgb = true;
			break;

		case TGA_TYPE_RLE_GREY:
			rle = true;
			// no break is intended!
		case TGA_TYPE_GREY:
			grey = true;
			break;

		default:
			return false;
	}

	const uint pixel_size = (tga->pixel_size/8);
	//assert(pixel_size <= 4);
	
	const uint size = tga->width * tga->height * pixel_size;
	
	// Read palette
	u8 const* palette = 0;
	if( pal ) {
		//assert(tga->colormap_length < 256);
		palette	= buffer;
		buffer	+= 3 * tga->colormap_length;
	}

	// Decode image.
	u8 const* mem = 0;
	u8* mem_temp = 0;
	if( rle ) {
		mem_temp = (u8*)allocator(size*sizeof(u8), true);
		mem = mem_temp;
		// Decompress image in src.
		u8* dst = mem_temp;
		int num = size;

		while (num > 0) {
			// Get packet header
			u8 c = *buffer; 
			++buffer;

			uint count = (c & 0x7f) + 1;
			num -= count * pixel_size;

			if (c & 0x80) {
				// RLE pixels.
				u8 const* pixel = buffer;
				buffer	+= pixel_size;
				do {
					memcpy(dst, pixel, pixel_size);
					dst += pixel_size;
				} while (--count);
			}
			else {
				// Raw pixels.
				count *= pixel_size;
				//file->Read8(dst, count);
				memcpy(dst, buffer, count);
				buffer	+= count;
				dst += count;
			}
		}
	}
	else {
		mem	= buffer;
	}

	// Allocate image.
	u32 const data_size		= tga->width*tga->height*sizeof(u32);
	texture.dwDataSize		= data_size;
	texture.pData			= (ATI_TC_BYTE*)(*allocator)(data_size, false);

	u32* pixels				= (u32*)texture.pData;
	int lstep;
	u32 * dst;
	if( tga->flags & TGA_ORIGIN_UPPER ) {
		lstep = tga->width;
		dst = pixels;
	}
	else {
		lstep = - tga->width;
		dst = pixels + (tga->height-1) * tga->width;
	}

	// Write image.
	u8 const* src = mem;
	if( pal ) {
		for( int y = 0; y < tga->height; y++ ) {
			for( int x = 0; x < tga->width; x++ ) {
				u8 idx = *src++;
				setBGRA(dst[x],palette[3*idx+0], palette[3*idx+1], palette[3*idx+2], 0xFF);
			}
			dst += lstep;
		}
	}
	else if( grey ) {
		for( int y = 0; y < tga->height; y++ ) {
			for( int x = 0; x < tga->width; x++ ) {
				setBGRA(dst[x], *src, *src, *src, *src);
				src++;
			}
			dst += lstep;
		}
	}
	else {
		if( tga->pixel_size == 16 ) {
			for( int y = 0; y < tga->height; y++ ) {
				for( int x = 0; x < tga->width; x++ ) {
					Color555 c = *reinterpret_cast<Color555 const*>(src);
					u8 b = u8((c.b << 3) | (c.b >> 2));
					u8 g = u8((c.g << 3) | (c.g >> 2));
					u8 r = u8((c.r << 3) | (c.r >> 2));
					setBGRA(dst[x], b, g, r, 0xFF);
					src += 2;
				}
				dst += lstep;
			}
		}
		else if( tga->pixel_size == 24 ) {
			for( int y = 0; y < tga->height; y++ ) {
				for( int x = 0; x < tga->width; x++ ) {
					setBGRA(dst[x], src[0], src[1], src[2], 0xFF);
					src += 3;
				}
				dst += lstep;
			}
		}
		else if( tga->pixel_size == 32 ) {
			for( int y = 0; y < tga->height; y++ ) {
				for( int x = 0; x < tga->width; x++ ) {
					setBGRA(dst[x], src[0], src[1], src[2], src[3]);
					src += 4;
				}
				dst += lstep;
			}
		}
	}

	// free uncompressed data.
	if ( mem_temp ) {
		(*deallocator)	( mem_temp );
	}

	return			pixels;
}
