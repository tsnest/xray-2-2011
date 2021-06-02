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
//  CShaderTechniqueDirectX9::CShaderTechniqueDirectX9

inline CShaderTechniqueDirectX9::CShaderTechniqueDirectX9( ) :
	m_pVertexShader(NULL),
	m_pPixelShader(NULL),
	m_pVertexConstantTable(NULL),
	m_pPixelConstantTable(NULL)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX9::~CShaderTechniqueDirectX9

inline CShaderTechniqueDirectX9::~CShaderTechniqueDirectX9( )
{
	if (m_pVertexShader)
	{
		m_pVertexShader->Release( );
		m_pVertexShader = NULL;
	}
	if (m_pPixelShader)
	{
		m_pPixelShader->Release( );
		m_pPixelShader = NULL;
	}
	if (m_pVertexConstantTable)
	{
		m_pVertexConstantTable->Release( );
		m_pVertexConstantTable = NULL;
	}
	if (m_pPixelConstantTable)
	{
		m_pPixelConstantTable->Release( );
		m_pPixelConstantTable = NULL;
	}
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX9::GetVertexConstantTable

inline LPD3DXCONSTANTTABLE CShaderTechniqueDirectX9::GetVertexConstantTable(void)
{
	return m_pVertexConstantTable;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX9::GetPixelConstantTable

inline LPD3DXCONSTANTTABLE CShaderTechniqueDirectX9::GetPixelConstantTable(void)
{
	return m_pPixelConstantTable;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX9::Bind

inline st_bool CShaderTechniqueDirectX9::Bind(void)
{
	st_bool bSuccess = false;

	#ifndef NDEBUG
		if (m_pVertexShader && m_pPixelShader)
		{
	#endif
			// vertex shader
			DX9::Device( )->SetVertexShader(m_pVertexShader);
			if (m_pVertexConstantTable)
				m_pVertexConstantTable->SetDefaults(DX9::Device( ));

			// pixel shader
			DX9::Device( )->SetPixelShader(m_pPixelShader);
			if (m_pPixelConstantTable)
				m_pPixelConstantTable->SetDefaults(DX9::Device( ));
		
			bSuccess = true;
	#ifndef NDEBUG
		}
		else
			CCore::SetError("CShaderTechniqueDirectX9::Bind, either vertex or pixel shader was not valid");
	#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX9::UnBind

inline st_bool CShaderTechniqueDirectX9::UnBind(void)
{
	DX9::Device( )->SetVertexShader(NULL);
	DX9::Device( )->SetPixelShader(NULL);

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX9::CommitConstants

inline st_bool CShaderTechniqueDirectX9::CommitConstants(void)
{
	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX9::CommitTextures

inline st_bool CShaderTechniqueDirectX9::CommitTextures(void)
{
	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderTechniqueDirectX9::IsValid

inline st_bool CShaderTechniqueDirectX9::IsValid(void) const
{
	return (m_pVertexShader != NULL && 
		    m_pPixelShader != NULL &&
			m_pVertexConstantTable != NULL &&
			m_pPixelConstantTable != NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::CShaderConstantDirectX9

inline CShaderConstantDirectX9::CShaderConstantDirectX9( ) :
	m_hVertexParameter(NULL),
	m_hPixelParameter(NULL),
	m_pTechnique(NULL)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::~CShaderConstantDirectX9

inline CShaderConstantDirectX9::~CShaderConstantDirectX9( )
{
#ifndef NDEBUG
	m_hVertexParameter = NULL;
	m_hPixelParameter = NULL;
	m_pTechnique = NULL;
#endif
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::IsValid

inline st_bool CShaderConstantDirectX9::IsValid(void) const
{
	return (m_hVertexParameter && m_pTechnique->GetVertexConstantTable( )) || 
		   (m_hPixelParameter && m_pTechnique->GetPixelConstantTable( ));
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::Set1f

inline st_bool CShaderConstantDirectX9::Set1f(st_float32 x) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetFloat(DX9::Device( ), m_hVertexParameter, x));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetFloat(DX9::Device( ), m_hPixelParameter, x));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}



///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::Set2f

inline st_bool CShaderConstantDirectX9::Set2f(st_float32 x, st_float32 y) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		D3DXVECTOR4 vVector(x, y, 0.0f, 0.0f); 
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetVector(DX9::Device( ), m_hVertexParameter, &vVector));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetVector(DX9::Device( ), m_hPixelParameter, &vVector));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::Set2fv

inline st_bool CShaderConstantDirectX9::Set2fv(const st_float32 afValues[2]) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		D3DXVECTOR4 vVector(afValues[0], afValues[1], 0.0f, 0.0f); 
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetVector(DX9::Device( ), m_hVertexParameter, &vVector));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetVector(DX9::Device( ), m_hPixelParameter, &vVector));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::Set3f

inline st_bool CShaderConstantDirectX9::Set3f(st_float32 x, st_float32 y, st_float32 z) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		D3DXVECTOR4 vVector(x, y, z, 0.0f); 
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetVector(DX9::Device( ), m_hVertexParameter, &vVector));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetVector(DX9::Device( ), m_hPixelParameter, &vVector));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::Set3fv

inline st_bool CShaderConstantDirectX9::Set3fv(const st_float32 afValues[3]) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		D3DXVECTOR4 vVector(afValues[0], afValues[1], afValues[2], 0.0f); 
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetVector(DX9::Device( ), m_hVertexParameter, &vVector));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetVector(DX9::Device( ), m_hPixelParameter, &vVector));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::Set3fvPlus1f

inline st_bool CShaderConstantDirectX9::Set3fvPlus1f(const st_float32 afValues[3], st_float32 w) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		D3DXVECTOR4 vVector(afValues[0], afValues[1], afValues[2], w); 
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetVector(DX9::Device( ), m_hVertexParameter, &vVector));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetVector(DX9::Device( ), m_hPixelParameter, &vVector));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::Set4f

inline st_bool CShaderConstantDirectX9::Set4f(st_float32 x, st_float32 y, st_float32 z, st_float32 w) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		D3DXVECTOR4 vVector(x, y, z, w);
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetVector(DX9::Device( ), m_hVertexParameter, &vVector));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetVector(DX9::Device( ), m_hPixelParameter, &vVector));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::Set4fv

inline st_bool CShaderConstantDirectX9::Set4fv(const st_float32 afValues[4]) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		const D3DXVECTOR4* pVector = reinterpret_cast<const D3DXVECTOR4*>(afValues);
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetVector(DX9::Device( ), m_hVertexParameter, pVector));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetVector(DX9::Device( ), m_hPixelParameter, pVector));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::SetArray4f

inline st_bool CShaderConstantDirectX9::SetArray4f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ) && pArray)
	{
#endif
		if (m_hVertexParameter)
		{
			D3DXHANDLE hElement = (nOffset > 0) ? m_pTechnique->GetVertexConstantTable( )->GetConstantElement(m_hVertexParameter, nOffset) : m_hVertexParameter;
			if (hElement)
				bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetVectorArray(DX9::Device( ), hElement, (const D3DXVECTOR4*) pArray, nSize));
			else
				CCore::SetError("CShaderConstantDirectX9::SetArray4f, GetConstantElement1 failed");
		}

		if (m_hPixelParameter)
		{
			D3DXHANDLE hElementMirror = (nOffset > 0) ? m_pTechnique->GetPixelConstantTable( )->GetConstantElement(m_hPixelParameter, nOffset) : m_hPixelParameter;
			if (hElementMirror)
				bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetVectorArray(DX9::Device( ), hElementMirror, (const D3DXVECTOR4*) pArray, nSize));
			else
				CCore::SetError("CShaderConstantDirectX9::SetArray4f, GetConstantElement2 failed");
		}
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::SetMatrix

inline st_bool CShaderConstantDirectX9::SetMatrix(const st_float32 afMatrix[16]) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		D3DXMATRIX cMatrix(afMatrix);
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetMatrixTranspose(DX9::Device( ), m_hVertexParameter, &cMatrix));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetMatrixTranspose(DX9::Device( ), m_hPixelParameter, &cMatrix));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::SetMatrixTranspose

inline st_bool CShaderConstantDirectX9::SetMatrixTranspose(const st_float32 afMatrix[16]) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		D3DXMATRIX cMatrix(afMatrix);
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetMatrix(DX9::Device( ), m_hVertexParameter, &cMatrix));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetMatrix(DX9::Device( ), m_hPixelParameter, &cMatrix));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::SetMatrixIndex

inline st_bool CShaderConstantDirectX9::SetMatrixIndex(const st_float32 afMatrix[16], st_uint32 uiIndex) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		D3DXMATRIX cMatrix(afMatrix);
		if (m_hVertexParameter)
		{
			D3DXHANDLE hElement = (uiIndex > 0) ? m_pTechnique->GetVertexConstantTable( )->GetConstantElement(m_hVertexParameter, uiIndex) : m_hVertexParameter;
			if (hElement)
				bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetMatrixTranspose(DX9::Device( ), hElement, &cMatrix));
			else
				CCore::SetError("CShaderConstantDirectX9::SetMatrixIndex, GetConstantElement1 failed");
		}

		if (m_hPixelParameter)
		{
			D3DXHANDLE hElement = (uiIndex > 0) ? m_pTechnique->GetPixelConstantTable( )->GetConstantElement(m_hPixelParameter, uiIndex) : m_hPixelParameter;
			if (hElement)
				bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetMatrixTranspose(DX9::Device( ), hElement, &cMatrix));
			else
				CCore::SetError("CShaderConstantDirectX9::SetMatrixIndex, GetConstantElement2 failed");
		}
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstantDirectX9::SetMatrix4x4Array

inline st_bool CShaderConstantDirectX9::SetMatrix4x4Array(const st_float32* pMatrixArray, st_uint32 uiNumMatrices) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ) && pMatrixArray)
	{
#endif
		if (m_hVertexParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetVertexConstantTable( )->SetMatrixTransposeArray(DX9::Device( ), m_hVertexParameter, (const D3DXMATRIX*) pMatrixArray, uiNumMatrices));
		if (m_hPixelParameter)
			bSuccess = SUCCEEDED(m_pTechnique->GetPixelConstantTable( )->SetMatrixTransposeArray(DX9::Device( ), m_hPixelParameter, (const D3DXMATRIX*) pMatrixArray, uiNumMatrices));
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstant::SetTexture

inline st_bool CShaderConstant::SetTexture(const CTexture& cTexture) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		UINT uiSamplerIndex = 0;

		if (m_tShaderConstantPolicy.m_hVertexParameter)
		{
			uiSamplerIndex = m_tShaderConstantPolicy.m_pTechnique->GetVertexConstantTable( )->GetSamplerIndex(m_tShaderConstantPolicy.m_hVertexParameter);
			bSuccess = SUCCEEDED(DX9::Device( )->SetTexture(uiSamplerIndex, cTexture.m_tTexturePolicy.GetTextureObject( )));

			// bind vertex texture sampler if used; any sampler index  higher than four will fail
			if (uiSamplerIndex < 4)
				bSuccess = SUCCEEDED(DX9::Device( )->SetTexture(D3DVERTEXTEXTURESAMPLER0 + uiSamplerIndex, cTexture.m_tTexturePolicy.GetTextureObject( )));
		}

		if (m_tShaderConstantPolicy.m_hPixelParameter)
		{
			uiSamplerIndex = m_tShaderConstantPolicy.m_pTechnique->GetPixelConstantTable( )->GetSamplerIndex(m_tShaderConstantPolicy.m_hPixelParameter);
			bSuccess = SUCCEEDED(DX9::Device( )->SetTexture(uiSamplerIndex, cTexture.m_tTexturePolicy.GetTextureObject( )));

			// bind vertex texture sampler if used; any sampler index higher than four will fail
			if (uiSamplerIndex < 4)
				bSuccess = SUCCEEDED(DX9::Device( )->SetTexture(D3DVERTEXTEXTURESAMPLER0 + uiSamplerIndex, cTexture.m_tTexturePolicy.GetTextureObject( )));
		}

		if (bSuccess)
		{
			// this would override anisotropic settings
			//DX9::Device( )->SetSamplerState(uiSamplerIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			//DX9::Device( )->SetSamplerState(uiSamplerIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			//DX9::Device( )->SetSamplerState(uiSamplerIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

			// set vertex texture sampler state if used; any sampler index higher than 4 will fail
			if (uiSamplerIndex < 4)
			{
				DX9::Device( )->SetSamplerState(D3DVERTEXTEXTURESAMPLER0 + uiSamplerIndex, D3DSAMP_MINFILTER, D3DTEXF_POINT);
				DX9::Device( )->SetSamplerState(D3DVERTEXTEXTURESAMPLER0 + uiSamplerIndex, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
				DX9::Device( )->SetSamplerState(D3DVERTEXTEXTURESAMPLER0 + uiSamplerIndex, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			}
		}
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderConstant::SetTexture

inline st_bool CShaderConstant::SetTexture(const CDepthTexture& cDepthTexture) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsValid( ))
	{
#endif
		if (m_tShaderConstantPolicy.m_hPixelParameter)
		{
			UINT uiSamplerIndex = m_tShaderConstantPolicy.m_pTechnique->GetPixelConstantTable( )->GetSamplerIndex(m_tShaderConstantPolicy.m_hPixelParameter);
			bSuccess = SUCCEEDED(DX9::Device( )->SetTexture(uiSamplerIndex, cDepthTexture.m_tDepthTexturePolicy.GetTextureObject( )));

			// set shadow map sampler state
			DX9::Device( )->SetSamplerState(uiSamplerIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			DX9::Device( )->SetSamplerState(uiSamplerIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			DX9::Device( )->SetSamplerState(uiSamplerIndex, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			DX9::Device( )->SetSamplerState(uiSamplerIndex, D3DSAMP_BORDERCOLOR, 0xffffffff);
			DX9::Device( )->SetSamplerState(uiSamplerIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			DX9::Device( )->SetSamplerState(uiSamplerIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		}

#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX9::CShaderLoaderDirectX9

inline CShaderLoaderDirectX9::CShaderLoaderDirectX9( ) :
	m_pShaderMacros(NULL),
	m_bUsePrecompiledShaders(false)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX9::~CShaderLoaderDirectX9

inline CShaderLoaderDirectX9::~CShaderLoaderDirectX9( )
{
	Release( );

	// destroy the shader macros array
	st_delete_array<const D3DXMACRO>(m_pShaderMacros);
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX9::GetError

inline const char* CShaderLoaderDirectX9::GetError(void) const
{
	return m_strError.c_str( );
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX9::OnResetDevice

inline void CShaderLoaderDirectX9::OnResetDevice(void)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX9::OnLostDevice

inline void CShaderLoaderDirectX9::OnLostDevice(void)
{
}


///////////////////////////////////////////////////////////////////////  
//  CShaderLoaderDirectX9::GetTextureConstantName

inline CFixedString CShaderLoaderDirectX9::GetTextureConstantName(const CFixedString& strBaseName)
{
	return CFixedString("sam") + strBaseName;
}
