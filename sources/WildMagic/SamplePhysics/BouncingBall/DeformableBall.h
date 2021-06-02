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

#ifndef DEFORMABLEBALL_H
#define DEFORMABLEBALL_H

#include "Wm4TriMesh.h"
using namespace Wm4;

class DeformableBall
{
public:
    // construction and destruction
    DeformableBall (float fDuration, float fPeriod);
    ~DeformableBall ();

    // member access
    TriMeshPtr& Mesh ();
    void Set (float fDuration, float fPeriod);
    float GetDuration () const;
    float GetPeriod () const;
    float GetMinActive () const;
    float GetMaxActive () const;
    float GetAmplitude (float fTime);
    bool& DoAffine ();

    // deform the ball
    bool DoSimulationStep (float fRealTime);

private:
    void CreateBall ();

    // Support for the mesh smoother of m_spkMesh.
    void CreateSmoother ();
    void Update (float fTime);

    // Influence function:  parameters (X,t)
    //   input:  X = point on surface
    //           t = current time
    // The return value is 'true' if and only if the point is within the
    // region of influence of the deformation *and* if the deformation
    // function is active at time t.  This allows MeshSmoother to avoid
    // evolving the surface in regions where no deformation is occuring.
    bool VertexInfluenced (int i, float fTime);

    float GetTangentWeight (int i, float fTime);
    float GetNormalWeight (int i, float fTime);

    // Level surface function:  parameters (X,t,F,Grad(F))
    //   input:  X = point in space
    //           t = time of deformation
    //   output: F(X,t) = scalar function at position and time
    //           Grad(F)(X,t) = gradient at level surface through X at time
    void ComputeFunction (const Vector3f& rkPos, float fTime, float& rfFunc,
        Vector3f& rkGrad);

    TriMeshPtr m_spkMesh;
    float m_fDuration, m_fPeriod, m_fDeformMult;
    float m_fMinActive, m_fMaxActive, m_fInvActiveRange;
    bool m_bDeforming, m_bDoAffine;

    // parameters for Newton's method in ComputeFunction
    int m_iMaxIterations;
    float m_fErrorTolerance;

    // mesh smoother data
    Vector3f* m_akNormal;
    Vector3f* m_akMean;
    int* m_aiNeighborCount;
};

#include "DeformableBall.inl"

#endif
