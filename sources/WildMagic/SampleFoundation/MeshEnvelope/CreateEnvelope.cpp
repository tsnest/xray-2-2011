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
// Version: 4.0.1 (2006/08/17)

#include "CreateEnvelope.h"
#include "EndPoint.h"
#include "SegmentGraph.h"

typedef std::map<Rational,RPoint2> EdgeMap;
static Rational Zero(0);
static Rational One(1);

//----------------------------------------------------------------------------
static void InsertParameter (EdgeMap& rkEdge, const Rational& rkT)
{
    if (Zero < rkT && rkT < One)
    {
        const RPoint2& rkPoint0 = rkEdge[Zero];
        const RPoint2& rkPoint1 = rkEdge[One];
        rkEdge[rkT] = rkPoint0 + (rkPoint1 - rkPoint0)*rkT;
    }
}
//----------------------------------------------------------------------------
static void UpdateEdges (EdgeMap& rkEdge0, EdgeMap& rkEdge1)
{
    const RPoint2& rkU0 = rkEdge0[0];
    const RPoint2& rkU1 = rkEdge0[1];
    const RPoint2& rkV0 = rkEdge1[0];
    const RPoint2& rkV1 = rkEdge1[1];

    // The segments have a common point when U0+s*(U1-U0) = V0+t*(V1-V0) for
    // some values s and t with 0 <= s <= 1 and 0 <= t <= 1.  Rearranging
    // the terms and applying some algebra,
    //   s*(U1-U0) - t*(V1-V0) = V0-U0
    //   s*DotPerp(U1-U0,V1-V0) = DotPerp(V0-U0,V1-V0)
    //   t*DotPerp(U1-U0,V1-V0) = DotPerp(V0-U0,U1-U0)
    // where I used the identities DotPerp(U,V) = -DotPerp(V,U) and
    // DotPerp(W,W) = 0.  Using the notation in the other PfEdgesIntersect
    // comments, we have s*C = B and t*C = A.  As long as C is not zero, the
    // parameters are s = B/C and t = A/C.  These correspond to the
    // intersection of two lines.   The segments intersect at these values
    // as long as 0 <= A/C <= 1 and 0 <= B/C <= 1

    RPoint2 kU1mU0 = rkU1 - rkU0;
    RPoint2 kV1mV0 = rkV1 - rkV0;
    RPoint2 kV0mU0 = rkV0 - rkU0;
    Rational kA = kV0mU0.DotPerp(kU1mU0);
    Rational kB = kV0mU0.DotPerp(kV1mV0);
    Rational kC = kU1mU0.DotPerp(kV1mV0);

    if (kA*(kA - kC) <= Zero && kB*(kB - kC) <= Zero)
    {
        if (kC != Zero)
        {
            Rational kInvC = One/kC;
            Rational kTU = kB*kInvC;
            Rational kTV = kA*kInvC;
            InsertParameter(rkEdge0,kTU);
            InsertParameter(rkEdge1,kTV);
            return;
        }

        // C = 0, so the two inequalities above force A = 0 and B = 0, in
        // which case the segments are collinear.
        RPoint2 kV1mU0 = rkV1 - rkU0;
        Rational kNumer0 = kV0mU0.Dot(kU1mU0);
        Rational kNumer1 = kV1mU0.Dot(kU1mU0);
        Rational kInvDenom = One/kU1mU0.Dot(kU1mU0);
        Rational kTMin, kTMax;

        kC = kU1mU0.Dot(kV1mV0);
        bool bCPositive = (kC > Zero);
        if (bCPositive)
        {
            // U1-U0 and V1-V0 in same direction
            kTMin = kNumer0*kInvDenom;
            kTMax = kNumer1*kInvDenom;
        }
        else
        {
            // U1-U0 and V1-V0 in opposite directions
            kTMax = kNumer0*kInvDenom;
            kTMin = kNumer1*kInvDenom;
        }

        // Segment <U0,U1> maps to parameters [0,1] and segment <V0,V1> maps
        // to parameters [tmin,tmax].  Edge updates only occur if the
        // intervals overlap at more than a single point.
        if (Zero < kTMax && One > kTMin)
        {
            RPoint2 kU1mV0;
            Rational kDenom, kS;

            if (Zero < kTMin)
            {
                if (One > kTMax)
                {
                    // intersection is [tmin,tmax], <V0,V1> in <U0,U1>
                    InsertParameter(rkEdge0,kTMin);
                    InsertParameter(rkEdge0,kTMax);
                }
                else
                {
                    // intersection is [tmin,1], U1 in <V0,V1>
                    InsertParameter(rkEdge0,kTMin);
                    kU1mV0 = rkU1 - rkV0;
                    kNumer1 = kU1mV0.Dot(kV1mV0);
                    kDenom = kV1mV0.Dot(kV1mV0);
                    kS = kNumer1/kDenom;
                    InsertParameter(rkEdge1,kS);
                }
            }
            else
            {
                if (One > kTMax)
                {
                    // intersection is [0,tmax], U0 in <V0,V1>
                    InsertParameter(rkEdge0,kTMax);
                    kNumer0 = -kV0mU0.Dot(kV1mV0);
                    kDenom = kV1mV0.Dot(kV1mV0);
                    kS = kNumer0/kDenom;
                    InsertParameter(rkEdge1,kS);
                }
                else
                {
                    // intersection is [0,1], <U0,U1> in <V0,V1>
                    kNumer0 = -kV0mU0.Dot(kV1mV0);
                    kInvDenom = One/kV1mV0.Dot(kV1mV0);
                    kS = kNumer0*kInvDenom;
                    InsertParameter(rkEdge1,kS);
                    kU1mV0 = rkU1 - rkV0;
                    kNumer1 = kU1mV0.Dot(kV1mV0);
                    kS = kNumer1*kInvDenom;
                    InsertParameter(rkEdge1,kS);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
static void UpdateAllEdges (int iQuantity, EdgeMap** apkEdgeMap)
{
    // Construct the axis-aligned bounding boxes of the edges.
    RPoint2* akMin = WM4_NEW RPoint2[iQuantity];
    RPoint2* akMax = WM4_NEW RPoint2[iQuantity];
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        EdgeMap& rkEdge = *apkEdgeMap[i];

        RPoint2& rkEnd0 = rkEdge[Zero];
        RPoint2& rkEnd1 = rkEdge[One];

        if (rkEnd0.X() <= rkEnd1.X())
        {
            akMin[i].X() = rkEnd0.X();
            akMax[i].X() = rkEnd1.X();
        }
        else
        {
            akMin[i].X() = rkEnd1.X();
            akMax[i].X() = rkEnd0.X();
        }

        if (rkEnd0.Y() <= rkEnd1.Y())
        {
            akMin[i].Y() = rkEnd0.Y();
            akMax[i].Y() = rkEnd1.Y();
        }
        else
        {
            akMin[i].Y() = rkEnd1.Y();
            akMax[i].Y() = rkEnd0.Y();
        }
    }

    // Store the x-extremes for the AABBs in a data structure to be sorted.
    // The "Type" field indicates whether the x-value is the minimum (Type
    // is 0) or maximum (Type is 1).  The "Index" field stores the edge
    // index for use as a lookup in the overlap tests.
    int iEndQuantity = 2*iQuantity;
    std::vector<EndPoint> kXEndPoint(iEndQuantity);
    int j;
    for (i = 0, j = 0; i < iQuantity; i++)
    {
        kXEndPoint[j].Type = 0;
        kXEndPoint[j].Value = akMin[i].X();
        kXEndPoint[j].Index = i;
        j++;

        kXEndPoint[j].Type = 1;
        kXEndPoint[j].Value = akMax[i].X();
        kXEndPoint[j].Index = i;
        j++;
    }

    // Sort the x-values.
    std::sort(kXEndPoint.begin(),kXEndPoint.end());

    // The active set of rectangles (stored by index in array).
    std::set<int> kActive;

    // The set of overlapping rectangles (stored by index pairs in array).
    std::set<std::pair<int,int> > kOverlap;

    // Sweep through the end points to determine overlapping x-intervals.
    for (i = 0; i < iEndQuantity; i++)
    {
        EndPoint& rkEnd = kXEndPoint[i];
        int iIndex = rkEnd.Index;
        if (rkEnd.Type == 0)  // an interval 'begin' value
        {
            // The current AABB overlaps in the x-direction with all the
            // active intervals.  Now check for y-overlap.
            std::set<int>::iterator pkIter = kActive.begin();
            for (/**/; pkIter != kActive.end(); pkIter++)
            {
                // Rectangles iAIndex and iIndex overlap in the x-dimension.
                // Test for overlap in the y-dimension.
                int iAIndex = *pkIter;
                if (akMax[iAIndex].Y() >= akMin[iIndex].Y()
                &&  akMin[iAIndex].Y() <= akMax[iIndex].Y())
                {
                    // If the edges share an end point, there is no need to
                    // test later for overlap.
                    EdgeMap& rkEdge0 = *apkEdgeMap[iIndex];
                    EdgeMap& rkEdge1 = *apkEdgeMap[iAIndex];
                    RPoint2& rkE0P0 = rkEdge0[Zero];
                    RPoint2& rkE0P1 = rkEdge0[One];
                    RPoint2& rkE1P0 = rkEdge1[Zero];
                    RPoint2& rkE1P1 = rkEdge1[One];
                    if (rkE0P0 == rkE1P0 || rkE0P0 == rkE1P1
                    ||  rkE0P1 == rkE1P0 || rkE0P1 == rkE1P1)
                    {
                        continue;
                    }

                    kOverlap.insert(std::make_pair(iAIndex,iIndex));
                }
            }
            kActive.insert(iIndex);
        }
        else  // an interval 'end' value
        {
            kActive.erase(iIndex);
        }
    }

    // Search for edge-edge intersections by comparing only those edges whose
    // AABBs overlap.
    std::set<std::pair<int,int> >::const_iterator pkIter = kOverlap.begin();
    for (/**/; pkIter != kOverlap.end(); pkIter++)
    {
        int i0 = pkIter->first;
        int i1 = pkIter->second;
        EdgeMap& rkEdge0 = *apkEdgeMap[i0];
        EdgeMap& rkEdge1 = *apkEdgeMap[i1];
        UpdateEdges(rkEdge0,rkEdge1);
    }

    WM4_DELETE[] akMax;
    WM4_DELETE[] akMin;
}
//----------------------------------------------------------------------------
void CreateEnvelope (int iVertexQuantity, const Wm4::Vector2f* akVertex,
    int iIndexQuantity, const int* aiIndex, int& riEnvelopeQuantity,
    Wm4::Vector2f*& rakEnvelopeVertex)
{
    // The graph of vertices and edges to be used for constructing the
    // obstacle envelope.
    SegmentGraph* pkGraph = WM4_NEW SegmentGraph;

    // Convert the vertices to rational points to allow exact arithmetic,
    // thereby avoiding problems with numerical round-off errors.
    RPoint2* akRVertex = WM4_NEW RPoint2[iVertexQuantity];
    int i;
    for (i = 0; i < iVertexQuantity; i++)
    {
        akRVertex[i].X() = Rational(akVertex[i].X());
        akRVertex[i].Y() = Rational(akVertex[i].Y());
    }

    // Insert the 2D mesh edges into the graph.
    const int* piIndex = aiIndex;
    int iTQuantity = iIndexQuantity/3;
    for (i = 0; i < iTQuantity; i++)
    {
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;
        pkGraph->InsertEdge(akRVertex[iV0],akRVertex[iV1]);
        pkGraph->InsertEdge(akRVertex[iV1],akRVertex[iV2]);
        pkGraph->InsertEdge(akRVertex[iV2],akRVertex[iV0]);
    }
    WM4_DELETE[] akRVertex;

    // Represent each edge as a map of points ordered by rational parameter
    // values, each point P(t) = End0 + t*(End1-End0), where End0 and End1
    // are the rational endpoints of the edge and t is the rational
    // parameter for the edge point P(t).
    std::set<SegmentGraph::Edge>& rkESet = pkGraph->GetEdges();
    int iEQuantity = (int)rkESet.size();
    EdgeMap** apkEdge = WM4_NEW EdgeMap*[iEQuantity];
    std::set<SegmentGraph::Edge>::iterator pkIter = rkESet.begin();
    for (i = 0; i < iEQuantity; i++, pkIter++)
    {
        SegmentGraph::Edge kEdge = *pkIter;
        EdgeMap* pkEdge = WM4_NEW EdgeMap;
        (*pkEdge)[0] = kEdge.GetVertex(0)->Position;
        (*pkEdge)[1] = kEdge.GetVertex(1)->Position;
        apkEdge[i] = pkEdge;
    }

    UpdateAllEdges(iEQuantity,apkEdge);

    // Recreate the graph, now using the segmented edges from the original
    // graph.
    WM4_DELETE pkGraph;
    pkGraph = WM4_NEW SegmentGraph;
    for (i = 0; i < iEQuantity; i++)
    {
        // Each graph edge is a pair of consecutive edge points.
        EdgeMap* pkEdge = apkEdge[i];

        // Get first point.
        EdgeMap::iterator pkEI = pkEdge->begin();
        RPoint2* pkPoint0 = &pkEI->second;

        // Get remaining points.
        for (++pkEI; pkEI != pkEdge->end(); pkEI++)
        {
            RPoint2* pkPoint1 = &pkEI->second;
            pkGraph->InsertEdge(*pkPoint0,*pkPoint1);
            pkPoint0 = pkPoint1;
        }

        WM4_DELETE pkEdge;
    }
    WM4_DELETE[] apkEdge;

    std::vector<RPoint2> kEnvelope;
    pkGraph->ExtractEnvelope(kEnvelope);

    // Convert the vertices back to floating-point values and return to the
    // caller.
    riEnvelopeQuantity = (int)kEnvelope.size();
    rakEnvelopeVertex = WM4_NEW Wm4::Vector2f[riEnvelopeQuantity];
    for (i = 0; i < riEnvelopeQuantity; i++)
    {
        const RPoint2& rkPoint = kEnvelope[i];
        rkPoint.X().ConvertTo(rakEnvelopeVertex[i].X());
        rkPoint.Y().ConvertTo(rakEnvelopeVertex[i].Y());
    }

    WM4_DELETE pkGraph;
}
//----------------------------------------------------------------------------
