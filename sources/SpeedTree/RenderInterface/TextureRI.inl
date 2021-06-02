///////////////////////////////////////////////////////////////////////  
//  TextureRI.inl
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
//  CTextureRI::Load

template<class TTexturePolicy>
inline st_bool CTextureRI<TTexturePolicy>::Load(const char* pFilename, st_int32 nMaxAnisotropy)
{
    if (IsValid( ))
        Unload( );

    if (m_tTexturePolicy.Load(pFilename, nMaxAnisotropy))
        m_strFilename = pFilename;
    else
        CCore::SetError("Failed to load texture [%s]\n", pFilename);

    return IsValid( );
}


///////////////////////////////////////////////////////////////////////  
//  CTextureRI::LoadColor

template<class TTexturePolicy>
inline st_bool CTextureRI<TTexturePolicy>::LoadColor(st_uint32 uiColor)
{
    if (IsValid( ))
        Unload( );

    if (m_tTexturePolicy.LoadColor(uiColor))
        m_strFilename = CFixedString::Format("Color_%x", uiColor).c_str( );

    return IsValid( );
}


///////////////////////////////////////////////////////////////////////  
//  CTextureRI::LoadAlphaNoise

template<class TTexturePolicy>
inline st_bool CTextureRI<TTexturePolicy>::LoadAlphaNoise(st_int32 nWidth, st_int32 nHeight)
{
    if (IsValid( ))
        Unload( );

    if (m_tTexturePolicy.LoadAlphaNoise(nWidth, nWidth))
        m_strFilename = CFixedString::Format("Noise_%dx%d", nWidth, nHeight).c_str( );

    return IsValid( );
}


///////////////////////////////////////////////////////////////////////  
//  CTextureRI::Unload

template<class TTexturePolicy>
inline st_bool CTextureRI<TTexturePolicy>::Unload(void)
{
    if (m_tTexturePolicy.Unload( ))
        m_strFilename.clear( );

    return !IsValid( );
}


///////////////////////////////////////////////////////////////////////  
//  CTextureRI::FixedFunctionBind

template<class TTexturePolicy>
inline st_bool CTextureRI<TTexturePolicy>::FixedFunctionBind(void)
{
    return m_tTexturePolicy.FixedFunctionBind( );
}


///////////////////////////////////////////////////////////////////////  
//  CTextureRI::GetPolicy

template<class TTexturePolicy>
inline const TTexturePolicy& CTextureRI<TTexturePolicy>::GetPolicy(void) const
{
    return m_tTexturePolicy;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureRI::GetFilename

template<class TTexturePolicy>
inline const char* CTextureRI<TTexturePolicy>::GetFilename(void) const
{
    return m_strFilename.c_str( );
}


///////////////////////////////////////////////////////////////////////  
//  CTextureRI::IsValid

template<class TTexturePolicy>
inline st_bool CTextureRI<TTexturePolicy>::IsValid(void) const
{
    return m_tTexturePolicy.IsValid( );
}


///////////////////////////////////////////////////////////////////////  
//  CTextureRI::operator=

template<class TTexturePolicy>
inline CTextureRI<TTexturePolicy>& CTextureRI<TTexturePolicy>::operator=(const CTextureRI& cRight)
{
    m_tTexturePolicy = cRight.m_tTexturePolicy;
    m_strFilename = cRight.m_strFilename;

    return *this;
}


///////////////////////////////////////////////////////////////////////  
//  CTextureRI::operator!=

template<class TTexturePolicy>
inline st_bool CTextureRI<TTexturePolicy>::operator!=(const CTextureRI& cRight) const
{
    return m_tTexturePolicy.operator!=(cRight.m_tTexturePolicy);
}
