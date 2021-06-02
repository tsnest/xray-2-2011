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
// Version: 4.0.1 (2006/08/05)

//----------------------------------------------------------------------------
inline int ClodMesh::GetRecordQuantity () const
{
    return m_spkRecordArray->GetQuantity();
}
//----------------------------------------------------------------------------
inline int& ClodMesh::TargetRecord ()
{
    return m_iTargetRecord;
}
//----------------------------------------------------------------------------
inline int ClodMesh::GetAutomatedTargetRecord ()
{
    return m_iTargetRecord;
}
//----------------------------------------------------------------------------
