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

#ifndef BOUNCINGSPHERES_H
#define BOUNCINGSPHERES_H

#include "Wm4WindowApplication3.h"
#include "RigidBall.h"
#include <vector>
using namespace Wm4;

class BouncingSpheres : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    BouncingSpheres ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:

    class Contact
    {
    public:
        RigidBodyf* A;  // ball containing face
        RigidBodyf* B;  // ball containing vertex
        Vector3f P;     // contact point
        Vector3f N;     // outward unit-length normal to face
    };

    void CreateBalls ();
    void CreateFloor ();
    void CreateBackWall ();
    void CreateSideWall1 ();
    void CreateSideWall2 ();
    void DoPhysical ();
    void DoVisual ();
    void DoCollisionDetection ();
    void DoCollisionResponse ();
    void ComputePreimpulseVelocity (float* afPreRelVel);
    void ComputeImpulseMagnitude (float* afPreRelVel, float* afImpulseMag);
    void DoImpulse (float* fImpulseMag);
    void DoMotion ();
    void SetBoundaryContact (int i, int iBIndex, const Vector3f& rkPos, 
        float fRadius, Contact& rkContact);

    // external forces and torques for this application
    static Vector3f Force (float fTime, float fMass, const Vector3f& rkPos,
        const Quaternionf& rkQOrient, const Vector3f& rkLinMom,
        const Vector3f& rkAngMom, const Matrix3f& rkOrient,
        const Vector3f& rkLinVel, const Vector3f& rkAngVel);

    static Vector3f Torque (float fTime, float fMass, const Vector3f& rkPos,
        const Quaternionf& rkQOrient, const Vector3f& rkLinMom,
        const Vector3f& rkAngMom, const Matrix3f& rkOrient,
        const Vector3f& rkLinVel, const Vector3f& rkAngVel);

    // rigid spheres are the rigid bodies
    enum { NUM_BALLS = 16 };
    RigidBall* m_apkBall[NUM_BALLS];
 
    // representation of boundaries (floor, ceiling, walls)
    RigidBodyf m_akBoundary[6];
    Vector3f m_akBLocation[6];
    Vector3f m_akBNormal[6];

    // contact points during one pass of the physical simulation
    int m_iNumContacts;
    std::vector<Contact> m_kBContact;

    // blocked directions
    std::vector<Vector3f> m_akBlocked[NUM_BALLS];

    // simulated clock
    float m_fSimTime, m_fSimDelta;

    // the scene graph
    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframeState;
    NodePtr m_aspkBall[NUM_BALLS];
    TriMeshPtr m_spkFloor, m_spkBackWall, m_spkSideWall1, m_spkSideWall2;
    PolylinePtr m_spkLine;
    Culler m_kCuller;
};

WM4_REGISTER_INITIALIZE(BouncingSpheres);

#endif
