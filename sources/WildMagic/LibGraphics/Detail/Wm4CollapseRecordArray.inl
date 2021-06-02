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
// Version: 4.0.0 (2006/08/05)

//----------------------------------------------------------------------------
inline int CollapseRecordArray::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline CollapseRecord* CollapseRecordArray::GetData () const
{
    return m_akArray;
}
//----------------------------------------------------------------------------
inline CollapseRecordArray::operator const CollapseRecord* () const
{
    return m_akArray;
}
//----------------------------------------------------------------------------
inline CollapseRecordArray::operator CollapseRecord* ()
{
    return m_akArray;
}
//----------------------------------------------------------------------------
inline const CollapseRecord& CollapseRecordArray::operator[] (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akArray[i];
}
//----------------------------------------------------------------------------
inline CollapseRecord& CollapseRecordArray::operator[] (int i)
{
    assert(0 <= i && i < m_iQuantity);
    return m_akArray[i];
}
//----------------------------------------------------------------------------
