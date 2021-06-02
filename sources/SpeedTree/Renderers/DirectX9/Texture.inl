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
//  CTextureDirectX9::CTextureDirectX9

inline CTextureDirectX9::CTextureDirectX9( ) :
	m_pTexture(NULL),
	m_pCubeTexture(NULL)
{
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX9::~CTextureDirectX9

inline CTextureDirectX9::~CTextureDirectX9( )
{
	(void) Unload( );
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX9::IsValid

inline st_bool CTextureDirectX9::IsValid(void) const
{
	return (m_pTexture != NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX9::FixedFunctionBind

inline bool CTextureDirectX9::FixedFunctionBind(void)
{
	bool bSuccess = false;

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX9::GetTextureObject

inline LPDIRECT3DTEXTURE9 CTextureDirectX9::GetTextureObject(void) const
{
	return m_pTexture;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX9::GetCubeTextureObject

inline LPDIRECT3DCUBETEXTURE9 CTextureDirectX9::GetCubeTextureObject(void) const
{
	return m_pCubeTexture;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX9::operator=

inline CTextureDirectX9& CTextureDirectX9::operator=(const CTextureDirectX9& cRight)
{
	m_pTexture = cRight.m_pTexture;

	return *this;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureDirectX9::operator!=

inline st_bool CTextureDirectX9::operator!=(const CTextureDirectX9& cRight) const
{
	return (m_pTexture != cRight.m_pTexture);
}

