/*  SCE CONFIDENTIAL
 *  PlayStation(R)3 Programmer Tool Runtime Library 300.001
 *  Copyright (C) 2008 Sony Computer Entertainment Inc.
 *  All Rights Reserved.
 */

#ifndef __CELL_GCMUTIL_ENUM_H__
#define __CELL_GCMUTIL_ENUM_H__

#include <math.h>
const float CELL_GCMUTIL_POSFIX = -1.0f;


#define CELL_GCMUTIL_ATTR_POSITION		(CG_ATTR0 - CG_ATTR0) //J 頂点（float4）
#define CELL_GCMUTIL_ATTR_BLENDWEIGHT	(CG_ATTR1 - CG_ATTR0) //J 頂点ウェイト（float）
#define CELL_GCMUTIL_ATTR_NORMAL		(CG_ATTR2 - CG_ATTR0) //J 法線（float3）
#define CELL_GCMUTIL_ATTR_COLOR			(CG_ATTR3 - CG_ATTR0) //J カラー（float4）
#define CELL_GCMUTIL_ATTR_COLOR0		(CG_ATTR3 - CG_ATTR0) //J カラー（float4）
#define CELL_GCMUTIL_ATTR_DIFFUSE		(CG_ATTR3 - CG_ATTR0) //J カラー（float4）
#define CELL_GCMUTIL_ATTR_COLOR1		(CG_ATTR4 - CG_ATTR0) //J セカンダリカラー（float4）
#define CELL_GCMUTIL_ATTR_SPECULAR		(CG_ATTR4 - CG_ATTR0) //J セカンダリカラー（float4）
#define CELL_GCMUTIL_ATTR_FOGCOORD		(CG_ATTR5 - CG_ATTR0) //J フォグ座標（float）
#define CELL_GCMUTIL_ATTR_TESSFACTOR	(CG_ATTR5 - CG_ATTR0) //J フォグ座標（float）
#define CELL_GCMUTIL_ATTR_PSIZE			(CG_ATTR6  - CG_ATTR0) //J ポイントサイズ（float）
#define CELL_GCMUTIL_ATTR_BLENDINDICES	(CG_ATTR7  - CG_ATTR0) //J スキニング用のパレットインデックス（float4）
#define CELL_GCMUTIL_ATTR_TEXCOORD0		(CG_ATTR8  - CG_ATTR0) //J テクスチャ座標0（float4）
#define CELL_GCMUTIL_ATTR_TEXCOORD1		(CG_ATTR9  - CG_ATTR0) //J テクスチャ座標1（float4）
#define CELL_GCMUTIL_ATTR_TEXCOORD2		(CG_ATTR10 - CG_ATTR0) //J テクスチャ座標2（float4）
#define CELL_GCMUTIL_ATTR_TEXCOORD3		(CG_ATTR11 - CG_ATTR0) //J テクスチャ座標3（float4）
#define CELL_GCMUTIL_ATTR_TEXCOORD4		(CG_ATTR12 - CG_ATTR0) //J テクスチャ座標4（float4）
#define CELL_GCMUTIL_ATTR_TEXCOORD5		(CG_ATTR13 - CG_ATTR0) //J テクスチャ座標5（float4）
#define CELL_GCMUTIL_ATTR_TEXCOORD6		(CG_ATTR14 - CG_ATTR0) //J テクスチャ座標6（float4）
#define CELL_GCMUTIL_ATTR_TANGENT		(CG_ATTR14 - CG_ATTR0) //J 正接ベクトル（float4）
#define CELL_GCMUTIL_ATTR_TEXCOORD7		(CG_ATTR15 - CG_ATTR0) //J テクスチャ座標7（float4）
#define CELL_GCMUTIL_ATTR_BINORMAL		(CG_ATTR15 - CG_ATTR0) //J 従法線ベクトル（float4）


#endif /* __CELL_GCMUTIL_ENUM_H__ */

