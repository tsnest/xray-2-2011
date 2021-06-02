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

#ifndef WM4CLODTERRAINBLOCK_H
#define WM4CLODTERRAINBLOCK_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Vector2.h"
#include "Wm4Vector3.h"

namespace Wm4
{

class Camera;
class Frustum;
class ClodTerrainPage;
class ClodTerrainVertex;

class WM4_GRAPHICS_ITEM ClodTerrainBlock
{
public:
    int GetX () const;
    int GetY () const;
    int GetStride () const;
    float GetDelta (int i) const;
    float GetDeltaMax () const;
    float GetDeltaL () const;
    float GetDeltaH () const;
    const Vector3f& GetMin () const;
    const Vector3f& GetMax () const;

    void SetEven (bool bSet);
    bool GetEven () const;
    void SetProcessed (bool bSet);
    bool GetProcessed () const;
    void SetVisible (bool bSet);
    bool GetVisible () const;
    void SetVisibilityTested (bool bSet);
    bool GetVisibilityTested () const;

    bool BitsSet () const;
    void ClearBits ();

    // creation of the quadtree
    void Initialize (ClodTerrainPage* pkPage, ClodTerrainBlock* pkBlock,
        int iBlock, int iX, int iY, int iStride, bool bEven);

    // allows for changing the height data during run time
    void UpdateBoundingBox (ClodTerrainPage* pkPage,
        ClodTerrainBlock* pkBlock, int iBlock, int iStride);

    // test for intersection of page's bounding box and view frustum
    void TestIntersectFrustum (const ClodTerrainPage* pkPage,
        const Camera* pkCamera);

    // distant terrain assumption
    void ComputeInterval (const Vector3f& rkModelEye, float fTolerance);

    // close terrain assumption
    void ComputeInterval (const Vector3f& rkModelDir,
        const Vector3f& rkModelEye, float fTolerance, Vector2f& rkLoc,
        float fSpacing);

    void SimplifyVertices (ClodTerrainPage* pkPage,
        const Vector3f& rkModelEye, const Vector3f& rkModelDir,
        float fTolerance, bool bCloseAssumption);

    void Disable (ClodTerrainPage* pkPage);

    // quadtree indexing
    static int GetParentIndex (int iChild);
    static int GetChildIndex (int iParent, int iIndex);
    static bool IsFirstChild (int iIndex);
    static bool IsSibling (int iIndex, int iTest);

protected:
    // bit flags for m_ucFlags
    enum
    {
        EVEN_PARITY       = 0x01,
        PROCESSED         = 0x02,
        VISIBLE           = 0x04,
        VISIBILITY_TESTED = 0x08,
        BITS_MASK         = 0x0E  // all but even parity bit
    };

    void GetVertex9 (unsigned short usSize, ClodTerrainVertex* pkVOrigin,
        ClodTerrainVertex* apkTVertex[9]);

    unsigned char m_ucX, m_ucY, m_ucStride, m_ucFlags;
    float m_fDelta[5], m_fDeltaMax;
    float m_fDeltaL, m_fDeltaH;
    Vector3f m_kMin, m_kMax;
};

#include "Wm4ClodTerrainBlock.inl"

}

#endif
