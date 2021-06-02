///////////////////////////////////////////////////////////////////////  
//  ShadowMapDownsampler.inl
//
//  *** INTERACTIVE DATA VISUALIZATION (IDV) CONFIDENTIAL AND PROPRIETARY INFORMATION ***
//
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Interactive Data Visualization, Inc. and
//  may not be copied, disclosed, or exploited except in accordance with 
//  the terms of that agreement.
//
//      Copyright (c) 2003-2009 IDV, Inc.
//      All rights reserved in all media.
//
//      IDV, Inc.
//      http://www.idvinc.com


///////////////////////////////////////////////////////////////////////
//  CShadowMapDownsamplerRI::CShadowMapDownsamplerRI

template<class TShadowMapDownsamplerPolicy, class TShaderLoaderPolicy, class TRenderTargetPolicy>
inline CShadowMapDownsamplerRI<TShadowMapDownsamplerPolicy, TShaderLoaderPolicy, TRenderTargetPolicy>::CShadowMapDownsamplerRI( )
{
}


///////////////////////////////////////////////////////////////////////
//  CShadowMapDownsamplerRI::~CShadowMapDownsamplerRI

template<class TShadowMapDownsamplerPolicy, class TShaderLoaderPolicy, class TRenderTargetPolicy>
inline CShadowMapDownsamplerRI<TShadowMapDownsamplerPolicy, TShaderLoaderPolicy, TRenderTargetPolicy>::~CShadowMapDownsamplerRI( )
{
}


///////////////////////////////////////////////////////////////////////
//  CShadowMapDownsamplerRI::Init

template<class TShadowMapDownsamplerPolicy, class TShaderLoaderPolicy, class TRenderTargetPolicy>
inline st_bool CShadowMapDownsamplerRI<TShadowMapDownsamplerPolicy, TShaderLoaderPolicy, TRenderTargetPolicy>::Init(TShaderLoaderPolicy* pShaderLoader)
{
    return m_tShadowMapDownsamplerPolicy.Init(pShaderLoader);
}


///////////////////////////////////////////////////////////////////////
//  CShadowMapDownsamplerRI::Downsample

template<class TShadowMapDownsamplerPolicy, class TShaderLoaderPolicy, class TRenderTargetPolicy>
inline st_bool CShadowMapDownsamplerRI<TShadowMapDownsamplerPolicy, TShaderLoaderPolicy, TRenderTargetPolicy>::Downsample(TRenderTargetPolicy* pRenderTarget)
{
    return m_tShadowMapDownsamplerPolicy.Downsample(pRenderTarget);
}
