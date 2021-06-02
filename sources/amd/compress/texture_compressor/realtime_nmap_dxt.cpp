#include "pch.h"

/*
Real-Time Normal Map Compression (SSE2)
Copyright (C) 2008 Id Software, Inc.
Written by J.M.P. van Waveren
This code is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.
*/
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

#define INSET_COLOR_SHIFT 4 // inset color channel
#define INSET_ALPHA_SHIFT 5 // inset alpha channel
#define C565_5_MASK 0xF8 // 0xFF minus last three bits
#define C565_6_MASK 0xFC // 0xFF minus last two bits
byte *globalOutData;


void EmitByte( byte b ) 
{
	globalOutData[0] = b;
	globalOutData += 1;
}

void EmitWord( word s ) 
{
	globalOutData[0] = ( s >> 0 ) & 255;
	globalOutData[1] = ( s >> 8 ) & 255;
	globalOutData += 2;
}

void EmitUShort( word s ) 
{
	EmitWord(s);
}

void EmitDoubleWord( dword i ) 
{
	globalOutData[0] = ( i >> 0 ) & 255;
	globalOutData[1] = ( i >> 8 ) & 255;
	globalOutData[2] = ( i >> 16 ) & 255;
	globalOutData[3] = ( i >> 24 ) & 255;
	globalOutData += 4;
}
void EmitUInt( dword i ) 
{
	EmitDoubleWord(i);
}

word NormalYTo565( byte y ) 
{
	return ( ( y >> 2 ) << 5 );
}

void ExtractBlock( const byte *inPtr, const int width, byte *block ) 
{
	for ( int j = 0; j < 4; j++ ) 
	{
		memcpy( &block[j*4*4], inPtr, 4*4 );
		inPtr += width * 4;
	}
}

void GetMinMaxNormalsBBox( const byte *block, byte *minNormal, byte *maxNormal ) 
{
	minNormal[0] = minNormal[1] = 255;
	maxNormal[0] = maxNormal[1] = 0;
	for ( int i = 0; i < 16; i++ ) 
	{
		if ( block[i*4+0] < minNormal[0] ) 
		{
			minNormal[0] = block[i*4+0];
		}

		if ( block[i*4+1] < minNormal[1] ) 
		{
			minNormal[1] = block[i*4+1];
		}

		if ( block[i*4+0] > maxNormal[0] ) 
		{
			maxNormal[0] = block[i*4+0];
		}

		if ( block[i*4+1] > maxNormal[1] ) 
		{
			maxNormal[1] = block[i*4+1];
		}
	}
}
void InsetNormalsBBoxDXT5( byte *minNormal, byte *maxNormal ) 
{
	int inset[4];
	int mini[4];
	int maxi[4];
	inset[0] = ( maxNormal[0] - minNormal[0] ) - ((1<<(INSET_ALPHA_SHIFT-1))-1);
	inset[1] = ( maxNormal[1] - minNormal[1] ) - ((1<<(INSET_COLOR_SHIFT-1))-1);
	mini[0] = ( ( minNormal[0] << INSET_ALPHA_SHIFT ) + inset[0] ) >> INSET_ALPHA_SHIFT;
	mini[1] = ( ( minNormal[1] << INSET_COLOR_SHIFT ) + inset[1] ) >> INSET_COLOR_SHIFT;
	maxi[0] = ( ( maxNormal[0] << INSET_ALPHA_SHIFT ) - inset[0] ) >> INSET_ALPHA_SHIFT;
	maxi[1] = ( ( maxNormal[1] << INSET_COLOR_SHIFT ) - inset[1] ) >> INSET_COLOR_SHIFT;
	mini[0] = ( mini[0] >= 0 ) ? mini[0] : 0;
	mini[1] = ( mini[1] >= 0 ) ? mini[1] : 0;
	maxi[0] = ( maxi[0] <= 255 ) ? maxi[0] : 255;
	maxi[1] = ( maxi[1] <= 255 ) ? maxi[1] : 255;
	minNormal[0] = mini[0];
	minNormal[1] = ( mini[1] & C565_6_MASK ) | ( mini[1] >> 6 );
	maxNormal[0] = maxi[0];
	maxNormal[1] = ( maxi[1] & C565_6_MASK ) | ( maxi[1] >> 6 );
}

void InsetNormalsBBox3Dc( byte *minNormal, byte *maxNormal ) 
{
	int inset[4];
	int mini[4];
	int maxi[4];
	inset[0] = ( maxNormal[0] - minNormal[0] ) - ((1<<(INSET_ALPHA_SHIFT-1))-1);
	inset[1] = ( maxNormal[1] - minNormal[1] ) - ((1<<(INSET_ALPHA_SHIFT-1))-1);
	mini[0] = ( ( minNormal[0] << INSET_ALPHA_SHIFT ) + inset[0] ) >> INSET_ALPHA_SHIFT;
	mini[1] = ( ( minNormal[1] << INSET_ALPHA_SHIFT ) + inset[1] ) >> INSET_ALPHA_SHIFT;
	maxi[0] = ( ( maxNormal[0] << INSET_ALPHA_SHIFT ) - inset[0] ) >> INSET_ALPHA_SHIFT;
	maxi[1] = ( ( maxNormal[1] << INSET_ALPHA_SHIFT ) - inset[1] ) >> INSET_ALPHA_SHIFT;
	mini[0] = ( mini[0] >= 0 ) ? mini[0] : 0;
	mini[1] = ( mini[1] >= 0 ) ? mini[1] : 0;
	maxi[0] = ( maxi[0] <= 255 ) ? maxi[0] : 255;
	maxi[1] = ( maxi[1] <= 255 ) ? maxi[1] : 255;
	minNormal[0] = mini[0];
	minNormal[1] = mini[1];
	maxNormal[0] = maxi[0];
	maxNormal[1] = maxi[1];
}

void EmitAlphaIndices( const byte *block, const int offset, const byte minAlpha, const byte maxAlpha ) 
{
	byte mid = ( maxAlpha - minAlpha ) / ( 2 * 7 );
	byte ab1 = maxAlpha - mid;
	byte ab2 = ( 6 * maxAlpha + 1 * minAlpha ) / 7 - mid;
	byte ab3 = ( 5 * maxAlpha + 2 * minAlpha ) / 7 - mid;
	byte ab4 = ( 4 * maxAlpha + 3 * minAlpha ) / 7 - mid;
	byte ab5 = ( 3 * maxAlpha + 4 * minAlpha ) / 7 - mid;
	byte ab6 = ( 2 * maxAlpha + 5 * minAlpha ) / 7 - mid;
	byte ab7 = ( 1 * maxAlpha + 6 * minAlpha ) / 7 - mid;
	block += offset;
	byte indices[16];

	for ( int i = 0; i < 16; i++ ) 
	{
		byte a = block[i*4];
		int b1 = ( a >= ab1 );
		int b2 = ( a >= ab2 );
		int b3 = ( a >= ab3 );
		int b4 = ( a >= ab4 );
		int b5 = ( a >= ab5 );
		int b6 = ( a >= ab6 );
		int b7 = ( a >= ab7 );
		int index = ( 8 - b1 - b2 - b3 - b4 - b5 - b6 - b7 ) & 7;
		indices[i] = index ^ ( 2 > index );
	}

	EmitByte( (indices[ 0] >> 0) | (indices[ 1] << 3) | (indices[ 2] << 6) );
	EmitByte( (indices[ 2] >> 2) | (indices[ 3] << 1) | (indices[ 4] << 4) | (indices[ 5] << 7) );
	EmitByte( (indices[ 5] >> 1) | (indices[ 6] << 2) | (indices[ 7] << 5) );
	EmitByte( (indices[ 8] >> 0) | (indices[ 9] << 3) | (indices[10] << 6) );
	EmitByte( (indices[10] >> 2) | (indices[11] << 1) | (indices[12] << 4) | (indices[13] << 7) );
	EmitByte( (indices[13] >> 1) | (indices[14] << 2) | (indices[15] << 5) );
}

void EmitGreenIndices( const byte *block, const int offset, const byte minGreen, const byte maxGreen ) 
{
	byte mid = ( maxGreen - minGreen ) / ( 2 * 3 );
	byte gb1 = maxGreen - mid;
	byte gb2 = ( 2 * maxGreen + 1 * minGreen ) / 3 - mid;
	byte gb3 = ( 1 * maxGreen + 2 * minGreen ) / 3 - mid;
	block += offset;
	unsigned int result = 0;
	for ( int i = 15; i >= 0; i-- ) 
	{
		result <<= 2;
		byte g = block[i*4];
		int b1 = ( g >= gb1 );
		int b2 = ( g >= gb2 );
		int b3 = ( g >= gb3 );
		int index = ( 4 - b1 - b2 - b3 ) & 3;
		index ^= ( 2 > index );
		result |= index;
	}
	EmitUInt( result );
}

void CompressNormalMapDXT5( const byte *inBuf, byte *outBuf, int width, int height, int& outputBytes ) 
{
	byte block[64];
	byte normalMin[4];
	byte normalMax[4];
	globalOutData = outBuf;
	for ( int j = 0; j < height; j += 4, inBuf += width * 4*4 ) 
	{
		for ( int i = 0; i < width; i += 4 ) 
		{
			ExtractBlock( inBuf + i * 4, width, block );
			GetMinMaxNormalsBBox( block, normalMin, normalMax );
			InsetNormalsBBoxDXT5( normalMin, normalMax );
			// Write out Nx into alpha channel.
			EmitByte( normalMax[0] );
			EmitByte( normalMin[0] );
			EmitAlphaIndices( block, 0, normalMin[0], normalMax[0] );
			// Write out Ny into green channel.
			EmitUShort( NormalYTo565( normalMax[1] ) );
			EmitUShort( NormalYTo565( normalMin[1] ) );
			EmitGreenIndices( block, 1, normalMin[1], normalMax[1] );
		}
	}
	//.outputBytes = outData - outBuf;
}

void CompressNormalMap3Dc( const byte *inBuf, byte *outBuf, int width, int height, int &outputBytes ) 
{
	byte block[64];
	byte normalMin[4];
	byte normalMax[4];
	globalOutData = outBuf;
	for ( int j = 0; j < height; j += 4, inBuf += width * 4*4 ) 
	{
		for ( int i = 0; i < width; i += 4 ) 
		{
			ExtractBlock( inBuf + i * 4, width, block );
			GetMinMaxNormalsBBox( block, normalMin, normalMax );
			InsetNormalsBBox3Dc( normalMin, normalMax );
			// Write out Nx as an alpha channel.
			EmitByte( normalMax[0] );
			EmitByte( normalMin[0] );
			EmitAlphaIndices( block, 0, normalMin[0], normalMax[0] );
			// Write out Ny as an alpha channel.
			EmitByte( normalMax[1] );
			EmitByte( normalMin[1] );
			EmitAlphaIndices( block, 1, normalMin[1], normalMax[1] );
		}
	}
	//.outputBytes = outData - outBuf;
}

/*

#define ALIGN16( x ) __declspec(align(16)) x
#define R_SHUFFLE_D( x, y, z, w ) (( (w) & 3 ) << 6 | ( (z) & 3 ) << 4 | ( (y) & 3 ) << 2 | ( (x) & 3 ))
ALIGN16( static dword SIMD_SSE2_dword_byte_mask[4] ) = { 0x000000FF, 0x000000FF,0x000000FF, 0x000000FF };
ALIGN16( static dword SIMD_SSE2_dword_alpha_bit_mask0[4] ) = { 7<<0, 0, 7<<0, 0 };
ALIGN16( static dword SIMD_SSE2_dword_alpha_bit_mask1[4] ) = { 7<<3, 0, 7<<3, 0 };
ALIGN16( static dword SIMD_SSE2_dword_alpha_bit_mask2[4] ) = { 7<<6, 0, 7<<6, 0 };
ALIGN16( static dword SIMD_SSE2_dword_alpha_bit_mask3[4] ) = { 7<<9, 0, 7<<9, 0 };
ALIGN16( static dword SIMD_SSE2_dword_alpha_bit_mask4[4] ) = { 7<<12, 0, 7<<12, 0 };
ALIGN16( static dword SIMD_SSE2_dword_alpha_bit_mask5[4] ) = { 7<<15, 0, 7<<15, 0 };
ALIGN16( static dword SIMD_SSE2_dword_alpha_bit_mask6[4] ) = { 7<<18, 0, 7<<18, 0 };
ALIGN16( static dword SIMD_SSE2_dword_alpha_bit_mask7[4] ) = { 7<<21, 0, 7<<21, 0 };
ALIGN16( static word SIMD_SSE2_word_0[8] ) = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
ALIGN16( static word SIMD_SSE2_word_div_by_3[8] ) = { (1<<16)/3+1, (1<<16)/3+1, (1<<16)/3+1, (1<<16)/3+1, (1<<16)/3+1, (1<<16)/3+1, (1<<16)/3+1, (1<<16)/3+1 };
ALIGN16( static word SIMD_SSE2_word_div_by_6[8] ) = { (1<<16)/6+1, (1<<16)/6+1, (1<<16)/6+1, (1<<16)/6+1, (1<<16)/6+1, (1<<16)/6+1, (1<<16)/6+1, (1<<16)/6+1 };
ALIGN16( static word SIMD_SSE2_word_div_by_7[8] ) = { (1<<16)/7+1, (1<<16)/7+1, (1<<16)/7+1, (1<<16)/7+1, (1<<16)/7+1, (1<<16)/7+1, (1<<16)/7+1, (1<<16)/7+1 };
ALIGN16( static word SIMD_SSE2_word_div_by_14[8] ) = { (1<<16)/14+1, (1<<16)/14+1, (1<<16)/14+1, (1<<16)/14+1, (1<<16)/14+1, (1<<16)/14+1, (1<<16)/14+1, (1<<16)/14+1 };
ALIGN16( static word SIMD_SSE2_word_scale66554400[8] ) = { 6, 6, 5, 5, 4, 4, 0, 0 };
ALIGN16( static word SIMD_SSE2_word_scale11223300[8] ) = { 1, 1, 2, 2, 3, 3, 0, 0 };
ALIGN16( static word SIMD_SSE2_word_insetNormalDXT5Round[8] ) = { ((1<<(INSET_ALPHA_SHIFT-1))-1), ((1<<(INSET_COLOR_SHIFT-1))-1), 0, 0, 0, 0, 0, 0 };
ALIGN16( static word SIMD_SSE2_word_insetNormalDXT5Mask[8] ) = { 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
ALIGN16( static word SIMD_SSE2_word_insetNormalDXT5ShiftUp[8] ) = { 1 << INSET_ALPHA_SHIFT, 1 << INSET_COLOR_SHIFT, 1, 1, 1, 1, 1, 1 };
ALIGN16( static word SIMD_SSE2_word_insetNormalDXT5ShiftDown[8] ) = { 1 << ( 16 - INSET_ALPHA_SHIFT ), 1 << ( 16 - INSET_COLOR_SHIFT ), 0, 0, 0, 0, 0, 0 };
ALIGN16( static word SIMD_SSE2_word_insetNormalDXT5QuantMask[8] ) = { 0xFF, C565_6_MASK, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
ALIGN16( static word SIMD_SSE2_word_insetNormalDXT5Rep[8] ) = { 0, 1 << ( 16 - 6 ), 0, 0, 0, 0, 0, 0 };
ALIGN16( static word SIMD_SSE2_word_insetNormal3DcRound[8] ) = { ((1<<(INSET_ALPHA_SHIFT-1))-1), ((1<<(INSET_ALPHA_SHIFT-1))-1), 0, 0, 0, 0, 0, 0 };
ALIGN16( static word SIMD_SSE2_word_insetNormal3DcMask[8] ) = { 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
ALIGN16( static word SIMD_SSE2_word_insetNormal3DcShiftUp[8] ) = { 1 << INSET_ALPHA_SHIFT, 1 << INSET_ALPHA_SHIFT, 1, 1, 1, 1, 1, 1 };
ALIGN16( static word SIMD_SSE2_word_insetNormal3DcShiftDown[8] ) = { 1 << ( 16 - INSET_ALPHA_SHIFT ), 1 << ( 16 - INSET_ALPHA_SHIFT ), 0, 0, 0, 0, 0, 0 };
ALIGN16( static byte SIMD_SSE2_byte_0[16] ) = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
ALIGN16( static byte SIMD_SSE2_byte_1[16] ) = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };
ALIGN16( static byte SIMD_SSE2_byte_2[16] ) = { 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02 };
ALIGN16( static byte SIMD_SSE2_byte_3[16] ) = { 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03 };
ALIGN16( static byte SIMD_SSE2_byte_4[16] ) = { 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 };
ALIGN16( static byte SIMD_SSE2_byte_7[16] ) = { 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07 };
ALIGN16( static byte SIMD_SSE2_byte_8[16] ) = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08 };

void ExtractBlock_SSE2( const byte *inPtr, int width, byte *block ) 
{
	__asm 
	{
		mov esi, inPtr
		mov edi, block
		mov eax, width
		shl eax, 2
		movdqa xmm0, [esi]
		movdqa xmmword ptr [edi+ 0], xmm0
		movdqa xmm1, xmmword ptr [esi+eax]
		movdqa xmmword ptr [edi+16], xmm1
		movdqa xmm2, xmmword ptr [esi+eax*2]
		add esi, eax
		movdqa xmmword ptr [edi+32], xmm2
		movdqa xmm3, xmmword ptr [esi+eax*2]
		movdqa xmmword ptr [edi+48], xmm3
	}
}

void GetMinMaxNormalsBBox_SSE2( const byte *block, byte *minNormal, byte *maxNormal ) 
{
	__asm 
	{
		mov eax, block
		mov esi, minNormal
		mov edi, maxNormal
		movdqa xmm0, xmmword ptr [eax+ 0]
		movdqa xmm1, xmmword ptr [eax+ 0]
		pminub xmm0, xmmword ptr [eax+16]
		pmaxub xmm1, xmmword ptr [eax+16]
		pminub xmm0, xmmword ptr [eax+32]
		pmaxub xmm1, xmmword ptr [eax+32]
		pminub xmm0, xmmword ptr [eax+48]
		pmaxub xmm1, xmmword ptr [eax+48]
		pshufd xmm3, xmm0, R_SHUFFLE_D( 2, 3, 2, 3 )
		pshufd xmm4, xmm1, R_SHUFFLE_D( 2, 3, 2, 3 )
		pminub xmm0, xmm3
		pmaxub xmm1, xmm4
		pshuflw xmm6, xmm0, R_SHUFFLE_D( 2, 3, 2, 3 )
		pshuflw xmm7, xmm1, R_SHUFFLE_D( 2, 3, 2, 3 )
		pminub xmm0, xmm6
		pmaxub xmm1, xmm7
		movd dword ptr [esi], xmm0
		movd dword ptr [edi], xmm1
	}
}

void InsetNormalsBBoxDXT5_SSE2( byte *minNormal, byte *maxNormal ) 
{
	__asm 
	{
		mov esi, minNormal
		mov edi, maxNormal
		movd xmm0, dword ptr [esi]
		movd xmm1, dword ptr [edi]
		punpcklbw xmm0, SIMD_SSE2_byte_0
		punpcklbw xmm1, SIMD_SSE2_byte_0
		movdqa xmm2, xmm1
		psubw xmm2, xmm0
		psubw xmm2, SIMD_SSE2_word_insetNormalDXT5Round
		pand xmm2, SIMD_SSE2_word_insetNormalDXT5Mask
		pmullw xmm0, SIMD_SSE2_word_insetNormalDXT5ShiftUp
		pmullw xmm1, SIMD_SSE2_word_insetNormalDXT5ShiftUp
		paddw xmm0, xmm2
		psubw xmm1, xmm2
		pmulhw xmm0, SIMD_SSE2_word_insetNormalDXT5ShiftDown
		pmulhw xmm1, SIMD_SSE2_word_insetNormalDXT5ShiftDown
		pmaxsw xmm0, SIMD_SSE2_word_0
		pmaxsw xmm1, SIMD_SSE2_word_0
		pand xmm0, SIMD_SSE2_word_insetNormalDXT5QuantMask
		pand xmm1, SIMD_SSE2_word_insetNormalDXT5QuantMask
		movdqa xmm2, xmm0
		movdqa xmm3, xmm1
		pmulhw xmm2, SIMD_SSE2_word_insetNormalDXT5Rep
		pmulhw xmm3, SIMD_SSE2_word_insetNormalDXT5Rep
		por xmm0, xmm2
		por xmm1, xmm3
		packuswb xmm0, xmm0
		packuswb xmm1, xmm1
		movd dword ptr [esi], xmm0
		movd dword ptr [edi], xmm1
	}
}

void InsetNormalsBBox3Dc_SSE2( byte *minNormal, byte *maxNormal ) 
{
__asm 
	{
	mov esi, minNormal
	mov edi, maxNormal
	movd xmm0, dword ptr [esi]
	movd xmm1, dword ptr [edi]
	punpcklbw xmm0, SIMD_SSE2_byte_0
	punpcklbw xmm1, SIMD_SSE2_byte_0
	movdqa xmm2, xmm1
	psubw xmm2, xmm0
	psubw xmm2, SIMD_SSE2_word_insetNormal3DcRound
	pand xmm2, SIMD_SSE2_word_insetNormal3DcMask
	pmullw xmm0, SIMD_SSE2_word_insetNormal3DcShiftUp
	pmullw xmm1, SIMD_SSE2_word_insetNormal3DcShiftUp
	paddw xmm0, xmm2
	psubw xmm1, xmm2
	pmulhw xmm0, SIMD_SSE2_word_insetNormal3DcShiftDown
	pmulhw xmm1, SIMD_SSE2_word_insetNormal3DcShiftDown
	pmaxsw xmm0, SIMD_SSE2_word_0
	pmaxsw xmm1, SIMD_SSE2_word_0
	packuswb xmm0, xmm0
	packuswb xmm1, xmm1
	movd dword ptr [esi], xmm0
	movd dword ptr [edi], xmm1
	}
}

void EmitAlphaIndices_SSE2( const byte *block, const int channelBitOffset, const int minAlpha, const int maxAlpha ) 
{
	__asm 
	{
		movd xmm7, channelBitOffset
		mov esi, block
		movdqa xmm0, xmmword ptr [esi+ 0]
		movdqa xmm5, xmmword ptr [esi+16]
		movdqa xmm6, xmmword ptr [esi+32]
		movdqa xmm4, xmmword ptr [esi+48]
		psrld xmm0, xmm7
		psrld xmm5, xmm7
		psrld xmm6, xmm7
		psrld xmm4, xmm7
		pand xmm0, SIMD_SSE2_dword_byte_mask
		pand xmm5, SIMD_SSE2_dword_byte_mask
		pand xmm6, SIMD_SSE2_dword_byte_mask
		pand xmm4, SIMD_SSE2_dword_byte_mask
		packuswb xmm0, xmm5
		packuswb xmm6, xmm4
		movd xmm5, maxAlpha
		pshuflw xmm5, xmm5, R_SHUFFLE_D( 0, 0, 0, 0 )
		pshufd xmm5, xmm5, R_SHUFFLE_D( 0, 0, 0, 0 )
		movdqa xmm7, xmm5
		movd xmm2, minAlpha
		pshuflw xmm2, xmm2, R_SHUFFLE_D( 0, 0, 0, 0 )
		pshufd xmm2, xmm2, R_SHUFFLE_D( 0, 0, 0, 0 )
		movdqa xmm3, xmm2
		movdqa xmm4, xmm5
		psubw xmm4, xmm2
		pmulhw xmm4, SIMD_SSE2_word_div_by_14
		movdqa xmm1, xmm5
		psubw xmm1, xmm4
		packuswb xmm1, xmm1 // ab1
		pmullw xmm5, SIMD_SSE2_word_scale66554400
		pmullw xmm7, SIMD_SSE2_word_scale11223300
		pmullw xmm2, SIMD_SSE2_word_scale11223300
		pmullw xmm3, SIMD_SSE2_word_scale66554400
		paddw xmm5, xmm2
		paddw xmm7, xmm3
		pmulhw xmm5, SIMD_SSE2_word_div_by_7
		pmulhw xmm7, SIMD_SSE2_word_div_by_7
		psubw xmm5, xmm4
		psubw xmm7, xmm4
		pshufd xmm2, xmm5, R_SHUFFLE_D( 0, 0, 0, 0 )
		pshufd xmm3, xmm5, R_SHUFFLE_D( 1, 1, 1, 1 )
		pshufd xmm4, xmm5, R_SHUFFLE_D( 2, 2, 2, 2 )
		packuswb xmm2, xmm2 // ab2
		packuswb xmm3, xmm3 // ab3
		packuswb xmm4, xmm4 // ab4
		packuswb xmm0, xmm6
		pshufd xmm5, xmm7, R_SHUFFLE_D( 2, 2, 2, 2 )
		pshufd xmm6, xmm7, R_SHUFFLE_D( 1, 1, 1, 1 )
		pshufd xmm7, xmm7, R_SHUFFLE_D( 0, 0, 0, 0 )
		packuswb xmm5, xmm5 // ab5
		packuswb xmm6, xmm6 // ab6
		packuswb xmm7, xmm7 // ab7
		pmaxub xmm1, xmm0
		pmaxub xmm2, xmm0
		pmaxub xmm3, xmm0
		pcmpeqb xmm1, xmm0
		pcmpeqb xmm2, xmm0
		pcmpeqb xmm3, xmm0
		pmaxub xmm4, xmm0
		pmaxub xmm5, xmm0
		pmaxub xmm6, xmm0
		pmaxub xmm7, xmm0
		pcmpeqb xmm4, xmm0
		pcmpeqb xmm5, xmm0
		pcmpeqb xmm6, xmm0
		pcmpeqb xmm7, xmm0
		movdqa xmm0, SIMD_SSE2_byte_8
		paddsb xmm0, xmm1
		paddsb xmm2, xmm3
		paddsb xmm4, xmm5
		paddsb xmm6, xmm7
		paddsb xmm0, xmm2
		paddsb xmm4, xmm6
		paddsb xmm0, xmm4
		pand xmm0, SIMD_SSE2_byte_7
		movdqa xmm1, SIMD_SSE2_byte_2
		pcmpgtb xmm1, xmm0
		pand xmm1, SIMD_SSE2_byte_1
		pxor xmm0, xmm1
		movdqa xmm1, xmm0
		movdqa xmm2, xmm0
		movdqa xmm3, xmm0
		movdqa xmm4, xmm0
		movdqa xmm5, xmm0
		movdqa xmm6, xmm0
		movdqa xmm7, xmm0
		psrlq xmm1, 8- 3
		psrlq xmm2, 16- 6
		psrlq xmm3, 24- 9
		psrlq xmm4, 32-12
		psrlq xmm5, 40-15
		psrlq xmm6, 48-18
		psrlq xmm7, 56-21
		pand xmm0, SIMD_SSE2_dword_alpha_bit_mask0
		pand xmm1, SIMD_SSE2_dword_alpha_bit_mask1
		pand xmm2, SIMD_SSE2_dword_alpha_bit_mask2
		pand xmm3, SIMD_SSE2_dword_alpha_bit_mask3
		pand xmm4, SIMD_SSE2_dword_alpha_bit_mask4
		pand xmm5, SIMD_SSE2_dword_alpha_bit_mask5
		pand xmm6, SIMD_SSE2_dword_alpha_bit_mask6
		pand xmm7, SIMD_SSE2_dword_alpha_bit_mask7
		por xmm0, xmm1
		por xmm2, xmm3
		por xmm4, xmm5
		por xmm6, xmm7
		por xmm0, xmm2
		por xmm4, xmm6
		por xmm0, xmm4
		mov esi, globalOutData
		movd [esi+0], xmm0
		pshufd xmm1, xmm0, R_SHUFFLE_D( 2, 3, 0, 1 )
		movd [esi+3], xmm1
	}
	globalOutData += 6;
}

void EmitGreenIndices_SSE2( const byte *block, const int channelBitOffset, const int minGreen, const int maxGreen ) 
{
	__asm 
	{
		movd xmm7, channelBitOffset
		mov esi, block
		movdqa xmm0, xmmword ptr [esi+ 0]
		movdqa xmm5, xmmword ptr [esi+16]
		movdqa xmm6, xmmword ptr [esi+32]
		movdqa xmm4, xmmword ptr [esi+48]
		psrld xmm0, xmm7
		psrld xmm5, xmm7
		psrld xmm6, xmm7
		psrld xmm4, xmm7
		pand xmm0, SIMD_SSE2_dword_byte_mask
		pand xmm5, SIMD_SSE2_dword_byte_mask
		pand xmm6, SIMD_SSE2_dword_byte_mask
		pand xmm4, SIMD_SSE2_dword_byte_mask
		packuswb xmm0, xmm5
		packuswb xmm6, xmm4
		movd xmm2, maxGreen
		pshuflw xmm2, xmm2, R_SHUFFLE_D( 0, 0, 0, 0 )
		pshufd xmm2, xmm2, R_SHUFFLE_D( 0, 0, 0, 0 )
		movdqa xmm1, xmm2
		movd xmm3, minGreen
		pshuflw xmm3, xmm3, R_SHUFFLE_D( 0, 0, 0, 0 )
		pshufd xmm3, xmm3, R_SHUFFLE_D( 0, 0, 0, 0 )
		movdqa xmm4, xmm2
		psubw xmm4, xmm3
		pmulhw xmm4, SIMD_SSE2_word_div_by_6
		psllw xmm2, 1
		paddw xmm2, xmm3
		pmulhw xmm2, SIMD_SSE2_word_div_by_3
		psubw xmm2, xmm4
		packuswb xmm2, xmm2 // gb2
		psllw xmm3, 1
		paddw xmm3, xmm1
		pmulhw xmm3, SIMD_SSE2_word_div_by_3
		psubw xmm3, xmm4
		packuswb xmm3, xmm3 // gb3
		psubw xmm1, xmm4
		packuswb xmm1, xmm1 // gb1
		packuswb xmm0, xmm6
		pmaxub xmm1, xmm0
		pmaxub xmm2, xmm0
		pmaxub xmm3, xmm0
		pcmpeqb xmm1, xmm0
		pcmpeqb xmm2, xmm0
		pcmpeqb xmm3, xmm0
		movdqa xmm0, SIMD_SSE2_byte_4
		paddsb xmm0, xmm1
		paddsb xmm2, xmm3
		paddsb xmm0, xmm2
		pand xmm0, SIMD_SSE2_byte_3
		movdqa xmm4, SIMD_SSE2_byte_2
		pcmpgtb xmm4, xmm0
		pand xmm4, SIMD_SSE2_byte_1
		pxor xmm0, xmm4
		movdqa xmm4, xmm0
		movdqa xmm5, xmm0
		movdqa xmm6, xmm0
		movdqa xmm7, xmm0
		psrlq xmm4, 8- 2
		psrlq xmm5, 16- 4
		psrlq xmm6, 24- 6
		psrlq xmm7, 32- 8
		pand xmm4, SIMD_SSE2_dword_color_bit_mask1
		pand xmm5, SIMD_SSE2_dword_color_bit_mask2
		pand xmm6, SIMD_SSE2_dword_color_bit_mask3
		pand xmm7, SIMD_SSE2_dword_color_bit_mask4
		por xmm5, xmm4
		por xmm7, xmm6
		por xmm7, xmm5
		movdqa xmm4, xmm0
		movdqa xmm5, xmm0
		movdqa xmm6, xmm0
		psrlq xmm4, 40-10
		psrlq xmm5, 48-12
		psrlq xmm6, 56-14
		pand xmm0, SIMD_SSE2_dword_color_bit_mask0
		pand xmm4, SIMD_SSE2_dword_color_bit_mask5
		pand xmm5, SIMD_SSE2_dword_color_bit_mask6
		pand xmm6, SIMD_SSE2_dword_color_bit_mask7
		por xmm4, xmm5
		por xmm0, xmm6
		por xmm7, xmm4
		por xmm7, xmm0
		mov esi, globalOutData
		movd [esi+0], xmm7
		pshufd xmm6, xmm7, R_SHUFFLE_D( 2, 3, 0, 1 )
		movd [esi+2], xmm6
	}
	globalOutData += 4;
}

bool CompressNormalMapDXT5_SSE2( const byte *inBuf, 
									byte *outBuf, 
									int width, 
									int height,
									int &outputBytes ) 
{
	ALIGN16( byte block[64] );
	ALIGN16( byte normalMin[4] );
	ALIGN16( byte normalMax[4] );
	globalOutData = outBuf;
	for ( int j = 0; j < height; j += 4, inBuf += width * 4*4 ) 
	{
		for ( int i = 0; i < width; i += 4 ) 
		{
			ExtractBlock_SSE2( inBuf + i * 4, width, block );
			GetMinMaxNormalsBBox_SSE2( block, normalMin, normalMax );
			InsetNormalsBBoxDXT5_SSE2( normalMin, normalMax );
			// Write out Nx into alpha channel.
			EmitByte( normalMax[0] );
			EmitByte( normalMin[0] );
			EmitAlphaIndices_SSE2( block, 0*8, normalMin[0], normalMax[0] );
			// Write out Ny into green channel.
			EmitUShort( NormalYTo565( normalMax[1] ) );
			EmitUShort( NormalYTo565( normalMin[1] ) );
			EmitGreenIndices_SSE2( block, 1*8, normalMin[1], normalMax[1] );
		}
	}
	//.outputBytes = outData - outBuf;
}

void CompressNormalMap3Dc_SSE2( const byte *inBuf, 
								byte *outBuf, 
								int width, 
								int height,
								int &outputBytes ) 
{
	ALIGN16( byte block[64] );
	ALIGN16( byte normalMin[4] );
	ALIGN16( byte normalMax[4] );
	globalOutData = outBuf;
	for ( int j = 0; j < height; j += 4, inBuf += width * 4*4 ) 
	{
		for ( int i = 0; i < width; i += 4 ) 
		{
			ExtractBlock_SSE2( inBuf + i * 4, width, block );
			GetMinMaxNormalsBBox_SSE2( block, normalMin, normalMax );
			InsetNormalsBBox3Dc_SSE2( normalMin, normalMax );
			// Write out Nx as an alpha channel.
			EmitByte( normalMax[0] );
			EmitByte( normalMin[0] );
			EmitAlphaIndices_SSE2( block, 0*8, normalMin[0], normalMax[0] );
			// Write out Ny as an alpha channel.
			EmitByte( normalMax[1] );
			EmitByte( normalMin[1] );
			EmitAlphaIndices_SSE2( block, 1*8, normalMin[1], normalMax[1] );
		}
	}
	//.outputBytes = outData - outBuf;
}
*/