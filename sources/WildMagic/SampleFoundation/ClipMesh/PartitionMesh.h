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

#ifndef PARTITIONMESH_H
#define PARTITIONMESH_H

#include "Wm4EdgeKey.h"
#include "Wm4Plane3.h"
using namespace Wm4;

class PartitionMesh
{
public:
    PartitionMesh (const std::vector<Vector3f>& rkVertices,
        const std::vector<int>& rkIndices, const Plane3f& rkPlane,
        std::vector<Vector3f>& rkClipVertices, std::vector<int>& rkNegIndices,
        std::vector<int>& rkPosIndices);

private:
    void ClassifyVertices (const std::vector<Vector3f>& rkClipVertices);
    void ClassifyEdges (std::vector<Vector3f>& rkClipVertices);
    void ClassifyTriangles (std::vector<Vector3f>& rkClipVertices,
        std::vector<int>& rkNegIndices, std::vector<int>& rkPosIndices);

    void AppendTriangle (std::vector<int>& rkIndices, int iV0, int iV1,
        int iV2);

    void SplitTrianglePPM (std::vector<int>& rkNegIndices,
        std::vector<int>& rkPosIndices, int iV0, int iV1, int iV2);

    void SplitTriangleMMP (std::vector<int>& rkNegIndices,
        std::vector<int>& rkPosIndices, int iV0, int iV1, int iV2);

    void SplitTrianglePMZ (std::vector<int>& rkNegIndices,
        std::vector<int>& rkPosIndices, int iV0, int iV1, int iV2);

    void SplitTriangleMPZ (std::vector<int>& rkNegIndices,
        std::vector<int>& rkPosIndices, int iV0, int iV1, int iV2);

    const std::vector<int>& m_rkIndices;
    const Plane3f& m_rkPlane;

    // Stores the signed distances from the vertices to the plane.
    std::vector<float> m_kSignedDistance;

    // Stores the edges whose vertices are on opposite sides of the
    // plane.  The key is a pair of indices into the vertex array.
    // The value is the point of intersection of the edge with the
    // plane and an index into m_kVertices (the index is larger or
    // equal to the number of vertices of incoming rkVertices).
    std::map<EdgeKey,std::pair<Vector3f, int> > m_kEMap;
};

#endif
