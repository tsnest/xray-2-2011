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
//
//  *** Release version 5.2.0 ***


///////////////////////////////////////////////////////////////////////  
//  CTextureOpenGL::CTextureOpenGL

inline CTextureOpenGL::CTextureOpenGL( ) :
	m_uiTexture(0)
{
}


///////////////////////////////////////////////////////////////////////  
//  CTextureOpenGL::~CTextureOpenGL

inline CTextureOpenGL::~CTextureOpenGL( )
{
	(void) Unload( );
}


///////////////////////////////////////////////////////////////////////  
//  CTextureOpenGL::Load

inline st_bool CTextureOpenGL::Load(const char* pFilename, st_int32 nMaxAnisotropy)
{
	CFixedString strExtension = CFixedString(pFilename).Extension( );

#ifdef __GNUC__
	if (strcasecmp(strExtension.c_str( ), "tga") == 0)
#else
	if (_stricmp(strExtension.c_str( ), "tga") == 0)
#endif
		return LoadTGA(pFilename, nMaxAnisotropy);
	else
		return LoadDDS(pFilename, nMaxAnisotropy);
}


///////////////////////////////////////////////////////////////////////  
//  CTextureOpenGL::IsValid

inline st_bool CTextureOpenGL::IsValid(void) const
{
	return (m_uiTexture != 0u);
}


///////////////////////////////////////////////////////////////////////  
//  CTextureOpenGL::FixedFunctionBind

inline st_bool CTextureOpenGL::FixedFunctionBind(void)
{
	st_bool bSuccess = false;

	if (m_uiTexture != 0)
	{
		glBindTexture(GL_TEXTURE_2D, m_uiTexture);
		bSuccess = true;
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureOpenGL::GetTextureObject

inline st_uint32 CTextureOpenGL::GetTextureObject(void) const
{
	return m_uiTexture;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureOpenGL::operator=

inline CTextureOpenGL& CTextureOpenGL::operator=(const CTextureOpenGL& cRight)
{
	m_uiTexture = cRight.m_uiTexture;

	return *this;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureOpenGL::operator!=

inline st_bool CTextureOpenGL::operator!=(const CTextureOpenGL& cRight) const
{
	return (m_uiTexture != cRight.m_uiTexture);
}
