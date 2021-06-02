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
//  CRenderStateDirectX10::ClearScreen

inline void CRenderStateDirectX10::ClearScreen(st_bool bClearColor, st_bool bClearDepth)
{
	if (bClearColor)
		DX10::Device( )->ClearRenderTargetView(DX10::MainRenderTargetView( ), D3DXVECTOR4(0, 0, 0, 1));

	if (bClearDepth)
		DX10::Device( )->ClearDepthStencilView(DX10::MainDepthStencilView( ), D3D10_CLEAR_DEPTH, 1.0f, 0);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX10::SetAlphaFunction

inline void CRenderStateDirectX10::SetAlphaFunction(EAlphaFunc /*eFunc*/, st_float32 /*fValue*/)
{
	assert(DX10::Device( ));
	// alpha testing state gone in d3d10
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX10::SetAlphaTesting

inline void CRenderStateDirectX10::SetAlphaTesting(st_bool /*bFlag*/)
{
	assert(DX10::Device( ));
	// alpha testing state gone in d3d10
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateDirectX10::SetPointSize

inline void CRenderStateDirectX10::SetPointSize(st_float32 /*fSize*/)
{
	assert(DX10::Device( ));
	//DX10::Device( )->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&fSize));
}

