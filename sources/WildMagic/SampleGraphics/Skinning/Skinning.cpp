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
// Version: 4.0.1 (2007/06/16)

#include "Skinning.h"

WM4_WINDOW_APPLICATION(Skinning);

//----------------------------------------------------------------------------
Skinning::Skinning ()
    :
    WindowApplication3("Skinning",0,0,640,480,ColorRGBA(0.5f,0.0f,1.0f,1.0f))
{
}
//----------------------------------------------------------------------------
bool Skinning::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    m_spkCamera->SetFrustum(-0.55f,0.55f,-0.4125f,0.4125f,1.0f,1000.0f);
    Vector3f kCLoc(0.0f, 0.0f, 0.0f);
    Vector3f kCDir(0.0f,0.0f,1.0f);
    Vector3f kCUp(0.0f,1.0f,0.0f);
    Vector3f kCRight = kCDir.Cross(kCUp);
    m_spkCamera->SetFrame(kCLoc,kCDir,kCUp,kCRight);

    CreateScene();

    // initial update of objects
    m_spkScene->UpdateGS();
    m_spkScene->UpdateRS();

    // initial culling of scene
    m_kCuller.SetCamera(m_spkCamera);
    m_kCuller.ComputeVisibleSet(m_spkScene);

    InitializeCameraMotion(0.1f,0.001f);
    InitializeObjectMotion(m_spkMesh);
    return true;
}
//----------------------------------------------------------------------------
void Skinning::OnTerminate ()
{
    m_spkScene = 0;
    m_spkEffect = 0;
    m_spkMesh = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Skinning::OnIdle ()
{
    MeasureTime();

    UpdateConstants((float)System::GetTime());

    if (MoveCamera())
    {
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    if (MoveObject())
    {
        m_spkScene->UpdateGS();
        m_kCuller.ComputeVisibleSet(m_spkScene);
    }

    m_pkRenderer->ClearBuffers();
    if (m_pkRenderer->BeginScene())
    {
        m_pkRenderer->DrawScene(m_kCuller.GetVisibleSet());
        DrawFrameRate(8,GetHeight()-8,ColorRGBA::BLACK);
        m_pkRenderer->EndScene();
    }
    m_pkRenderer->DisplayBackBuffer();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Skinning::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication3::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case 's':
    case 'S':
        TestStreaming(m_spkScene,128,128,640,480,"Skinning.wmof");
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Skinning::CreateScene ()
{
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetColorChannels(0,3);  // RGB vertex colors
    kAttr.SetTCoordChannels(0,4);  // weights for skinning

    // The skinned object is a cylinder.
    const int iRadialSamples = 10;
    const int iAxisSamples = 7;
    const float fRadius = 10.0f;
    const float fHeight = 80.0f;
    const float fInvRS = 1.0f/(float)iRadialSamples;
    const float fInvASm1 = 1.0f/(float)(iAxisSamples-1);
    const float fHalfHeight = 0.5f*fHeight;
    const Vector3f kCenter(0.0f,0.0f,100.0f);
    const Vector3f kU(0.0f,0.0f,-1.0f);
    const Vector3f kV(0.0f,1.0f,0.0f);
    const Vector3f kAxis(1.0f,0.0f,0.0f);

    // generate geometry
    int iVQuantity = iAxisSamples*(iRadialSamples+1);
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(kAttr,iVQuantity);

    // Generate points on the unit circle to be used in computing the mesh
    // points on a cylinder slice.
    int iR, iA, iAStart, i;
    float* afSin = WM4_NEW float[iRadialSamples+1];
    float* afCos = WM4_NEW float[iRadialSamples+1];
    for (iR = 0; iR < iRadialSamples; iR++)
    {
        float fAngle = Mathf::TWO_PI*fInvRS*iR;
        afCos[iR] = Mathf::Cos(fAngle);
        afSin[iR] = Mathf::Sin(fAngle);
    }
    afSin[iRadialSamples] = afSin[0];
    afCos[iRadialSamples] = afCos[0];

    // generate the cylinder itself
    for (iA = 0, i = 0; iA < iAxisSamples; iA++)
    {
        float fAxisFraction = iA*fInvASm1;  // in [0,1]
        float fZ = -fHalfHeight + fHeight*fAxisFraction;

        // compute center of slice
        Vector3f kSliceCenter = kCenter + fZ*kAxis;

        // compute slice vertices with duplication at end point
        ColorRGB kColor(fAxisFraction,1.0f-fAxisFraction,0.3f);
        Vector4f kTCoord;
        int iSave = i;
        for (iR = 0; iR < iRadialSamples; iR++)
        {
            // vertex position and color
            Vector3f kNormal = afCos[iR]*kU + afSin[iR]*kV;
            pkVB->Position3(i) = kSliceCenter + fRadius*kNormal;
            pkVB->Color3(0,i) = kColor;

            // Set texture coordinates to the correct vertex weights.
            if (iA == 0)
            {
                kTCoord[0] = 1.0f;
            }
            else if (iA == 1)
            {
                kTCoord[0] = 0.5f;
            }
            else
            {
                kTCoord[0] = 0.0f;
            }

            if (iA == 1 || iA == 3)
            {
                kTCoord[1] = 0.5f;
            }
            else if (iA == 2)
            {
                kTCoord[1] = 1.0f;
            }
            else
            {
                kTCoord[1] = 0.0f;
            }

            if (iA == 3 || iA == 5)
            {
                kTCoord[2] = 0.5f;
            }
            else if (iA == 4)
            {
                kTCoord[2] = 1.0f;
            }
            else
            {
                kTCoord[2] = 0.0f;
            }

            if (iA == 6)
            {
                kTCoord[3] = 1.0f;
            }
            else if (iA == 5)
            {
                kTCoord[3] = 0.5f;
            }
            else
            {
                kTCoord[3] = 0.0f;
            }

            pkVB->TCoord4(0,i) = kTCoord;
            i++;
        }

        // vertex position and color
        pkVB->Position3(i) = pkVB->Position3(iSave);
        pkVB->Color3(0,i) = kColor;

        // Set texture coordinates to the correct vertex weights.
        if (iA == 0)
        {
            kTCoord[0] = 1.0f;
        }
        else if (iA == 1)
        {
            kTCoord[0] = 0.5f;
        }
        else
        {
            kTCoord[0] = 0.0f;
        }

        if (iA == 1 || iA == 3)
        {
            kTCoord[1] = 0.5f;
        }
        else if (iA == 2)
        {
            kTCoord[1] = 1.0f;
        }
        else
        {
            kTCoord[1] = 0.0f;
        }

        if (iA == 3 || iA == 5)
        {
            kTCoord[2] = 0.5f;
        }
        else if (iA == 4)
        {
            kTCoord[2] = 1.0f;
        }
        else
        {
            kTCoord[2] = 0.0f;
        }

        if (iA == 6)
        {
            kTCoord[3] = 1.0f;
        }
        else if (iA == 5)
        {
            kTCoord[3] = 0.5f;
        }
        else
        {
            kTCoord[3] = 0.0f;
        }

        pkVB->TCoord4(0,i) = kTCoord;
        i++;
    }

    // generate connectivity
    int iTQuantity = 2*(iAxisSamples-1)*iRadialSamples;
    int iIQuantity = 3*iTQuantity;
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(iIQuantity);
    int* aiIndex = pkIB->GetData();
    int* aiLocalIndex = aiIndex;
    for (iA = 0, iAStart = 0; iA < iAxisSamples-1; iA++)
    {
        int i0 = iAStart;
        int i1 = i0 + 1;
        iAStart += iRadialSamples + 1;
        int i2 = iAStart;
        int i3 = i2 + 1;
        for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 6)
        {
            aiLocalIndex[0] = i0;
            aiLocalIndex[1] = i1;
            aiLocalIndex[2] = i2;
            aiLocalIndex[3] = i1;
            aiLocalIndex[4] = i3;
            aiLocalIndex[5] = i2;
            i0++;
            i1++;
            i2++;
            i3++;
        }
    }

    WM4_DELETE[] afCos;
    WM4_DELETE[] afSin;

    m_spkMesh = WM4_NEW TriMesh(pkVB,pkIB);

    m_spkScene = WM4_NEW Node;
    m_spkScene->AttachChild(m_spkMesh);
    m_spkEffect = WM4_NEW SkinningEffect;
    m_spkMesh->AttachEffect(m_spkEffect);
}
//----------------------------------------------------------------------------
void Skinning::UpdateConstants (float fTime)
{
    // create some arbitrary skinning transformations
    const float fFactor = Mathf::PI/1.25f;
    int iDiv = (int)(fTime/fFactor);

    // the angle now ranges from -factor/4 to +factor/4
    float fAngle = Mathf::FAbs(fTime-iDiv*fFactor-fFactor/2.0f)-
        fFactor/4.0f;

    for (int i = 0; i < 4; i++)
    {
        float fThisAngle;
        if ((int)(fTime/fFactor+0.25f) % 2)
        {
            fThisAngle = Mathf::FAbs((float)i-1.5f)*fAngle;
        }
        else
        {
            fThisAngle = ((float)i-1.5f)*fAngle;
        }

        Matrix3f kRot(Vector3f::UNIT_Z,fThisAngle);
        Vector3f kTrn(0.0f,10.0f*Mathf::Sin(fTime+0.5f*(float)i),0.0f);

        // The vector-on-left convention needs to be used since this is what
        // the shader system expects.
        Matrix4f kMatrix(
            kRot[0][0],kRot[1][0],kRot[2][0],0.0f,
            kRot[0][1],kRot[1][1],kRot[2][1],0.0f,
            kRot[0][2],kRot[1][2],kRot[2][2],0.0f,
            kTrn[0],   kTrn[1],   kTrn[2],   1.0f);

        m_spkEffect->SetMatrix(i,kMatrix);
    }
}
//----------------------------------------------------------------------------
