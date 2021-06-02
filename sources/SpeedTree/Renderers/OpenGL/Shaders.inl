///////////////////////////////////////////////////////////////////////  
//  Shaders.inl
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
//  CShaderTechniqueOpenGL::CShaderTechniqueOpenGL

inline CShaderTechniqueOpenGL::CShaderTechniqueOpenGL( ) :
	m_cgVertexShader(NULL),
	m_cgPixelShader(NULL)
{
}


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueOpenGL::~CShaderTechniqueOpenGL

inline CShaderTechniqueOpenGL::~CShaderTechniqueOpenGL( )
{
	if (m_cgVertexShader)
	{
		cgDestroyProgram(m_cgVertexShader);
		m_cgVertexShader = NULL;
	}
	if (m_cgPixelShader)
	{
		cgDestroyProgram(m_cgPixelShader);
		m_cgPixelShader = NULL;
	}
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueOpenGL::Bind

inline st_bool CShaderTechniqueOpenGL::Bind(void)
{
	st_bool bSuccess = false;

	#ifndef NDEBUG
		if (m_cgVertexShader && m_cgPixelShader)
		{
	#endif
			cgGLBindProgram(m_cgVertexShader);
			cgGLEnableProfile(SPEEDTREE_CG_VERTEX_PROFILE);

			cgGLBindProgram(m_cgPixelShader);
			cgGLEnableProfile(SPEEDTREE_CG_FRAGMENT_PROFILE);
		
			bSuccess = true;
	#ifndef NDEBUG
		}
		else
			CCore::SetError("CShaderTechniqueOpenGL::Bind, either vertex or pixel shader was not valid");
	#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueOpenGL::UnBind

inline st_bool CShaderTechniqueOpenGL::UnBind(void)
{
	cgGLDisableProfile(SPEEDTREE_CG_VERTEX_PROFILE);
	cgGLDisableProfile(SPEEDTREE_CG_FRAGMENT_PROFILE);

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueOpenGL::CommitConstants

inline st_bool CShaderTechniqueOpenGL::CommitConstants(void)
{
	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueOpenGL::CommitTextures

inline st_bool CShaderTechniqueOpenGL::CommitTextures(void)
{
	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueOpenGL::IsValid

inline st_bool CShaderTechniqueOpenGL::IsValid(void) const
{
	return (m_cgVertexShader != NULL && m_cgPixelShader != NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::CShaderConstantOpenGL

inline CShaderConstantOpenGL::CShaderConstantOpenGL( ) :
	m_cgParameter(NULL),
	m_cgParameterMirror(NULL),
	m_nTexUnit(-1)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::~CShaderConstantOpenGL

inline CShaderConstantOpenGL::~CShaderConstantOpenGL( )
{
#ifndef NDEBUG
	m_cgParameter = NULL;
	m_cgParameterMirror = NULL;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::IsValid

inline st_bool CShaderConstantOpenGL::IsValid(void) const
{
	return (m_cgParameter != NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::Set1f

inline st_bool CShaderConstantOpenGL::Set1f(st_float32 x) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		cgGLSetParameter1f(m_cgParameter, x);
		if (m_cgParameterMirror)
			cgGLSetParameter1f(m_cgParameterMirror, x);

		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::Set2f

inline st_bool CShaderConstantOpenGL::Set2f(st_float32 x, st_float32 y) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		cgGLSetParameter2f(m_cgParameter, x, y);
		if (m_cgParameterMirror)
			cgGLSetParameter2f(m_cgParameterMirror, x, y);

		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::Set2fv

inline st_bool CShaderConstantOpenGL::Set2fv(const st_float32 afValues[2]) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		cgGLSetParameter2fv(m_cgParameter, afValues);
		if (m_cgParameterMirror)
			cgGLSetParameter2fv(m_cgParameterMirror, afValues);

		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::Set2f

inline st_bool CShaderConstantOpenGL::Set3f(st_float32 x, st_float32 y, st_float32 z) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		cgGLSetParameter3f(m_cgParameter, x, y, z);
		if (m_cgParameterMirror)
			cgGLSetParameter3f(m_cgParameterMirror, x, y, z);

		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::Set3fv

inline st_bool CShaderConstantOpenGL::Set3fv(const st_float32 afValues[3]) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		cgGLSetParameter3fv(m_cgParameter, afValues);
		if (m_cgParameterMirror)
			cgGLSetParameter3fv(m_cgParameterMirror, afValues);

		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::Set3fvPlus1f

inline st_bool CShaderConstantOpenGL::Set3fvPlus1f(const st_float32 afValues[3], st_float32 w) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		cgGLSetParameter4f(m_cgParameter, afValues[0], afValues[1], afValues[2], w);
		if (m_cgParameterMirror)
			cgGLSetParameter4f(m_cgParameterMirror, afValues[0], afValues[1], afValues[2], w);

		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::Set4f

inline st_bool CShaderConstantOpenGL::Set4f(st_float32 x, st_float32 y, st_float32 z, st_float32 w) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		cgGLSetParameter4f(m_cgParameter, x, y, z, w);
		if (m_cgParameterMirror)
			cgGLSetParameter4f(m_cgParameterMirror, x, y, z, w);

		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::Set4fv

inline st_bool CShaderConstantOpenGL::Set4fv(const st_float32 afValues[4]) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		cgGLSetParameter4fv(m_cgParameter, afValues);
		if (m_cgParameterMirror)
			cgGLSetParameter4fv(m_cgParameterMirror, afValues);

		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::SetArray1f

inline st_bool CShaderConstantOpenGL::SetArray1f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset) const
{
#ifndef NDEBUG
	if (IsValid( ) && pArray)
	{
#endif

#ifdef PS3
		cgGLSetParameterArray1f(m_cgParameter, nOffset, nSize, pArray);
		if (m_cgParameterMirror)
			cgGLSetParameterArray1f(m_cgParameterMirror, nOffset, nSize, pArray);
#else

		for (st_int32 i = 0; i < nSize; ++i)
		{
			CGparameter pElement = cgGetArrayParameter(m_cgParameter, i + nOffset);
			cgGLSetParameter1fv(pElement, pArray + i);
		}
		if (m_cgParameterMirror)
		{
			for (st_int32 i = 0; i < nSize; ++i)
			{
				CGparameter pElement = cgGetArrayParameter(m_cgParameterMirror, i + nOffset);
				cgGLSetParameter1fv(pElement, pArray + i);
			}
		}

		return true;
#endif

#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::SetArray3f

inline st_bool CShaderConstantOpenGL::SetArray3f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset) const
{
#ifndef NDEBUG
	if (IsValid( ) && pArray)
	{
#endif

#ifdef PS3
		cgGLSetParameterArray3f(m_cgParameter, nOffset, nSize, pArray);
		if (m_cgParameterMirror)
			cgGLSetParameterArray3f(m_cgParameterMirror, nOffset, nSize, pArray);
#else
		for (st_int32 i = 0; i < nSize; ++i)
		{
			CGparameter pElement = cgGetArrayParameter(m_cgParameter, i + nOffset);
			cgGLSetParameter3fv(pElement, pArray + i * 3);
		}
		if (m_cgParameterMirror)
		{
			for (st_int32 i = 0; i < nSize; ++i)
			{
				CGparameter pElement = cgGetArrayParameter(m_cgParameterMirror, i + nOffset);
				cgGLSetParameter3fv(pElement, pArray + i * 3);
			}
		}

		return true;
#endif

#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::SetArray4f

inline st_bool CShaderConstantOpenGL::SetArray4f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset) const
{
#ifndef NDEBUG
	if (IsValid( ) && pArray)
	{
#endif

#ifdef PS3
		cgGLSetParameterArray4f(m_cgParameter, nOffset, nSize, pArray);
		if (m_cgParameterMirror)
			cgGLSetParameterArray4f(m_cgParameterMirror, nOffset, nSize, pArray);
#else
		// this still doesn't work
		//cgGLSetParameterArray4f(m_cgParameter, nOffset, nSize, pArray);

		// alternate code (cgGLSetParameterArray4f() hasn't been reliable in the Windows version of Cg);
		// when cgGLSetParameterArray4f() doesn't work correctly, the symptom is largely unanimated leaf cards
		for (st_int32 i = 0; i < nSize; ++i)
		{
			CGparameter pElement = cgGetArrayParameter(m_cgParameter, i + nOffset);
			cgGLSetParameter4fv(pElement, pArray + i * 4);
		}
		if (m_cgParameterMirror)
		{
			for (st_int32 i = 0; i < nSize; ++i)
			{
				CGparameter pElement = cgGetArrayParameter(m_cgParameterMirror, i + nOffset);
				cgGLSetParameter4fv(pElement, pArray + i * 4);
			}
		}

		return true;
#endif

#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::SetMatrix

inline st_bool CShaderConstantOpenGL::SetMatrix(const st_float32 afMatrix[16]) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		cgGLSetMatrixParameterfc(m_cgParameter, afMatrix);
		if (m_cgParameterMirror)
			cgGLSetMatrixParameterfc(m_cgParameterMirror, afMatrix);
		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::SetMatrixTranspose

inline st_bool CShaderConstantOpenGL::SetMatrixTranspose(const st_float32 afMatrix[16]) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		cgGLSetMatrixParameterfr(m_cgParameter, afMatrix);
		if (m_cgParameterMirror)
			cgGLSetMatrixParameterfr(m_cgParameterMirror, afMatrix);
		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::SetMatrixIndex

inline st_bool CShaderConstantOpenGL::SetMatrixIndex(const st_float32 afMatrix[16], st_uint32 uiIndex) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif

	// alternate code (cgGLSetMatrixParameterArrayfr wasn't reliable)
	CGparameter pElement = cgGetArrayParameter(m_cgParameter, uiIndex);
	cgGLSetMatrixParameterfc(pElement, afMatrix);

	if (m_cgParameterMirror)
	{
		CGparameter pElementMirror = cgGetArrayParameter(m_cgParameterMirror, uiIndex);
		cgGLSetMatrixParameterfc(pElementMirror, afMatrix);
	}

	return true;

#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantOpenGL::SetMatrix4x4Array

inline st_bool CShaderConstantOpenGL::SetMatrix4x4Array(const st_float32* pMatrixArray, st_uint32 uiOffset, st_uint32 uiNumMatrices) const
{
#ifndef NDEBUG
	if (IsValid( ) && pMatrixArray)
	{
#endif
		cgGLSetMatrixParameterArrayfr(m_cgParameter, uiOffset, uiNumMatrices, pMatrixArray);
		if (m_cgParameterMirror)
			cgGLSetMatrixParameterArrayfr(m_cgParameterMirror, uiOffset, uiNumMatrices, pMatrixArray);
		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstant::SetTexture

inline st_bool CShaderConstant::SetTexture(const CTexture& texTexture) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif

		cgGLSetupSampler(m_tShaderConstantPolicy.m_cgParameter, texTexture.m_tTexturePolicy.GetTextureObject( ));

		// as of Cg 2.2, we had a difficult time getting textures to bind correctly; Cg would not correctly bind
		// to the texture unit we specified in the shader. Cg's function cgGLGetTextureEnum() should correctly 
		// return the texture unit but fails, returning CG_INVALID_OPERATION.  We extract the texture unit number 
		// using cgGetParameterSemantic() in CShaderTechniqueOpenGL::ProcessParameterSemantic() and manually bind 
		// the texture to the correct layer here.

		// only attempt to manually set the texture layer if m_nTexUnit was correctly determined during init
		if (m_tShaderConstantPolicy.m_nTexUnit > -1)
		{
			glActiveTextureARB(GL_TEXTURE0_ARB + m_tShaderConstantPolicy.m_nTexUnit);
			glBindTexture(GL_TEXTURE_2D, texTexture.m_tTexturePolicy.GetTextureObject( ));

			cgSetSamplerState(m_tShaderConstantPolicy.m_cgParameter);
			cgGLEnableTextureParameter(m_tShaderConstantPolicy.m_cgParameter);

			glActiveTextureARB(GL_TEXTURE0_ARB);
		}
		else
			glBindTexture(GL_TEXTURE_2D, texTexture.m_tTexturePolicy.GetTextureObject( ));

		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstant::SetTexture

inline st_bool CShaderConstant::SetTexture(const CDepthTexture& cDepthTexture) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif

		// as of Cg 2.2, we had a difficult time getting textures to bind correctly; Cg would not correctly bind
		// to the texture unit we specified in the shader. Cg's function cgGLGetTextureEnum() should correctly 
		// return the texture unit but fails, returning CG_INVALID_OPERATION.  We extract the texture unit number 
		// using cgGetParameterSemantic() in CShaderTechniqueOpenGL::ProcessParameterSemantic() and manually bind 
		// the texture to the correct layer here.

		// only attempt to manually set the texture layer if m_nTexUnit was correctly determined during init
		if (m_tShaderConstantPolicy.m_nTexUnit > -1)
		{
			glActiveTextureARB(GL_TEXTURE0_ARB + m_tShaderConstantPolicy.m_nTexUnit);
			glBindTexture(GL_TEXTURE_2D, cDepthTexture.m_tDepthTexturePolicy.GetTextureObject( ));

			glActiveTextureARB(GL_TEXTURE0_ARB);
		}
		else
			glBindTexture(GL_TEXTURE_2D, cDepthTexture.m_tDepthTexturePolicy.GetTextureObject( ));

		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstant::SetTextureIndex

inline st_bool CShaderConstant::SetTextureIndex(const CTexture& texTexture, st_uint32 uiIndex) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		CGparameter cgElement = cgGetArrayParameter(m_tShaderConstantPolicy.m_cgParameter, uiIndex);
		cgGLSetupSampler(cgElement, texTexture.m_tTexturePolicy.GetTextureObject( ));
		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstant::SetTextureIndex

inline st_bool CShaderConstant::SetTextureIndex(const CDepthTexture& cDepthTexture, st_uint32 uiIndex) const
{
#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		CGparameter cgElement = cgGetArrayParameter(m_tShaderConstantPolicy.m_cgParameter, uiIndex);
		cgGLSetupSampler(cgElement, cDepthTexture.m_tDepthTexturePolicy.GetTextureObject( ));
		return true;
#ifndef NDEBUG	
	}
	else
		return false;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderOpenGL::CShaderLoaderOpenGL

inline CShaderLoaderOpenGL::CShaderLoaderOpenGL( ) :
	m_pShaderMacros(NULL)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderOpenGL::~CShaderLoaderOpenGL

inline CShaderLoaderOpenGL::~CShaderLoaderOpenGL( )
{
	// Cg context is referenced counted, so an actual release may not occur
	ReleaseCgContext( );

	// destroy the shader macros array
	const char** pDefine = m_pShaderMacros;
	while (*pDefine)
		st_delete_array<const char>(*pDefine++);
	st_delete_array<const char*>(m_pShaderMacros);
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderOpenGL::GetError

inline const char* CShaderLoaderOpenGL::GetError(void) const
{
	return m_strError.c_str( );
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderOpenGL::GetTextureConstantName

inline CFixedString CShaderLoaderOpenGL::GetTextureConstantName(const CFixedString& strBaseName)
{
	return CFixedString("sam") + strBaseName;
}

