///////////////////////////////////////////////////////////////////////  
//  DepthTextureRI.inl
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
//  CDepthTextureRI::CDepthTextureRI

template<class TDepthTexturePolicy>
inline CDepthTextureRI<TDepthTexturePolicy>::CDepthTextureRI( )
{
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureRI::~CDepthTextureRI

template<class TDepthTexturePolicy>
inline CDepthTextureRI<TDepthTexturePolicy>::~CDepthTextureRI( )
{
    Release( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureRI::Create

template<class TDepthTexturePolicy>
inline bool CDepthTextureRI<TDepthTexturePolicy>::Create(st_int32 nWidth, st_int32 nHeight)
{
#ifndef NDEBUG
    if (nWidth > 0 && nHeight > 0)
    {
#endif
        return m_tDepthTexturePolicy.Create(nWidth, nHeight);
#ifndef NDEBUG
    }
    else
        return false;
#endif
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureRI::Release

template<class TDepthTexturePolicy>
inline void CDepthTextureRI<TDepthTexturePolicy>::Release(void)
{
    m_tDepthTexturePolicy.Release( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureRI::OnResetDevice

template<class TDepthTexturePolicy>
inline void CDepthTextureRI<TDepthTexturePolicy>::OnResetDevice(void)
{
    m_tDepthTexturePolicy.OnResetDevice( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureRI::OnLostDevice

template<class TDepthTexturePolicy>
inline void CDepthTextureRI<TDepthTexturePolicy>::OnLostDevice(void)
{
    m_tDepthTexturePolicy.OnLostDevice( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureRI::SetAsTarget

template<class TDepthTexturePolicy>
inline bool CDepthTextureRI<TDepthTexturePolicy>::SetAsTarget(void)
{
    return m_tDepthTexturePolicy.SetAsTarget( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureRI::ReleaseAsTarget

template<class TDepthTexturePolicy>
inline bool CDepthTextureRI<TDepthTexturePolicy>::ReleaseAsTarget(void)
{
    return m_tDepthTexturePolicy.ReleaseAsTarget( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureRI::BindAsTexture

template<class TDepthTexturePolicy>
inline bool CDepthTextureRI<TDepthTexturePolicy>::BindAsTexture(void)
{
    return m_tDepthTexturePolicy.BindAsTexture( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureRI::UnBindAsTexture

template<class TDepthTexturePolicy>
inline bool CDepthTextureRI<TDepthTexturePolicy>::UnBindAsTexture(void)
{
    return m_tDepthTexturePolicy.UnBindAsTexture( );
}


///////////////////////////////////////////////////////////////////////
//  CDepthTextureRI::EnableShadowMapComparison

template<class TDepthTexturePolicy>
inline void CDepthTextureRI<TDepthTexturePolicy>::EnableShadowMapComparison(st_bool bFlag)
{
    m_tDepthTexturePolicy.EnableShadowMapComparison(bFlag);
}
