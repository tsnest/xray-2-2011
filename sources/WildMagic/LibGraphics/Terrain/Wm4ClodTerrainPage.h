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
// Version: 4.0.1 (2007/01/19)

#ifndef WM4CLODTERRAINPAGE_H
#define WM4CLODTERRAINPAGE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4TriMesh.h"

namespace Wm4
{

class Camera;
class ClodTerrainBlock;
class ClodTerrainVertex;

class WM4_GRAPHICS_ITEM ClodTerrainPage : public TriMesh
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // size = 2^p + 1, p <= 7 (size = 3, 5, 9, 17, 33, 65, 129)
    ClodTerrainPage (const Attributes& rkAttr, int iSize,
        unsigned short* ausHeight, const Vector2f& rkOrigin,
        float fMinElevation, float fMaxElevation, float fSpacing,
        float fUVBias);

    virtual ~ClodTerrainPage ();

    // height field access
    int GetSize () const;
    const unsigned short* GetHeights () const;
    const Vector2f& GetOrigin () const;
    float GetMinElevation () const;
    float GetMaxElevation () const;
    float GetSpacing () const;

    // pixel tolerance on projected vertex height
    void SetPixelTolerance (const Renderer* pkRenderer, float fTolerance);
    float GetPixelTolerance () const;

    // Height field measurements.  If the location is not in the page, the
    // return value is INFINITY.
    float GetHeight (float fX, float fY) const;

    // Texture coordinates for the page may be obtained by querying VBuffer.
    // The bias is obtained by the following function.
    float& UVBias ();

protected:
    friend class ClodTerrainBlock;

    // streaming support
    ClodTerrainPage ();
    void InitializeDerivedData ();

    // queue handlers
    void AddToQueue (unsigned short usBlock);
    unsigned short RemoveFromQueue ();
    unsigned short ReadFromQueue (unsigned short usIndex);

    // page simplification
    bool IntersectFrustum (const Camera* pkCamera);

    // block simplification
    void SimplifyBlocks (const Camera* pCamera, const Vector3f& rkModelEye,
        const Vector3f& rkModelDir, bool bCloseAssumption);

    // vertex simplification
    void SimplifyVertices (const Vector3f& rkModelEye,
        const Vector3f& rkModelDir, bool bCloseTerrainAssumption);

    // simplification
    friend class ClodTerrain;
    void ResetBlocks ();
    void Simplify (const Renderer* pkRenderer, const Vector3f& rkModelEye,
        const Vector3f& rkModelDir, bool bCloseAssumption);

    // tessellation
    float GetX (int iX) const;
    float GetY (int iY) const;
    float GetHeight (int iIndex) const;
    float GetTextureCoordinate (int iIndex) const;
    void Render (ClodTerrainBlock& rkBlock);
    void RenderTriangle (int iT, int iL, int iR);
    void RenderBlocks ();

    // culling
    virtual void GetVisibleSet (Culler& rkCuller, bool bNoCull);

    // height field
    int m_iSize, m_iSizeM1;
    unsigned short* m_ausHeight;
    Vector2f m_kOrigin;
    float m_fMinElevation, m_fMaxElevation, m_fSpacing;
    float m_fInvSpacing, m_fTextureSpacing, m_fMultiplier;

    // texture parameters
    float m_fUVBias;

    // simplification
    float m_fPixelTolerance, m_fWorldTolerance;
    mutable bool m_bNeedsTessellation;
    int* m_aiLookup;
    int m_iConnectLength;

    // (2^p+1) by (2^p+1) array of vertices, row-major order
    ClodTerrainVertex* m_akTVertex;

    // maximum quantities
    int m_iTotalVQuantity, m_iTotalTQuantity, m_iTotalIQuantity;

    // quadtree of blocks
    int m_iBlockQuantity;
    ClodTerrainBlock* m_akBlock;

    // circular queue of indices for active blocks
    unsigned short* m_ausQueue;
    unsigned short m_usQueueQuantity;
    unsigned short m_usFront, m_usRear;
    unsigned short m_usNumUnprocessed;
    unsigned short m_usItemsInQueue;

// for internal use (by Terrain)
public:
    // The height fields are functions z = h(x,y) with the xy-plane using
    // right-handed coordinates.  The column index increases with x and the
    // row index increases with y.  For example, looking down the positive
    // z-axis, a 2-by-2 grid of pages is labeled as
    //
    // y
    // ^
    // | col 0   col 1
    // +--------+-------+
    // |   10   |  11   | row 1
    // +--------+-------+
    // |   00   |  01   | row 0
    // +--------+-------+--> x

    // stitch/unstitch (r,c) and (r,c+1)
    void StitchNextCol (ClodTerrainPage* pkNextCol);
    void UnstitchNextCol (ClodTerrainPage* pkNextCol);

    // stitch/unstitch (r,c) and (r+1,c)
    void StitchNextRow (ClodTerrainPage* pkNextRow);
    void UnstitchNextRow (ClodTerrainPage* pkNextRow);
};

WM4_REGISTER_STREAM(ClodTerrainPage);
typedef Pointer<ClodTerrainPage> ClodTerrainPagePtr;
#include "Wm4ClodTerrainPage.inl"

}

#endif
