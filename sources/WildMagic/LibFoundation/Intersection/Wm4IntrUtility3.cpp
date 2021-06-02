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

#include "Wm4FoundationPCH.h"
#include "Wm4IntrUtility3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
// IntrAxis<Real>
//----------------------------------------------------------------------------
template <class Real>
bool IntrAxis<Real>::Test (const Vector3<Real>& rkAxis,
    const Vector3<Real> akSegment[2], const Triangle3<Real>& rkTriangle,
    const Vector3<Real>& rkVelocity, Real fTMax, Real& rfTFirst,
    Real& rfTLast)
{
    Real fMin0, fMax0;
    GetProjection(rkAxis,akSegment,fMin0,fMax0);

    Real fMin1, fMax1;
    GetProjection(rkAxis,rkTriangle,fMin1,fMax1);

    return Test(rkAxis,rkVelocity,fMin0,fMax0,fMin1,fMax1,fTMax,rfTFirst,
        rfTLast);
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrAxis<Real>::Test (const Vector3<Real>& rkAxis,
    const Vector3<Real> akSegment[2], const Box3<Real>& rkBox,
    const Vector3<Real>& rkVelocity, Real fTMax, Real& rfTFirst,
    Real& rfTLast)
{
    Real fMin0, fMax0;
    GetProjection(rkAxis,akSegment,fMin0,fMax0);

    Real fMin1, fMax1;
    GetProjection(rkAxis,rkBox,fMin1,fMax1);
    
    return Test(rkAxis,rkVelocity,fMin0,fMax0,fMin1,fMax1,fTMax,rfTFirst,
        rfTLast);
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrAxis<Real>::Test (const Vector3<Real>& rkAxis,
    const Triangle3<Real>& rkTriangle, const Box3<Real>& rkBox,
    const Vector3<Real>& rkVelocity, Real fTMax, Real& rfTFirst,
    Real& rfTLast)
{
    Real fMin0, fMax0;
    GetProjection(rkAxis,rkTriangle,fMin0,fMax0);

    Real fMin1, fMax1;
    GetProjection(rkAxis,rkBox,fMin1,fMax1);
    
    return Test(rkAxis,rkVelocity,fMin0,fMax0,fMin1,fMax1,fTMax,rfTFirst,
        rfTLast);
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrAxis<Real>::Test (const Vector3<Real>& rkAxis, const Box3<Real>& rkBox0,
    const Box3<Real>& rkBox1, const Vector3<Real>& rkVelocity, Real fTMax,
    Real& rfTFirst, Real& rfTLast)
{
    Real fMin0, fMax0;
    GetProjection(rkAxis,rkBox0,fMin0,fMax0);

    Real fMin1, fMax1;
    GetProjection(rkAxis,rkBox1,fMin1,fMax1);
    
    return Test(rkAxis,rkVelocity,fMin0,fMax0,fMin1,fMax1,fTMax,rfTFirst,
        rfTLast);
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrAxis<Real>::Find (const Vector3<Real>& rkAxis,
    const Vector3<Real> akSegment[2], const Triangle3<Real>& rkTriangle,
    const Vector3<Real>& rkVelocity, Real fTMax, Real& rfTFirst,
    Real& rfTLast, int& reSide, IntrConfiguration<Real>& rkSegCfgFinal,
    IntrConfiguration<Real>& rkTriCfgFinal)
{
	IntrConfiguration<Real> kSegCfgStart;
    GetConfiguration(rkAxis,akSegment,kSegCfgStart);

	IntrConfiguration<Real> kTriCfgStart;
    GetConfiguration(rkAxis,rkTriangle,kTriCfgStart);

    return Find(rkAxis,rkVelocity,kSegCfgStart,kTriCfgStart,fTMax,reSide,
        rkSegCfgFinal,rkTriCfgFinal,rfTFirst,rfTLast);
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrAxis<Real>::Find (const Vector3<Real>& rkAxis,
    const Vector3<Real> akSegment[2], const Box3<Real>& rkBox,
    const Vector3<Real>& rkVelocity, Real fTMax, Real& rfTFirst,
    Real& rfTLast, int& reSide, IntrConfiguration<Real>& rkSegCfgFinal,
    IntrConfiguration<Real>& rkBoxCfgFinal)
{
    IntrConfiguration<Real> kSegCfgStart;
    GetConfiguration(rkAxis,akSegment,kSegCfgStart);

    IntrConfiguration<Real> kBoxCfgStart;
    GetConfiguration(rkAxis,rkBox,kBoxCfgStart);

    return Find(rkAxis,rkVelocity,kSegCfgStart,kBoxCfgStart,fTMax,reSide,
        rkSegCfgFinal,rkBoxCfgFinal,rfTFirst,rfTLast);
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrAxis<Real>::Find (const Vector3<Real>& rkAxis,
    const Triangle3<Real>& rkTriangle, const Box3<Real>& rkBox,
    const Vector3<Real>& rkVelocity, Real fTMax, Real& rfTFirst,
    Real& rfTLast, int& reSide, IntrConfiguration<Real>& rkTriCfgFinal,
    IntrConfiguration<Real>& rkBoxCfgFinal)
{
    IntrConfiguration<Real> kTriCfgStart;
    GetConfiguration(rkAxis,rkTriangle,kTriCfgStart);

    IntrConfiguration<Real> kBoxCfgStart;
    GetConfiguration(rkAxis,rkBox,kBoxCfgStart);

    return Find(rkAxis,rkVelocity,kTriCfgStart,kBoxCfgStart,fTMax,reSide,
        rkTriCfgFinal,rkBoxCfgFinal,rfTFirst,rfTLast);
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrAxis<Real>::Find (const Vector3<Real>& rkAxis,
    const Box3<Real>& rkBox0, const Box3<Real>& rkBox1,
    const Vector3<Real>& rkVelocity, Real fTMax, Real& rfTFirst,
    Real& rfTLast, int& reSide, IntrConfiguration<Real>& rkBox0CfgFinal,
    IntrConfiguration<Real>& rkBox1CfgFinal)
{
    IntrConfiguration<Real> kBox0CfgStart;
    GetConfiguration(rkAxis,rkBox0,kBox0CfgStart);

    IntrConfiguration<Real> kBox1CfgStart;
    GetConfiguration(rkAxis,rkBox1,kBox1CfgStart);

    return Find(rkAxis,rkVelocity,kBox0CfgStart,kBox1CfgStart,fTMax,
        reSide,rkBox0CfgFinal,rkBox1CfgFinal,rfTFirst,rfTLast);
}
//----------------------------------------------------------------------------
template <class Real>
void IntrAxis<Real>::GetProjection (const Vector3<Real>& rkAxis,
    const Vector3<Real> akSegment[2], Real& rfMin, Real& rfMax)
{
    Real afDot[2] =
    {
        rkAxis.Dot(akSegment[0]),
        rkAxis.Dot(akSegment[1])
    };

    rfMin = afDot[0];
    rfMax = rfMin;

    if (afDot[1] < rfMin)
    {
        rfMin = afDot[1];
    }
    else if (afDot[1] > rfMax)
    {
        rfMax = afDot[1];
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntrAxis<Real>::GetProjection (const Vector3<Real>& rkAxis,
    const Triangle3<Real>& rkTriangle, Real& rfMin, Real& rfMax)
{
    Real afDot[3] =
    {
        rkAxis.Dot(rkTriangle.V[0]),
        rkAxis.Dot(rkTriangle.V[1]),
        rkAxis.Dot(rkTriangle.V[2])
    };

    rfMin = afDot[0];
    rfMax = rfMin;

    if (afDot[1] < rfMin)
    {
        rfMin = afDot[1];
    }
    else if (afDot[1] > rfMax)
    {
        rfMax = afDot[1];
    }

    if (afDot[2] < rfMin)
    {
        rfMin = afDot[2];
    }
    else if (afDot[2] > rfMax)
    {
        rfMax = afDot[2];
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntrAxis<Real>::GetProjection (const Vector3<Real>& rkAxis,
    const Box3<Real>& rkBox, Real& rfMin, Real& rfMax)
{
    Real fOrigin = rkAxis.Dot(rkBox.Center);
    Real fMaximumExtent =
        Math<Real>::FAbs(rkBox.Extent[0]*rkAxis.Dot(rkBox.Axis[0])) +
        Math<Real>::FAbs(rkBox.Extent[1]*rkAxis.Dot(rkBox.Axis[1])) +
        Math<Real>::FAbs(rkBox.Extent[2]*rkAxis.Dot(rkBox.Axis[2]));

    rfMin = fOrigin - fMaximumExtent;
    rfMax = fOrigin + fMaximumExtent;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrAxis<Real>::GetConfiguration (const Vector3<Real>& rkAxis, 
    const Vector3<Real> akSegment[2], IntrConfiguration<Real>& rkCfg)
{
    Real afDot[2] =
    {
        rkAxis.Dot(akSegment[0]),
        rkAxis.Dot(akSegment[1])
    };

    if (Math<Real>::FAbs(afDot[1]-afDot[0]) < Math<Real>::ZERO_TOLERANCE)
    {
        rkCfg.Map = IntrConfiguration<Real>::m2;
    }
    else
    {
        rkCfg.Map = IntrConfiguration<Real>::m11;
    }

    if (afDot[0] < afDot[1])
    {
        rkCfg.Min = afDot[0];
        rkCfg.Max = afDot[1];
        rkCfg.Index[0] = 0;
        rkCfg.Index[1] = 1;
    }
    else
    {
        rkCfg.Min = afDot[1];
        rkCfg.Max = afDot[0];
        rkCfg.Index[0] = 1;
        rkCfg.Index[1] = 0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntrAxis<Real>::GetConfiguration (const Vector3<Real>& rkAxis, 
    const Triangle3<Real>& rkTriangle, IntrConfiguration<Real>& rkCfg)
{
    // find projections of vertices onto potential separating axis
    Real fD0 = rkAxis.Dot(rkTriangle.V[0]);
    Real fD1 = rkAxis.Dot(rkTriangle.V[1]);
    Real fD2 = rkAxis.Dot(rkTriangle.V[2]);

    // explicit sort of vertices to construct a ContactConfig
    if (fD0 <= fD1)
    {
        if (fD1 <= fD2) // D0 <= D1 <= D2
        {
            if (fD0 != fD1)
            {
                if (fD1 != fD2)
                {
                    rkCfg.Map = IntrConfiguration<Real>::m111;
                }
                else
                {
                    rkCfg.Map = IntrConfiguration<Real>::m12;
                }
            }
            else // ( D0 == D1 )
            {
                if (fD1 != fD2)
                {
                    rkCfg.Map = IntrConfiguration<Real>::m21;
                }
                else
                {
                    rkCfg.Map = IntrConfiguration<Real>::m3;
                }
            }
            rkCfg.Index[0] = 0;
            rkCfg.Index[1] = 1;
            rkCfg.Index[2] = 2;
            rkCfg.Min = fD0;
            rkCfg.Max = fD2;
        }
        else if (fD0 <= fD2) // D0 <= D2 < D1
        {
            if (fD0 != fD2)
            {
                rkCfg.Map = IntrConfiguration<Real>::m111;
                rkCfg.Index[0] = 0;
                rkCfg.Index[1] = 2;
                rkCfg.Index[2] = 1;
            }
            else
            {
                rkCfg.Map = IntrConfiguration<Real>::m21;
                rkCfg.Index[0] = 2;
                rkCfg.Index[1] = 0;
                rkCfg.Index[2] = 1;
            }
            rkCfg.Min = fD0;
            rkCfg.Max = fD1;
        }
        else // D2 < D0 <= D1
        {
            if (fD0 != fD1)
            {
                rkCfg.Map = IntrConfiguration<Real>::m111;
            }
            else
            {
                rkCfg.Map = IntrConfiguration<Real>::m12;
            }

            rkCfg.Index[0] = 2;
            rkCfg.Index[1] = 0;
            rkCfg.Index[2] = 1;
            rkCfg.Min = fD2;
            rkCfg.Max = fD1;
        }
    }
    else if (fD2 <= fD1) // D2 <= D1 < D0
    {
        if (fD2 != fD1)
        {
            rkCfg.Map = IntrConfiguration<Real>::m111;
            rkCfg.Index[0] = 2;
            rkCfg.Index[1] = 1;
            rkCfg.Index[2] = 0;
        }
        else
        {
            rkCfg.Map = IntrConfiguration<Real>::m21;
            rkCfg.Index[0] = 1;
            rkCfg.Index[1] = 2;
            rkCfg.Index[2] = 0;

        }
        rkCfg.Min = fD2;
        rkCfg.Max = fD0;
    }
    else if (fD2 <= fD0) // D1 < D2 <= D0
    {
        if (fD2 != fD0) 
        {
            rkCfg.Map = IntrConfiguration<Real>::m111;
        }
        else
        {
            rkCfg.Map = IntrConfiguration<Real>::m12;
        }

        rkCfg.Index[0] = 1;
        rkCfg.Index[1] = 2;
        rkCfg.Index[2] = 0;
        rkCfg.Min = fD1;
        rkCfg.Max = fD0;
    }
    else // D1 < D0 < D2
    {
        rkCfg.Map = IntrConfiguration<Real>::m111;
        rkCfg.Index[0] = 1;
        rkCfg.Index[1] = 0;
        rkCfg.Index[2] = 2;
        rkCfg.Min = fD1;
        rkCfg.Max = fD2;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntrAxis<Real>::GetConfiguration (const Vector3<Real>& rkAxis, 
    const Box3<Real>& rkBox, IntrConfiguration<Real>& rkCfg)
{
    // Description of coordinate ordering scheme for ContactConfig.Index.
    //
    // Vertex number (up/down) vs. sign of extent (only matters in mapping
    // back)
    //   012
    // 0 ---
    // 1 +--
    // 2 -+-
    // 3 ++-
    // 4 --+
    // 5 +-+
    // 6 -++
    // 7 +++
    //
    // When it returns an ordering in the ContactConfig, it is also
    // guarenteed to be in-order (if 4 vertices, then they are guarunteed in
    // an order that will create a box, e.g. 0,1,3,2).
    //
    // assert: akAxis is an array containing unit length vectors

    Real afAxis[3] =
    {
        rkAxis.Dot(rkBox.Axis[0]),
        rkAxis.Dot(rkBox.Axis[1]),
        rkAxis.Dot(rkBox.Axis[2])
    };

    Real afAAxis[3] =
    {
        Math<Real>::FAbs(afAxis[0]),
        Math<Real>::FAbs(afAxis[1]),
        Math<Real>::FAbs(afAxis[2])
    };

    Real fMaxProjectedExtent;

    if (afAAxis[0] < Math<Real>::ZERO_TOLERANCE)
    {
        if (afAAxis[1] < Math<Real>::ZERO_TOLERANCE)
        {
            // face-face
            rkCfg.Map = IntrConfiguration<Real>::m44;

            fMaxProjectedExtent = afAAxis[2]*rkBox.Extent[2];

            // faces have normals along axis[2]
            if (afAxis[2] > (Real)0)
            {       
                rkCfg.Index[0] = 0;
                rkCfg.Index[1] = 1;
                rkCfg.Index[2] = 3;
                rkCfg.Index[3] = 2;

                rkCfg.Index[4] = 6;
                rkCfg.Index[5] = 7;
                rkCfg.Index[6] = 5;
                rkCfg.Index[7] = 4;
            }
            else
            {
                rkCfg.Index[0] = 6;
                rkCfg.Index[1] = 7;
                rkCfg.Index[2] = 5;
                rkCfg.Index[3] = 4;

                rkCfg.Index[4] = 0;
                rkCfg.Index[5] = 1;
                rkCfg.Index[6] = 3;
                rkCfg.Index[7] = 2;
            }
        }
        else if (afAAxis[2] < Math<Real>::ZERO_TOLERANCE)
        {
            // face-face
            rkCfg.Map = IntrConfiguration<Real>::m44;

            fMaxProjectedExtent = afAAxis[1]*rkBox.Extent[1];

            // faces have normals along axis[1]
            if (afAxis[1] > (Real)0) 
            {
                rkCfg.Index[0] = 4;
                rkCfg.Index[1] = 5;
                rkCfg.Index[2] = 1;
                rkCfg.Index[3] = 0;

                rkCfg.Index[4] = 2;
                rkCfg.Index[5] = 3;
                rkCfg.Index[6] = 7;
                rkCfg.Index[7] = 6;
            }
            else
            {
                rkCfg.Index[0] = 2;
                rkCfg.Index[1] = 3;
                rkCfg.Index[2] = 7;
                rkCfg.Index[3] = 6;

                rkCfg.Index[4] = 4;
                rkCfg.Index[5] = 5;
                rkCfg.Index[6] = 1;
                rkCfg.Index[7] = 0;
            }
        }
        else // only afAxis[0] is equal to 0
        {
            // seg-seg
            rkCfg.Map = IntrConfiguration<Real>::m2_2;

            fMaxProjectedExtent = afAAxis[1]*rkBox.Extent[1] +
                afAAxis[2]*rkBox.Extent[2];

            // axis 0 is perpendicular to rkAxis
            if (afAxis[1] > (Real)0)
            {
                if (afAxis[2] > (Real)0) 
                {
                    rkCfg.Index[0] = 0;
                    rkCfg.Index[1] = 1;

                    rkCfg.Index[6] = 6;
                    rkCfg.Index[7] = 7;
                }
                else 
                {
                    rkCfg.Index[0] = 4;
                    rkCfg.Index[1] = 5;

                    rkCfg.Index[6] = 2;
                    rkCfg.Index[7] = 3;
                }
            }
            else // afAxis[1] < 0
            {
                if (afAxis[2] > (Real)0)
                {
                    rkCfg.Index[0] = 2;
                    rkCfg.Index[1] = 3;

                    rkCfg.Index[6] = 4;
                    rkCfg.Index[7] = 5;
                }
                else
                {
                    rkCfg.Index[0] = 6;
                    rkCfg.Index[1] = 7;

                    rkCfg.Index[6] = 0;
                    rkCfg.Index[7] = 1;
                }
            }
        }
    }
    else if (afAAxis[1] < Math<Real>::ZERO_TOLERANCE)
    {
        if (afAAxis[2] < Math<Real>::ZERO_TOLERANCE)
        {
            // face-face
            rkCfg.Map = IntrConfiguration<Real>::m44;

            fMaxProjectedExtent = afAAxis[0]*rkBox.Extent[0];

            // faces have normals along axis[0]
            if (afAxis[0] > (Real)0)
            {
                rkCfg.Index[0] = 0;
                rkCfg.Index[1] = 2;
                rkCfg.Index[2] = 6;
                rkCfg.Index[3] = 4;

                rkCfg.Index[4] = 5;
                rkCfg.Index[5] = 7;
                rkCfg.Index[6] = 3;
                rkCfg.Index[7] = 1;
            }
            else
            {
                rkCfg.Index[4] = 0;
                rkCfg.Index[5] = 2;
                rkCfg.Index[6] = 6;
                rkCfg.Index[7] = 4;

                rkCfg.Index[0] = 5;
                rkCfg.Index[1] = 7;
                rkCfg.Index[2] = 3;
                rkCfg.Index[3] = 1;
            }

        }
        else // only afAxis[1] is equal to 0
        {
            // seg-seg
            rkCfg.Map = IntrConfiguration<Real>::m2_2;

            fMaxProjectedExtent = afAAxis[0]*rkBox.Extent[0] + 
                afAAxis[2]*rkBox.Extent[2];

            // axis 1 is perpendicular to rkAxis
            if (afAxis[0] > (Real)0)
            {
                if (afAxis[2] > (Real)0) 
                {
                    rkCfg.Index[0] = 0;
                    rkCfg.Index[1] = 2;

                    rkCfg.Index[6] = 5;
                    rkCfg.Index[7] = 7;
                }
                else 
                {
                    rkCfg.Index[0] = 4;
                    rkCfg.Index[1] = 6;

                    rkCfg.Index[6] = 1;
                    rkCfg.Index[7] = 3;
                }
            }
            else // afAxis[0] < 0
            {
                if (afAxis[2] > (Real)0)
                {
                    rkCfg.Index[0] = 1;
                    rkCfg.Index[1] = 3;

                    rkCfg.Index[6] = 4;
                    rkCfg.Index[7] = 6;
                }
                else
                {
                    rkCfg.Index[0] = 5;
                    rkCfg.Index[1] = 7;

                    rkCfg.Index[6] = 0;
                    rkCfg.Index[7] = 2;
                }
            }
        }
    }
    
    else if (afAAxis[2] < Math<Real>::ZERO_TOLERANCE)
    {
        // only axis2 less than zero
        // seg-seg
        rkCfg.Map = IntrConfiguration<Real>::m2_2;

        fMaxProjectedExtent = afAAxis[0]*rkBox.Extent[0] +
            afAAxis[1]*rkBox.Extent[1];

        // axis 2 is perpendicular to rkAxis
        if (afAxis[0] > (Real)0)
        {
            if (afAxis[1] > (Real)0) 
            {
                rkCfg.Index[0] = 0;
                rkCfg.Index[1] = 4;

                rkCfg.Index[6] = 3;
                rkCfg.Index[7] = 7;
            }
            else 
            {
                rkCfg.Index[0] = 2;
                rkCfg.Index[1] = 6;

                rkCfg.Index[6] = 1;
                rkCfg.Index[7] = 5;
            }
        }
        else // afAxis[0] < 0
        {
            if (afAxis[1] > (Real)0)
            {
                rkCfg.Index[0] = 1;
                rkCfg.Index[1] = 5;

                rkCfg.Index[6] = 2;
                rkCfg.Index[7] = 6;
            }
            else
            {
                rkCfg.Index[0] = 3;
                rkCfg.Index[1] = 7;

                rkCfg.Index[6] = 0;
                rkCfg.Index[7] = 4;
            }
        }
    }
  
    else // no axis is equal to zero
    {
        // point-point (unique maximal and minimal vertex)
        rkCfg.Map = IntrConfiguration<Real>::m1_1;

        fMaxProjectedExtent = afAAxis[0]*rkBox.Extent[0] +
            afAAxis[1]*rkBox.Extent[1] + afAAxis[2]*rkBox.Extent[2];

        // only these two vertices matter, the rest are irrelevant
        rkCfg.Index[0] = 
            (afAxis[0] > (Real)0.0 ? 0 : 1) + 
            (afAxis[1] > (Real)0.0 ? 0 : 2) + 
            (afAxis[2] > (Real)0.0 ? 0 : 4);
        // by ordering the vertices this way, opposite corners add up to 7
        rkCfg.Index[7] = 7 - rkCfg.Index[0];
    }

    // Find projections onto line
    Real fOrigin = rkAxis.Dot(rkBox.Center);
    rkCfg.Min = fOrigin - fMaxProjectedExtent;
    rkCfg.Max = fOrigin + fMaxProjectedExtent;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrAxis<Real>::Test (const Vector3<Real>& rkAxis,
    const Vector3<Real>& rkVelocity, Real fMin0, Real fMax0, Real fMin1,
    Real fMax1, Real fTMax, Real& rfTFirst, Real& rfTLast)
{
    // Static separating axis test.  Returns false iff object0 and object1
    // do not intersect in the interval [0,TMax] on any separating axis
    // ( TFirst > TLast || TFirst > TMax ) during the time interval, that is,
    // a quick out.  Returns true otherwise.
    //
    // fMin0, fMax0, fMin1, and fMax1 are the minimal and maximal points of
    // whatever object object0 and object1 are projected onto the test axis.
    //
    // rkVelocity is Velocity1 - Velocity0

    Real fT;
    Real fSpeed = rkAxis.Dot(rkVelocity);
    
    if (fMax1 < fMin0) // object1 on left of object0
    {
        if (fSpeed <= (Real)0) // object1 moving away from object0
        {
            return false;
        }

        // find first time of contact on this axis
        fT = (fMin0 - fMax1)/fSpeed;
        if (fT > rfTFirst)
        {
            rfTFirst = fT;
        }

        // quick out: intersection after desired interval
        if (rfTFirst > fTMax)
        {
            return false;   
        }

        // find last time of contact on this axis
        fT = (fMax0 - fMin1)/fSpeed;
        if (fT < rfTLast)
        {
            rfTLast = fT;
        }

        // quick out: intersection before desired interval
        if (rfTFirst > rfTLast)
        {
            return false; 
        }
    }
    else if (fMax0 < fMin1)   // object1 on right of object0
    {
        if (fSpeed >= (Real)0) // object1 moving away from object0
        {
            return false;
        }

        // find first time of contact on this axis
        fT = (fMax0 - fMin1)/fSpeed;
        if (fT > rfTFirst)
        {
            rfTFirst = fT;
        }

        // quick out: intersection after desired interval
        if (rfTFirst > fTMax)
        {
            return false;   
        }

        // find last time of contact on this axis
        fT = (fMin0 - fMax1)/fSpeed;
        if (fT < rfTLast)
        {
            rfTLast = fT;
        }

        // quick out: intersection before desired interval
        if (rfTFirst > rfTLast)
        {
            return false; 
        }

    }
    else // object1 and object0 on overlapping interval
    {
        if (fSpeed > (Real)0)
        {
            // find last time of contact on this axis
            fT = (fMax0 - fMin1)/fSpeed;
            if (fT < rfTLast)
            {
                rfTLast = fT;
            }

            // quick out: intersection before desired interval
            if (rfTFirst > rfTLast)
            {
                return false; 
            }
        }
        else if (fSpeed < (Real)0)
        {
            // find last time of contact on this axis
            fT = (fMin0 - fMax1)/fSpeed;
            if (fT < rfTLast)
            {
                rfTLast = fT;
            }

            // quick out: intersection before desired interval
            if (rfTFirst > rfTLast)
            {
                return false;
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrAxis<Real>::Find (const Vector3<Real>& rkAxis,
    const Vector3<Real>& rkVelocity,
    const IntrConfiguration<Real>& rkCfg0Start,
    const IntrConfiguration<Real>& rkCfg1Start,
    Real fTMax, int& rkSide, IntrConfiguration<Real>& rkCfg0Final,
    IntrConfiguration<Real>& rkCfg1Final, Real& rfTFirst, Real& rfTLast)
{
    // Constant velocity separating axis test.  The configurations rkCfg0Start
    // and rkCfg1Start are the current potential configurations for contact,
    // and rkCfg0Final and rkCfg1Final are improved configurations.

    Real fT;
    Real fSpeed = rkAxis.Dot(rkVelocity);

    if (rkCfg1Start.Max < rkCfg0Start.Min) // object1 left of object0
    {
        if (fSpeed <= (Real)0) // object1 moving away from object0
        {
            return false;
        }

        // find first time of contact on this axis
        fT = (rkCfg0Start.Min - rkCfg1Start.Max)/fSpeed;

        // If this is the new maximum first time of contact, set side and
        // configuration.
        if (fT > rfTFirst)
        {
            rfTFirst = fT;
            rkSide = IntrConfiguration<Real>::LEFT;
            rkCfg0Final = rkCfg0Start;
            rkCfg1Final = rkCfg1Start;
        }

        // quick out: intersection after desired interval
        if (rfTFirst > fTMax)
        {
            return false;
        }

        // find last time of contact on this axis
        fT = (rkCfg0Start.Max - rkCfg1Start.Min)/fSpeed;
        if (fT < rfTLast)
        {
            rfTLast = fT;
        }

        // quick out: intersection before desired interval
        if (rfTFirst > rfTLast)
        {
            return false;
        }
    }
    else if (rkCfg0Start.Max < rkCfg1Start.Min)  // obj1 right of obj0
    {
        if (fSpeed >= (Real)0) // object1 moving away from object0
        {
            return false;
        }

        // find first time of contact on this axis
        fT = (rkCfg0Start.Max - rkCfg1Start.Min)/fSpeed;

        // If this is the new maximum first time of contact,  set side and
        // configuration.
        if (fT > rfTFirst)
        {
            rfTFirst = fT;
            rkSide = IntrConfiguration<Real>::RIGHT;
            rkCfg0Final = rkCfg0Start;
            rkCfg1Final = rkCfg1Start;
        }

        // quick out: intersection after desired interval
        if (rfTFirst > fTMax)
        {
            return false;   
        }

        // find last time of contact on this axis
        fT = (rkCfg0Start.Min - rkCfg1Start.Max)/fSpeed;
        if (fT < rfTLast)
        {
            rfTLast = fT;
        }

        // quick out: intersection before desired interval
        if (rfTFirst > rfTLast)
        {
            return false;
        }
    }
    else // object1 and object0 on overlapping interval
    {
        if (fSpeed > (Real)0)
        {
            // find last time of contact on this axis
            fT = (rkCfg0Start.Max - rkCfg1Start.Min)/fSpeed;
            if (fT < rfTLast)
            {
                rfTLast = fT;
            }

            // quick out: intersection before desired interval
            if (rfTFirst > rfTLast)
            {
                return false; 
            }
        }
        else if (fSpeed < (Real)0)
        {
            // find last time of contact on this axis
            fT = (rkCfg0Start.Min - rkCfg1Start.Max)/fSpeed;
            if (fT < rfTLast)
            {
                rfTLast = fT;
            }

            // quick out: intersection before desired interval
            if (rfTFirst > rfTLast)
            {
                return false; 
            }
        }
    }
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// FindContactSet<Real>
//----------------------------------------------------------------------------
template <class Real>
FindContactSet<Real>::FindContactSet (const Vector3<Real> akSegment[2],
    const Triangle3<Real>& rkTriangle, int eSide,
    const IntrConfiguration<Real>& rkSegCfg,
    const IntrConfiguration<Real>& rkTriCfg,
    const Vector3<Real>& rkSegVelocity, const Vector3<Real>& rkTriVelocity,
    Real fTFirst, int& riQuantity, Vector3<Real>* akP)
{
    Vector3<Real> akSegFinal[2] =
    {
        akSegment[0] + fTFirst*rkSegVelocity, 
        akSegment[1] + fTFirst*rkSegVelocity
    };

    Vector3<Real> akTriFinal[3] =
    {
        rkTriangle.V[0] + fTFirst*rkTriVelocity,
        rkTriangle.V[1] + fTFirst*rkTriVelocity,
        rkTriangle.V[2] + fTFirst*rkTriVelocity
    };

    const int* aiSIndex = rkSegCfg.Index;
    const int* aiTIndex = rkTriCfg.Index;

    if (eSide == IntrConfiguration<Real>::LEFT) // tri on left of seg
    {
        if (rkSegCfg.Map == IntrConfiguration<Real>::m11)
        {
            riQuantity = 1;
            akP[0] = akSegFinal[aiSIndex[0]];
        }
        else if (rkTriCfg.Map == IntrConfiguration<Real>::m111
        ||  rkTriCfg.Map == IntrConfiguration<Real>::m21)
        {
            riQuantity = 1;
            akP[0] = akTriFinal[aiTIndex[2]];
        }
        else if (rkTriCfg.Map == IntrConfiguration<Real>::m12)
        {
            Vector3<Real> kTemp[2];
            kTemp[0] = akTriFinal[aiTIndex[1]];
            kTemp[1] = akTriFinal[aiTIndex[2]];
            SegmentSegment(akSegFinal,kTemp,riQuantity,akP);
        }
        else // seg is m2, tri is m3
        {
            ColinearSegmentTriangle(akSegFinal,akTriFinal,riQuantity,akP);
        }
    }
    else // seg on left of tri
    {
        if (rkSegCfg.Map == IntrConfiguration<Real>::m11)
        {
            riQuantity = 1;
            akP[0] = akSegFinal[aiSIndex[1]];
        }
        else if (rkTriCfg.Map == IntrConfiguration<Real>::m111
        ||  rkTriCfg.Map == IntrConfiguration<Real>::m12)
        {
            riQuantity = 1;
            akP[0] = akTriFinal[aiTIndex[0]];
        }
        else if (rkTriCfg.Map == IntrConfiguration<Real>::m21)
        {
            Vector3<Real> kTemp[2];
            kTemp[0] = akTriFinal[aiTIndex[0]];
            kTemp[1] = akTriFinal[aiTIndex[1]];
            SegmentSegment(akSegFinal,kTemp,riQuantity,akP);
        }
        else // seg is m2, tri is m3
        {
            ColinearSegmentTriangle(akSegFinal,akTriFinal,riQuantity,akP);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
FindContactSet<Real>::FindContactSet (const Vector3<Real> akSegment[2], 
    const Box3<Real>& rkBox, int eSide,
    const IntrConfiguration<Real>& rkSegCfg,
    const IntrConfiguration<Real>& rkBoxCfg,
    const Vector3<Real>& rkSegVelocity, const Vector3<Real>& rkBoxVelocity,
    Real fTFirst, int& riQuantity, Vector3<Real>* akP)
{
    // Move the segment to its new position.
    Vector3<Real> akNewSeg[2] =
    {
        akSegment[0] + fTFirst*rkSegVelocity,
        akSegment[1] + fTFirst*rkSegVelocity
    };

    // Move the box to its new position.
    Box3<Real> kNewBox;
    kNewBox.Center = rkBox.Center + fTFirst*rkBoxVelocity;
    for (int i = 0; i < 3; i++)
    {
        kNewBox.Axis[i] = rkBox.Axis[i];
        kNewBox.Extent[i] = rkBox.Extent[i];
    }

    const int* aiSIndex = rkSegCfg.Index;
    const int* aiBIndex = rkBoxCfg.Index;

    if (eSide == IntrConfiguration<Real>::LEFT)
    {
        // box on left of seg
        if (rkSegCfg.Map == IntrConfiguration<Real>::m11)
        {
            riQuantity = 1;
            akP[0] = akNewSeg[aiSIndex[0]];
        }
        else if (rkBoxCfg.Map == IntrConfiguration<Real>::m1_1)
        {
            riQuantity = 1;
            akP[0] = GetPointFromIndex(aiBIndex[7],kNewBox);
        }
        else if (rkBoxCfg.Map == IntrConfiguration<Real>::m2_2)
        {
            // segment-segment intersection
            Vector3<Real> akBoxSeg[2];
            akBoxSeg[0] = GetPointFromIndex(aiBIndex[6],kNewBox);
            akBoxSeg[1] = GetPointFromIndex(aiBIndex[7],kNewBox);
            SegmentSegment(akNewSeg,akBoxSeg,riQuantity,akP);
        }
        else // rkBoxCfg.Map == IntrConfiguration<Real>::m44
        {
            // segment-boxface intersection
            Vector3<Real> akBoxFace[4];
            akBoxFace[0] = GetPointFromIndex(aiBIndex[4],kNewBox);
            akBoxFace[1] = GetPointFromIndex(aiBIndex[5],kNewBox);
            akBoxFace[2] = GetPointFromIndex(aiBIndex[6],kNewBox);
            akBoxFace[3] = GetPointFromIndex(aiBIndex[7],kNewBox);
            CoplanarSegmentRectangle(akNewSeg,akBoxFace,riQuantity,akP);
        }
    }
    else // eSide == RIGHT 
    {
        // box on right of seg
        if (rkSegCfg.Map == IntrConfiguration<Real>::m11)
        {
            riQuantity = 1;
            akP[0] = akNewSeg[aiSIndex[1]];
        }
        else if (rkBoxCfg.Map == IntrConfiguration<Real>::m1_1)
        {
            riQuantity = 1;
            akP[0] = GetPointFromIndex(aiBIndex[0],kNewBox);
        }
        else if (rkBoxCfg.Map == IntrConfiguration<Real>::m2_2)
        {
            // segment-segment intersection
            Vector3<Real> akBoxSeg[2];
            akBoxSeg[0] = GetPointFromIndex(aiBIndex[0],kNewBox);
            akBoxSeg[1] = GetPointFromIndex(aiBIndex[1],kNewBox);
            SegmentSegment(akNewSeg,akBoxSeg,riQuantity,akP);
        }
        else // rkBoxCfg.Map == IntrConfiguration<Real>::m44
        {
            // segment-boxface intersection
            Vector3<Real> akBoxFace[4];
            akBoxFace[0] = GetPointFromIndex(aiBIndex[0],kNewBox);
            akBoxFace[1] = GetPointFromIndex(aiBIndex[1],kNewBox);
            akBoxFace[2] = GetPointFromIndex(aiBIndex[2],kNewBox);
            akBoxFace[3] = GetPointFromIndex(aiBIndex[3],kNewBox);
            CoplanarSegmentRectangle(akNewSeg,akBoxFace,riQuantity,akP);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
FindContactSet<Real>::FindContactSet (const Triangle3<Real>& rkTriangle,
    const Box3<Real>& rkBox, int eSide,
    const IntrConfiguration<Real>& rkTriCfg,
    const IntrConfiguration<Real>& rkBoxCfg,
    const Vector3<Real>& rkTriVelocity, const Vector3<Real>& rkBoxVelocity,
    Real fTFirst, int& riQuantity, Vector3<Real>* akP)
{
    // move triangle to new position
    Vector3<Real> akNewTri[3] =
    {
        rkTriangle.V[0] + fTFirst*rkTriVelocity,
        rkTriangle.V[1] + fTFirst*rkTriVelocity,
        rkTriangle.V[2] + fTFirst*rkTriVelocity,
    };

    // move box to new position
    Box3<Real> kNewBox;
    kNewBox.Center = rkBox.Center + fTFirst*rkBoxVelocity;
    for (int i = 0; i < 3; i++)
    {
        kNewBox.Axis[i] = rkBox.Axis[i];
        kNewBox.Extent[i] = rkBox.Extent[i];
    }

    const int* aiTIndex = rkTriCfg.Index;
    const int* aiBIndex = rkBoxCfg.Index;

    if (eSide == IntrConfiguration<Real>::LEFT)
    {
        // box on left of tri
        if (rkTriCfg.Map == IntrConfiguration<Real>::m111
        ||  rkTriCfg.Map == IntrConfiguration<Real>::m12)
        {
            riQuantity = 1;
            akP[0] = akNewTri[aiTIndex[0]];
        }
        else if (rkBoxCfg.Map == IntrConfiguration<Real>::m1_1)
        {
            riQuantity = 1;
            akP[0] = GetPointFromIndex(aiBIndex[7],kNewBox);
        }
        else if (rkTriCfg.Map == IntrConfiguration<Real>::m21)
        {
            if (rkBoxCfg.Map == IntrConfiguration<Real>::m2_2)
            {
                // triseg-boxseg intersection
                Vector3<Real> akTriSeg[2], akBoxSeg[2];
                akTriSeg[0] = akNewTri[aiTIndex[0]];
                akTriSeg[1] = akNewTri[aiTIndex[1]];
                akBoxSeg[0] = GetPointFromIndex(aiBIndex[6],kNewBox);
                akBoxSeg[1] = GetPointFromIndex(aiBIndex[7],kNewBox);
                SegmentSegment(akTriSeg,akBoxSeg,riQuantity,akP);
            }
            else // rkBoxCfg.Map == IntrConfiguration<Real>::m44
            {
                // triseg-boxface intersection
                Vector3<Real> akTriSeg[2], akBoxFace[4];
                akTriSeg[0] = akNewTri[aiTIndex[0]];
                akTriSeg[1] = akNewTri[aiTIndex[1]];
                akBoxFace[0] = GetPointFromIndex(aiBIndex[4],kNewBox);
                akBoxFace[1] = GetPointFromIndex(aiBIndex[5],kNewBox);
                akBoxFace[2] = GetPointFromIndex(aiBIndex[6],kNewBox);
                akBoxFace[3] = GetPointFromIndex(aiBIndex[7],kNewBox);
                CoplanarSegmentRectangle(akTriSeg,akBoxFace,riQuantity,akP);
            }
        }
        else // rkTriCfg.Map == IntrConfiguration<Real>::m3
        {
            if (rkBoxCfg.Map == IntrConfiguration<Real>::m2_2)
            {
                // boxseg-triface intersection
                Vector3<Real> akBoxSeg[2];
                akBoxSeg[0] = GetPointFromIndex(aiBIndex[6],kNewBox);
                akBoxSeg[1] = GetPointFromIndex(aiBIndex[7],kNewBox);
                ColinearSegmentTriangle(akBoxSeg,akNewTri,riQuantity,akP);
            }
            else
            {
                // triface-boxface intersection
                Vector3<Real> akBoxFace[4];
                akBoxFace[0] = GetPointFromIndex(aiBIndex[4],kNewBox);
                akBoxFace[1] = GetPointFromIndex(aiBIndex[5],kNewBox);
                akBoxFace[2] = GetPointFromIndex(aiBIndex[6],kNewBox);
                akBoxFace[3] = GetPointFromIndex(aiBIndex[7],kNewBox);
                CoplanarTriangleRectangle(akNewTri,akBoxFace,riQuantity,akP);
            }
        }
    }
    else // eSide == RIGHT 
    {
        // box on right of tri
        if (rkTriCfg.Map == IntrConfiguration<Real>::m111
        ||  rkTriCfg.Map == IntrConfiguration<Real>::m21)
        {
            riQuantity = 1;
            akP[0] = akNewTri[aiTIndex[2]];
        }
        else if (rkBoxCfg.Map == IntrConfiguration<Real>::m1_1)
        {
            riQuantity = 1;
            akP[0] = GetPointFromIndex(aiBIndex[0],kNewBox);
        }
        else if (rkTriCfg.Map == IntrConfiguration<Real>::m12)
        {
            if (rkBoxCfg.Map == IntrConfiguration<Real>::m2_2)
            {
                // segment-segment intersection
                Vector3<Real> akTriSeg[2], akBoxSeg[2];
                akTriSeg[0] = akNewTri[aiTIndex[1]];
                akTriSeg[1] = akNewTri[aiTIndex[2]];
                akBoxSeg[0] = GetPointFromIndex(aiBIndex[0],kNewBox);
                akBoxSeg[1] = GetPointFromIndex(aiBIndex[1],kNewBox);
                SegmentSegment(akTriSeg,akBoxSeg,riQuantity,akP);
            }
            else // rkBoxCfg.Map == IntrConfiguration<Real>::m44
            {
                // triseg-boxface intersection
                Vector3<Real> akTriSeg[2], akBoxFace[4];
                akTriSeg[0] = akNewTri[aiTIndex[1]];
                akTriSeg[1] = akNewTri[aiTIndex[2]];
                akBoxFace[0] = GetPointFromIndex(aiBIndex[0],kNewBox);
                akBoxFace[1] = GetPointFromIndex(aiBIndex[1],kNewBox);
                akBoxFace[2] = GetPointFromIndex(aiBIndex[2],kNewBox);
                CoplanarSegmentRectangle(akTriSeg,akBoxFace,riQuantity,akP);
            }
        }
        else // rkTriCfg.Map == IntrConfiguration<Real>::m3
        {
            if (rkBoxCfg.Map == IntrConfiguration<Real>::m2_2)
            {
                // boxseg-triface intersection
                Vector3<Real> akBoxSeg[2];
                akBoxSeg[0] = GetPointFromIndex(aiBIndex[0],kNewBox);
                akBoxSeg[1] = GetPointFromIndex(aiBIndex[1],kNewBox);
                ColinearSegmentTriangle(akBoxSeg,akNewTri,riQuantity,akP);
            }
            else
            {
                // triface-boxface intersection
                Vector3<Real> akBoxFace[4];
                akBoxFace[0] = GetPointFromIndex(aiBIndex[0],kNewBox);
                akBoxFace[1] = GetPointFromIndex(aiBIndex[1],kNewBox);
                akBoxFace[2] = GetPointFromIndex(aiBIndex[2],kNewBox);
                akBoxFace[3] = GetPointFromIndex(aiBIndex[3],kNewBox);
                CoplanarTriangleRectangle(akNewTri,akBoxFace,riQuantity,akP);
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
FindContactSet<Real>::FindContactSet (const Box3<Real>& rkBox0,
    const Box3<Real>& rkBox1, int eSide,
    const IntrConfiguration<Real>& rkBox0Cfg,
    const IntrConfiguration<Real>& rkBox1Cfg,
    const Vector3<Real>& rkBox0Velocity, const Vector3<Real>& rkBox1Velocity,
    Real fTFirst, int& riQuantity, Vector3<Real>* akP)
{
    // move boxes to new position
    Box3<Real> kNewBox0, kNewBox1;
    kNewBox0.Center = rkBox0.Center + fTFirst*rkBox0Velocity;
    kNewBox1.Center = rkBox1.Center + fTFirst*rkBox1Velocity;
    for (int i = 0; i < 3; i++)
    {
        kNewBox0.Axis[i] = rkBox0.Axis[i];
        kNewBox0.Extent[i] = rkBox0.Extent[i];
        kNewBox1.Axis[i] = rkBox1.Axis[i];
        kNewBox1.Extent[i] = rkBox1.Extent[i];
    }

    const int* aiB0Index = rkBox0Cfg.Index;
    const int* aiB1Index = rkBox1Cfg.Index;

    if (eSide == IntrConfiguration<Real>::LEFT)
    {
        // box1 on left of box0
        if (rkBox0Cfg.Map == IntrConfiguration<Real>::m1_1)
        {
            riQuantity = 1;
            akP[0] = GetPointFromIndex(aiB0Index[0],kNewBox0);
        }
        else if (rkBox1Cfg.Map == IntrConfiguration<Real>::m1_1)
        {
            riQuantity = 1;
            akP[0] = GetPointFromIndex(aiB1Index[7],kNewBox1);
        }
        else if (rkBox0Cfg.Map == IntrConfiguration<Real>::m2_2)
        {
            if (rkBox1Cfg.Map == IntrConfiguration<Real>::m2_2)
            {
                // box0edge-box1edge intersection
                Vector3<Real> akEdge0[2], akEdge1[2];
                akEdge0[0] = GetPointFromIndex(aiB0Index[0],kNewBox0);
                akEdge0[1] = GetPointFromIndex(aiB0Index[1],kNewBox0);
                akEdge1[0] = GetPointFromIndex(aiB1Index[6],kNewBox1);
                akEdge1[1] = GetPointFromIndex(aiB1Index[7],kNewBox1);
                SegmentSegment(akEdge0,akEdge1,riQuantity,akP);
            }
            else // rkBox1Cfg.Map == IntrConfiguration<Real>::m44
            {
                // box0edge-box1face intersection
                Vector3<Real> akEdge0[2], akFace1[4];
                akEdge0[0] = GetPointFromIndex(aiB0Index[0],kNewBox0);
                akEdge0[1] = GetPointFromIndex(aiB0Index[1],kNewBox0); 
                akFace1[0] = GetPointFromIndex(aiB1Index[4],kNewBox1); 
                akFace1[1] = GetPointFromIndex(aiB1Index[5],kNewBox1); 
                akFace1[2] = GetPointFromIndex(aiB1Index[6],kNewBox1); 
                akFace1[3] = GetPointFromIndex(aiB1Index[7],kNewBox1); 
                CoplanarSegmentRectangle(akEdge0,akFace1,riQuantity,akP);
            }
        }
        else // rkBox0Cfg.Map == IntrConfiguration<Real>::m44
        {
            if (rkBox1Cfg.Map == IntrConfiguration<Real>::m2_2)
            {
                // box0face-box1edge intersection
                Vector3<Real> akFace0[4], akEdge1[2];
                akFace0[0] = GetPointFromIndex(aiB0Index[0],kNewBox0);
                akFace0[1] = GetPointFromIndex(aiB0Index[1],kNewBox0);
                akFace0[2] = GetPointFromIndex(aiB0Index[2],kNewBox0);
                akFace0[3] = GetPointFromIndex(aiB0Index[3],kNewBox0);
                akEdge1[0] = GetPointFromIndex(aiB1Index[6],kNewBox1);
                akEdge1[1] = GetPointFromIndex(aiB1Index[7],kNewBox1);
                CoplanarSegmentRectangle(akEdge1,akFace0,riQuantity,akP);
            }
            else
            {
                // box0face-box1face intersection
                Vector3<Real> akFace0[4], akFace1[4];
                akFace0[0] = GetPointFromIndex(aiB0Index[0],kNewBox0);
                akFace0[1] = GetPointFromIndex(aiB0Index[1],kNewBox0);
                akFace0[2] = GetPointFromIndex(aiB0Index[2],kNewBox0);
                akFace0[3] = GetPointFromIndex(aiB0Index[3],kNewBox0);
                akFace1[0] = GetPointFromIndex(aiB1Index[4],kNewBox1);
                akFace1[1] = GetPointFromIndex(aiB1Index[5],kNewBox1);
                akFace1[2] = GetPointFromIndex(aiB1Index[6],kNewBox1);
                akFace1[3] = GetPointFromIndex(aiB1Index[7],kNewBox1);
                CoplanarRectangleRectangle(akFace0,akFace1,riQuantity,akP);
            }
        }
    }
    else // rkSide == RIGHT 
    {
        // box1 on right of box0
        if (rkBox0Cfg.Map == IntrConfiguration<Real>::m1_1)
        {
            riQuantity = 1;
            akP[0] = GetPointFromIndex(aiB0Index[7],kNewBox0);
        }
        else if (rkBox1Cfg.Map == IntrConfiguration<Real>::m1_1)
        {
            riQuantity = 1;
            akP[0] = GetPointFromIndex(aiB0Index[0],kNewBox1);
        }
        else if (rkBox0Cfg.Map == IntrConfiguration<Real>::m2_2)
        {
            if (rkBox1Cfg.Map == IntrConfiguration<Real>::m2_2)
            {
                // box0edge-box1edge intersection
                Vector3<Real> akEdge0[2], akEdge1[2];
                akEdge0[0] = GetPointFromIndex(aiB0Index[6],kNewBox0);
                akEdge0[1] = GetPointFromIndex(aiB0Index[7],kNewBox0);
                akEdge1[0] = GetPointFromIndex(aiB1Index[0],kNewBox1);
                akEdge1[1] = GetPointFromIndex(aiB1Index[1],kNewBox1);
                SegmentSegment(akEdge0,akEdge1,riQuantity,akP);
            }
            else // rkBox1Cfg.Map == IntrConfiguration<Real>::m44
            {
                // box0edge-box1face intersection
                Vector3<Real> akEdge0[2], akFace1[4];
                akEdge0[0] = GetPointFromIndex(aiB0Index[6],kNewBox0);
                akEdge0[1] = GetPointFromIndex(aiB0Index[7],kNewBox0);
                akFace1[0] = GetPointFromIndex(aiB1Index[0],kNewBox1);
                akFace1[1] = GetPointFromIndex(aiB1Index[1],kNewBox1);
                akFace1[2] = GetPointFromIndex(aiB1Index[2],kNewBox1);
                akFace1[3] = GetPointFromIndex(aiB1Index[3],kNewBox1);
                CoplanarSegmentRectangle(akEdge0,akFace1,riQuantity,akP);
            }
        }
        else // rkBox0Cfg.Map == IntrConfiguration<Real>::m44
        {
            if (rkBox1Cfg.Map == IntrConfiguration<Real>::m2_2)
            {
                // box0face-box1edge intersection
                Vector3<Real> akFace0[4], akEdge1[2];
                akFace0[0] = GetPointFromIndex(aiB0Index[4],kNewBox0);
                akFace0[1] = GetPointFromIndex(aiB0Index[5],kNewBox0);
                akFace0[2] = GetPointFromIndex(aiB0Index[6],kNewBox0);
                akFace0[3] = GetPointFromIndex(aiB0Index[7],kNewBox0);
                akEdge1[0] = GetPointFromIndex(aiB1Index[0],kNewBox1);
                akEdge1[1] = GetPointFromIndex(aiB1Index[1],kNewBox1);
                CoplanarSegmentRectangle(akEdge1,akFace0,riQuantity,akP);
            }
            else // rkBox1Cfg.Map == IntrConfiguration<Real>::m44
            {
                // box0face-box1face intersection
                Vector3<Real> akFace0[4], akFace1[4];
                akFace0[0] = GetPointFromIndex(aiB0Index[4],kNewBox0);
                akFace0[1] = GetPointFromIndex(aiB0Index[5],kNewBox0);
                akFace0[2] = GetPointFromIndex(aiB0Index[6],kNewBox0);
                akFace0[3] = GetPointFromIndex(aiB0Index[7],kNewBox0);
                akFace1[0] = GetPointFromIndex(aiB1Index[0],kNewBox1);
                akFace1[1] = GetPointFromIndex(aiB1Index[1],kNewBox1);
                akFace1[2] = GetPointFromIndex(aiB1Index[2],kNewBox1);
                akFace1[3] = GetPointFromIndex(aiB1Index[3],kNewBox1);
                CoplanarRectangleRectangle(akFace0,akFace1,riQuantity,akP);
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void FindContactSet<Real>::ColinearSegments (
    const Vector3<Real> akSegment0[2], const Vector3<Real> akSegment1[2],
    int& riQuantity, Vector3<Real>* akP)
{
    // The potential intersection is initialized to segment0 and clipped
    // against segment1.
    riQuantity = 2;
    for (int i = 0; i < 2; i++)
    {
        akP[i] = akSegment0[i];
    }

    // point 0
    Vector3<Real> kV = akSegment1[1] - akSegment1[0];
    Real fC = kV.Dot(akSegment1[0]);
    ClipConvexPolygonAgainstPlane(kV,fC,riQuantity,akP);

    // point 1
    kV = -kV;
    fC = kV.Dot(akSegment1[1]);
    ClipConvexPolygonAgainstPlane(kV,fC,riQuantity,akP);
}
//----------------------------------------------------------------------------
template <class Real>
void FindContactSet<Real>::SegmentThroughPlane (
    const Vector3<Real> akSegment[2], const Vector3<Real>& rkPlaneOrigin,
    const Vector3<Real>& rkPlaneNormal, int& riQuantity, Vector3<Real>* akP)
{
    riQuantity = 1;

    Real fU = rkPlaneNormal.Dot(rkPlaneOrigin);
    Real fV0 = rkPlaneNormal.Dot(akSegment[0]);
    Real fV1 = rkPlaneNormal.Dot(akSegment[1]);

    // Now that there it has been reduced to a 1-dimensional problem via
    // projection, it becomes easy to find the ratio along V that V 
    // intersects with U.
    Real fRatio = (fU - fV0)/(fV1 - fV0);
    akP[0] = akSegment[0] + fRatio*(akSegment[1] - akSegment[0]);
}
//----------------------------------------------------------------------------
template <class Real>
void FindContactSet<Real>::SegmentSegment (const Vector3<Real> akSegment0[2], 
    const Vector3<Real> akSegment1[2], int& riQuantity, Vector3<Real>* akP)
{
    Vector3<Real> kDir0 = akSegment0[1] - akSegment0[0];
    Vector3<Real> kDir1 = akSegment1[1] - akSegment1[0];
    Vector3<Real> kNormal = kDir0.Cross(kDir1);

    // the comparison is sin(kDir0,kDir1) < epsilon
    Real fSqrLen0 = kDir0.SquaredLength();
    Real fSqrLen1 = kDir1.SquaredLength();
    Real fSqrLenN = kNormal.SquaredLength();
    if (fSqrLenN < Math<Real>::ZERO_TOLERANCE*fSqrLen0*fSqrLen1)
    {
        ColinearSegments(akSegment0,akSegment1,riQuantity,akP);
    }
    else
    {
        SegmentThroughPlane(akSegment1,akSegment0[0],
            kNormal.Cross(akSegment0[1]-akSegment0[0]),riQuantity,akP);
    }
}
//----------------------------------------------------------------------------
template <class Real>
void FindContactSet<Real>::ColinearSegmentTriangle (
    const Vector3<Real> akSegment[2], const Vector3<Real> akTriangle[3],
    int& riQuantity, Vector3<Real>* akP)
{
    // The potential intersection is initialized to the line segment and then
    // clipped against the three sides of the tri
    riQuantity = 2;
    int i;
    for (i = 0; i < 2; i++)
    {
        akP[i] = akSegment[i];
    }

    Vector3<Real> akSide[3] =
    {
        akTriangle[1] - akTriangle[0],
        akTriangle[2] - akTriangle[1],
        akTriangle[0] - akTriangle[2]
    };

    Vector3<Real> kN = akSide[0].Cross(akSide[1]);
    for (i = 0; i < 3; i++)
    {
        // normal pointing inside the triangle
        Vector3<Real> kSideN = kN.Cross(akSide[i]);
        Real fConstant = kSideN.Dot(akTriangle[i]);
        ClipConvexPolygonAgainstPlane(kSideN,fConstant,riQuantity,akP);
    }
}
//----------------------------------------------------------------------------
template <class Real>
void FindContactSet<Real>::CoplanarSegmentRectangle (
    const Vector3<Real> akSegment[2], const Vector3<Real> akRectangle[4],
    int& riQuantity, Vector3<Real>* akP)
{
    // The potential intersection is initialized to the line segment and then
    // clipped against the four sides of the rect
    riQuantity = 2;
    for (int i = 0; i < 2; i++)
    {
        akP[i] = akSegment[i];
    }

    for (int i0 = 3, i1 = 0; i1 < 4; i0 = i1++)
    {
        Vector3<Real> kNormal = akRectangle[i1] - akRectangle[i0];
        Real fConstant = kNormal.Dot(akRectangle[i0]);
        ClipConvexPolygonAgainstPlane(kNormal,fConstant,riQuantity,akP);
    }
}
//----------------------------------------------------------------------------
template <class Real>
void FindContactSet<Real>::CoplanarTriangleRectangle (
    const Vector3<Real> akTriangle[3], const Vector3<Real> akRectangle[4],
    int& riQuantity, Vector3<Real>* akP)
{
    // The potential intersection is initialized to the triangle, and then
    // clipped against the sides of the box
    riQuantity = 3;
    for (int i = 0; i < 3; i++)
    {
        akP[i] = akTriangle[i];
    }

    for (int i0 = 3, i1 = 0; i1 < 4; i0 = i1++)
    {
        Vector3<Real> kNormal = akRectangle[i1] - akRectangle[i0];
        Real fConstant = kNormal.Dot(akRectangle[i0]);
        ClipConvexPolygonAgainstPlane(kNormal,fConstant,riQuantity,akP);
    }
}
//----------------------------------------------------------------------------
template <class Real>
void FindContactSet<Real>::CoplanarRectangleRectangle (
    const Vector3<Real> akRectangle0[4], const Vector3<Real> akRectangle1[4],
    int& riQuantity, Vector3<Real>* akP)
{
    // The potential intersection is initialized to face 0, and then clipped
    // against the four sides of face 1.
    riQuantity = 4;
    for (int i = 0; i < 4; i++)
    {
        akP[i] = akRectangle0[i];
    }

    for (int i0 = 3, i1 = 0; i1 < 4; i0 = i1++)
    {
        Vector3<Real> kNormal = akRectangle1[i1] - akRectangle1[i0];
        Real fConstant = kNormal.Dot(akRectangle1[i0]);
        ClipConvexPolygonAgainstPlane(kNormal,fConstant,riQuantity,akP);
    }
}
//----------------------------------------------------------------------------
template <class Real>
void ClipConvexPolygonAgainstPlane (const Vector3<Real>& rkNormal,
    Real fConstant, int& riQuantity, Vector3<Real>* akP)
{
    // The input vertices are assumed to be in counterclockwise order.  The
    // ordering is an invariant of this function.  The size of array akP is
    // assumed to be large enough to store the clipped polygon vertices.

    // test on which side of line are the vertices
    int iPositive = 0, iNegative = 0, iPIndex = -1;
    int iQuantity = riQuantity;

    Real afTest[8];
    int i;
    for (i = 0; i < riQuantity; i++)
    {

        // An epsilon is used here because it is possible for the dot product
        // and fC to be exactly equal to each other (in theory), but differ
        // slightly because of floating point problems.  Thus, add a little
        // to the test number to push actually equal numbers over the edge
        // towards the positive.

        // TO DO: This should probably be a relative tolerance.  Multiplying
        // by the constant is probably not the best way to do this.
        afTest[i] = rkNormal.Dot(akP[i]) - fConstant +
            Math<Real>::FAbs(fConstant)*Math<Real>::ZERO_TOLERANCE;

        if (afTest[i] >= (Real)0)
        {
            iPositive++;
            if (iPIndex < 0)
            {
                iPIndex = i;
            }
        }
        else
        {
            iNegative++;
        }
    }

    if (riQuantity == 2)
    {
        // Lines are a little different, in that clipping the segment
        // cannot create a new segment, as clipping a polygon would
        if (iPositive > 0)
        {
            if (iNegative > 0) 
            {
                int iClip;

                if (iPIndex == 0)
                {
                    // vertex0 positive, vertex1 is clipped
                    iClip = 1;
                }
                else // iPIndex == 1
                {
                    // vertex1 positive, vertex0 clipped
                    iClip = 0;
                }

                Real fT = afTest[iPIndex]/(afTest[iPIndex] - afTest[iClip]);
                akP[iClip] = akP[iPIndex] + fT*(akP[iClip] - akP[iPIndex]);
            }
            // otherwise both positive, no clipping
        }
        else
        {
            // Assert:  The entire line is clipped, but we should not
            // get here.
            riQuantity = 0;
        }
    }
    else
    {
        if (iPositive > 0)
        {
            if (iNegative > 0)
            {
                // plane transversely intersects polygon
                Vector3<Real> akCV[8];
                int iCQuantity = 0, iCur, iPrv;
                Real fT;

                if (iPIndex > 0)
                {
                    // first clip vertex on line
                    iCur = iPIndex;
                    iPrv = iCur-1;
                    fT = afTest[iCur]/(afTest[iCur]-afTest[iPrv]);
                    akCV[iCQuantity++] = akP[iCur]+fT*(akP[iPrv]-akP[iCur]);

                    // vertices on positive side of line
                    while (iCur < iQuantity && afTest[iCur] >= (Real)0)
                    {
                        akCV[iCQuantity++] = akP[iCur++];
                    }

                    // last clip vertex on line
                    if (iCur < iQuantity)
                    {
                        iPrv = iCur-1;
                    }
                    else
                    {
                        iCur = 0;
                        iPrv = iQuantity - 1;
                    }
                    fT = afTest[iCur]/(afTest[iCur]-afTest[iPrv]);
                    akCV[iCQuantity++] = akP[iCur]+fT*(akP[iPrv]-akP[iCur]);
                }
                else  // iPIndex is 0
                {
                    // vertices on positive side of line
                    iCur = 0;
                    while (iCur < iQuantity && afTest[iCur] >= (Real)0)
                    {
                        akCV[iCQuantity++] = akP[iCur++];
                    }

                    // last clip vertex on line
                    iPrv = iCur-1;
                    fT = afTest[iCur]/(afTest[iCur]-afTest[iPrv]);
                    akCV[iCQuantity++] = akP[iCur]+fT*(akP[iPrv]-akP[iCur]);

                    // skip vertices on negative side
                    while (iCur < iQuantity && afTest[iCur] < (Real)0)
                    {
                        iCur++;
                    }

                    // first clip vertex on line
                    if (iCur < iQuantity)
                    {
                        iPrv = iCur-1;
                        fT = afTest[iCur]/(afTest[iCur] - afTest[iPrv]);
                        akCV[iCQuantity++] = akP[iCur]+fT*(akP[iPrv]-
                            akP[iCur]);

                        // vertices on positive side of line
                        while (iCur < iQuantity && afTest[iCur] >= (Real)0)
                        {
                            akCV[iCQuantity++] = akP[iCur++];
                        }
                    }
                    else
                    {
                        // iCur = 0
                        iPrv = iQuantity - 1;
                        fT = afTest[0]/(afTest[0]-afTest[iPrv]);
                        akCV[iCQuantity++] = akP[0]+fT*(akP[iPrv]-akP[0]);
                    }
                }

                iQuantity = iCQuantity;
                memcpy(akP,akCV,iCQuantity*sizeof(Vector3<Real>));
            }
            // else polygon fully on positive side of plane, nothing to do

            riQuantity = iQuantity;
        }
        else
        {
            // Polygon does not intersect positive side of plane, clip all.
            // This should not ever happen if called by the findintersect
            // routines after an intersection has been determined.
            riQuantity = 0;
        }    
    }
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> GetPointFromIndex (int iIndex, const Box3<Real>& rkBox)
{
    Vector3<Real> kPoint = rkBox.Center;

    if (iIndex & 4)
    {
        kPoint += rkBox.Extent[2]*rkBox.Axis[2];
    }
    else
    {
        kPoint -= rkBox.Extent[2]*rkBox.Axis[2];
    }

    if (iIndex & 2)
    {
        kPoint += rkBox.Extent[1]*rkBox.Axis[1];
    }
    else
    {
        kPoint -= rkBox.Extent[1]*rkBox.Axis[1];
    }

    if (iIndex & 1)
    {
        kPoint += rkBox.Extent[0]*rkBox.Axis[0];
    }
    else
    {
        kPoint -= rkBox.Extent[0]*rkBox.Axis[0];
    }

    return kPoint;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrConfiguration<float>;

template WM4_FOUNDATION_ITEM
class IntrAxis<float>;

template WM4_FOUNDATION_ITEM
class FindContactSet<float>;

template WM4_FOUNDATION_ITEM
void ClipConvexPolygonAgainstPlane<float> (const Vector3<float>&, float,
    int&, Vector3<float>*);

template WM4_FOUNDATION_ITEM
Vector3<float> GetPointFromIndex<float> (int, const Box3<float>&);


template WM4_FOUNDATION_ITEM
class IntrConfiguration<double>;

template WM4_FOUNDATION_ITEM
class IntrAxis<double>;

template WM4_FOUNDATION_ITEM
class FindContactSet<double>;

template WM4_FOUNDATION_ITEM
void ClipConvexPolygonAgainstPlane<double> (const Vector3<double>&, double,
    int&, Vector3<double>*);

template WM4_FOUNDATION_ITEM
Vector3<double> GetPointFromIndex<double> (int, const Box3<double>&);
//----------------------------------------------------------------------------
}
