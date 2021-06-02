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
//
//  *** Release version 5.2.0 ***


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::ApplyStates

inline void CRenderStateOpenGL::ApplyStates(void)
{
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::Destroy

inline void CRenderStateOpenGL::Destroy(void)
{
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::ClearScreen

inline void CRenderStateOpenGL::ClearScreen(st_bool bClearColor, st_bool bClearDepth)
{
	GLbitfield uiGlBitField = (bClearColor ? GL_COLOR_BUFFER_BIT : 0) |
							  (bClearDepth ? GL_DEPTH_BUFFER_BIT : 0);
	glClear(uiGlBitField);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetAlphaFunction

inline void CRenderStateOpenGL::SetAlphaFunction(EAlphaFunc eFunc, st_float32 fValue)
{
	(eFunc == ALPHAFUNC_GREATER) ? glAlphaFunc(GL_GREATER, fValue / 255.0f) : glAlphaFunc(GL_LESS, fValue / 255.0f);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetAlphaTesting

inline void CRenderStateOpenGL::SetAlphaTesting(st_bool bFlag)
{
	bFlag ?	glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetBlending

inline void CRenderStateOpenGL::SetBlending(st_bool bFlag)
{
	if (bFlag)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetColorMask

inline void CRenderStateOpenGL::SetColorMask(st_bool bRed, st_bool bGreen, st_bool bBlue, st_bool bAlpha)
{
	glColorMask(bRed, bGreen, bBlue, bAlpha);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetDepthMask

inline void CRenderStateOpenGL::SetDepthMask(st_bool bFlag)
{
	bFlag ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetDepthTestFunc

inline void CRenderStateOpenGL::SetDepthTestFunc(EDepthTestFunc eDepthTestFunc)
{
	GLenum aDepthFuncs[ ] =
	{
		GL_NEVER,
		GL_LESS,
		GL_EQUAL,
		GL_LEQUAL,
		GL_GREATER,
		GL_NOTEQUAL,
		GL_GEQUAL,
		GL_ALWAYS
	};

	glDepthFunc(aDepthFuncs[eDepthTestFunc]);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetDepthTesting

inline void CRenderStateOpenGL::SetDepthTesting(st_bool bFlag)
{
	bFlag ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetFaceCulling

inline void CRenderStateOpenGL::SetFaceCulling(ECullType eCullType)
{
	if (eCullType == CULLTYPE_NONE)
		glDisable(GL_CULL_FACE);
	else 
	{
		glEnable(GL_CULL_FACE);
		if (eCullType == CULLTYPE_BACK)
			glCullFace(GL_BACK);
		else
			glCullFace(GL_FRONT);
	}
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetMultisampling

inline void CRenderStateOpenGL::SetMultisampling(st_bool bMultisample)
{
	if (bMultisample)
		glEnable(GL_MULTISAMPLE);
	else
		glDisable(GL_MULTISAMPLE);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetPointSize

inline void CRenderStateOpenGL::SetPointSize(st_float32 fSize)
{
	glPointSize(fSize);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetPolygonOffset

inline void CRenderStateOpenGL::SetPolygonOffset(st_float32 fFactor, st_float32 fUnits)
{
	if (fFactor == 0.0f && fUnits == 0.0f)
		glDisable(GL_POLYGON_OFFSET_FILL);
	else
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(fFactor, fUnits);
	}
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateOpenGL::SetRenderStyle

inline void CRenderStateOpenGL::SetRenderStyle(ERenderStyle eStyle)
{
	glPolygonMode(GL_FRONT_AND_BACK, (eStyle == RENDERSTYLE_WIREFRAME) ? GL_LINE : GL_FILL);
}

