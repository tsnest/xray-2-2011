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

#include "Wm4GraphicsPCH.h"
#include "Wm4RevolutionSurface.h"
#include "Wm4StandardMesh.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,RevolutionSurface,TriMesh);
WM4_IMPLEMENT_STREAM(RevolutionSurface);
WM4_IMPLEMENT_DEFAULT_NAME_ID(RevolutionSurface,TriMesh);

const char* RevolutionSurface::ms_aacTopology[MAX_TOPOLOGY_TYPES] =
{
    "REV_DISK_TOPOLOGY",
    "REV_CYLINDER_TOPOLOGY",
    "REV_SPHERE_TOPOLOGY",
    "REV_TORUS_TOPOLOGY"
};

//----------------------------------------------------------------------------
RevolutionSurface::RevolutionSurface (Curve2f* pkCurve, float fXCenter,
    TopologyType eTopology, int iCurveSamples, int iRadialSamples,
    const Attributes& rkAttr, bool bSampleByArcLength, bool bOutsideView)
{
    assert(pkCurve);
    m_pkCurve = pkCurve;
    m_fXCenter = fXCenter;
    m_eTopology = eTopology;
    m_iCurveSamples = iCurveSamples;
    m_iRadialSamples = iRadialSamples;
    m_bSampleByArcLength = bSampleByArcLength;

    ComputeSampleData();

    // The topology of the meshes is all that matters.  The vertices will be
    // modified later based on the curve of revolution.
    StandardMesh kSM(rkAttr,!bOutsideView);
    TriMesh* pkMesh = 0;
    switch (m_eTopology)
    {
    case REV_DISK_TOPOLOGY:
        pkMesh = kSM.Disk(m_iCurveSamples,m_iRadialSamples,1.0f);
        break;
    case REV_CYLINDER_TOPOLOGY:
        pkMesh = kSM.Cylinder(m_iCurveSamples,m_iRadialSamples,1.0f,1.0f,
            true);
        break;
    case REV_SPHERE_TOPOLOGY:
        pkMesh = kSM.Sphere(m_iCurveSamples,m_iRadialSamples,1.0f);
        break;
    case REV_TORUS_TOPOLOGY:
        pkMesh = kSM.Torus(m_iCurveSamples,m_iRadialSamples,1.0f,0.25f);
        break;
    default:
        assert(false);
        break;
    }
    assert(pkMesh);

    VBuffer = pkMesh->VBuffer;
    IBuffer = pkMesh->IBuffer;
    WM4_DELETE pkMesh;

    // generate the actual surface by replacing the vertex values
    UpdateSurface();
}
//----------------------------------------------------------------------------
RevolutionSurface::RevolutionSurface ()
{
    m_pkCurve = 0;
    m_afSin = 0;
    m_afCos = 0;
    m_akSample = 0;
}
//----------------------------------------------------------------------------
RevolutionSurface::~RevolutionSurface ()
{
    WM4_DELETE[] m_afSin;
    WM4_DELETE[] m_afCos;
    WM4_DELETE[] m_akSample;
}
//----------------------------------------------------------------------------
void RevolutionSurface::ComputeSampleData ()
{
    // Compute slice vertex coefficients.  The first and last coefficients
    // are duplicated to allow a closed cross section that has two different
    // pairs of texture coordinates at the shared vertex.
    m_afSin = WM4_NEW float[m_iRadialSamples+1];
    m_afCos = WM4_NEW float[m_iRadialSamples+1];
    float fInvRS = 1.0f/(float)m_iRadialSamples;
    for (int i = 0; i < m_iRadialSamples; i++)
    {
        float fAngle = Mathf::TWO_PI*fInvRS*i;
        m_afCos[i] = Mathf::Cos(fAngle);
        m_afSin[i] = Mathf::Sin(fAngle);
    }
    m_afSin[m_iRadialSamples] = m_afSin[0];
    m_afCos[m_iRadialSamples] = m_afCos[0];

    // allocate storage for curve samples
    m_akSample = WM4_NEW Vector3f[m_iCurveSamples];
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateSurface ()
{
    // parameters for evaluating curve
    float fTMin = m_pkCurve->GetMinTime();
    float fTRange = m_pkCurve->GetMaxTime() - fTMin;

    // sampling by arc length requires the total length of the curve
    float fTotalLength;
    if (m_bSampleByArcLength)
    {
        fTotalLength = m_pkCurve->GetTotalLength();
    }
    else
    {
        fTotalLength = 0.0f;
    }

    // sample the curve of revolution
    float fInvCSm1 = 1.0f/(float)(m_iCurveSamples - 1);
    for (int i = 0; i < m_iCurveSamples; i++)
    {
        float fT;
        if (m_bSampleByArcLength)
        {
            fT = m_pkCurve->GetTime(i*fTotalLength*fInvCSm1);
        }
        else
        {
            fT = fTMin + i*fTRange*fInvCSm1;
        }

        Vector2f kPos = m_pkCurve->GetPosition(fT);
        m_akSample[i].X() = kPos.X();
        m_akSample[i].Y() = 0.0f;
        m_akSample[i].Z() = kPos.Y();
    }

    // Store the samples and their rotated equivalents.  The storage layout
    // is dependent on the topology of the mesh.
    switch (m_eTopology)
    {
    case REV_DISK_TOPOLOGY:
        UpdateDisk();
        break;
    case REV_CYLINDER_TOPOLOGY:
        UpdateCylinder();
        break;
    case REV_SPHERE_TOPOLOGY:
        UpdateSphere();
        break;
    case REV_TORUS_TOPOLOGY:
        UpdateTorus();
        break;
    default:
        assert(false);
        break;
    }

    UpdateModelNormals();
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateDisk ()
{
    // initial ray
    int iC;
    for (iC = 0; iC < m_iCurveSamples; iC++)
    {
        VBuffer->Position3(iC) = m_akSample[iC];
    }

    // remaining rays obtained by revolution
    int iCSm1 = m_iCurveSamples - 1;
    for (int iR = 1; iR < m_iRadialSamples; iR++)
    {
        for (iC = 1; iC < m_iCurveSamples; iC++)
        {
            float fRadius = m_akSample[iC].X() - m_fXCenter;
            if (fRadius < 0.0f)
            {
                fRadius = 0.0f;
            }
            int i = iC+iCSm1*iR;
            Vector3f kPos(m_fXCenter + fRadius*m_afCos[iR],
                fRadius*m_afSin[iR],m_akSample[iC].Z());
            VBuffer->Position3(i) = kPos;
        }
    }
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateSphere ()
{
    int iVQuantity = VBuffer->GetVertexQuantity();

    // south pole
    VBuffer->Position3(iVQuantity-2) = m_akSample[0];

    // north pole
    VBuffer->Position3(iVQuantity-1) = m_akSample[m_iCurveSamples-1];

    // initial and final ray
    int iC, i;
    for (iC = 1; iC <= m_iCurveSamples-2; iC++)
    {
        i = (iC-1)*(m_iRadialSamples+1);
        VBuffer->Position3(i) = m_akSample[iC];

        i += m_iRadialSamples;
        VBuffer->Position3(i) = m_akSample[iC];
    }

    // remaining rays obtained by revolution
    for (int iR = 1; iR < m_iRadialSamples; iR++)
    {
        for (iC = 1; iC <= m_iCurveSamples-2; iC++)
        {
            float fRadius = m_akSample[iC].X() - m_fXCenter;
            if (fRadius < 0.0f)
            {
                fRadius = 0.0f;
            }
            i = (iC-1)*(m_iRadialSamples+1) + iR;
            Vector3f kPos(m_fXCenter + fRadius*m_afCos[iR],
                fRadius*m_afSin[iR],m_akSample[iC].Z());
            VBuffer->Position3(i) = kPos;
        }
    }
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateCylinder ()
{
    // initial and final ray
    int iC, i;
    for (iC = 0; iC < m_iCurveSamples; iC++)
    {
        i = iC*(m_iRadialSamples+1);
        VBuffer->Position3(i) = m_akSample[iC];

        i += m_iRadialSamples;
        VBuffer->Position3(i) = m_akSample[iC];
    }

    // remaining rays obtained by revolution
    for (int iR = 1; iR < m_iRadialSamples; iR++)
    {
        for (iC = 0; iC < m_iCurveSamples; iC++)
        {
            float fRadius = m_akSample[iC].X() - m_fXCenter;
            if (fRadius < 0.0f)
            {
                fRadius = 0.0f;
            }
            i = iC*(m_iRadialSamples+1) + iR;
            Vector3f kPos(m_fXCenter + fRadius*m_afCos[iR],
                fRadius*m_afSin[iR],m_akSample[iC].Z());
            VBuffer->Position3(i) = kPos;
        }
    }
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateTorus ()
{
    // initial and final ray
    int iVQuantity = VBuffer->GetVertexQuantity();
    int iC, i;
    for (iC = 0; iC < m_iCurveSamples; iC++)
    {
        i = iC*(m_iRadialSamples+1);
        VBuffer->Position3(i) = m_akSample[iC];

        i += m_iRadialSamples;
        VBuffer->Position3(i) = m_akSample[iC];
    }

    // remaining rays obtained by revolution
    int iR;
    for (iR = 1; iR < m_iRadialSamples; iR++)
    {
        for (iC = 0; iC < m_iCurveSamples; iC++)
        {
            float fRadius = m_akSample[iC].X() - m_fXCenter;
            if (fRadius < 0.0f)
            {
                fRadius = 0.0f;
            }
            i = iC*(m_iRadialSamples+1) + iR;
            Vector3f kPos(m_fXCenter + fRadius*m_afCos[iR],
                fRadius*m_afSin[iR],m_akSample[iC].Z());
            VBuffer->Position3(i) = kPos;
        }
    }

    i = iVQuantity - (m_iRadialSamples+1);
    for (iR = 0; iR <= m_iRadialSamples; iR++, i++)
    {
        VBuffer->Position3(i) = VBuffer->Position3(iR);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void RevolutionSurface::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    int iTmp;
    rkStream.Read(m_fXCenter);
    rkStream.Read(iTmp);
    m_eTopology = (TopologyType)iTmp;
    rkStream.Read(m_iCurveSamples);
    rkStream.Read(m_iRadialSamples);
    rkStream.Read(m_iRadialSamples+1,m_afSin);
    rkStream.Read(m_iRadialSamples+1,m_afCos);
    rkStream.Read(m_bSampleByArcLength);

    // TO DO.  See note in RevolutionSurface::Save.
    m_pkCurve = 0;

    WM4_END_DEBUG_STREAM_LOAD(RevolutionSurface);
}
//----------------------------------------------------------------------------
void RevolutionSurface::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool RevolutionSurface::Register (Stream& rkStream) const
{
    return TriMesh::Register(rkStream);
}
//----------------------------------------------------------------------------
void RevolutionSurface::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_fXCenter);
    rkStream.Write((int)m_eTopology);
    rkStream.Write(m_iCurveSamples);
    rkStream.Write(m_iRadialSamples);
    rkStream.Write(m_iRadialSamples+1,m_afSin);
    rkStream.Write(m_iRadialSamples+1,m_afCos);
    rkStream.Write(m_bSampleByArcLength);

    // TO DO.  The class Curve2 is abstract and does not know about the data
    // representation for the derived-class object that is passed to the
    // RevolutionSurface constructor.  Because of this, any loaded
    // RevolutionSurface object will require the application to manually set
    // the curve via the Curve() member.
    //
    // Streaming support should probably be added to the curve classes and
    // a mechanism for saving function pointers should probably be created.

    WM4_END_DEBUG_STREAM_SAVE(RevolutionSurface);
}
//----------------------------------------------------------------------------
int RevolutionSurface::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return TriMesh::GetDiskUsed(rkVersion) +
        sizeof(m_fXCenter) +
        sizeof(int) + // m_eTopology
        sizeof(m_iCurveSamples) +
        sizeof(m_iRadialSamples) +
        2*(m_iRadialSamples+1)*sizeof(float) +  // m_afSin, m_afCos
        sizeof(char);  // m_bSampleByArcLength
}
//----------------------------------------------------------------------------
StringTree* RevolutionSurface::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("x center = ",m_fXCenter));
    pkTree->Append(Format("topology = ",ms_aacTopology[m_eTopology]));
    pkTree->Append(Format("curve samples = ",m_iCurveSamples));
    pkTree->Append(Format("radial samples = ",m_iRadialSamples));
    pkTree->Append(Format("sample by arc length = ",m_bSampleByArcLength));

    // children
    pkTree->Append(TriMesh::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
