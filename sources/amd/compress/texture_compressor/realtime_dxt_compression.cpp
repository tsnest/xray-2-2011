#include "pch.h"

namespace realtime_dxt{

///-----dxt1
typedef unsigned char   byte; 
typedef unsigned short  word; 
typedef unsigned int    dword; 

	void ExtractBlock( const byte *inPtr, int width, byte *colorBlock );
	void GetMinMaxColors_dxt1( const byte *colorBlock, byte *minColor, byte *maxColor );
	void GetMinMaxColors_dxt5( const byte *colorBlock, byte *minColor, byte *maxColor );
	void EmitWord( word s );
	word ColorTo565( const byte *color );
	void EmitColorIndices( const byte *colorBlock, const byte *minColor, const byte *maxColor );

byte *globalOutData;

void CompressImageDXT1( const byte *inBuf, byte *outBuf, int width, int height, int &outputBytes ) 
{ 
	_declspec(align(16)) byte block[64];
    _declspec(align(16)) byte minColor[4]; 
    _declspec(align(16)) byte maxColor[4]; 

	globalOutData = outBuf; 
    for ( int j = 0; j < height; j += 4, inBuf += width * 4*4 ) 
	{ 
        for ( int i = 0; i < width; i += 4 ) 
		{ 
            ExtractBlock( inBuf + i * 4, width, block ); 
            GetMinMaxColors_dxt1( block, minColor, maxColor ); 
            EmitWord( ColorTo565( maxColor ) ); 
            EmitWord( ColorTo565( minColor ) ); 
            EmitColorIndices( block, minColor, maxColor ); 
		}        
	}    
    outputBytes = globalOutData - outBuf; 
}

void ExtractBlock( const byte *inPtr, int width, byte *colorBlock ) 
{
	for ( int j = 0; j < 4; j++ ) 
	{
		memcpy( &colorBlock[j*4*4], inPtr, 4*4 );
		inPtr += width * 4;
	}
}

word ColorTo565( const byte *color ) 
{
	return ( ( color[ 0 ] >> 3 ) << 11 ) | ( ( color[ 1 ] >> 2 ) << 5 ) | ( color[ 2 ] >> 3 );
}

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

void EmitDoubleWord( dword i ) 
{
	globalOutData[0] = ( i >> 0 ) & 255;
	globalOutData[1] = ( i >> 8 ) & 255;
	globalOutData[2] = ( i >> 16 ) & 255;
	globalOutData[3] = ( i >> 24 ) & 255;
	globalOutData += 4;
}

int ColorDistance( const byte *c1, const byte *c2 ) 
{
	return ( ( c1[0] - c2[0] ) * ( c1[0] - c2[0] ) ) +
			( ( c1[1] - c2[1] ) * ( c1[1] - c2[1] ) ) +
			( ( c1[2] - c2[2] ) * ( c1[2] - c2[2] ) );
}

void SwapColors( byte *c1, byte *c2 ) 
{
	byte tm[3];
	memcpy( tm, c1, 3 );
	memcpy( c1, c2, 3 );
	memcpy( c2, tm, 3 );
}

//void GetMinMaxColors( const byte *colorBlock, byte *minColor, byte *maxColor ) 
//{
//	int maxDistance = -1;
//	for ( int i = 0; i < 64 - 4; i += 4 ) 
//	{
//		for ( int j = i + 4; j < 64; j += 4 ) 
//		{
//			int distance = ColorDistance( &colorBlock[i], &colorBlock[j] );
//			if ( distance > maxDistance ) 
//			{
//			maxDistance = distance;
//			memcpy( minColor, colorBlock+i, 3 );
//			memcpy( maxColor, colorBlock+j, 3 );
//			}
//		}
//	}
//	if ( ColorTo565( maxColor ) < ColorTo565( minColor ) ) 
//	{
//		SwapColors( minColor, maxColor );
//	}
//}

int ColorLuminance( const byte *color ) 
{
	return ( color[0] + color[1] * 2 + color[2] );
}

//void GetMinMaxColors_not_optimized( const byte *colorBlock, byte *minColor, byte *maxColor ) 
//{
//	int maxLuminance = -1, minLuminance = MAX_INT;
//	for ( i = 0; i < 16; i++ ) 
//	{
//		int luminance = ColorLuminance( colorBlock+i*4 );
//		if ( luminance > maxLuminance ) 
//		{
//			maxLuminance = luminance;
//			memcpy( maxColor, colorBlock+i*4, 3 );
//		}
//		if ( luminance < minLuminance ) 
//		{
//			minLuminance = luminance;
//			memcpy( minColor, colorBlock+i*4, 3 );
//		}
//	}
//	if ( ColorTo565( maxColor ) < ColorTo565( minColor ) ) 
//	{
//		SwapColors( minColor, maxColor );
//	}
//}

#define INSET_SHIFT 4 // inset the bounding box with ( range >> shift )
void GetMinMaxColors_dxt1( const byte *colorBlock, byte *minColor, byte *maxColor ) 
{
	int i;
	byte inset[3];
	minColor[0] = minColor[1] = minColor[2] = 255;
	maxColor[0] = maxColor[1] = maxColor[2] = 0;
	for ( i = 0; i < 16; i++ ) 
	{
		if ( colorBlock[i*4+0] < minColor[0] ) { minColor[0] = colorBlock[i*4+0]; }
		if ( colorBlock[i*4+1] < minColor[1] ) { minColor[1] = colorBlock[i*4+1]; }
		if ( colorBlock[i*4+2] < minColor[2] ) { minColor[2] = colorBlock[i*4+2]; }
		if ( colorBlock[i*4+0] > maxColor[0] ) { maxColor[0] = colorBlock[i*4+0]; }
		if ( colorBlock[i*4+1] > maxColor[1] ) { maxColor[1] = colorBlock[i*4+1]; }
		if ( colorBlock[i*4+2] > maxColor[2] ) { maxColor[2] = colorBlock[i*4+2]; }
	}
	inset[0] = ( maxColor[0] - minColor[0] ) >> INSET_SHIFT;
	inset[1] = ( maxColor[1] - minColor[1] ) >> INSET_SHIFT;
	inset[2] = ( maxColor[2] - minColor[2] ) >> INSET_SHIFT;

	minColor[0] = ( minColor[0] + inset[0] <= 255 ) ? minColor[0] + inset[0] : 255;
	minColor[1] = ( minColor[1] + inset[1] <= 255 ) ? minColor[1] + inset[1] : 255;
	minColor[2] = ( minColor[2] + inset[2] <= 255 ) ? minColor[2] + inset[2] : 255;
	maxColor[0] = ( maxColor[0] >= inset[0] ) ? maxColor[0] - inset[0] : 0;
	maxColor[1] = ( maxColor[1] >= inset[1] ) ? maxColor[1] - inset[1] : 0;
	maxColor[2] = ( maxColor[2] >= inset[2] ) ? maxColor[2] - inset[2] : 0;
}

#define C565_5_MASK 0xF8 // 0xFF minus last three bits
#define C565_6_MASK 0xFC // 0xFF minus last two bits

//void EmitColorIndices( const byte *colorBlock, const byte *minColor, const byte *maxColor ) 
//{
//	byte colors[4][4];
//	unsigned int indices[16];
//	colors[0][0] = ( maxColor[0] & C565_5_MASK ) | ( maxColor[0] >> 5 );
//	colors[0][1] = ( maxColor[1] & C565_6_MASK ) | ( maxColor[1] >> 6 );
//	colors[0][2] = ( maxColor[2] & C565_5_MASK ) | ( maxColor[2] >> 5 );
//	colors[1][0] = ( minColor[0] & C565_5_MASK ) | ( minColor[0] >> 5 );
//	colors[1][1] = ( minColor[1] & C565_6_MASK ) | ( minColor[1] >> 6 );
//	colors[1][2] = ( minColor[2] & C565_5_MASK ) | ( minColor[2] >> 5 );
//	colors[2][0] = ( 2 * colors[0][0] + 1 * colors[1][0] ) / 3;
//	colors[2][1] = ( 2 * colors[0][1] + 1 * colors[1][1] ) / 3;
//	colors[2][2] = ( 2 * colors[0][2] + 1 * colors[1][2] ) / 3;
//	colors[3][0] = ( 1 * colors[0][0] + 2 * colors[1][0] ) / 3;
//	colors[3][1] = ( 1 * colors[0][1] + 2 * colors[1][1] ) / 3;
//	colors[3][2] = ( 1 * colors[0][2] + 2 * colors[1][2] ) / 3;
//
//	for ( int i = 0; i < 16; i++ ) 
//	{
//		unsigned int minDistance = INT_MAX;
//		for ( int j = 0; j < 4; j++ ) 
//		{
//			unsigned int dist = ColorDistance( &colorBlock[i*4], &colors[j][0] );
//			if ( dist < minDistance ) 
//			{
//				minDistance = dist;
//				indices[i] = j;
//			}
//		}
//	}
//
//	dword result = 0;
//	for ( int i = 0; i < 16; i++ ) 
//	{
//		result |= ( indices[i] << (unsigned int)( i << 1 ) );
//	}
//
//	EmitDoubleWord( result );
//}

void EmitColorIndices( const byte *colorBlock, const byte *minColor, const byte *maxColor ) 
{
	word colors[4][4];
	dword result = 0;
	colors[0][0] = ( maxColor[0] & C565_5_MASK ) | ( maxColor[0] >> 5 );
	colors[0][1] = ( maxColor[1] & C565_6_MASK ) | ( maxColor[1] >> 6 );
	colors[0][2] = ( maxColor[2] & C565_5_MASK ) | ( maxColor[2] >> 5 );
	colors[1][0] = ( minColor[0] & C565_5_MASK ) | ( minColor[0] >> 5 );
	colors[1][1] = ( minColor[1] & C565_6_MASK ) | ( minColor[1] >> 6 );
	colors[1][2] = ( minColor[2] & C565_5_MASK ) | ( minColor[2] >> 5 );
	colors[2][0] = ( 2 * colors[0][0] + 1 * colors[1][0] ) / 3;
	colors[2][1] = ( 2 * colors[0][1] + 1 * colors[1][1] ) / 3;
	colors[2][2] = ( 2 * colors[0][2] + 1 * colors[1][2] ) / 3;
	colors[3][0] = ( 1 * colors[0][0] + 2 * colors[1][0] ) / 3;
	colors[3][1] = ( 1 * colors[0][1] + 2 * colors[1][1] ) / 3;
	colors[3][2] = ( 1 * colors[0][2] + 2 * colors[1][2] ) / 3;
	
	for ( int i = 15; i >= 0; i-- ) 
	{
		int c0 = colorBlock[i*4+0];
		int c1 = colorBlock[i*4+1];
		int c2 = colorBlock[i*4+2];
		int d0 = abs( colors[0][0] - c0 ) + abs( colors[0][1] - c1 ) + abs( colors[0][2] - c2 );
		int d1 = abs( colors[1][0] - c0 ) + abs( colors[1][1] - c1 ) + abs( colors[1][2] - c2 );
		int d2 = abs( colors[2][0] - c0 ) + abs( colors[2][1] - c1 ) + abs( colors[2][2] - c2 );
		int d3 = abs( colors[3][0] - c0 ) + abs( colors[3][1] - c1 ) + abs( colors[3][2] - c2 );
		int b0 = d0 > d3;
		int b1 = d1 > d2;
		int b2 = d0 > d2;
		int b3 = d1 > d3;
		int b4 = d2 > d3;
		int x0 = b1 & b2;
		int x1 = b0 & b3;
		int x2 = b0 & b4;
		result |= ( x2 | ( ( x0 | x1 ) << 1 ) ) << ( i << 1 );
	}
	EmitDoubleWord( result );
}

//---dxt5
void EmitAlphaIndices( const byte *colorBlock, const byte minAlpha, const byte maxAlpha );

void CompressImageDXT5( const byte *inBuf, byte *outBuf, int width, int height, int &outputBytes ) 
{
	_declspec(align(16)) byte block[64];
	_declspec(align(16)) byte minColor[4];
	_declspec(align(16)) byte maxColor[4];

	//ALIGN16( byte block[64] );
	//ALIGN16( byte minColor[4] );
	//ALIGN16( byte maxColor[4] );
	globalOutData = outBuf;
	for ( int j = 0; j < height; j += 4, inBuf += width * 4*4 ) 
	{
		for ( int i = 0; i < width; i += 4 ) 
		{
			ExtractBlock( inBuf + i * 4, width, block );
			GetMinMaxColors_dxt5( block, minColor, maxColor );
			EmitByte( maxColor[3] );
			EmitByte( minColor[3] );
			EmitAlphaIndices( block, minColor[3], maxColor[3] );
			EmitWord( ColorTo565( maxColor ) );
			EmitWord( ColorTo565( minColor ) );
			EmitColorIndices( block, minColor, maxColor );
		}
	}
	outputBytes = globalOutData - outBuf;
}

#define INSET_SHIFT 4 // inset the bounding box with ( range >> shift )
void GetMinMaxColors_dxt5( const byte *colorBlock, byte *minColor, byte *maxColor ) 
{
	int i;
	byte inset[4];
	minColor[0] = minColor[1] = minColor[2] = minColor[3] = 255;
	maxColor[0] = maxColor[1] = maxColor[2] = maxColor[3] = 0;
	
	for ( i = 0; i < 16; i++ ) 
	{
		if ( colorBlock[i*4+0] < minColor[0] ) { minColor[0] = colorBlock[i*4+0]; }
		if ( colorBlock[i*4+1] < minColor[1] ) { minColor[1] = colorBlock[i*4+1]; }
		if ( colorBlock[i*4+2] < minColor[2] ) { minColor[2] = colorBlock[i*4+2]; }
		if ( colorBlock[i*4+3] < minColor[3] ) { minColor[3] = colorBlock[i*4+3]; }
		if ( colorBlock[i*4+0] > maxColor[0] ) { maxColor[0] = colorBlock[i*4+0]; }
		if ( colorBlock[i*4+1] > maxColor[1] ) { maxColor[1] = colorBlock[i*4+1]; }
		if ( colorBlock[i*4+2] > maxColor[2] ) { maxColor[2] = colorBlock[i*4+2]; }
		if ( colorBlock[i*4+3] > maxColor[3] ) { maxColor[3] = colorBlock[i*4+3]; }
	}
	inset[0] = ( maxColor[0] - minColor[0] ) >> INSET_SHIFT;
	inset[1] = ( maxColor[1] - minColor[1] ) >> INSET_SHIFT;
	inset[2] = ( maxColor[2] - minColor[2] ) >> INSET_SHIFT;
	inset[3] = ( maxColor[3] - minColor[3] ) >> INSET_SHIFT;
	minColor[0] = ( minColor[0] + inset[0] <= 255 ) ? minColor[0] + inset[0] : 255;
	minColor[1] = ( minColor[1] + inset[1] <= 255 ) ? minColor[1] + inset[1] : 255;
	minColor[2] = ( minColor[2] + inset[2] <= 255 ) ? minColor[2] + inset[2] : 255;
	minColor[3] = ( minColor[3] + inset[3] <= 255 ) ? minColor[3] + inset[3] : 255;
	maxColor[0] = ( maxColor[0] >= inset[0] ) ? maxColor[0] - inset[0] : 0;
	maxColor[1] = ( maxColor[1] >= inset[1] ) ? maxColor[1] - inset[1] : 0;
	maxColor[2] = ( maxColor[2] >= inset[2] ) ? maxColor[2] - inset[2] : 0;
	maxColor[3] = ( maxColor[3] >= inset[3] ) ? maxColor[3] - inset[3] : 0;
}
//
//void EmitAlphaIndices( const byte *colorBlock, const byte minAlpha, const byte maxAlpha ) 
//{
//	byte indices[16];
//	byte alphas[8];
//	alphas[0] = maxAlpha;
//	alphas[1] = minAlpha;
//	alphas[2] = ( 6 * maxAlpha + 1 * minAlpha ) / 7;
//	alphas[3] = ( 5 * maxAlpha + 2 * minAlpha ) / 7;
//	alphas[4] = ( 4 * maxAlpha + 3 * minAlpha ) / 7;
//	alphas[5] = ( 3 * maxAlpha + 4 * minAlpha ) / 7;
//	alphas[6] = ( 2 * maxAlpha + 5 * minAlpha ) / 7;
//	alphas[7] = ( 1 * maxAlpha + 6 * minAlpha ) / 7;
//	colorBlock += 3;
//	for ( int i = 0; i < 16; i++ ) 
//	{
//		int minDistance = INT_MAX;
//		byte a = colorBlock[i*4];
//		for ( int j = 0; j < 8; j++ ) 
//		{
//			int dist = abs( a - alphas[j] );
//			if ( dist < minDistance ) 
//			{
//				minDistance = dist; indices[i] = j;
//			}
//		}
//	}
//	EmitByte( (indices[ 0] >> 0) | (indices[ 1] << 3) | (indices[ 2] << 6) );
//	EmitByte( (indices[ 2] >> 2) | (indices[ 3] << 1) | (indices[ 4] << 4) | (indices[ 5] << 7) );
//	EmitByte( (indices[ 5] >> 1) | (indices[ 6] << 2) | (indices[ 7] << 5) );
//	EmitByte( (indices[ 8] >> 0) | (indices[ 9] << 3) | (indices[10] << 6) );
//	EmitByte( (indices[10] >> 2) | (indices[11] << 1) | (indices[12] << 4) | (indices[13] << 7) );
//	EmitByte( (indices[13] >> 1) | (indices[14] << 2) | (indices[15] << 5) );
//}

void EmitAlphaIndices( const byte *colorBlock, const byte minAlpha, const byte maxAlpha ) 
{
	assert( maxAlpha > minAlpha );
	byte indices[16];
	byte mid = ( maxAlpha - minAlpha ) / ( 2 * 7 );
	byte ab1 = minAlpha + mid;
	byte ab2 = ( 6 * maxAlpha + 1 * minAlpha ) / 7 + mid;
	byte ab3 = ( 5 * maxAlpha + 2 * minAlpha ) / 7 + mid;
	byte ab4 = ( 4 * maxAlpha + 3 * minAlpha ) / 7 + mid;
	byte ab5 = ( 3 * maxAlpha + 4 * minAlpha ) / 7 + mid;
	byte ab6 = ( 2 * maxAlpha + 5 * minAlpha ) / 7 + mid;
	byte ab7 = ( 1 * maxAlpha + 6 * minAlpha ) / 7 + mid;
	colorBlock += 3;
	
	for ( int i = 0; i < 16; i++ ) 
	{
		byte a = colorBlock[i*4];
		int b1 = ( a <= ab1 );
		int b2 = ( a <= ab2 );
		int b3 = ( a <= ab3 );
		int b4 = ( a <= ab4 );
		int b5 = ( a <= ab5 );
		int b6 = ( a <= ab6 );
		int b7 = ( a <= ab7 );
		int index = ( b1 + b2 + b3 + b4 + b5 + b6 + b7 + 1 ) & 7;
		indices[i] = index ^ ( 2 > index );
	}
	EmitByte( (indices[ 0] >> 0) | (indices[ 1] << 3) | (indices[ 2] << 6) );
	EmitByte( (indices[ 2] >> 2) | (indices[ 3] << 1) | (indices[ 4] << 4) | (indices[ 5] << 7) );
	EmitByte( (indices[ 5] >> 1) | (indices[ 6] << 2) | (indices[ 7] << 5) );
	EmitByte( (indices[ 8] >> 0) | (indices[ 9] << 3) | (indices[10] << 6) );
	EmitByte( (indices[10] >> 2) | (indices[11] << 1) | (indices[12] << 4) | (indices[13] << 7) );
	EmitByte( (indices[13] >> 1) | (indices[14] << 2) | (indices[15] << 5) );
}
} //namespace realtime_dxt
