///////////////////////////////////////////////////////////////////////  
//  RenderStateRI.inl
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
//  CRenderStateRI::Initialize

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::Initialize(void)
{
    TRenderStatePolicy::Initialize( );
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::ApplyStates

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::ApplyStates(void)
{
    TRenderStatePolicy::ApplyStates( );
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::Destroy

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::Destroy(void)
{
    TRenderStatePolicy::Destroy( );
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::ClearScreen

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::ClearScreen(bool bClearColor, bool bClearDepth)
{
    TRenderStatePolicy::ClearScreen(bClearColor, bClearDepth);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetAlphaFunction

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetAlphaFunction(EAlphaFunc eFunc, st_float32 fValue)
{
    TRenderStatePolicy::SetAlphaFunction(eFunc, fValue);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetAlphaTesting

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetAlphaTesting(bool bFlag)
{
    TRenderStatePolicy::SetAlphaTesting(bFlag);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetAlphaToCoverage

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetAlphaToCoverage(bool bFlag)
{
    TRenderStatePolicy::SetAlphaToCoverage(bFlag);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetBlending

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetBlending(bool bFlag)
{
    TRenderStatePolicy::SetBlending(bFlag);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetColorMask

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetColorMask(bool bRed, bool bGreen, bool bBlue, bool bAlpha)
{
    TRenderStatePolicy::SetColorMask(bRed, bGreen, bBlue, bAlpha);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetDepthMask

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetDepthMask(bool bFlag)
{
    TRenderStatePolicy::SetDepthMask(bFlag);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetDepthTestFunc

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetDepthTestFunc(EDepthTestFunc eDepthTestFunc)
{
    TRenderStatePolicy::SetDepthTestFunc(eDepthTestFunc);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetDepthTesting

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetDepthTesting(bool bFlag)
{
    TRenderStatePolicy::SetDepthTesting(bFlag);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetFaceCulling

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetFaceCulling(ECullType eCullType)
{
    TRenderStatePolicy::SetFaceCulling(eCullType);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetPointSize

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetPointSize(st_float32 fSize)
{
    TRenderStatePolicy::SetPointSize(fSize);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetPointSize

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetMultisampling(bool bMultisample)
{
    TRenderStatePolicy::SetMultisampling(bMultisample);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetPolygonOffset

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetPolygonOffset(st_float32 fFactor, st_float32 fUnits)
{
    TRenderStatePolicy::SetPolygonOffset(fFactor, fUnits);
}


///////////////////////////////////////////////////////////////////////  
//  CRenderStateRI::SetRenderStyle

template<class TRenderStatePolicy>
inline void CRenderStateRI<TRenderStatePolicy>::SetRenderStyle(ERenderStyle eStyle)
{
    TRenderStatePolicy::SetRenderStyle(eStyle);
}

