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
// Version: 4.0.1 (2007/06/07)

#include "Wm4GraphicsPCH.h"
#include "Wm4MorphController.h"
#include "Wm4Geometry.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,MorphController,Controller);
WM4_IMPLEMENT_STREAM(MorphController);

//----------------------------------------------------------------------------
MorphController::MorphController (int iVertexQuantity, int iTargetQuantity,
    int iKeyQuantity)
{
    m_iVertexQuantity = iVertexQuantity;
    m_iTargetQuantity = iTargetQuantity;
    m_iKeyQuantity = iKeyQuantity;
    m_iLastIndex = 0;
    m_aspkVertices = WM4_NEW Vector3fArrayPtr[m_iTargetQuantity];
    m_aspkWeights = WM4_NEW FloatArrayPtr[m_iKeyQuantity];
}
//----------------------------------------------------------------------------
MorphController::MorphController ()
{
    m_iVertexQuantity = 0;
    m_iTargetQuantity = 0;
    m_iKeyQuantity = 0;
    m_iLastIndex = 0;
    m_aspkVertices = 0;
    m_aspkWeights = 0;
}
//----------------------------------------------------------------------------
MorphController::~MorphController ()
{
    WM4_DELETE[] m_aspkVertices;
    WM4_DELETE[] m_aspkWeights;
}
//----------------------------------------------------------------------------
void MorphController::GetKeyInfo (float fCtrlTime, float& rfTime,
    float& rfOmTime, int& ri0, int& ri1)
{
    float* afTime = m_spkTimes->GetData();

    if (fCtrlTime <= afTime[0])
    {
        rfTime = 0.0f;
        rfOmTime = 1.0f;
        m_iLastIndex = 0;
        ri0 = 0;
        ri1 = 0;
        return;
    }

    if (fCtrlTime >= afTime[m_iKeyQuantity-1])
    {
        rfTime = 0.0f;
        rfOmTime = 1.0f;
        m_iLastIndex = m_iKeyQuantity - 1;
        ri0 = m_iLastIndex;
        ri1 = m_iLastIndex;
        return;
    }

    int iNextIndex;
    if (fCtrlTime > afTime[m_iLastIndex])
    {
        iNextIndex = m_iLastIndex + 1;
        while (fCtrlTime >= afTime[iNextIndex])
        {
            m_iLastIndex = iNextIndex;
            iNextIndex++;
        }

        ri0 = m_iLastIndex;
        ri1 = iNextIndex;
        rfTime = (fCtrlTime - afTime[ri0])/(afTime[ri1] - afTime[ri0]);
    }
    else if (fCtrlTime < afTime[m_iLastIndex])
    {
        iNextIndex = m_iLastIndex - 1;
        while (fCtrlTime <= afTime[iNextIndex])
        {
            m_iLastIndex = iNextIndex;
            iNextIndex--;
        }

        ri0 = iNextIndex;
        ri1 = m_iLastIndex;
        rfTime = (fCtrlTime - afTime[ri0])/(afTime[ri1] - afTime[ri0]);
    }
    else
    {
        rfTime = 0.0f;
        ri0 = m_iLastIndex;
        ri1 = m_iLastIndex;
    }

    rfOmTime = 1.0f - rfTime;
}
//----------------------------------------------------------------------------
bool MorphController::Update (double dAppTime)
{
    // The key interpolation uses linear interpolation.  To get higher-order
    // interpolation, you need to provide a more sophisticated key (Bezier
    // cubic or TCB spline, for example).

    if (!Controller::Update(dAppTime))
    {
        return false;
    }

    // set vertices to target[0]
    Geometry* pkGeom = StaticCast<Geometry>(m_pkObject);
    VertexBuffer* pkVB = pkGeom->VBuffer;
    Vector3f* akTarget = m_aspkVertices[0]->GetData();
    int i;
    for (i = 0; i < m_iVertexQuantity; i++)
    {
        pkVB->Position3(i) = akTarget[i];
    }

    // lookup the bounding keys
    float fCtrlTime = (float)GetControlTime(dAppTime);
    float fNT, fOmNT;
    int i0, i1;
    GetKeyInfo(fCtrlTime,fNT,fOmNT,i0,i1);

    // add the remaining components in the convex composition
    float* afWeights0 = m_aspkWeights[i0]->GetData();
    float* afWeights1 = m_aspkWeights[i1]->GetData();
    for (i = 1; i < m_iTargetQuantity; i++)
    {
        float fCoeff = fOmNT*afWeights0[i-1] + fNT*afWeights1[i-1];
        akTarget = m_aspkVertices[i]->GetData();
        for (int j = 0; j < m_iVertexQuantity; j++)
        {
            pkVB->Position3(j) += fCoeff*akTarget[j];
        }
    }

    pkGeom->UpdateMS();
    pkVB->Release();
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* MorphController::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Controller::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    int i;

    if (m_aspkVertices)
    {
        for (i = 0; i < m_iTargetQuantity; i++)
        {
            pkFound = m_aspkVertices[i]->GetObjectByName(rkName);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    if (m_spkTimes)
    {
        pkFound = m_spkTimes->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (m_aspkWeights)
    {
        for (i = 0; i < m_iKeyQuantity; i++)
        {
            pkFound = m_aspkWeights[i]->GetObjectByName(rkName);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void MorphController::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Controller::GetAllObjectsByName(rkName,rkObjects);

    int i;

    if (m_aspkVertices)
    {
        for (i = 0; i < m_iTargetQuantity; i++)
        {
            m_aspkVertices[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }

    if (m_spkTimes)
    {
        m_spkTimes->GetAllObjectsByName(rkName,rkObjects);
    }

    if (m_aspkWeights)
    {
        for (i = 0; i < m_iKeyQuantity; i++)
        {
            m_aspkWeights[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }
}
//----------------------------------------------------------------------------
Object* MorphController::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Controller::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    int i;

    if (m_aspkVertices)
    {
        for (i = 0; i < m_iTargetQuantity; i++)
        {
            pkFound = m_aspkVertices[i]->GetObjectByID(uiID);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    if (m_spkTimes)
    {
        pkFound = m_spkTimes->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (m_aspkWeights)
    {
        for (i = 0; i < m_iKeyQuantity; i++)
        {
            pkFound = m_aspkWeights[i]->GetObjectByID(uiID);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void MorphController::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Controller::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iVertexQuantity);
    rkStream.Read(m_iTargetQuantity);
    rkStream.Read(m_iKeyQuantity);

    // link data
    m_aspkVertices = WM4_NEW Vector3fArrayPtr[m_iTargetQuantity];
    m_aspkWeights = WM4_NEW FloatArrayPtr[m_iKeyQuantity];

    Object* pkObject;
    int i;

    // m_aspkVertices
    for (i = 0; i < m_iTargetQuantity; i++)
    {
        rkStream.Read(pkObject);
        pkLink->Add(pkObject);
    }

    // m_spkTimes
    rkStream.Read(pkObject);
    pkLink->Add(pkObject);

    // m_aspkWeight
    for (i = 0; i < m_iKeyQuantity; i++)
    {
        rkStream.Read(pkObject);
        pkLink->Add(pkObject);
    }

    WM4_END_DEBUG_STREAM_LOAD(MorphController);
}
//----------------------------------------------------------------------------
void MorphController::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Controller::Link(rkStream,pkLink);

    Object* pkLinkID;
    int i;

    // m_aspkVertices
    for (i = 0; i < m_iTargetQuantity; i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_aspkVertices[i] = (Vector3fArray*)rkStream.GetFromMap(pkLinkID);
    }

    // m_spkTimes
    pkLinkID = pkLink->GetLinkID();
    m_spkTimes = (FloatArray*)rkStream.GetFromMap(pkLinkID);

    // m_aspkWeights
    for (i = 0; i < m_iKeyQuantity; i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_aspkWeights[i] = (FloatArray*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool MorphController::Register (Stream& rkStream) const
{
    if (!Controller::Register(rkStream))
    {
        return false;
    }

    int i;
    for (i = 0; i < m_iTargetQuantity; i++)
    {
        if (m_aspkVertices[i])
        {
            m_aspkVertices[i]->Register(rkStream);
        }
    }

    if (m_spkTimes)
    {
        m_spkTimes->Register(rkStream);
    }

    for (i = 0; i < m_iKeyQuantity; i++)
    {
        if (m_aspkWeights[i])
        {
            m_aspkWeights[i]->Register(rkStream);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void MorphController::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Controller::Save(rkStream);

    // native data
    rkStream.Write(m_iVertexQuantity);
    rkStream.Write(m_iTargetQuantity);
    rkStream.Write(m_iKeyQuantity);

    // link data
    ObjectPtr* apkArray = (ObjectPtr*)m_aspkVertices;
    //rkStream.Write(m_iTargetQuantity,apkArray);
    int i;
    for (i = 0; i < m_iTargetQuantity; i++)
    {
        rkStream.Write(apkArray[i]);
    }

    rkStream.Write(m_spkTimes);
    apkArray = (ObjectPtr*)m_aspkWeights;
    //rkStream.Write(m_iKeyQuantity,apkArray);
    for (i = 0; i < m_iKeyQuantity; i++)
    {
        rkStream.Write(apkArray[i]);
    }

    WM4_END_DEBUG_STREAM_SAVE(MorphController);
}
//----------------------------------------------------------------------------
int MorphController::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Controller::GetDiskUsed(rkVersion) +
        sizeof(m_iVertexQuantity) +
        sizeof(m_iTargetQuantity) +
        sizeof(m_iKeyQuantity) +
        m_iTargetQuantity*WM4_PTRSIZE(m_aspkVertices[0]) +
        WM4_PTRSIZE(m_spkTimes) +
        m_iKeyQuantity*WM4_PTRSIZE(m_aspkWeights[0]);
}
//----------------------------------------------------------------------------
StringTree* MorphController::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("vertex quantity =",m_iVertexQuantity));
    pkTree->Append(Format("target quantity =",m_iTargetQuantity));
    pkTree->Append(Format("key quantity =",m_iKeyQuantity));

    // children
    pkTree->Append(Controller::SaveStrings());

    const size_t uiTitleSize = 256;
    char acTitle[uiTitleSize];
    int i;

    // vertex/target array
    for (i = 0; i < m_iTargetQuantity; i++)
    {
        System::Sprintf(acTitle,uiTitleSize,"verts for target[%d]",i);
        pkTree->Append(m_aspkVertices[i]->SaveStrings(acTitle));
    }

    // morph keys
    pkTree->Append(m_spkTimes->SaveStrings("times"));

    for (i = 0; i < m_iKeyQuantity; i++)
    {
        System::Sprintf(acTitle,uiTitleSize,"weights for key[%d]",i);
        pkTree->Append(m_aspkWeights[i]->SaveStrings(acTitle));
    }

    return pkTree;
}
//----------------------------------------------------------------------------
