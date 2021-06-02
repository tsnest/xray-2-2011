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

#ifndef WM4RIEMANNIANGEODESIC_H
#define WM4RIEMANNIANGEODESIC_H

#include "Wm4FoundationLIB.h"
#include "Wm4GMatrix.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM RiemannianGeodesic
{
public:
    // Construction and destruction.  The input dimension must be two or
    // larger.
    RiemannianGeodesic (int iDimension);
    virtual ~RiemannianGeodesic ();

    // Tweakable parameters.
    // 1. The integral samples are the number of samples used in the Trapezoid
    //    Rule numerical integrator.
    // 2. The search samples are the number of samples taken along a ray for
    //    the steepest descent algorithm used to refine the vertices of the
    //    polyline approximation to the geodesic curve.
    // 3. The derivative step is the value of h used for centered difference
    //    approximations df/dx = (f(x+h)-f(x-h))/(2*h) in the steepest
    //    descent algorithm.
    // 4. The number of subdivisions indicates how many times the polyline
    //    segments should be subdivided.  The number of polyline vertices
    //    will be pow(2,subdivisions)+1.
    // 5. The number of refinements per subdivision.  Setting this to a
    //    positive value appears necessary when the geodesic curve has a
    //    large length.
    // 6. The search radius is the distance over which the steepest descent
    //    algorithm searches for a minimum on the line whose direction is the
    //    estimated gradient.  The default of 1 means the search interval is
    //    [-L,L], where L is the length of the gradient.  If the search
    //    radius is r, then the interval is [-r*L,r*L].
    int IntegralSamples;  // default = 16
    int SearchSamples;    // default = 32
    Real DerivativeStep;  // default = 0.0001
    int Subdivisions;     // default = 7
    int Refinements;      // default = 8
    Real SearchRadius;    // default = 1.0

    // The dimension of the manifold.
    int GetDimension () const;

    // Returns the length of the line segment connecting the points.
    Real ComputeSegmentLength (const GVector<Real>& rkPoint0,
        const GVector<Real>& rkPoint1);

    // Compute the total length of the polyline.  The lengths of the segments
    // are computed relative to the metric tensor.
    Real ComputeTotalLength (int iQuantity, const GVector<Real>* akPath);

    // Returns a polyline approximation to a geodesic curve connecting the
    // points.  The caller is responsible for deleting the output array (it
    // is dynamically allocated).
    void ComputeGeodesic (const GVector<Real>& rkEnd0,
        const GVector<Real>& rkEnd1, int& riQuantity,
        GVector<Real>*& rakPath);

    // Start with the midpoint M of the line segment (E0,E1) and use a
    // steepest descent algorithm to move M so that
    //     Length(E0,M) + Length(M,E1) < Length(E0,E1)
    // This is essentially a relaxation scheme that inserts points into the
    // current polyline approximation to the geodesic curve.
    bool Subdivide (const GVector<Real>& rkEnd0, GVector<Real>& rkMid,
        const GVector<Real>& rkEnd1);

    // Apply the steepest descent algorithm to move the midpoint M of the
    // line segment (E0,E1) so that
    //     Length(E0,M) + Length(M,E1) < Length(E0,E1)
    // This is essentially a relaxation scheme that inserts points into the
    // current polyline approximation to the geodesic curve.
    bool Refine (const GVector<Real>& rkEnd0, GVector<Real>& rkMid,
        const GVector<Real>& rkEnd1);

    // A callback that is executed during each call of Refine.
    typedef void (*RefineCallbackFunction)();
    RefineCallbackFunction RefineCallback;

    // Information to be used during the callback.
    int GetSubdivisionStep () const;
    int GetRefinementStep () const;
    int GetCurrentQuantity () const;

    // Curvature computations to measure how close the approximating
    // polyline is to a geodesic.

    // Returns the total curvature of the line segment connecting the points.
    Real ComputeSegmentCurvature (const GVector<Real>& rkPoint0,
        const GVector<Real>& rkPoint1);

    // Compute the total curvature of the polyline.  The curvatures of the
    // segments are computed relative to the metric tensor.
    Real ComputeTotalCurvature (int iQuantity, const GVector<Real>* akPath);

protected:
    // Support for ComputeSegmentCurvature.
    Real ComputeIntegrand (const GVector<Real>& rkPos,
        const GVector<Real>& rkDer);

    // Compute the metric tensor for the specified point.  Derived classes
    // are responsible for implementing this function.
    virtual void ComputeMetric (const GVector<Real>& rkPoint) = 0;

    // Compute the Christoffel symbols of the first kind for the current
    // point.  Derived classes are responsible for implementing this function.
    virtual void ComputeChristoffel1 (const GVector<Real>& rkPoint) = 0;

    // Compute the inverse of the current metric tensor.  The function
    // returns 'true' iff the inverse exists.
    bool ComputeMetricInverse ();

    // Compute the derivative of the metric tensor for the current state.
    // This is a triply indexed quantity, the values computed using the
    // Christoffel symbols of the first kind.
    void ComputeMetricDerivative ();

    // Compute the Christoffel symbols of the second kind for the current
    // state.  The values depend on the inverse of the metric tensor, so
    // they may be computed only when the inverse exists.  The function
    // returns 'true' whenever the inverse metric tensor exists.
    bool ComputeChristoffel2 ();

    int m_iDimension;
    GMatrix<Real> m_kMetric;
    GMatrix<Real> m_kMetricInverse;
    GMatrix<Real>* m_akChristoffel1;
    GMatrix<Real>* m_akChristoffel2;
    GMatrix<Real>* m_akMetricDerivative;
    bool m_bMetricInverseExists;

    // Progress parameters that are useful to RefineCallback.
    int m_iSubdivide, m_iRefine, m_iCurrentQuantity;

    // Derived tweaking parameters.
    Real m_fIntegralStep;      // = 1/(IntegralQuantity-1)
    Real m_fSearchStep;        // = 1/SearchQuantity
    Real m_fDerivativeFactor;  // = 1/(2*DerivativeStep)
};

typedef RiemannianGeodesic<float> RiemannianGeodesicf;
typedef RiemannianGeodesic<double> RiemannianGeodesicd;

}

#endif
