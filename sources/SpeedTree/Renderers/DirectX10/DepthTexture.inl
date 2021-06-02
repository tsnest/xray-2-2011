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
//  CDepthTextureDirectX10::CDepthTextureDirectX10

inline CDepthTextureDirectX10::CDepthTextureDirectX10( ) :
	m_pColorTexture(NULL),
	m_pDepthTexture(NULL),
	m_pDepthTextureView(NULL),
	m_pDepthStencilView(NULL),
	m_pOldDepthTextureView(NULL),
	m_pOldDepthStencilView(NULL),
	m_nBufferWidth(-1),
	m_nBufferHeight(-1),
	m_bInitSucceeded(false),
	m_bBoundAsTarget(false)
{
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureDirectX10::~CDepthTextureDirectX10

inline CDepthTextureDirectX10::~CDepthTextureDirectX10( )
{
	Release( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureDirectX10::GetTextureObject

inline ID3D10ShaderResourceView* CDepthTextureDirectX10::GetTextureObject(void) const
{
	return m_pDepthTexture;
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureDirectX10::OnResetDevice

inline void CDepthTextureDirectX10::OnResetDevice(void)
{
	if (m_bInitSucceeded)
	{
		m_bInitSucceeded = false;
		Create(m_nBufferWidth, m_nBufferHeight);
	}
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureDirectX10::OnLostDevice

inline void CDepthTextureDirectX10::OnLostDevice(void)
{
	Release( );
}

