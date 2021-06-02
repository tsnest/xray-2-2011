///////////////////////////////////////////////////////////////////////
//  DepthTexture.inl
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
//  CDepthTextureDirectX9::CDepthTextureDirectX9

inline CDepthTextureDirectX9::CDepthTextureDirectX9( ) :
	m_pColorTexture(NULL),
	m_pDepthTexture(NULL),
	m_pColorDepthTexture(NULL),
	m_pDepthStencilSurface(NULL),
	m_pOldColorDepthTexture(NULL),
	m_pOldDepthStencilSurface(NULL),
	m_nBufferWidth(-1),
	m_nBufferHeight(-1),
	m_bInitSucceeded(false),
	m_bBoundAsTarget(false)
{
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureDirectX9::~CDepthTextureDirectX9

inline CDepthTextureDirectX9::~CDepthTextureDirectX9( )
{
	Release( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureDirectX9::GetTextureObject

inline LPDIRECT3DTEXTURE9 CDepthTextureDirectX9::GetTextureObject(void) const
{
	return m_pDepthTexture;
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureDirectX9::OnResetDevice

inline void CDepthTextureDirectX9::OnResetDevice(void)
{
	if (m_bInitSucceeded)
	{
		m_bInitSucceeded = false;
		Create(m_nBufferWidth, m_nBufferHeight);
	}
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureDirectX9::OnLostDevice

inline void CDepthTextureDirectX9::OnLostDevice(void)
{
	Release( );
}

