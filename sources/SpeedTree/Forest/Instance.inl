///////////////////////////////////////////////////////////////////////  
//  Instance.inl
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
//  CInstance::SetPos

inline void CInstance::SetPos(const Vec3& vPos)
{
    m_vPos = vPos;
}


///////////////////////////////////////////////////////////////////////  
//  CInstance::SetScale

inline void CInstance::SetScale(st_float32 fScale)
{
    m_fScale = fScale;
}


///////////////////////////////////////////////////////////////////////  
//  CInstance::SetRotation

inline void CInstance::SetRotation(st_float32 fRadians)
{
    // fit angle into [-2pi,2pi] range
    fRadians = fmod(fRadians, c_fTwoPi);

    // can't use a negative rotation value (billboard shader optimization)
    fRadians += c_fTwoPi;

    //m_fRotation = fRadians;
    m_nRotation = st_uint8(255 * (fRadians / c_fTwoPi));
    
    // assume that sinf() and cosf() always return [-1.0, 1.0]
    m_anRotationVector[0] = st_int8(127 * sinf(fRadians));
    m_anRotationVector[1] = st_int8(127 * cosf(fRadians));
}


///////////////////////////////////////////////////////////////////////  
//  CInstance::GetPos

inline const Vec3& CInstance::GetPos(void) const
{
    return m_vPos;
}


///////////////////////////////////////////////////////////////////////  
//  CInstance::GetScale

inline st_float32 CInstance::GetScale(void) const
{
    return m_fScale;
}


///////////////////////////////////////////////////////////////////////  
//  CInstance::GetPosAndScale

inline const st_float32* CInstance::GetPosAndScale(void) const
{
    return &m_vPos.x;
}


///////////////////////////////////////////////////////////////////////  
//  CInstance::GetRotationAngle

inline st_float32 CInstance::GetRotationAngle(void) const
{
    return c_fTwoPi * (m_nRotation / 255.0f);
}


///////////////////////////////////////////////////////////////////////  
//  CInstance::GetRotationVector

inline Vec3 CInstance::GetRotationVector(void) const
{
    st_float32 fFirstEntry = (m_anRotationVector[0] / 127.0f);
    st_float32 fSecondEntry = (m_anRotationVector[1] / 127.0f);

    return Vec3(fFirstEntry, fSecondEntry, -fFirstEntry);
}


///////////////////////////////////////////////////////////////////////  
//  CInstance::GetGeometricCenter

inline const Vec3& CInstance::GetGeometricCenter(void) const
{
    return m_vGeometricCenter;
}


///////////////////////////////////////////////////////////////////////  
//  CInstance::GetCullingRadius

inline st_float32 CInstance::GetCullingRadius(void) const
{
    return m_fCullingRadius;
}


///////////////////////////////////////////////////////////////////////
//  CInstance::operator<

inline bool CInstance::operator<(const CInstance& cIn) const
{
    if (m_vPos == cIn.m_vPos)
    {
        if (m_fScale == cIn.m_fScale)
            return m_nRotation < cIn.m_nRotation;
        else
            return m_fScale < cIn.m_fScale;
    }
    else
        return m_vPos < cIn.m_vPos;
}


///////////////////////////////////////////////////////////////////////
//  CInstance::operator==

inline bool CInstance::operator==(const CInstance& cIn) const
{
    return (m_vPos == cIn.m_vPos &&
            m_fScale == cIn.m_fScale &&
            m_nRotation == cIn.m_nRotation);
}

