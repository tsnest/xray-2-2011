///////////////////////////////////////////////////////////////////////  
//  Forest.inl
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
//
//  *** Release version 5.2.0 ***


///////////////////////////////////////////////////////////////////////  
//  CView::CView

inline CView::CView( ) :
    m_bLodRefPointSet(false),
    m_fNearClip(1.0f),
    m_fFarClip(100.0f),
    m_fCameraAzimuth(0.0f),
    m_fCameraPitch(0.0f),
    m_fHorzFadeStartAngle(DegToRad(30.0f)),
    m_fHorzFadeEndAngle(DegToRad(60.0f))
{
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetCameraPos

inline const Vec3& CView::GetCameraPos(void) const
{
    return m_vCameraPos;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetLodRefPoint

inline const Vec3& CView::GetLodRefPoint(void) const
{
    return m_bLodRefPointSet ? m_vLodRefPoint : m_vCameraPos;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetProjection

inline const Mat4x4& CView::GetProjection(void) const
{
    return m_mProjection;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetModelview

inline const Mat4x4& CView::GetModelview(void) const
{
    return m_mModelview;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetModelviewNoTranslate

inline const Mat4x4& CView::GetModelviewNoTranslate(void) const
{
    return m_mModelviewNoTranslate;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetNearClip

inline st_float32 CView::GetNearClip(void) const
{
    return m_fNearClip;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetFarClip

inline st_float32 CView::GetFarClip(void) const
{
    return m_fFarClip;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetCameraDir

inline const Vec3& CView::GetCameraDir(void) const
{
    return m_vCameraDir;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetComposite

inline const Mat4x4& CView::GetComposite(void) const
{
    return m_mComposite;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetCompositeNoTranslate

inline const Mat4x4& CView::GetCompositeNoTranslate(void) const
{
    return m_mCompositeNoTranslate;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetCameraAzimuth

inline st_float32 CView::GetCameraAzimuth(void) const
{
    return m_fCameraAzimuth;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetCameraPitch

inline st_float32 CView::GetCameraPitch(void) const
{
    return m_fCameraPitch;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetFrustumPoints

inline const Vec3* CView::GetFrustumPoints(void) const
{
    return m_avFrustumPoints;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetFrustumPlanes

inline const Vec4* CView::GetFrustumPlanes(void) const
{
    return m_avFrustumPlanes;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetFrustumExtents

inline const CExtents& CView::GetFrustumExtents(void) const
{
    return m_cFrustumExtents;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetCameraFacingMatrix

inline const Mat4x4& CView::GetCameraFacingMatrix(void) const
{
    return m_mCameraFacingMatrix;
}


///////////////////////////////////////////////////////////////////////  
//  CView::SetHorzBillboardFadeAngles

inline void CView::SetHorzBillboardFadeAngles(st_float32 fStart, st_float32 fEnd)
{
    m_fHorzFadeStartAngle = fStart;
    m_fHorzFadeEndAngle = fEnd;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetHorzBillboardFadeAngles

inline void CView::GetHorzBillboardFadeAngles(st_float32& fStart, st_float32& fEnd) const
{
    fStart = m_fHorzFadeStartAngle;
    fEnd = m_fHorzFadeEndAngle;
}


///////////////////////////////////////////////////////////////////////  
//  CView::GetHorzBillboardFadeValue

inline st_float32 CView::GetHorzBillboardFadeValue(void) const
{
    return m_fHorzFadeValue;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::GetBaseTrees

inline const TTreeArray& CForest::GetBaseTrees(void) const
{
    return m_aBaseTrees;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::GetTreeCellMap

inline const TTreeCellMap& CForest::GetTreeCellMap(void) const
{
    return m_cTreeCellMap;
}


///////////////////////////////////////////////////////////////////////
//  CForest::SetCullCellSize

inline void CForest::SetCullCellSize(st_float32 fCellSize)
{
    m_cTreeCellMap.SetCellSize(fCellSize);
}


///////////////////////////////////////////////////////////////////////
//  CForest::FrameEnd

inline void CForest::FrameEnd(void)
{
    ++m_nUpdateIndex;
    m_bLightDirChanged = false;
    m_bBaseTreesChanged = false;
    m_bCellDeletedSinceLastCull = false;
}


///////////////////////////////////////////////////////////////////////
//  CForest::GetUpdateIndex

inline st_int32 CForest::GetUpdateIndex(void) const
{
    return m_nUpdateIndex;
}


///////////////////////////////////////////////////////////////////////
//  CForest::EnableWind

inline void CForest::EnableWind(bool bFlag)
{
    m_bWindEnabled = bFlag;
}


///////////////////////////////////////////////////////////////////////
//  CForest::IsWindEnabled

inline bool CForest::IsWindEnabled(void) const
{
    return m_bWindEnabled;
}


///////////////////////////////////////////////////////////////////////
//  CForest::SetGlobalWindStrength

inline void CForest::SetGlobalWindStrength(st_float32 fStrength)
{
    m_fGlobalWindStrength = st_min(1.0f, st_max(fStrength, 0.0f));

    // update all of the base trees
    for (st_uint32 i = 0; i < m_aBaseTrees.size( ); ++i)
    {
        assert(m_aBaseTrees[i]);
        m_aBaseTrees[i]->GetWind( ).SetStrength(m_fGlobalWindStrength);
    }

    // update main wind object
    m_cWindLeader.SetStrength(m_fGlobalWindStrength);
}


///////////////////////////////////////////////////////////////////////
//  CForest::GetGlobalWindStrength

inline st_float32 CForest::GetGlobalWindStrength(void) const
{
    return m_fGlobalWindStrength;
}


///////////////////////////////////////////////////////////////////////
//  CForest::SetGlobalWindDirection

inline void CForest::SetGlobalWindDirection(const Vec3& vDir)
{
    m_vWindDir = vDir;

    // update all of the base trees
    for (st_uint32 i = 0; i < m_aBaseTrees.size( ); ++i)
    {
        assert(m_aBaseTrees[i]);
        m_aBaseTrees[i]->GetWind( ).SetDirection(m_vWindDir);
    }

    // update main wind object
    m_cWindLeader.SetDirection(m_vWindDir);
}


///////////////////////////////////////////////////////////////////////
//  CForest::GetGlobalWindDirection

inline const Vec3& CForest::GetGlobalWindDirection(void) const
{
    return m_vWindDir;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::AdvanceGlobalWind

inline void CForest::AdvanceGlobalWind(void)
{
    // update main wind object
    m_cWindLeader.Advance(m_bWindEnabled, m_fGlobalTime);

    // update all of the base trees
    for (st_uint32 i = 0; i < m_aBaseTrees.size( ); ++i)
    {
        assert(m_aBaseTrees[i]);
        m_aBaseTrees[i]->GetWind( ).Advance(m_bWindEnabled, m_fGlobalTime);
    }
}


///////////////////////////////////////////////////////////////////////  
//  CForest::SetWindLeader

inline void CForest::SetWindLeader(const CWind* pLeader)
{
    assert(pLeader);

    m_cWindLeader = *pLeader;

    // update all of the base trees
    for (st_uint32 i = 0; i < m_aBaseTrees.size( ); ++i)
    {
        assert(m_aBaseTrees[i]);
        m_aBaseTrees[i]->GetWind( ).SetWindLeader(&m_cWindLeader);
    }

    m_cWindLeader.SetWindLeader(NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CForest::GetWindLeader

inline const CWind& CForest::GetWindLeader(void) const
{
    return m_cWindLeader;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::SetGlobalTime

inline void CForest::SetGlobalTime(st_float32 fSeconds)
{
    m_fGlobalTime = fSeconds;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::GetGlobalTime

inline st_float32 CForest::GetGlobalTime(void) const
{
    return m_fGlobalTime;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::SetLightDir

inline void CForest::SetLightDir(const Vec3& vLightDir)
{
    if (m_vLightDir != vLightDir)
    {
        m_vLightDir = vLightDir;
        m_bLightDirChanged = true;
    }
}


///////////////////////////////////////////////////////////////////////  
//  CForest::GetLightDir

inline const Vec3& CForest::GetLightDir(void) const
{
    return m_vLightDir;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::LightDirChanged

inline st_bool CForest::LightDirChanged(void) const
{
    return m_bLightDirChanged;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::SetCascadedShadowMapDistances

inline void CForest::SetCascadedShadowMapDistances(const float afSplits[c_nMaxNumShadowMaps], st_float32 fFarClip)
{
    for (st_int32 i = 0; i < c_nMaxNumShadowMaps; ++i)
        m_afCascadedShadowMapSplits[i + 1] = afSplits[i] / fFarClip;

    m_bLightDirChanged = true;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::GetCascadedShadowMapDistances

inline const float* CForest::GetCascadedShadowMapDistances(void) const
{
    return m_afCascadedShadowMapSplits;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::SetShadowFadePercentage

inline void CForest::SetShadowFadePercentage(st_float32 fFade)
{
    m_fShadowFadePercentage = fFade;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::GetShadowFadePercentage

inline st_float32 CForest::GetShadowFadePercentage(void) const
{
    return m_fShadowFadePercentage;
}


///////////////////////////////////////////////////////////////////////  
//  CForest::SPopulationStats::SPopulationStats

inline CForest::SPopulationStats::SPopulationStats( ) :
    m_nNumCells(-1),
    m_nNumBaseTrees(-1),
    m_nNumInstances(-1),
    m_fAverageNumInstancesPerBase(-1.0f),
    m_nMaxNumBillboardsPerCell(-1),
    m_nMaxNumInstancesPerCell(-1),
    m_fAverageInstancesPerCell(-1.0f),
    m_nMaxNumBillboardImages(-1)
{
}



