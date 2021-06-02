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

#ifndef WM4CURVE3_H
#define WM4CURVE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM Curve3
{
public:
    // abstract base class
    Curve3 (Real fTMin, Real fTMax);
    virtual ~Curve3 ();

    // Interval on which curve parameter is defined.  If you are interested
    // in only a subinterval of the actual domain of the curve, you may set
    // that subinterval with SetTimeInterval.  This function requires that
    // fTMin < fTMax.
    Real GetMinTime () const;
    Real GetMaxTime () const;
    void SetTimeInterval (Real fTMin, Real fTMax);

    // position and derivatives
    virtual Vector3<Real> GetPosition (Real fTime) const = 0;
    virtual Vector3<Real> GetFirstDerivative (Real fTime) const = 0;
    virtual Vector3<Real> GetSecondDerivative (Real fTime) const = 0;
    virtual Vector3<Real> GetThirdDerivative (Real fTime) const = 0;

    // differential geometric quantities
    Real GetSpeed (Real fTime) const;
    virtual Real GetLength (Real fT0, Real fT1) const = 0;
    Real GetTotalLength () const;
    Vector3<Real> GetTangent (Real fTime) const;
    Vector3<Real> GetNormal (Real fTime) const;
    Vector3<Real> GetBinormal (Real fTime) const;
    void GetFrame (Real fTime, Vector3<Real>& rkPosition,
        Vector3<Real>& rkTangent, Vector3<Real>& rkNormal,
        Vector3<Real>& rkBinormal) const;
    Real GetCurvature (Real fTime) const;
    Real GetTorsion (Real fTime) const;

    // inverse mapping of s = Length(t) given by t = Length^{-1}(s)
    virtual Real GetTime (Real fLength, int iIterations = 32,
        Real fTolerance = (Real)1e-06) const = 0;

    // subdivision
    void SubdivideByTime (int iNumPoints, Vector3<Real>*& rakPoint) const;
    void SubdivideByLength (int iNumPoints, Vector3<Real>*& rakPoint) const;

    // Subdivision by variation. The pointers pkP0 and pkP1 correspond to the
    // curve points at fT0 and fT1.  If the pointer values are not null, the
    // assumption is that the caller has passed in the curve points.
    // Otherwise, the function computes the curve points.
    virtual Real GetVariation (Real fT0, Real fT1,
        const Vector3<Real>* pkP0 = 0, const Vector3<Real>* pkP1 = 0)
        const = 0;
    void SubdivideByVariation (Real fMinVariation, int iMaxLevel,
        int& riNumPoints, Vector3<Real>*& rakPoint) const;

protected:
    // curve parameter is t where tmin <= t <= tmax
    Real m_fTMin, m_fTMax;

    // subdivision
    class WM4_FOUNDATION_ITEM PointList
    {
    public:
        PointList (const Vector3<Real>& rkPoint, PointList* pkNext)
        {
            m_kPoint = rkPoint;
            m_kNext = pkNext;
        }

        Vector3<Real> m_kPoint;
        PointList* m_kNext;
    };

    void SubdivideByVariation (Real fT0, const Vector3<Real>& rkP0, Real fT1,
        const Vector3<Real>& rkP1, Real kMinVariation, int iLevel,
        int& riNumPoints, PointList*& rpkList) const;
};

typedef Curve3<float> Curve3f;
typedef Curve3<double> Curve3d;

}

#endif
