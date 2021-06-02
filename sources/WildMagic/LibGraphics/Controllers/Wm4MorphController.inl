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
inline int MorphController::GetTargetQuantity () const
{
    return m_iTargetQuantity;
}
//----------------------------------------------------------------------------
inline int MorphController::GetKeyQuantity () const
{
    return m_iKeyQuantity;
}
//----------------------------------------------------------------------------
inline void MorphController::SetVertices (int i, Vector3fArray* pkVertices)
{
    assert(0 <= i && i < m_iTargetQuantity);
    assert(pkVertices && pkVertices->GetQuantity() == m_iVertexQuantity);
    m_aspkVertices[i] = pkVertices;
}
//----------------------------------------------------------------------------
inline Vector3fArray* MorphController::GetVertices (int i) const
{
    assert(0 <= i && i < m_iTargetQuantity);
    return m_aspkVertices[i];
}
//----------------------------------------------------------------------------
inline void MorphController::SetTimes (FloatArray* pkTimes)
{
    m_spkTimes = pkTimes;
}
//----------------------------------------------------------------------------
inline FloatArray* MorphController::GetTimes () const
{
    return m_spkTimes;
}
//----------------------------------------------------------------------------
inline void MorphController::SetWeights (int i, FloatArray* pkWeights)
{
    assert(0 <= i && i < m_iKeyQuantity);
    m_aspkWeights[i] = pkWeights;
}
//----------------------------------------------------------------------------
inline FloatArray* MorphController::GetWeights (int i) const
{
    assert(0 <= i && i < m_iKeyQuantity);
    return m_aspkWeights[i];
}
//----------------------------------------------------------------------------
