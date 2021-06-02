///////////////////////////////////////////////////////////////////////  
//  ShaderRI.inl
//
//  *** INTERACTIVE DATA VISUALIZATION (IDV) CONFIDENTIAL AND PROPRIETARY INFORMATION ***
//
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Interactive Data Visualization, Inc. and
//  may not be copied, disclosed, or exploited except in accordance with 
//  the terms of that agreement.
//
//      Copyright (c) 2003-2010 IDV, Inc.
//      All rights reserved in all media.
//
//      IDV, Inc.
//      http://www.idvinc.com


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueRI::CShaderTechniqueRI

template<class TShaderTechniquePolicy, class TShaderConstantRI>
inline CShaderTechniqueRI<TShaderTechniquePolicy, TShaderConstantRI>::CShaderTechniqueRI( )
{
}


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueRI::~CShaderTechniqueRI

template<class TShaderTechniquePolicy, class TShaderConstantRI>
inline CShaderTechniqueRI<TShaderTechniquePolicy, TShaderConstantRI>::~CShaderTechniqueRI( )
{
}


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueRI::Bind

template<class TShaderTechniquePolicy, class TShaderConstantRI>
inline bool CShaderTechniqueRI<TShaderTechniquePolicy, TShaderConstantRI>::Bind(void)
{
    bool bSuccess = false;

#ifndef NDEBUG
    if (IsValid( ))
    {
#endif
        bSuccess = m_tShaderTechniquePolicy.Bind( );
#ifndef NDEBUG
    }
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueRI::UnBind

template<class TShaderTechniquePolicy, class TShaderConstantRI>
inline bool CShaderTechniqueRI<TShaderTechniquePolicy, TShaderConstantRI>::UnBind(void)
{
    bool bSuccess = false;

#ifndef NDEBUG
    if (IsValid( ))
    {
#endif
        bSuccess = m_tShaderTechniquePolicy.UnBind( );
#ifndef NDEBUG
    }
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueRI::CommitConstants

template<class TShaderTechniquePolicy, class TShaderConstantRI>
inline bool CShaderTechniqueRI<TShaderTechniquePolicy, TShaderConstantRI>::CommitConstants(void)
{
    bool bSuccess = false;

#ifndef NDEBUG
    if (IsValid( ))
    {
#endif
        bSuccess = m_tShaderTechniquePolicy.CommitConstants( );
#ifndef NDEBUG
    }
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueRI::CommitTextures

template<class TShaderTechniquePolicy, class TShaderConstantRI>
inline bool CShaderTechniqueRI<TShaderTechniquePolicy, TShaderConstantRI>::CommitTextures(void)
{
    bool bSuccess = false;

#ifndef NDEBUG
    if (IsValid( ))
    {
#endif
        bSuccess = m_tShaderTechniquePolicy.CommitTextures( );
#ifndef NDEBUG
    }
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueRI::IsValid

template<class TShaderTechniquePolicy, class TShaderConstantRI>
inline bool CShaderTechniqueRI<TShaderTechniquePolicy, TShaderConstantRI>::IsValid(void) const
{
    return m_tShaderTechniquePolicy.IsValid( );
}


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueRI::GetName

template<class TShaderTechniquePolicy, class TShaderConstantRI>
inline const char* CShaderTechniqueRI<TShaderTechniquePolicy, TShaderConstantRI>::GetName(void) const
{
    return m_strName.c_str( );
}


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueRI::SetName

template<class TShaderTechniquePolicy, class TShaderConstantRI>
inline void CShaderTechniqueRI<TShaderTechniquePolicy, TShaderConstantRI>::SetName(const char* pName)
{
    assert(pName);
    m_strName = pName;
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::CShaderConstantRI

template<class TShaderConstantPolicy>
inline CShaderConstantRI<TShaderConstantPolicy>::CShaderConstantRI( )
{
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::~CShaderConstantRI

template<class TShaderConstantPolicy>
inline CShaderConstantRI<TShaderConstantPolicy>::~CShaderConstantRI( )
{
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::Set1f

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::Set1f(st_float32 x) const
{
    return m_tShaderConstantPolicy.Set1f(x);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::Set2f

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::Set2f(st_float32 x, st_float32 y) const
{
    return m_tShaderConstantPolicy.Set2f(x, y);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::Set2fv

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::Set2fv(const st_float32 afValues[2]) const
{
    return m_tShaderConstantPolicy.Set2fv(afValues);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::Set3f

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::Set3f(st_float32 x, st_float32 y, st_float32 z) const
{
    return m_tShaderConstantPolicy.Set3f(x, y, z);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::Set3fv

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::Set3fv(const st_float32 afValues[3]) const
{
    return m_tShaderConstantPolicy.Set3fv(afValues);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::Set3fvPlus1f

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::Set3fvPlus1f(const st_float32 afValues[3], float w) const
{
    return m_tShaderConstantPolicy.Set3fvPlus1f(afValues, w);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::Set4f

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::Set4f(st_float32 x, st_float32 y, st_float32 z, st_float32 w) const
{
    return m_tShaderConstantPolicy.Set4f(x, y, z, w);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::Set4fv

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::Set4fv(const st_float32 afValues[4]) const
{
    return m_tShaderConstantPolicy.Set4fv(afValues);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::SetArray4f

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::SetArray4f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset) const
{
    return m_tShaderConstantPolicy.SetArray4f(pArray, nSize, nOffset);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::SetMatrix

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::SetMatrix(const st_float32 afMatrix[16]) const
{
    return m_tShaderConstantPolicy.SetMatrix(afMatrix);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::SetMatrixTranspose

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::SetMatrixTranspose(const st_float32 afMatrix[16]) const
{
    return m_tShaderConstantPolicy.SetMatrixTranspose(afMatrix);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::SetMatrixIndex

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::SetMatrixIndex(const st_float32 afMatrix[16], st_uint32 uiIndex) const
{
    return m_tShaderConstantPolicy.SetMatrixIndex(afMatrix, uiIndex);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::SetMatrix4x4Array

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::SetMatrix4x4Array(const st_float32* pMatrixArray, st_uint32 uiOffset, st_uint32 uiNumMatrices) const
{
    return m_tShaderConstantPolicy.SetMatrix4x4Array(pMatrixArray, uiOffset, uiNumMatrices);
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::IsValid

template<class TShaderConstantPolicy>
inline bool CShaderConstantRI<TShaderConstantPolicy>::IsValid(void) const
{
    return m_tShaderConstantPolicy.IsValid( );
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::GetName

template<class TShaderConstantPolicy>
inline const char* CShaderConstantRI<TShaderConstantPolicy>::GetName(void) const
{
    return m_strName.c_str( );
}


///////////////////////////////////////////////////////////////////////
//  CShaderConstantRI::SetName

template<class TShaderConstantPolicy>
inline void CShaderConstantRI<TShaderConstantPolicy>::SetName(const char* pName)
{
    assert(pName);
    m_strName = pName;
}


///////////////////////////////////////////////////////////////////////
//  CShaderLoaderRI::CShaderLoaderRI

template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
inline CShaderLoaderRI<TShaderLoaderPolicy, TShaderTechniqueRI, TShaderConstantRI>::CShaderLoaderRI( )
{
}


///////////////////////////////////////////////////////////////////////
//  CShaderLoaderRI::~CShaderLoaderRI

template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
inline CShaderLoaderRI<TShaderLoaderPolicy, TShaderTechniqueRI, TShaderConstantRI>::~CShaderLoaderRI( )
{
}


///////////////////////////////////////////////////////////////////////
//  CShaderLoaderRI::Init

template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
inline bool CShaderLoaderRI<TShaderLoaderPolicy, TShaderTechniqueRI, TShaderConstantRI>::Init(const CArray<CFixedString>& aDefines, 
                                                                                              const char* pEffectFilename,
                                                                                              st_bool bUsePrecompiledShaders)
{
    bool bSuccess = false;

    // aDefines array must have an even number of entries (the define name and then the value)
    if (aDefines.size( ) % 2 == 0)
    {
        bSuccess = m_tShaderLoaderPolicy.Init(aDefines, pEffectFilename, bUsePrecompiledShaders);
        if (!bSuccess)
            CCore::SetError("Shader compilation error, %s\n", GetError( ));
    }
    else
        CCore::SetError("CShaderLoaderRI::Load, array of defines must have an even number of elements");

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CShaderLoaderRI::GetError

template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
inline const char* CShaderLoaderRI<TShaderLoaderPolicy, TShaderTechniqueRI, TShaderConstantRI>::GetError(void) const
{
    return m_tShaderLoaderPolicy.GetError( );
}


///////////////////////////////////////////////////////////////////////
//  CShaderLoaderRI::GetTechnique

template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
inline bool CShaderLoaderRI<TShaderLoaderPolicy, TShaderTechniqueRI, TShaderConstantRI>::GetTechnique(const char* pFilename, const char* pVertexShaderName, const char* pPixelShaderName, 
                                                                                                      const char* pTechniqueName, TShaderTechniqueRI& tTechnique) const
{
    assert(pFilename);
    assert(pVertexShaderName);
    assert(pPixelShaderName);

    bool bSuccess = true;
    if (m_tShaderLoaderPolicy.RendererUsesEffects( ))
    {
        tTechnique.SetName(pTechniqueName);
        bSuccess = m_tShaderLoaderPolicy.GetTechnique(pTechniqueName, tTechnique);
    }
    else
    {
        // use a combination of the vertex and pixel shader names to name the technique
        tTechnique.SetName(CFixedString::Format("%s/%s", pVertexShaderName, pPixelShaderName).c_str( ));

        bSuccess &= m_tShaderLoaderPolicy.GetVertexShader(pFilename, pVertexShaderName, tTechnique);
        bSuccess &= m_tShaderLoaderPolicy.GetPixelShader(pFilename, pPixelShaderName, tTechnique);
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CShaderTechniqueRI::GetConstant

template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
inline bool CShaderLoaderRI<TShaderLoaderPolicy, TShaderTechniqueRI, TShaderConstantRI>::GetConstant(const char* pName, const TShaderTechniqueRI& cTechnique, TShaderConstantRI& tConstant, EConstantType eType) const
{
    assert(pName);
    tConstant.SetName(pName);

    if (m_tShaderLoaderPolicy.RendererUsesEffects( ))
        return m_tShaderLoaderPolicy.GetConstant(pName, tConstant);
    else
        return cTechnique.m_tShaderTechniquePolicy.GetConstant(pName, tConstant, eType);
}


///////////////////////////////////////////////////////////////////////
//  CShaderLoaderRI::Release

template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
inline void CShaderLoaderRI<TShaderLoaderPolicy, TShaderTechniqueRI, TShaderConstantRI>::Release(void)
{
    m_tShaderLoaderPolicy.Release( );
}


///////////////////////////////////////////////////////////////////////
//  CShaderLoaderRI::OnResetDevice

template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
inline void CShaderLoaderRI<TShaderLoaderPolicy, TShaderTechniqueRI, TShaderConstantRI>::OnResetDevice(void)
{
    m_tShaderLoaderPolicy.OnResetDevice( );
}


///////////////////////////////////////////////////////////////////////
//  CShaderLoaderRI::OnLostDevice

template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
inline void CShaderLoaderRI<TShaderLoaderPolicy, TShaderTechniqueRI, TShaderConstantRI>::OnLostDevice(void)
{
    m_tShaderLoaderPolicy.OnLostDevice( );
}


///////////////////////////////////////////////////////////////////////
//  CShaderLoaderRI::GetTextureConstantName

template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
inline CFixedString CShaderLoaderRI<TShaderLoaderPolicy, TShaderTechniqueRI, TShaderConstantRI>::GetTextureConstantName(const CFixedString& strBaseName)
{
    return m_tShaderLoaderPolicy.GetTextureConstantName(strBaseName);
}



