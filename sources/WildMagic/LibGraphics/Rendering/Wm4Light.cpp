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
// Version: 4.0.2 (2007/09/24)

#include "Wm4GraphicsPCH.h"
#include "Wm4Light.h"
#include "Wm4Matrix3.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Light,Object);
WM4_IMPLEMENT_STREAM(Light);
WM4_IMPLEMENT_DEFAULT_NAME_ID(Light,Object);

const char* Light::ms_aacType[LT_QUANTITY] =
{
    "LT_AMBIENT",
    "LT_DIRECTIONAL",
    "LT_POINT",
    "LT_SPOT"
};

//----------------------------------------------------------------------------
Light::Light ()
    :
    Type(LT_AMBIENT),
    Ambient(0.0f,0.0f,0.0f),
    Diffuse(0.0f,0.0f,0.0f),
    Specular(0.0f,0.0f,0.0f),
    Position(0.0f,0.0f,0.0f),
    DVector(0.0f,0.0f,0.0f),
    UVector(0.0f,0.0f,0.0f),
    RVector(0.0f,0.0f,0.0f)
{
    Constant = 0.0f;
    Linear = 0.0f;
    Quadratic = 0.0f;
    Intensity = 0.0f;
    Angle = 0.0f;
    CosAngle = 0.0f;
    SinAngle = 0.0f;
    Exponent = 0.0f;
}
//----------------------------------------------------------------------------
Light::Light (LightType eType)
    :
    Type(eType),
    Ambient(0.0f,0.0f,0.0f),
    Diffuse(0.0f,0.0f,0.0f),
    Specular(0.0f,0.0f,0.0f),
    Position(0.0f,0.0f,0.0f),
    DVector(0.0f,0.0f,-1.0f),
    UVector(0.0f,1.0f,0.0f),
    RVector(1.0f,0.0f,0.0f)
{
    Constant = 1.0f;
    Linear = 0.0f;
    Quadratic = 0.0f;
    Intensity = 1.0f;
    Angle = Mathf::PI;
    CosAngle = -1.0f;
    SinAngle = 0.0f;
    Exponent = 1.0f;
}
//----------------------------------------------------------------------------
Light::~Light ()
{
}
//----------------------------------------------------------------------------
void Light::SetAngle (float fAngle)
{
    assert(0.0f < fAngle && fAngle <= Mathf::PI);
    Angle = fAngle;
    CosAngle = Mathf::Cos(fAngle);
    SinAngle = Mathf::Sin(fAngle);
}
//----------------------------------------------------------------------------
void Light::SetDirection (const Vector3f& rkDirection)
{
    DVector = rkDirection;
    Vector3f::GenerateOrthonormalBasis(UVector,RVector,DVector);
}
//----------------------------------------------------------------------------
bool Light::IsValidFrame () const
{
    float fTest = DVector.Dot(UVector);
    if (Mathf::FAbs(fTest) > Mathf::ZERO_TOLERANCE)
    {
        return false;
    }

    fTest = DVector.Dot(RVector);
    if (Mathf::FAbs(fTest) > Mathf::ZERO_TOLERANCE)
    {
        return false;
    }

    fTest = UVector.Dot(RVector);
    if (Mathf::FAbs(fTest) > Mathf::ZERO_TOLERANCE)
    {
        return false;
    }

    fTest = DVector.Dot(UVector.Cross(RVector));
    return Mathf::FAbs(1.0f - fTest) <= Mathf::ZERO_TOLERANCE;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Light::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    if (rkStream.GetVersion() >= StreamVersion(4,7))
    {
        int iTmp;
        rkStream.Read(iTmp);
        Type = (LightType)iTmp;
    }
    else
    {
        // Previous streamversions failed to save the light type, so all
        // loaded lights were ambient (an error).
        Type = LT_AMBIENT;
    }
    rkStream.Read(Position);
    rkStream.Read(DVector);
    rkStream.Read(UVector);
    rkStream.Read(RVector);
    rkStream.Read(Ambient);
    rkStream.Read(Diffuse);
    rkStream.Read(Specular);
    rkStream.Read(Intensity);
    rkStream.Read(Constant);
    rkStream.Read(Linear);
    rkStream.Read(Quadratic);
    rkStream.Read(Angle);
    rkStream.Read(CosAngle);
    rkStream.Read(SinAngle);
    rkStream.Read(Exponent);

    WM4_END_DEBUG_STREAM_LOAD(Light);
}
//----------------------------------------------------------------------------
void Light::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Light::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void Light::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write((int)Type);
    rkStream.Write(Position);
    rkStream.Write(DVector);
    rkStream.Write(UVector);
    rkStream.Write(RVector);
    rkStream.Write(Ambient);
    rkStream.Write(Diffuse);
    rkStream.Write(Specular);
    rkStream.Write(Intensity);
    rkStream.Write(Constant);
    rkStream.Write(Linear);
    rkStream.Write(Quadratic);
    rkStream.Write(Angle);
    rkStream.Write(CosAngle);
    rkStream.Write(SinAngle);
    rkStream.Write(Exponent);

    WM4_END_DEBUG_STREAM_SAVE(Light);
}
//----------------------------------------------------------------------------
int Light::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = Object::GetDiskUsed(rkVersion) +
        sizeof(Position) +
        sizeof(DVector) +
        sizeof(UVector) +
        sizeof(RVector) +
        sizeof(Ambient) +
        sizeof(Diffuse) +
        sizeof(Specular) +
        sizeof(Intensity) +
        sizeof(Constant) +
        sizeof(Linear) +
        sizeof(Quadratic) +
        sizeof(Angle) +
        sizeof(CosAngle) +
        sizeof(SinAngle) +
        sizeof(Exponent);

    if (rkVersion >= StreamVersion(4,7))
    {
        iSize += sizeof(int);  // Type
    }

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* Light::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("type = ",ms_aacType[Type]));
    pkTree->Append(Format("position =",Position));
    pkTree->Append(Format("d vector =",DVector));
    pkTree->Append(Format("u vector =",UVector));
    pkTree->Append(Format("r vector =",RVector));
    pkTree->Append(Format("ambient =",Ambient));
    pkTree->Append(Format("diffuse =",Diffuse));
    pkTree->Append(Format("specular =",Specular));
    pkTree->Append(Format("intensity =",Intensity));
    pkTree->Append(Format("attenuation constant =",Constant));
    pkTree->Append(Format("attenuation linear =",Linear));
    pkTree->Append(Format("attenuation quadratic =",Quadratic));
    pkTree->Append(Format("angle =",Angle));
    pkTree->Append(Format("exponent =",Exponent));

    // children
    pkTree->Append(Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
