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
inline bool Transformation::IsIdentity () const
{
    return m_bIsIdentity;
}
//----------------------------------------------------------------------------
inline bool Transformation::IsRSMatrix () const
{
    return m_bIsRSMatrix;
}
//----------------------------------------------------------------------------
inline bool Transformation::IsUniformScale () const
{
    return m_bIsRSMatrix && m_bIsUniformScale;
}
//----------------------------------------------------------------------------
inline const Matrix3f& Transformation::GetRotate () const
{
    assert(m_bIsRSMatrix);
    return m_kMatrix;
}
//----------------------------------------------------------------------------
inline const Matrix3f& Transformation::GetMatrix () const
{
    return m_kMatrix;
}
//----------------------------------------------------------------------------
inline const Vector3f& Transformation::GetTranslate () const
{
    return m_kTranslate;
}
//----------------------------------------------------------------------------
inline const Vector3f& Transformation::GetScale () const
{
    assert(m_bIsRSMatrix);
    return m_kScale;
}
//----------------------------------------------------------------------------
inline float Transformation::GetUniformScale () const
{
    assert(m_bIsRSMatrix && m_bIsUniformScale);
    return m_kScale.X();
}
//----------------------------------------------------------------------------
