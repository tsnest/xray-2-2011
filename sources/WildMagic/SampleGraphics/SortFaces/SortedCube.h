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

#ifndef SORTEDCUBE_H
#define SORTEDCUBE_H

#include "Wm4Node.h"
#include "Wm4Camera.h"
#include "Wm4TriMesh.h"

namespace Wm4
{

class SortedCube : public Node
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    SortedCube (Camera* pkCamera,
        const std::string& rkXpImage, const std::string& rkXmImage,
        const std::string& rkYpImage, const std::string& rkYmImage,
        const std::string& rkZpImage, const std::string& rkZmImage);

    virtual ~SortedCube ();

    void SetCamera (Camera* pkCamera);

protected:
    SortedCube ();

    void SortFaces ();
    virtual void GetVisibleSet (Culler& rkCuller, bool bNoCull = false);

    CameraPtr m_spkCamera;

    // 0 = face xp
    // 1 = face xm
    // 2 = face yp
    // 3 = face ym
    // 4 = face zp
    // 5 = face zm
    TriMeshPtr m_aspkFace[6];

    // bit 0 = face xp
    // bit 1 = face xm
    // bit 2 = face yp
    // bit 3 = face ym
    // bit 4 = face zp
    // bit 5 = face zm
    int m_iBackFacing;
};

WM4_REGISTER_STREAM(SortedCube);
typedef Pointer<SortedCube> SortedCubePtr;

}

#endif
