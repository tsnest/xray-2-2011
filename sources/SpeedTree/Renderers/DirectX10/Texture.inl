///////////////////////////////////////////////////////////////////////
//  Texture.inl
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
//  CTextureDirectX10::CTextureDirectX10

inline CTextureDirectX10::CTextureDirectX10( ) :
	m_pTexture(NULL)
{
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX10::~CTextureDirectX10

inline CTextureDirectX10::~CTextureDirectX10( )
{
	(void) Unload( );
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX10::IsValid

inline bool CTextureDirectX10::IsValid(void) const
{
	return (m_pTexture != NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX10::FixedFunctionBind

inline bool CTextureDirectX10::FixedFunctionBind(void)
{
	bool bSuccess = false;

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX10::GetTextureObject

inline ID3D10ShaderResourceView* CTextureDirectX10::GetTextureObject(void) const
{
	return m_pTexture;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX10::operator=

inline CTextureDirectX10& CTextureDirectX10::operator=(const CTextureDirectX10& cRight)
{
	m_pTexture = cRight.m_pTexture;

	return *this;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX10::operator!=

inline st_bool CTextureDirectX10::operator!=(const CTextureDirectX10& cRight) const
{
	return (m_pTexture != cRight.m_pTexture);
}

