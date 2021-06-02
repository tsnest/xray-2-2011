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


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX10::CShaderTechniqueDirectX10

inline CShaderTechniqueDirectX10::CShaderTechniqueDirectX10( ) :
	m_pTechnique(NULL)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX10::~CShaderTechniqueDirectX10

inline CShaderTechniqueDirectX10::~CShaderTechniqueDirectX10( )
{
#ifdef _DEBUG
	m_pTechnique = NULL;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX10::Bind

inline st_bool CShaderTechniqueDirectX10::Bind(void)
{
	CRenderState::ApplyStates( );
	return (m_pTechnique->GetPassByIndex(0)->Apply(0) == S_OK);
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX10::UnBind

inline st_bool CShaderTechniqueDirectX10::UnBind(void)
{
	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX10::CommitConstants

inline st_bool CShaderTechniqueDirectX10::CommitConstants(void)
{
	CRenderState::ApplyStates( );
	return (m_pTechnique->GetPassByIndex(0)->Apply(0) == S_OK);
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX10::CommitTextures

inline st_bool CShaderTechniqueDirectX10::CommitTextures(void)
{
	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX10::IsValid

inline st_bool CShaderTechniqueDirectX10::IsValid(void) const
{
	return (m_pTechnique != NULL && m_pTechnique->IsValid( ));
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX10::GetDX10Technique

inline ID3D10EffectTechnique* CShaderTechniqueDirectX10::GetDX10Technique(void) const
{
	return m_pTechnique;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::CShaderConstantDirectX10

inline CShaderConstantDirectX10::CShaderConstantDirectX10( ) :
	m_pParameter(NULL)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::~CShaderConstantDirectX10

inline CShaderConstantDirectX10::~CShaderConstantDirectX10( )
{
#ifdef _DEBUG
	m_pParameter = NULL;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::IsValid

inline st_bool CShaderConstantDirectX10::IsValid(void) const
{
	return (m_pParameter != NULL && m_pParameter->IsValid( ));
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::Set1f

inline st_bool CShaderConstantDirectX10::Set1f(st_float32 x) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		bSuccess = (m_pParameter->AsScalar( )->SetFloat(x) == S_OK);
#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::Set2f

inline st_bool CShaderConstantDirectX10::Set2f(st_float32 x, st_float32 y) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		st_float32 afVector[4] = { x, y, 0.0f, 0.0f }; 
		bSuccess = (m_pParameter->AsVector( )->SetFloatVector(afVector) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::Set2fv

inline st_bool CShaderConstantDirectX10::Set2fv(const st_float32 afValues[2]) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		st_float32 afVector[4] = { afValues[0], afValues[1], 0.0f, 0.0f }; 
		bSuccess = (m_pParameter->AsVector( )->SetFloatVector(afVector) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::Set2f

inline st_bool CShaderConstantDirectX10::Set3f(st_float32 x, st_float32 y, st_float32 z) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		st_float32 afVector[4] = { x, y, z, 0.0f }; 
		bSuccess = (m_pParameter->AsVector( )->SetFloatVector(afVector) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::Set3fv

inline st_bool CShaderConstantDirectX10::Set3fv(const st_float32 afValues[3]) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		st_float32 afVector[4] = { afValues[0], afValues[1], afValues[2], 0.0f }; 
		bSuccess = (m_pParameter->AsVector( )->SetFloatVector(afVector) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::Set3fvPlus1f

inline st_bool CShaderConstantDirectX10::Set3fvPlus1f(const st_float32 afValues[3], st_float32 w) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		st_float32 afVector[4] = { afValues[0], afValues[1], afValues[2], w }; 
		bSuccess = (m_pParameter->AsVector( )->SetFloatVector(afVector) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::Set4f

inline st_bool CShaderConstantDirectX10::Set4f(st_float32 x, st_float32 y, st_float32 z, st_float32 w) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		st_float32 afVector[4] = { x, y, z, w }; 
		bSuccess = (m_pParameter->AsVector( )->SetFloatVector(afVector) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::Set4fv

inline st_bool CShaderConstantDirectX10::Set4fv(const st_float32 afValues[4]) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		bSuccess = (m_pParameter->AsVector( )->SetFloatVector(const_cast<st_float32*>(afValues)) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::SetArray4f

inline st_bool CShaderConstantDirectX10::SetArray4f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ) && pArray)
	{
#endif
		bSuccess = (m_pParameter->AsVector( )->SetFloatVectorArray(const_cast<st_float32*>(pArray), nOffset, nSize) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::SetMatrix

inline st_bool CShaderConstantDirectX10::SetMatrix(const st_float32 afMatrix[16]) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		bSuccess = (m_pParameter->AsMatrix( )->SetMatrixTranspose(const_cast<st_float32*>(afMatrix)) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::SetMatrixTranspose

inline st_bool CShaderConstantDirectX10::SetMatrixTranspose(const st_float32 afMatrix[16]) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		bSuccess = (m_pParameter->AsMatrix( )->SetMatrix(const_cast<st_float32*>(afMatrix)) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::SetMatrixIndex

inline st_bool CShaderConstantDirectX10::SetMatrixIndex(const st_float32 afMatrix[16], st_uint32 uiIndex) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		bSuccess = (m_pParameter->GetElement(uiIndex)->AsMatrix( )->SetMatrixTranspose(const_cast<st_float32*>(afMatrix)) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX10::SetMatrix4x4Array

inline st_bool CShaderConstantDirectX10::SetMatrix4x4Array(const st_float32* pMatrixArray, st_uint32 uiNumMatrices) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ) && pMatrixArray)
	{
#endif
		bSuccess = (m_pParameter->AsMatrix( )->SetMatrixTransposeArray(const_cast<st_float32*>(pMatrixArray), 0, uiNumMatrices) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstant::SetTexture

inline st_bool CShaderConstant::SetTexture(const CTexture& texTexture) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		bSuccess = (m_tShaderConstantPolicy.m_pParameter->AsShaderResource( )->SetResource(texTexture.m_tTexturePolicy.GetTextureObject( )) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstant::SetTexture

inline st_bool CShaderConstant::SetTexture(const CDepthTexture& cDepthTexture) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (IsValid( ))
	{
#endif
		bSuccess = (m_tShaderConstantPolicy.m_pParameter->AsShaderResource( )->SetResource(cDepthTexture.m_tDepthTexturePolicy.GetTextureObject( )) == S_OK);

#ifdef _DEBUG	
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX10::CShaderLoaderDirectX10

inline CShaderLoaderDirectX10::CShaderLoaderDirectX10( ) :
	m_pEffect(NULL)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX10::~CShaderLoaderDirectX10

inline CShaderLoaderDirectX10::~CShaderLoaderDirectX10( )
{
	Release( );

#ifdef _DEBUG
	m_pEffect = NULL;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX10::GetError

inline const char* CShaderLoaderDirectX10::GetError(void) const
{
	return m_strError.c_str( );
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX10::OnResetDevice

inline void CShaderLoaderDirectX10::OnResetDevice(void)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX10::OnLostDevice

inline void CShaderLoaderDirectX10::OnLostDevice(void)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX10::GetTextureConstantName

inline CFixedString CShaderLoaderDirectX10::GetTextureConstantName(const CFixedString& strBaseName)
{
	return CFixedString("g_t") + strBaseName;
}
