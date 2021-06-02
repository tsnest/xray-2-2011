///////////////////////////////////////////////////////////////////////
//  RenderState.inl
//
//	*** INTERACTIVE DATA VISUALIZATION (IDV) CONFIDENTIAL AND PROPRIETARY INFORMATION ***
//
//	This software is supplied under the terms of a license agreement or
//	nondisclosure agreement with Interactive Data Visualization, Inc. and
//  may not be copied, disclosed, or exploited except in accordance with 
//  the terms of that agreement.
//
//      Copyright (c) 2003-2010 IDV, Inc.
//      All rights reserved in all media.
//
//      IDV, Inc.
//      http://www.idvinc.com


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::ApplyStates

inline void CRenderStateDirectX9::ApplyStates(void)
{
}

///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::Destroy

inline void CRenderStateDirectX9::Destroy(void)
{
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::ClearScreen

inline void CRenderStateDirectX9::ClearScreen(st_bool bClearColor, st_bool bClearDepth)
{
	DWORD dwDirectXBitField = (bClearColor ? D3DCLEAR_TARGET : 0) |
							  (bClearDepth ? D3DCLEAR_ZBUFFER : 0);
	DX9::Device( )->Clear(0, NULL, dwDirectXBitField, 0xff0000ff, 1.0f, 0);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetAlphaFunction

inline void CRenderStateDirectX9::SetAlphaFunction(EAlphaFunc eFunc, st_float32 fValue)
{
	assert(DX9::Device( ));

	DX9::Device( )->SetRenderState(D3DRS_ALPHAREF, DWORD(fValue));
	DX9::Device( )->SetRenderState(D3DRS_ALPHAFUNC, (eFunc == ALPHAFUNC_GREATER) ? D3DCMP_GREATER : D3DCMP_LESS);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetAlphaTesting

inline void CRenderStateDirectX9::SetAlphaTesting(st_bool bFlag)
{
	assert(DX9::Device( ));

	DX9::Device( )->SetRenderState(D3DRS_ALPHATESTENABLE, bFlag ? TRUE : FALSE);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetBlending

inline void CRenderStateDirectX9::SetBlending(st_bool bFlag)
{
	assert(DX9::Device( ));

	if (bFlag)
	{
		DX9::Device( )->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		DX9::Device( )->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		DX9::Device( )->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	else
		DX9::Device( )->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetColorMask

inline void CRenderStateDirectX9::SetColorMask(st_bool bRed, st_bool bGreen, st_bool bBlue, st_bool bAlpha)
{
	DWORD dwMask = 0;

	if (bRed)
		dwMask |= D3DCOLORWRITEENABLE_RED;
	if (bGreen)
		dwMask |= D3DCOLORWRITEENABLE_GREEN;
	if (bBlue)
		dwMask |= D3DCOLORWRITEENABLE_BLUE;
	if (bAlpha)
		dwMask |= D3DCOLORWRITEENABLE_ALPHA;

	DX9::Device( )->SetRenderState(D3DRS_COLORWRITEENABLE, dwMask);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetDepthMask

inline void CRenderStateDirectX9::SetDepthMask(st_bool bFlag)
{
	assert(DX9::Device( ));

	DX9::Device( )->SetRenderState(D3DRS_ZWRITEENABLE, bFlag ? TRUE : FALSE);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetDepthTestFunc

inline void CRenderStateDirectX9::SetDepthTestFunc(EDepthTestFunc eDepthTestFunc)
{
	assert(DX9::Device( ));

	static DWORD aCmpFuncs[ ] =
	{
		D3DCMP_NEVER,
		D3DCMP_LESS,
		D3DCMP_EQUAL,
		D3DCMP_LESSEQUAL,
		D3DCMP_GREATER,
		D3DCMP_NOTEQUAL,
		D3DCMP_GREATEREQUAL,
		D3DCMP_ALWAYS
	};

	DX9::Device( )->SetRenderState(D3DRS_ZFUNC, aCmpFuncs[eDepthTestFunc]);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetDepthTesting

inline void CRenderStateDirectX9::SetDepthTesting(st_bool bFlag)
{
	assert(DX9::Device( ));

	DX9::Device( )->SetRenderState(D3DRS_ZENABLE, bFlag ? TRUE : FALSE);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetFaceCulling

inline void CRenderStateDirectX9::SetFaceCulling(ECullType eCullType)
{
	assert(DX9::Device( ));

	if (eCullType == CULLTYPE_BACK)
		DX9::Device( )->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	else if (eCullType == CULLTYPE_FRONT)
		DX9::Device( )->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	else
		DX9::Device( )->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetPointSize

inline void CRenderStateDirectX9::SetPointSize(st_float32 fSize)
{
	assert(DX9::Device( ));

	DX9::Device( )->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&fSize));
}


///////////////////////////////////////////////////////////////////////  
//  Utility Function: FloatToDWord

inline DWORD FloatToDWord(FLOAT f) 
{
	return *((DWORD*) &f); 
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetPolygonOffset

inline void CRenderStateDirectX9::SetPolygonOffset(st_float32 fFactor, st_float32 fUnits)
{
	assert(DX9::Device( ));

	// attempt to match DirectX to OpenGL's glPolygonOffset
	if (fFactor != 0.0f || fUnits != 0.0f)
	{
		fFactor = 1.0f;
		fUnits = 0.0004f;
	}

	DX9::Device( )->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, FloatToDWord(fFactor));
	DX9::Device( )->SetRenderState(D3DRS_DEPTHBIAS, FloatToDWord(fUnits));
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetMultisampling

inline void CRenderStateDirectX9::SetMultisampling(st_bool bMultisample)
{
	assert(DX9::Device( ));

	DX9::Device( )->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, bMultisample ? TRUE : FALSE);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX9::SetRenderStyle

inline void CRenderStateDirectX9::SetRenderStyle(ERenderStyle eStyle)
{
	assert(DX9::Device( ));

	DX9::Device( )->SetRenderState(D3DRS_FILLMODE, (eStyle == RENDERSTYLE_WIREFRAME) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
}
