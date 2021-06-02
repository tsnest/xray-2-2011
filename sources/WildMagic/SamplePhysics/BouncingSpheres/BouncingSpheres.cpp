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
// Version: 4.0.2 (2007/06/16)

#include "BouncingSpheres.h"

WM4_WINDOW_APPLICATION(BouncingSpheres);

//----------------------------------------------------------------------------
BouncingSpheres::BouncingSpheres ()
    :
    WindowApplication3("BouncingSpheres",0,0,640,480,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_fSimTime = 0.0f;
    m_fSimDelta = 1.0f/10.0f;
}
//----------------------------------------------------------------------------
bool BouncingSpheres::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // set up camera
    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    float fAngle = 0.02f*Mathf::PI;
    float fCos = Mathf::Cos(fAngle), fSin = Mathf::Sin(fAngle);
    Vector3f kCLoc(27.5f,8.0f,8.9f);
    Vector3f kCDir(-fCos,0.0f,-fSin);
    Vector3f kCUp(-fSin,0.0f,fCos);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    // create scene components
    CreateBalls();
    CreateFloor();
    CreateBackWall();
    CreateSideWall1();
    CreateSideWall2();

    // ** layout of scene graph **
    // scene
    //     room
    //         backwall
    //         floor
    //         sidewall1
    //         sidewall2
    //     balls

    m_spkScene = WM4_NEW Node;
    Node* pkRoom = WM4_NEW Node;
    Node* pkBalls = WM4_NEW Node;
    m_spkScene->AttachChild(pkRoom);
    pkRoom->AttachChild(m_spkFloor);
    pkRoom->AttachChild(m_spkSideWall1);
    pkRoom->AttachChild(m_spkSideWall2);
    pkRoom->AttachChild(m_spkBackWall);
    m_spkScene->AttachChild(pkBalls);
    int i;
    for (i = 0; i < NUM_BALLS; i++)
    {
        pkBalls->AttachChild(m_aspkBall[i]);
    }

    // wireframe
    m_spkWireframeState = WM4_NEW WireframeState;
    m_spkScene->AttachGlobalState(m_spkWireframeState);

    // initial update of objects
    m_spkScene->UpdateGS(0.0f);
    m_spkScene->UpdateRS();

    // The balls are constrained to bounce around in a rectangular solid
    // region.  The six defining planes are defined to be immovable rigid
    // bodies.  The boundaries are parallel to coordinate axes and pass
    // through the points indicated by the value other than +-100.  That is,
    // the back wall is at x = 1, the left wall is at y = 2, the floor is at
    // z = 1, the right wall is at y = 15, the ceiling is at z = 17, and the
    // front wall is at x = 9.  The ceiling and front wall are invisible
    // objects (not rendered), but you will see balls bouncing against it
    // and reflecting away from it towards the back wall.
    m_akBLocation[0] = Vector3f(1.0,-100.0,-100.0);
    m_akBNormal[0] = Vector3f(1.0,0,0);
    m_akBLocation[1] = Vector3f(-100.0,2.0,-100.0);
    m_akBNormal[1] = Vector3f(0,1.0,0);
    m_akBLocation[2] = Vector3f( -100.0,-100.0,1.0);
    m_akBNormal[2] = Vector3f(0,0,1.0);
    m_akBLocation[3] = Vector3f( 100.0,15.0,100.0);
    m_akBNormal[3] = Vector3f(0,-1.0,0);
    m_akBLocation[4] = Vector3f( 100.0,100.0,17.0);
    m_akBNormal[4] = Vector3f(0,0,-1.0);
    m_akBLocation[5] = Vector3f( 8.0,100.0,100.0);
    m_akBNormal[5] = Vector3f(-1.0,0,0);
    for (i = 0; i < 6; i++)
    {
        m_akBoundary[i].SetMass(0.0f);
        m_akBoundary[i].SetPosition(m_akBLocation[i]);
    }

    // initialize ball with correct transformations
    DoPhysical();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    return true;
}
//----------------------------------------------------------------------------
void BouncingSpheres::OnTerminate()
{
    for (int i = 0; i < NUM_BALLS; i++)
    {
        WM4_DELETE m_apkBall[i];
        m_aspkBall[i] = 0;
    }

    m_spkFloor = 0;
    m_spkSideWall1 = 0;
    m_spkSideWall2 = 0;
    m_spkBackWall = 0;
    m_spkWireframeState = 0;
    m_spkScene = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BouncingSpheres::OnIdle ()
{
    MeasureTime();
    DoPhysical();
    m_kCuller.ComputeVisibleSet(m_spkScene);
    DoVisual();
    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BouncingSpheres::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    switch (ucKey)
    {
    case 't':  // turn simulation on/off
    {
        static float s_fSimDelta = 0.0f;
        if (s_fSimDelta == 0.0f)
        {
            s_fSimDelta = m_fSimDelta;
            m_fSimDelta = 0.0f;
        }
        else
        {
            m_fSimDelta = s_fSimDelta;
            s_fSimDelta = 0.0f;
        }
        return true;
    }
    case 'w':  // toggle wireframe
        m_spkWireframeState->Enabled = !m_spkWireframeState->Enabled;
        return true;
    }

    return WindowApplication3::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
void BouncingSpheres::CreateBalls ()
{
    // TO DO.  Adjust this so that the physics simulation runs with the
    // real clock.
    m_fSimDelta = 0.001f;

    Vector3f kPos, kLinMom;
    float fMass = 2.0f;

    for (int i = 0; i < NUM_BALLS; i++)
    {
        m_apkBall[i] = WM4_NEW RigidBall(Mathf::IntervalRandom(0.25f,1.0f));
        RigidBall& rkBall = *m_apkBall[i];
        m_aspkBall[i] = WM4_NEW Node;
        m_aspkBall[i]->AttachChild(rkBall.Mesh());
        fMass += 1.2f;
        if (i > 3)
        {
            kPos = Vector3f(5.0f,4.0f,13.0f) -
                1.0f*((float)(i-4))*Vector3f::UNIT_Z +
                2.0f*((float)(i-4))*Vector3f::UNIT_Y;
        }
        else
        {
            kPos = Vector3f(3.0f,4.0f,10.0f) -
                2.0f*((float)i)*Vector3f::UNIT_Z+
                2.0f*((float)i)*Vector3f::UNIT_Y;
        }

        kLinMom = Vector3f(2.0f,2.0f,-1.2f) +
            50.0f*(Mathf::SymmetricRandom())*Vector3f::UNIT_X +
            50.0f*(Mathf::SymmetricRandom())*Vector3f::UNIT_Y +
            50.0f*(Mathf::SymmetricRandom())*Vector3f::UNIT_Z ;

        rkBall.SetMass(fMass);
        rkBall.SetPosition(kPos);
        rkBall.SetLinearMomentum(kLinMom);
        rkBall.Force = Force;
        rkBall.Torque = Torque;
    }
}
//----------------------------------------------------------------------------
void BouncingSpheres::CreateFloor ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    ColorRGB kFloorColor(155.0f/255.0f,177.0f/255.0f,164.0f/255.0f);

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(1.0f,1.0f,1.0f);
    pkVBuffer->Position3(1) = Vector3f(17.0f,1.0f,1.0f);
    pkVBuffer->Position3(2) = Vector3f(17.0f,20.0f,1.0f);
    pkVBuffer->Position3(3) = Vector3f(1.0f,20.0f,1.0f);
    pkVBuffer->Color3(0,0) = kFloorColor;
    pkVBuffer->Color3(0,1) = kFloorColor;
    pkVBuffer->Color3(0,2) = kFloorColor;
    pkVBuffer->Color3(0,3) = kFloorColor;

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    m_spkFloor = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkFloor->AttachEffect(WM4_NEW VertexColor3Effect);
}
//----------------------------------------------------------------------------
void BouncingSpheres::CreateSideWall1 ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    ColorRGB kSideWall1Color(170.0f/255.0f,187.0f/255.0f,219.0f/255.0f);

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(1.0f,15.0f,1.0f);
    pkVBuffer->Position3(1) = Vector3f(17.0f,15.0f,1.0f);
    pkVBuffer->Position3(2) = Vector3f(17.0f,15.0f,17.0f);
    pkVBuffer->Position3(3) = Vector3f(1.0f,15.0f,17.0f);
    pkVBuffer->Color3(0,0) = kSideWall1Color;
    pkVBuffer->Color3(0,1) = kSideWall1Color;
    pkVBuffer->Color3(0,2) = kSideWall1Color;
    pkVBuffer->Color3(0,3) = kSideWall1Color;

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    m_spkSideWall1 = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkSideWall1->AttachEffect(WM4_NEW VertexColor3Effect);
}
//----------------------------------------------------------------------------
void BouncingSpheres::CreateSideWall2 ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    ColorRGB kSideWall2Color(170.0f/255.0f,187.0f/255.0f,219.0f/255.0f);

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(17.0f,2.0f,1.0f);
    pkVBuffer->Position3(1) = Vector3f(1.0f,2.0f,1.0f);
    pkVBuffer->Position3(2) = Vector3f(1.0f,2.0f,17.0f);
    pkVBuffer->Position3(3) = Vector3f(17.0f,2.0f,17.0f);
    pkVBuffer->Color3(0,0) = kSideWall2Color;
    pkVBuffer->Color3(0,1) = kSideWall2Color;
    pkVBuffer->Color3(0,2) = kSideWall2Color;
    pkVBuffer->Color3(0,3) = kSideWall2Color;

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    m_spkSideWall2 = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkSideWall2->AttachEffect(WM4_NEW VertexColor3Effect);
}
//----------------------------------------------------------------------------
void BouncingSpheres::CreateBackWall ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);
    ColorRGB kBackWallColor(209.0f/255.0f,204.0f/255.0f,180.0f/255.0f);

    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,4);
    pkVBuffer->Position3(0) = Vector3f(1.0f,1.0f,1.0f);
    pkVBuffer->Position3(1) = Vector3f(1.0f,20.0f,1.0f);
    pkVBuffer->Position3(2) = Vector3f(1.0f,20.0f,17.0f);
    pkVBuffer->Position3(3) = Vector3f(1.0f,1.0f,17.0f);
    pkVBuffer->Color3(0,0) = kBackWallColor;
    pkVBuffer->Color3(0,1) = kBackWallColor;
    pkVBuffer->Color3(0,2) = kBackWallColor;
    pkVBuffer->Color3(0,3) = kBackWallColor;

    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(6);
    int* aiIndex = pkIBuffer->GetData();
    aiIndex[0] = 0;  aiIndex[1] = 1;  aiIndex[2] = 2;
    aiIndex[3] = 0;  aiIndex[4] = 2;  aiIndex[5] = 3;

    m_spkBackWall = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);
    m_spkBackWall->AttachEffect(WM4_NEW VertexColor3Effect);
}
//----------------------------------------------------------------------------
void BouncingSpheres::DoPhysical ()
{
    DoCollisionDetection();
    DoCollisionResponse();

    // update the scene graph
    for (int i = 0; i < NUM_BALLS; i++)
    {
        m_aspkBall[i]->Local.SetTranslate(m_apkBall[i]->GetPosition());
    }

    m_spkScene->UpdateGS(0.0f);

    // next simulation time
    m_fSimTime += m_fSimDelta;
}
//----------------------------------------------------------------------------
void BouncingSpheres::DoVisual ()
{
    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);

        const size_t uiSize = 256;
        char acMsg[uiSize];
        System::Sprintf(acMsg,uiSize,"Time = %5.2f",m_fSimTime);
        m_pkRenderer->Draw(90,GetHeight()-8,ColorRGBA::BLACK,acMsg);

        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();
}
//----------------------------------------------------------------------------
void BouncingSpheres::DoCollisionDetection ()
{
    m_kBContact.clear();

    // collisions with boundaries
    Contact kContact;
    int i;
    for (i = 0; i < NUM_BALLS; i++)
    {
        Vector3f kPos = m_apkBall[i]->GetPosition();
        float fRadius = m_apkBall[i]->GetRadius();
        m_apkBall[i]->Moved = false;
        m_akBlocked[i].clear();

        // These checks are done in pairs under the assumption that the ball 
        // radii are smaller than the separation of opposite boundaries, hence
        // only one of each opposite pair of boundaries may be touched at any
        // time.

        // rear [0] and front[5] boundaries
        if (kPos.X() < m_akBLocation[0].X() + fRadius)
        {
            SetBoundaryContact(i,0,kPos,fRadius,kContact);
        }
        else if (kPos.X() > m_akBLocation[5].X() - fRadius)
        {
            SetBoundaryContact(i,5,kPos,fRadius,kContact);
        }

        // left [1] and right [3] boundaries
        if (kPos.Y() < m_akBLocation[1].Y() + fRadius)
        {
            SetBoundaryContact(i,1,kPos,fRadius,kContact);
        }
        else if (kPos.Y() > m_akBLocation[3].Y() - fRadius)
        {
            SetBoundaryContact(i,3,kPos,fRadius,kContact);
        }

        // bottom [2] and top [4] boundaries
        if (kPos.Z() < m_akBLocation[2].Z() + fRadius)
        {
            SetBoundaryContact(i,2,kPos,fRadius,kContact);
        }
        else if (kPos.Z() > m_akBLocation[4].Z() - fRadius)
        {
            SetBoundaryContact(i,4,kPos,fRadius,kContact);
        }
    }

    // collisions between balls
    for (i = 0; i < NUM_BALLS-1; i++)
    {
        for (int j = i + 1; j < NUM_BALLS; j++)
        {
            Vector3f kDiff = m_apkBall[j]->GetPosition() -
                m_apkBall[i]->GetPosition();
            float fDiffLen = kDiff.Length();
            float fRadiusI = m_apkBall[i]->GetRadius();
            float fRadiusJ = m_apkBall[j]->GetRadius();
            float fMagnitude = fDiffLen - fRadiusI - fRadiusJ;
            if (fMagnitude < 0.0f)
            {
                kContact.A = m_apkBall[i];
                kContact.B = m_apkBall[j];
                kContact.N = kDiff/fDiffLen;
                Vector3f kDeltaPos = fMagnitude*kContact.N;

                if (m_apkBall[i]->Moved && !m_apkBall[j]->Moved)
                {
                    // i moved but j not
                    m_apkBall[j]->Position() -= kDeltaPos;
                }
                else if (!m_apkBall[i]->Moved && m_apkBall[j]->Moved)
                {
                    // j moved but i not
                    m_apkBall[i]->Position() += kDeltaPos;
                }
                else
                {
                    // neither or both moved already
                    kDeltaPos *= 0.5f;
                    m_apkBall[j]->Position() -= kDeltaPos;
                    m_apkBall[i]->Position() += kDeltaPos;
                }

                kContact.P = m_apkBall[i]->Position() + fRadiusI*kContact.N; 
                m_kBContact.push_back(kContact);
            }
        }
    }

    m_iNumContacts = (int)m_kBContact.size();
}
//----------------------------------------------------------------------------
void BouncingSpheres::SetBoundaryContact (int i, int iBIndex, 
    const Vector3f& rkPos, float fRadius, Contact& rkContact)
{
    rkContact.B = m_apkBall[i];
    rkContact.A = &m_akBoundary[iBIndex];
    rkContact.N = m_akBNormal[iBIndex];
    rkContact.P = rkPos;
    m_akBlocked[i].push_back(-rkContact.N);
    m_kBContact.push_back(rkContact);

    Vector3f kBPos = m_akBoundary[iBIndex].GetPosition();
    switch (iBIndex)
    {
    case 0:  rkContact.B->Position().X() = kBPos.X() + fRadius;  break;
    case 1:  rkContact.B->Position().Y() = kBPos.Y() + fRadius;  break;
    case 2:  rkContact.B->Position().Z() = kBPos.Z() + fRadius;  break;
    case 3:  rkContact.B->Position().Y() = kBPos.Y() - fRadius;  break;
    case 4:  rkContact.B->Position().Z() = kBPos.Z() - fRadius;  break;
    case 5:  rkContact.B->Position().X() = kBPos.X() - fRadius;  break;
    default:  assert(false);
    }
    m_apkBall[i]->Moved = true;
}
//----------------------------------------------------------------------------
void BouncingSpheres::DoCollisionResponse ()
{
    if (m_iNumContacts > 0)
    {
        float* afPreRelVel = WM4_NEW float[m_iNumContacts];
        float* afImpulseMag = WM4_NEW float[m_iNumContacts];

        ComputePreimpulseVelocity(afPreRelVel);
        ComputeImpulseMagnitude(afPreRelVel,afImpulseMag);
        DoImpulse(afImpulseMag);

        WM4_DELETE[] afPreRelVel;
        WM4_DELETE[] afImpulseMag;
    }
    DoMotion();
}
//----------------------------------------------------------------------------
void BouncingSpheres::ComputeImpulseMagnitude (float* afPreRelVel,
    float* afImpulseMag)
{
    // coefficient of restitution
    const float fRestitution = 0.8f;

    Vector3f kLinVelDiff, kRelA, kRelB;
    Vector3f kAxN, kBxN, kJInvAxN, kJInvBxN;

    for (int i = 0; i < m_iNumContacts; i++)
    {
        const Contact& rkContact = m_kBContact[i];
        const RigidBodyf& rkBodyA = *rkContact.A;
        const RigidBodyf& rkBodyB = *rkContact.B;

        if (afPreRelVel[i] < 0.0f)
        {
            kLinVelDiff = rkBodyA.GetLinearVelocity() -
                rkBodyB.GetLinearVelocity();
            kRelA = rkContact.P - rkBodyA.GetPosition();
            kRelB = rkContact.P - rkBodyB.GetPosition();
            kAxN = kRelA.Cross(rkContact.N);
            kBxN = kRelB.Cross(rkContact.N);
            kJInvAxN = rkBodyA.GetWorldInverseInertia()*kAxN;
            kJInvBxN = rkBodyB.GetWorldInverseInertia()*kBxN;

            float fNumer = -(1.0f+fRestitution)*(rkContact.N.Dot(kLinVelDiff)
                + rkBodyA.GetAngularVelocity().Dot(kAxN)
                - rkBodyB.GetAngularVelocity().Dot(kBxN));

            float fDenom = rkBodyA.GetInverseMass() + rkBodyB.GetInverseMass()
                + kAxN.Dot(kJInvAxN)
                + kBxN.Dot(kJInvBxN);

            afImpulseMag[i] = fNumer/fDenom;
        }
        else
        {
            afImpulseMag[i] = 0.0f;
        }
    }
}
//----------------------------------------------------------------------------
void BouncingSpheres::ComputePreimpulseVelocity (float* afPreRelVel)
{
    for (int i = 0; i < m_iNumContacts; i++)
    {
        const Contact& rkContact = m_kBContact[i];
        const RigidBodyf& rkBodyA = *rkContact.A;
        const RigidBodyf& rkBodyB = *rkContact.B;

        Vector3f kRelA = rkContact.P - rkBodyA.GetPosition();
        Vector3f kRelB = rkContact.P - rkBodyB.GetPosition();
        Vector3f kVelA = rkBodyA.GetLinearVelocity() +
            rkBodyA.GetAngularVelocity().Cross(kRelA);
        Vector3f kVelB = rkBodyB.GetLinearVelocity() +
            rkBodyB.GetAngularVelocity().Cross(kRelB);
        afPreRelVel[i] = rkContact.N.Dot(kVelB-kVelA);
    }
}
//----------------------------------------------------------------------------
void BouncingSpheres::DoImpulse (float* afImpulseMag)
{
    for (int i = 0; i < m_iNumContacts; i++)
    {
        Contact& rkContact = m_kBContact[i];
        RigidBodyf& rkBodyA = *rkContact.A;
        RigidBodyf& rkBodyB = *rkContact.B;

        // update linear/angular momentum
        Vector3f kImpulse = afImpulseMag[i]*rkContact.N;
        rkBodyA.SetLinearMomentum(rkBodyA.GetLinearMomentum() + kImpulse);
        rkBodyB.SetLinearMomentum(rkBodyB.GetLinearMomentum() - kImpulse);
    }
}
//----------------------------------------------------------------------------
void BouncingSpheres::DoMotion ()
{
    for (int i = 0; i < NUM_BALLS; i++)
    {
        m_apkBall[i]->Update(m_fSimTime,m_fSimDelta);
    }
}
//----------------------------------------------------------------------------
Vector3f BouncingSpheres::Force (float, float fMass, const Vector3f&,
    const Quaternionf&, const Vector3f&, const Vector3f&, const Matrix3f&,
    const Vector3f&, const Vector3f&)
{
    const float fGravityConstant = 9.81f;   // m/sec/sec
    const Vector3f kGravityDirection = Vector3f(0.0f,0.0f,-1.0f);
    return (fMass*fGravityConstant)*kGravityDirection;
}
//----------------------------------------------------------------------------
Vector3f BouncingSpheres::Torque (float, float, const Vector3f&,
    const Quaternionf&, const Vector3f&, const Vector3f&, const Matrix3f&,
    const Vector3f&, const Vector3f&)
{
    return Vector3f::ZERO;
}
//----------------------------------------------------------------------------
