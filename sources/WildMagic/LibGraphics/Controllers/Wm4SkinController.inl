// Wild Magic Source Code
// David Eberly
// http://www.geometrictools.com
// Copyright (c) 1998-2009
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// either of the locations:
//     http://www.gnu.org/copyleft/lgpl.html
//     http://www.geometrictools.com/License/WildMagicLicense.pdf
//
// Version: 4.0.0 (2006/06/28)

//----------------------------------------------------------------------------
inline int SkinController::GetVertexQuantity () const
{
    return m_iVertexQuantity;
}
//----------------------------------------------------------------------------
inline int SkinController::GetBoneQuantity () const
{
    return m_iBoneQuantity;
}
//----------------------------------------------------------------------------
inline Node* SkinController::GetBone (int iBone) const
{
    assert(0 <= iBone && iBone < m_iBoneQuantity);
    return m_apkBones[iBone];
}
//----------------------------------------------------------------------------
inline float& SkinController::Weight (int iVertex, int iBone)
{
    assert(0 <= iVertex && iVertex < m_iVertexQuantity);
    assert(0 <= iBone && iBone < m_iBoneQuantity);
    return m_aafWeight[iVertex][iBone];
}
//----------------------------------------------------------------------------
inline Vector3f& SkinController::Offset (int iVertex, int iBone)
{
    assert(0 <= iVertex && iVertex < m_iVertexQuantity);
    assert(0 <= iBone && iBone < m_iBoneQuantity);
    return m_aakOffset[iVertex][iBone];
}
//----------------------------------------------------------------------------
