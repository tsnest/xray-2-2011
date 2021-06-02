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

#ifndef WM4CONTSPHERE3_H
#define WM4CONTSPHERE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Sphere3.h"

namespace Wm4
{

// Compute the smallest axis-aligned bounding box of the points, then
// compute a sphere containing the box.
template <class Real> WM4_FOUNDATION_ITEM
Sphere3<Real> ContSphereOfAABB (int iQuantity, const Vector3<Real>* akPoint);

// Compute the smallest sphere whose center is the average of the input
// points.
template <class Real> WM4_FOUNDATION_ITEM
Sphere3<Real> ContSphereAverage (int iQuantity, const Vector3<Real>* akPoint);

// Test for containment of a point inside a sphere.
template <class Real> WM4_FOUNDATION_ITEM
bool InSphere (const Vector3<Real>& rkPoint, const Sphere3<Real>& rkSphere);

// Compute the smallest sphere that contains the input spheres.
template <class Real> WM4_FOUNDATION_ITEM
Sphere3<Real> MergeSpheres (const Sphere3<Real>& rkSphere0,
    const Sphere3<Real>& rkSphere1);

// Compute the minimum volume sphere containing the input set of points.  The
// algorithm randomly permutes the input points so that the construction
// occurs in 'expected' O(N) time.
template <class Real>
class WM4_FOUNDATION_ITEM MinSphere3
{
public:
    // The epsilon value is a floating-point tolerance used for various
    // computations.
    MinSphere3 (int iQuantity, const Vector3<Real>* akPoint,
        Sphere3<Real>& rkMinimal, Real fEpsilon = (Real)1.0e-03);

private:
    // indices of points that support current minimum volume sphere
    class WM4_FOUNDATION_ITEM Support
    {
    public:
        bool Contains (int iIndex, Vector3<Real>** apkPoint, Real fEpsilon)
        {
            for (int i = 0; i < Quantity; i++)
            {
                Vector3<Real> kDiff = *apkPoint[iIndex] - *apkPoint[Index[i]];
                if (kDiff.SquaredLength() < fEpsilon)
                {
                    return true;
                }
            }
            return false;
        }

        int Quantity;
        int Index[4];
    };

    // test if point P is inside sphere S
    bool Contains (const Vector3<Real>& rkP, const Sphere3<Real>& rkS,
        Real& rfDistDiff);

    Sphere3<Real> ExactSphere1 (const Vector3<Real>& rkP);
    Sphere3<Real> ExactSphere2 (const Vector3<Real>& rkP0,
        const Vector3<Real>& rkP1);
    Sphere3<Real> ExactSphere3 (const Vector3<Real>& rkP0,
        const Vector3<Real>& rkP1, const Vector3<Real>& rkP2);
    Sphere3<Real> ExactSphere4 (const Vector3<Real>& rkP0,
        const Vector3<Real>& rkP1, const Vector3<Real>& rkP2,
        const Vector3<Real>& rkP3);

    Sphere3<Real> UpdateSupport1 (int i, Vector3<Real>** apkPerm,
        Support& rkSupp);
    Sphere3<Real> UpdateSupport2 (int i, Vector3<Real>** apkPerm,
        Support& rkSupp);
    Sphere3<Real> UpdateSupport3 (int i, Vector3<Real>** apkPerm,
        Support& rkSupp);
    Sphere3<Real> UpdateSupport4 (int i, Vector3<Real>** apkPerm,
        Support& rkSupp);

    typedef Sphere3<Real> (MinSphere3<Real>::*UpdateFunction)(
        int,Vector3<Real>**,Support&);

    Real m_fEpsilon, m_fOnePlusEpsilon;
    UpdateFunction m_aoUpdate[5];
};

typedef MinSphere3<float> MinSphere3f;
typedef MinSphere3<double> MinSphere3d;

}

#endif
