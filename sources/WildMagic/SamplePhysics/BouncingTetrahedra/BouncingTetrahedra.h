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

#ifndef BOUNCINGTETRAHEDRA_H
#define BOUNCINGTETRAHEDRA_H

#include "Wm4WindowApplication3.h"
#include "RigidTetra.h"
#include <vector>
using namespace Wm4;

class BouncingTetrahedra : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    BouncingTetrahedra ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    // scene graph construction
    void CreateTetra ();
    void CreateFloor ();
    void CreateBackWall ();
    void CreateSideWall1 ();
    void CreateSideWall2 ();

    // visual rendering of the scene
    void DoVisual ();

    // contact points for the collision detection system
    struct Contact
    {
        RigidBodyf* A;     // tetrahedron containing face
        RigidBodyf* B;     // tetrahedron containing vertex
        Vector3f PA;       // contact point for tetrahedron A
        Vector3f PB;       // contact point for tetrahedron B
        Vector3f N;        // outward unit-length normal to face
        Vector3f EA;       // edge from A
        Vector3f EB;       // edge from B
        bool IsVFContact;  // true if vertex-face, false if edge-edge
    };

    // physical simulation
    void DoPhysical ();

    // collision detection
    void DoCollisionDetection ();
    bool FarFromBoundary (int i);
    bool FarApart (int iT0, int iT1);
    bool TetraBoundaryIntersection (int iTetra, int iBoundary,
        float* afDistance, Contact& rkContact);
    void BuildContactMoveTetra (int iTetra, int iBoundary, int iHitIndex, 
        float fDepthMax, Contact& rkContact);
    void Reposition (int iT0, int iT1, Contact& rkContact);
    bool IsVertex (const Vector3f* akVertex, const Vector3f& rkRes);
    void CalculateNormal (const Vector3f* akVertex, const Vector3f& rkRes,
        Contact& rkContact);
    Vector3f ClosestEdge (const Vector3f* akVertex, const Vector3f& rkRes,
        Vector3f& rkOtherVertex);

    // collision response
    void DoCollisionResponse ();
    void ComputePreimpulseVelocity (float* afPreVel);
    void ComputeImpulseMagnitude (float* afPreRelVel, float* afImpulseMag);
    void DoImpulse (float* afImpulseMag);
    void DoMotion ();

    // external forces and torques for this application
    static Vector3f Force (float fTime, float fMass, const Vector3f& rkPos,
        const Quaternionf& rkQOrient, const Vector3f& rkLinMom,
        const Vector3f& rkAngMom, const Matrix3f& rkOrient,
        const Vector3f& rkLinVel, const Vector3f& rkAngVel);

    static Vector3f Torque (float fTime, float fMass, const Vector3f& rkPos,
        const Quaternionf& rkQOrient, const Vector3f& rkLinMom,
        const Vector3f& rkAngMom, const Matrix3f& rkOrient,
        const Vector3f& rkLinVel, const Vector3f& rkAngVel);


    // rigid tetrahedra are the rigid bodies
    enum { NUM_TETRA = 4 };
    RigidTetra* m_apkTetra[NUM_TETRA];

    // representation of boundaries (floor, ceiling, walls)
    RigidBodyf m_akBoundary[6];
    Vector3f m_akBLocation[6];
    Vector3f m_akBNormal[6];

    // contact points during one pass of the physical simulation
    int m_iNumContacts;
    std::vector<Contact> m_kContact;

    // simulated clock
    float m_fSimTime, m_fSimDelta;

    // number of times the LCP solver was called during a simulation step
    int m_iLCPCount;

    // the connectivity of a tetrahedron, used by the LCP solver
    TTuple<3,int> m_akFaces[4];

    // error tolerance used for interpenetration calculations
    float m_fTolerance;

    // kinetic energy of the system
    float m_fTotalKE;

    // the scene graph
    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframeState;
    NodePtr m_aspkTetra[NUM_TETRA];
    TriMeshPtr m_spkFloor, m_spkBackWall, m_spkSideWall1, m_spkSideWall2;
    Culler m_kCuller;
};

WM4_REGISTER_INITIALIZE(BouncingTetrahedra);

#endif
