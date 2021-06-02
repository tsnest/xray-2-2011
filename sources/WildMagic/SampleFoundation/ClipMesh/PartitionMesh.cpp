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
// Version: 4.7.0 (2008/09/15)

#include "PartitionMesh.h"

//----------------------------------------------------------------------------
PartitionMesh::PartitionMesh (const std::vector<Vector3f>& rkVertices,
    const std::vector<int>& rkIndices, const Plane3f& rkPlane,
    std::vector<Vector3f>& rkClipVertices, std::vector<int>& rkNegIndices,
    std::vector<int>& rkPosIndices)
    :
    m_rkIndices(rkIndices),
    m_rkPlane(rkPlane),
    m_kSignedDistance(rkVertices.size())
{
    // Make a copy of the incoming vertices.  If the mesh intersects the
    // plane, new vertices must be generated.  These are appended to the
    // rkClipVertices array.
    rkClipVertices = rkVertices;

    ClassifyVertices(rkClipVertices);
    ClassifyEdges(rkClipVertices);
    ClassifyTriangles(rkClipVertices,rkNegIndices,rkPosIndices);
}
//----------------------------------------------------------------------------
void PartitionMesh::ClassifyVertices (
    const std::vector<Vector3f>& rkClipVertices)
{
    const int iVQuantity = (const int)rkClipVertices.size();
    for (int i = 0; i < iVQuantity; i++)
    {
        m_kSignedDistance[i] = m_rkPlane.DistanceTo(rkClipVertices[i]);
    }
}
//----------------------------------------------------------------------------
void PartitionMesh::ClassifyEdges (std::vector<Vector3f>& rkClipVertices)
{
    const int iTQuantity = (const int)m_rkIndices.size()/3;
    int iNextIndex = (int)rkClipVertices.size();
    for (int i = 0; i < iTQuantity; i++)
    {
        int iV0 = m_rkIndices[3*i+0];
        int iV1 = m_rkIndices[3*i+1];
        int iV2 = m_rkIndices[3*i+2];
        float fSDist0 = m_kSignedDistance[iV0];
        float fSDist1 = m_kSignedDistance[iV1];
        float fSDist2 = m_kSignedDistance[iV2];

        EdgeKey kKey;
        float fT;
        Vector3f kIntr, kDiff;

        // The change-in-sign tests are structured this way to avoid numerical
        // round-off problems.  For example, fSDist0 > 0 and fSDist1 < 0, but
        // both are very small and fSDist0*fSDist1 = 0 due to round-off
        // errors.  The tests also guarantee consistency between this function
        // and ClassifyTriangles, the latter function using sign tests only on
        // the individual fSDist values.

        if ((fSDist0 > 0.0f && fSDist1 < 0.0f)
        ||  (fSDist0 < 0.0f && fSDist1 > 0.0f))
        {
            kKey = EdgeKey(iV0,iV1);
            if (m_kEMap.find(kKey) == m_kEMap.end())
            {
                fT = fSDist0/(fSDist0 - fSDist1);
                kDiff = rkClipVertices[iV1] - rkClipVertices[iV0];
                kIntr = rkClipVertices[iV0] + fT*kDiff;
                rkClipVertices.push_back(kIntr);
                m_kEMap[kKey] = std::make_pair(kIntr,iNextIndex);
                iNextIndex++;
            }
        }

        if ((fSDist1 > 0.0f && fSDist2 < 0.0f)
        ||  (fSDist1 < 0.0f && fSDist2 > 0.0f))
        {
            kKey = EdgeKey(iV1,iV2);
            if (m_kEMap.find(kKey) == m_kEMap.end())
            {
                fT = fSDist1/(fSDist1 - fSDist2);
                kDiff = rkClipVertices[iV2] - rkClipVertices[iV1];
                kIntr = rkClipVertices[iV1] + fT*kDiff;
                rkClipVertices.push_back(kIntr);
                m_kEMap[kKey] = std::make_pair(kIntr,iNextIndex);
                iNextIndex++;
            }
        }

        if ((fSDist2 > 0.0f && fSDist0 < 0.0f)
        ||  (fSDist2 < 0.0f && fSDist0 > 0.0f))
        {
            kKey = EdgeKey(iV2,iV0);
            if (m_kEMap.find(kKey) == m_kEMap.end())
            {
                fT = fSDist2/(fSDist2 - fSDist0);
                kDiff = rkClipVertices[iV0] - rkClipVertices[iV2];
                rkClipVertices.push_back(kIntr);
                m_kEMap[kKey] = std::make_pair(kIntr,iNextIndex);
                iNextIndex++;
            }
        }
    }
}
//----------------------------------------------------------------------------
void PartitionMesh::ClassifyTriangles (std::vector<Vector3f>& rkClipVertices,
    std::vector<int>& rkNegIndices, std::vector<int>& rkPosIndices)
{
    const int iTQuantity = (const int)m_rkIndices.size()/3;
    for (int i = 0; i < iTQuantity; i++)
    {
        int iV0 = m_rkIndices[3*i+0];
        int iV1 = m_rkIndices[3*i+1];
        int iV2 = m_rkIndices[3*i+2];
        float fSDist0 = m_kSignedDistance[iV0];
        float fSDist1 = m_kSignedDistance[iV1];
        float fSDist2 = m_kSignedDistance[iV2];

        if (fSDist0 > 0.0f)
        {
            if (fSDist1 > 0.0f)
            {
                if (fSDist2 > 0.0f)
                {
                    // +++
                    AppendTriangle(rkPosIndices,iV0,iV1,iV2);
                }
                else if (fSDist2 < 0.0f)
                {
                    // ++-
                    SplitTrianglePPM(rkNegIndices,rkPosIndices,iV0,iV1,iV2);
                }
                else
                {
                    // ++0
                    AppendTriangle(rkPosIndices,iV0,iV1,iV2);
                }
            }
            else if (fSDist1 < 0.0f)
            {
                if (fSDist2 > 0.0f)
                {
                    // +-+
                    SplitTrianglePPM(rkNegIndices,rkPosIndices,iV2,iV0,iV1);
                }
                else if (fSDist2 < 0.0f)
                {
                    // +--
                    SplitTriangleMMP(rkNegIndices,rkPosIndices,iV1,iV2,iV0);
                }
                else
                {
                    // +-0
                    SplitTrianglePMZ(rkNegIndices,rkPosIndices,iV0,iV1,iV2);
                }
            }
            else
            {
                if (fSDist2 > 0.0f)
                {
                    // +0+
                    AppendTriangle(rkPosIndices,iV0,iV1,iV2);
                }
                else if (fSDist2 < 0.0f)
                {
                    // +0-
                    SplitTriangleMPZ(rkNegIndices,rkPosIndices,iV2,iV0,iV1);
                }
                else
                {
                    // +00
                    AppendTriangle(rkPosIndices,iV0,iV1,iV2);
                }
            }
        }
        else if (fSDist0 < 0.0f)
        {
            if (fSDist1 > 0.0f)
            {
                if (fSDist2 > 0.0f)
                {
                    // -++
                    SplitTrianglePPM(rkNegIndices,rkPosIndices,iV1,iV2,iV0);
                }
                else if (fSDist2 < 0.0f)
                {
                    // -+-
                    SplitTriangleMMP(rkNegIndices,rkPosIndices,iV2,iV0,iV1);
                }
                else
                {
                    // -+0
                    SplitTriangleMPZ(rkNegIndices,rkPosIndices,iV0,iV1,iV2);
                }
            }
            else if (fSDist1 < 0.0f)
            {
                if (fSDist2 > 0.0f)
                {
                    // --+
                    SplitTriangleMMP(rkNegIndices,rkPosIndices,iV0,iV1,iV2);
                }
                else if (fSDist2 < 0.0f)
                {
                    // ---
                    AppendTriangle(rkNegIndices,iV0,iV1,iV2);
                }
                else
                {
                    // --0
                    AppendTriangle(rkNegIndices,iV0,iV1,iV2);
                }
            }
            else
            {
                if (fSDist2 > 0.0f)
                {
                    // -0+
                    SplitTrianglePMZ(rkNegIndices,rkPosIndices,iV2,iV0,iV1);
                }
                else if (fSDist2 < 0.0f)
                {
                    // -0-
                    AppendTriangle(rkNegIndices,iV0,iV1,iV2);
                }
                else
                {
                    // -00
                    AppendTriangle(rkNegIndices,iV0,iV1,iV2);
                }
            }
        }
        else
        {
            if (fSDist1 > 0.0f)
            {
                if (fSDist2 > 0.0f)
                {
                    // 0++
                    AppendTriangle(rkPosIndices,iV0,iV1,iV2);
                }
                else if (fSDist2 < 0.0f)
                {
                    // 0+-
                    SplitTrianglePMZ(rkNegIndices,rkPosIndices,iV1,iV2,iV0);
                }
                else
                {
                    // 0+0
                    AppendTriangle(rkPosIndices,iV0,iV1,iV2);
                }
            }
            else if (fSDist1 < 0.0f)
            {
                if (fSDist2 > 0.0f)
                {
                    // 0-+
                    SplitTriangleMPZ(rkNegIndices,rkPosIndices,iV1,iV2,iV0);
                }
                else if (fSDist2 < 0.0f)
                {
                    // 0--
                    AppendTriangle(rkNegIndices,iV0,iV1,iV2);
                }
                else
                {
                    // 0-0
                    AppendTriangle(rkNegIndices,iV0,iV1,iV2);
                }
            }
            else
            {
                if (fSDist2 > 0.0f)
                {
                    // 00+
                    AppendTriangle(rkPosIndices,iV0,iV1,iV2);
                }
                else if (fSDist2 < 0.0f)
                {
                    // 00-
                    AppendTriangle(rkNegIndices,iV0,iV1,iV2);
                }
                else
                {
                    // 000, reject triangles lying in the plane
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void PartitionMesh::AppendTriangle (std::vector<int>& rkIndices, int iV0,
    int iV1, int iV2)
{
    rkIndices.push_back(iV0);
    rkIndices.push_back(iV1);
    rkIndices.push_back(iV2);
}
//----------------------------------------------------------------------------
void PartitionMesh::SplitTrianglePPM (std::vector<int>& rkNegIndices,
    std::vector<int>& rkPosIndices, int iV0, int iV1, int iV2)
{
    int iV12 = m_kEMap[EdgeKey(iV1,iV2)].second;
    int iV20 = m_kEMap[EdgeKey(iV2,iV0)].second;
    rkPosIndices.push_back(iV0);
    rkPosIndices.push_back(iV1);
    rkPosIndices.push_back(iV12);
    rkPosIndices.push_back(iV0);
    rkPosIndices.push_back(iV12);
    rkPosIndices.push_back(iV20);
    rkNegIndices.push_back(iV2);
    rkNegIndices.push_back(iV20);
    rkNegIndices.push_back(iV12);
}
//----------------------------------------------------------------------------
void PartitionMesh::SplitTriangleMMP (std::vector<int>& rkNegIndices,
    std::vector<int>& rkPosIndices, int iV0, int iV1, int iV2)
{
    int iV12 = m_kEMap[EdgeKey(iV1,iV2)].second;
    int iV20 = m_kEMap[EdgeKey(iV2,iV0)].second;
    rkNegIndices.push_back(iV0);
    rkNegIndices.push_back(iV1);
    rkNegIndices.push_back(iV12);
    rkNegIndices.push_back(iV0);
    rkNegIndices.push_back(iV12);
    rkNegIndices.push_back(iV20);
    rkPosIndices.push_back(iV2);
    rkPosIndices.push_back(iV20);
    rkPosIndices.push_back(iV12);
}
//----------------------------------------------------------------------------
void PartitionMesh::SplitTrianglePMZ (std::vector<int>& rkNegIndices,
    std::vector<int>& rkPosIndices, int iV0, int iV1, int iV2)
{
    int iV01 = m_kEMap[EdgeKey(iV0,iV1)].second;
    rkPosIndices.push_back(iV2);
    rkPosIndices.push_back(iV0);
    rkPosIndices.push_back(iV01);
    rkNegIndices.push_back(iV2);
    rkNegIndices.push_back(iV01);
    rkNegIndices.push_back(iV1);
}
//----------------------------------------------------------------------------
void PartitionMesh::SplitTriangleMPZ (std::vector<int>& rkNegIndices,
    std::vector<int>& rkPosIndices, int iV0, int iV1, int iV2)
{
    int iV01 = m_kEMap[EdgeKey(iV0,iV1)].second;
    rkNegIndices.push_back(iV2);
    rkNegIndices.push_back(iV0);
    rkNegIndices.push_back(iV01);
    rkPosIndices.push_back(iV2);
    rkPosIndices.push_back(iV01);
    rkPosIndices.push_back(iV1);
}
//----------------------------------------------------------------------------
