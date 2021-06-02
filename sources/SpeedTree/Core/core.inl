///////////////////////////////////////////////////////////////////////  
//  Core.inl
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
//  SLodProfile::SLodProfile

inline SLodProfile::SLodProfile( ) :
    m_fHighDetail3dDistance(300.0f),
    m_fLowDetail3dDistance(1200.0f),
    m_fBillboardStartDistance(1300.0f),
    m_fBillboardFinalDistance(1500.0f),
    m_bLodIsPresent(true)
{
    ComputeDerived( );
    assert(IsValid( ));
}


///////////////////////////////////////////////////////////////////////
//  SLodProfile::IsValid

inline st_bool SLodProfile::IsValid(void) const
{
    return (m_fHighDetail3dDistance < m_fLowDetail3dDistance) &&
           (m_fBillboardStartDistance < m_fBillboardFinalDistance) &&
           (m_fLowDetail3dDistance < m_fBillboardStartDistance) &&
           (m_f3dRange >= 0.0f) &&
           (m_fBillboardRange >= 0.0f);
}


///////////////////////////////////////////////////////////////////////
//  SLodProfile::ComputeDerived

inline void SLodProfile::ComputeDerived(void)
{
    m_f3dRange = m_fLowDetail3dDistance - m_fHighDetail3dDistance;
    m_fBillboardRange = m_fBillboardFinalDistance - m_fBillboardStartDistance;
}


///////////////////////////////////////////////////////////////////////
//  SLodProfile::Scale

inline void SLodProfile::Scale(st_float32 fScale)
{
    m_fHighDetail3dDistance *= fScale;
    m_fLowDetail3dDistance *= fScale;
    m_fBillboardStartDistance *= fScale;
    m_fBillboardFinalDistance *= fScale;

    ComputeDerived( );
}


///////////////////////////////////////////////////////////////////////
//  SLodProfile::Square

inline void SLodProfile::Square(SLodProfile& sSquaredProfile) const
{
    sSquaredProfile.m_fHighDetail3dDistance = m_fHighDetail3dDistance * m_fHighDetail3dDistance;
    sSquaredProfile.m_fLowDetail3dDistance = m_fLowDetail3dDistance * m_fLowDetail3dDistance;
    sSquaredProfile.m_fBillboardStartDistance = m_fBillboardStartDistance * m_fBillboardStartDistance;
    sSquaredProfile.m_fBillboardFinalDistance = m_fBillboardFinalDistance * m_fBillboardFinalDistance;

    sSquaredProfile.ComputeDerived( );
}


///////////////////////////////////////////////////////////////////////
//  SLodSnapshot::SLodSnapshot

inline SLodSnapshot::SLodSnapshot( ) :
    m_nBranchLodIndex(-1),
    m_nFrondLodIndex(-1),
    m_nLeafCardLodIndex(-1),
    m_nLeafMeshLodIndex(-1)
{
}


///////////////////////////////////////////////////////////////////////
//  CCore::GetFilename

inline const st_char* CCore::GetFilename(void) const
{
    return m_strFilename.c_str( );
}


///////////////////////////////////////////////////////////////////////
//  CCore::GetGeometry

inline const SGeometry* CCore::GetGeometry(void) const
{
    return &m_sGeometry;
}


///////////////////////////////////////////////////////////////////////
//  CCore::HasGeometryType

inline st_bool CCore::HasGeometryType(EGeometryType eType) const
{
    return m_abGeometryTypesPresent[eType];
}


///////////////////////////////////////////////////////////////////////
//  CCore::GetGeometryTypeName

inline const st_char* CCore::GetGeometryTypeName(EGeometryType eType)
{
    switch (eType)
    {
    case GEOMETRY_TYPE_BRANCHES:
        return "Branches";
    case GEOMETRY_TYPE_FRONDS:
        return "Fronds";
    case GEOMETRY_TYPE_LEAF_CARDS:
        return "Leaf Cards";
    case GEOMETRY_TYPE_LEAF_MESHES:
        return "Leaf Meshes";
    case GEOMETRY_TYPE_VERTICAL_BILLBOARDS:
        return "Vertical Billboards";
    case GEOMETRY_TYPE_HORIZONTAL_BILLBOARDS:
        return "Horizontal Billboards";
    default:
        return "Unknown";
    };
}


///////////////////////////////////////////////////////////////////////
//  CCore::GetExtents

inline const CExtents& CCore::GetExtents(void) const
{
    return m_cExtents;
}


///////////////////////////////////////////////////////////////////////
//  CCore::GetLodProfile

inline const SLodProfile& CCore::GetLodProfile(void) const
{
    return m_sLodProfile;
}


///////////////////////////////////////////////////////////////////////
//  CCore::GetLodProfileSquared

inline const SLodProfile& CCore::GetLodProfileSquared(void) const
{
    return m_sLodProfileSquared;
}


///////////////////////////////////////////////////////////////////////
//  CCore::SetLodProfile

inline st_bool CCore::SetLodProfile(const SLodProfile& sLodProfile)
{
    st_bool bSuccess = false;

    if (sLodProfile.IsValid( ))
    {
        // standard profile
        m_sLodProfile = sLodProfile;
        m_sLodProfile.ComputeDerived( );

        // profile squared
        sLodProfile.Square(m_sLodProfileSquared);

        bSuccess = true;
    }
    else
        CCore::SetError("CCore::SetLodRange, one of the near/start values exceeds its corresponding far/end value");

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CCore::ComputeLodByDistance

inline st_float32 CCore::ComputeLodByDistance(st_float32 fDistance) const
{
    st_float32 fLod = -1.0f;

    if (fDistance < m_sLodProfile.m_fHighDetail3dDistance)
        fLod = 1.0f;
    else if (fDistance < m_sLodProfile.m_fLowDetail3dDistance)
        fLod = 1.0f - (fDistance - m_sLodProfile.m_fHighDetail3dDistance) / m_sLodProfile.m_f3dRange;
    else if (fDistance < m_sLodProfile.m_fBillboardStartDistance)
        fLod = 0.0f;
    else if (fDistance < m_sLodProfile.m_fBillboardFinalDistance)
        fLod = -(fDistance - m_sLodProfile.m_fBillboardStartDistance) / m_sLodProfile.m_fBillboardRange;

    return fLod;
}


///////////////////////////////////////////////////////////////////////
//  CCore::ComputeLodByDistanceSquared

inline st_float32 CCore::ComputeLodByDistanceSquared(st_float32 fDistance) const
{
    st_float32 fLod = -1.0f;

    if (fDistance < m_sLodProfileSquared.m_fHighDetail3dDistance)
        fLod = 1.0f;
    else if (fDistance < m_sLodProfileSquared.m_fLowDetail3dDistance)
        fLod = 1.0f - (fDistance - m_sLodProfileSquared.m_fHighDetail3dDistance) / m_sLodProfileSquared.m_f3dRange;
    else if (fDistance < m_sLodProfileSquared.m_fBillboardStartDistance)
        fLod = 0.0f;
    else if (fDistance < m_sLodProfileSquared.m_fBillboardFinalDistance)
        fLod = -(fDistance - m_sLodProfileSquared.m_fBillboardStartDistance) / m_sLodProfileSquared.m_fBillboardRange;

    return fLod;
}


///////////////////////////////////////////////////////////////////////
//  CCore::ComputeShaderLerp
//
//  Optimization is disabled for Xbox 360 as the optimizer causes it to
//  compute incorrect values.

#ifdef _XBOX
  #pragma optimize("", off)
#endif

inline st_float32 CCore::ComputeShaderLerp(st_float32 fLod, st_int32 nNumDiscreteLevels)
{
    if (nNumDiscreteLevels == 0)
        return 1.0f;

    st_float32 fLodClamped = st_max(0.0f, fLod);
    st_float32 fSpacing = 1.0f / st_float32(nNumDiscreteLevels);

    st_float32 fMod = fLodClamped - st_int32(fLodClamped / fSpacing) * fSpacing;

    if (fLod <= 0.0f)
        return fMod / fSpacing;
    else
        return (fMod == 0.0f) ? 1.0f : fMod / fSpacing;
}

#ifdef _XBOX
  #pragma optimize("", on)
#endif


///////////////////////////////////////////////////////////////////////
//  CCore::GetCollisionObjects

inline const SCollisionObject* CCore::GetCollisionObjects(st_int32& nNumObjects) const
{
    nNumObjects = m_nNumCollisionObjects;
    
    return m_pCollisionObjects;
}


///////////////////////////////////////////////////////////////////////
//  CCore::GetUserString

inline const char* CCore::GetUserString(EUserStringOrdinal eOrdinal) const
{
    return m_pUserStrings[eOrdinal];
}


///////////////////////////////////////////////////////////////////////
//  CCore::UncompressVec3

inline Vec3 CCore::UncompressVec3(const st_uint8* pCompressedVector)
{
    return Vec3(UncompressScalar(pCompressedVector[0]), UncompressScalar(pCompressedVector[1]), UncompressScalar(pCompressedVector[2]));
}


///////////////////////////////////////////////////////////////////////
//  CCore::UncompressScalar

inline st_float32 CCore::UncompressScalar(st_uint8 uiCompressedScalar)
{
    if (uiCompressedScalar == 127) // return true zero, not approximate
        return 0.0f;
    else
        return st_float32(uiCompressedScalar) / 127.5f - 1.0f;
}


///////////////////////////////////////////////////////////////////////
//  CCore::CompressScalar

inline st_uint8 CCore::CompressScalar(st_float32 fUncompressedScalar)
{
    return st_uint8(255 * (fUncompressedScalar * 0.5f + 0.5f));
}




