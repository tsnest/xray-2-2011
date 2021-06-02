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

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Wm4Vector3.h"
using namespace Wm4;

class RayTracer
{
public:
    typedef float (*Function)(const Vector3f&);
    typedef Vector3f (*Gradient)(const Vector3f&);

    // Ray trace level surface F(x,y,z) = 0.  The surface normals are
    // computed from DF(x,y,z), the gradient of F.
    RayTracer (Function oF, Gradient oDF, int iWidth, int iHeight);
    ~RayTracer ();

    // camera and view frustum
    Vector3f& Location ();
    Vector3f& Direction ();
    Vector3f& Up ();
    Vector3f& Right ();
    float& Near ();
    float& Far ();
    float& HalfWidth ();
    float& HalfHeight ();

    // rendered image
    int GetWidth () const;
    int GetHeight () const;
    const float* GetImage () const;

    // Ray trace the view frustum region.  The tracing uses a single
    // directional light.  TO DO:  Allow more lights and different light
    // types.  The number of samples per ray for computing intersection of
    // rays with the surface is specified in iMaxSample.
    void DrawSurface (int iMaxSample, const Vector3f& rkLightDir, bool bBlur);

private:
    // find intersection of ray with surface
    void FindSurface (float fS0, float fF0, const Vector3f& rkP0, float fS1,
        float fF1, const Vector3f& rkP1, const Vector3f& kRayDir,
        Vector3f& rkPos, Vector3f& rkNormal);

    // blur the image for a cheap antialiasing
    void BlurImage ();

    // camera and view frustum
    Vector3f m_kLocation, m_kDirection, m_kUp, m_kRight;
    float m_fNear, m_fFar, m_fHalfWidth, m_fHalfHeight;

    // level surface F(x,y,z) = 0, surface normal DF(x,y,z)
    Function m_oF;
    Gradient m_oDF;

    // rendered image
    int m_iWidth, m_iHeight;
    float* m_afImage;
    float* m_afBlur;
};

#include "RayTracer.inl"

#endif
