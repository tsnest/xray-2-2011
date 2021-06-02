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
//
//  *** Release version 5.2.0 ***


///////////////////////////////////////////////////////////////////////
//  CDepthTextureOpenGL::CDepthTextureOpenGL

inline CDepthTextureOpenGL::CDepthTextureOpenGL( ) :
	m_uiFrameBufferObject(0),
	m_uiTextureObject(0),
	m_nBufferWidth(-1),
	m_nBufferHeight(-1),
	m_bInitSucceeded(false),
	m_bBoundAsTarget(false)
{
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureOpenGL::~CDepthTextureOpenGL

inline CDepthTextureOpenGL::~CDepthTextureOpenGL( )
{
	Release( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureOpenGL::Release

inline void CDepthTextureOpenGL::Release(void)
{
	if (m_uiFrameBufferObject > 0)
	{
		glDeleteFramebuffersEXT(1, &m_uiFrameBufferObject);
		m_uiFrameBufferObject = 0;
	}

	if (m_uiTextureObject > 0)
	{
		glDeleteTextures(1, &m_uiTextureObject);
		m_uiTextureObject = 0;
	}

	m_bInitSucceeded = false;
	m_nBufferWidth = m_nBufferHeight = -1;
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureOpenGL::SetAsTarget

inline st_bool CDepthTextureOpenGL::SetAsTarget(void)
{
	st_bool bSuccess = false;

	if (!m_bBoundAsTarget && m_bInitSucceeded)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_uiFrameBufferObject);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0, 0, m_nBufferWidth, m_nBufferHeight);

		m_bBoundAsTarget = true;

		bSuccess = true;
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureOpenGL::ReleaseAsTarget

inline st_bool CDepthTextureOpenGL::ReleaseAsTarget(void)
{
	st_bool bSuccess = false;

	if (m_bInitSucceeded && m_bBoundAsTarget)
	{
		glPopAttrib( );
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
		
		m_bBoundAsTarget = false;

		bSuccess = true;
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureOpenGL::BindAsTexture

inline st_bool CDepthTextureOpenGL::BindAsTexture(void)
{
	st_bool bSuccess = false;

	if (m_bInitSucceeded)
	{
		glBindTexture(GL_TEXTURE_2D, m_uiTextureObject);
		bSuccess = true;
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureOpenGL::UnBindAsTexture

inline st_bool CDepthTextureOpenGL::UnBindAsTexture(void)
{
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureOpenGL::EnableShadowMapComparison

inline void CDepthTextureOpenGL::EnableShadowMapComparison(st_bool bEnable)
{
	BindAsTexture( );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, bEnable ? GL_COMPARE_R_TO_TEXTURE_ARB : GL_NONE);
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureOpenGL::GetTextureObject

inline GLuint CDepthTextureOpenGL::GetTextureObject(void) const
{
	return m_uiTextureObject;
}
