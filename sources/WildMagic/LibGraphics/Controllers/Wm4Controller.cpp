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
#include "Wm4Controller.h"
#include "Wm4Math.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Controller,Object);
WM4_IMPLEMENT_ABSTRACT_STREAM(Controller);
WM4_IMPLEMENT_DEFAULT_NAME_ID(Controller,Object);

const char* Controller::ms_aacRepeatType[Controller::RT_QUANTITY] =
{
    "RT_CLAMP",
    "RT_WRAP",
    "RT_CYCLE"
};

//----------------------------------------------------------------------------
Controller::Controller ()
{
    m_pkObject = 0;
    Repeat = RT_CLAMP;
    MinTime = 0.0;
    MaxTime = 0.0;
    Phase = 0.0;
    Frequency = 1.0;
    Active = true;
    m_dLastAppTime = -DBL_MAX;
}
//----------------------------------------------------------------------------
Controller::~Controller ()
{
}
//----------------------------------------------------------------------------
double Controller::GetControlTime (double dAppTime)
{
    double dCtrlTime = Frequency*dAppTime + Phase;

    if (Repeat == RT_CLAMP)
    {
        if (dCtrlTime < MinTime)
        {
            return MinTime;
        }
        if (dCtrlTime > MaxTime)
        {
            return MaxTime;
        }
        return dCtrlTime;
    }

    double dRange = MaxTime - MinTime;
    if (dRange > 0.0)
    {
        double dMultiples = (dCtrlTime - MinTime)/dRange;
        double dIntTime = Mathd::Floor(dMultiples);
        double dFrcTime = dMultiples - dIntTime;
        if (Repeat == RT_WRAP)
        {
            return MinTime + dFrcTime*dRange;
        }

        // Repeat == RT_CYCLE
        if (((int)dIntTime) & 1)
        {
            // backward time
            return MaxTime - dFrcTime*dRange;
        }
        else
        {
            // forward time
            return MinTime + dFrcTime*dRange;
        }
    }
    else
    {
        return MinTime;
    }
}
//----------------------------------------------------------------------------
bool Controller::Update (double dAppTime)
{
    if (Active
    &&  (dAppTime == -Mathd::MAX_REAL || dAppTime != m_dLastAppTime))
    {
        m_dLastAppTime = dAppTime;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Controller::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    int iRepeat;
    rkStream.Read(iRepeat);
    Repeat = (RepeatType)iRepeat;

    rkStream.Read(MinTime);
    rkStream.Read(MaxTime);
    rkStream.Read(Phase);
    rkStream.Read(Frequency);
    rkStream.Read(Active);
    // m_dLastAppTime is not streamed

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);
    pkLink->Add(pkObject);

    WM4_END_DEBUG_STREAM_LOAD(Controller);
}
//----------------------------------------------------------------------------
void Controller::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_pkObject = rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool Controller::Register (Stream& rkStream) const
{
    if (!Object::Register(rkStream))
    {
        return false;
    }

    if (m_pkObject)
    {
        m_pkObject->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void Controller::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write((int)Repeat);
    rkStream.Write(MinTime);
    rkStream.Write(MaxTime);
    rkStream.Write(Phase);
    rkStream.Write(Frequency);
    rkStream.Write(Active);

    // link data
    rkStream.Write(m_pkObject);

    WM4_END_DEBUG_STREAM_SAVE(Controller);
}
//----------------------------------------------------------------------------
int Controller::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Object::GetDiskUsed(rkVersion) +
        sizeof(int) +  // Repeat
        sizeof(MinTime) +
        sizeof(MaxTime) +
        sizeof(Phase) +
        sizeof(Frequency) +
        sizeof(char) +  // Active
        WM4_PTRSIZE(m_pkObject);
}
//----------------------------------------------------------------------------
StringTree* Controller::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("repeat type = ",ms_aacRepeatType[Repeat]));
    pkTree->Append(Format("min time =",MinTime));
    pkTree->Append(Format("max time =",MaxTime));
    pkTree->Append(Format("phase =",Phase));
    pkTree->Append(Format("frequency =",Frequency));
    pkTree->Append(Format("active =",Active));

    // children
    pkTree->Append(Object::SaveStrings());

    // Object will iterate over controllers to save strings
    return pkTree;
}
//----------------------------------------------------------------------------
