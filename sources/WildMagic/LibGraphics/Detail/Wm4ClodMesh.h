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

#ifndef WM4CLODMESH_H
#define WM4CLODMESH_H

#include "Wm4GraphicsLIB.h"
#include "Wm4CollapseRecordArray.h"
#include "Wm4TriMesh.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM ClodMesh : public TriMesh
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.  ClodMesh will make a copy of the index
    // buffer, because it needs to be able to update the indices independently
    // when two or more ClodMesh objects share the same vertex buffer and
    // collapse records.
    ClodMesh (VertexBuffer* pkVBuffer, const IndexBuffer* pkIBuffer,
        CollapseRecordArray* pkRecordArray);

    virtual ~ClodMesh ();

    // LOD selection is based on manual selection by the application.  To
    // use distance from camera or screen space coverage, derive a class
    // from Wm4ClodMesh and override 'GetAutomatedTargetRecord'.
    int GetRecordQuantity () const;
    int& TargetRecord ();
    virtual int GetAutomatedTargetRecord ();

    // Geometric updates.  The Draw method will call this update and adjust
    // the TriMesh quantities according to the current value of the target
    // record.  You can call this manually in an application that does not
    // need to display the mesh.
    void SelectLevelOfDetail ();

protected:
    ClodMesh ();

    // culling
    virtual void GetVisibleSet (Culler& rkCuller, bool bNoCull);

    // selection of LOD
    int m_iCurrentRecord, m_iTargetRecord;
    CollapseRecordArrayPtr m_spkRecordArray;
};

WM4_REGISTER_STREAM(ClodMesh);
typedef Pointer<ClodMesh> ClodMeshPtr;
#include "Wm4ClodMesh.inl"

}

#endif
