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
#include "Wm4StringTree.h"
#include "Wm4BoundingVolume.h"
#include "Wm4ColorRGB.h"
#include "Wm4ColorRGBA.h"
using namespace Wm4;

//----------------------------------------------------------------------------
StringTree::StringTree ()
{
}
//----------------------------------------------------------------------------
StringTree::~StringTree ()
{
    int i;
    for (i = 0; i < (int)m_kStrings.size(); i++)
    {
        WM4_DELETE[] m_kStrings[i];
    }

    for (i = 0; i < (int)m_kChildren.size(); i++)
    {
        WM4_DELETE m_kChildren[i];
    }
}
//----------------------------------------------------------------------------
bool StringTree::Save (const char* acFilename, int iTabSize)
{
    FILE* pkFile = System::Fopen(acFilename,"wt");
    if (!pkFile)
    {
        return false;
    }

    Save(pkFile,0,iTabSize);

    return System::Fclose(pkFile) == 0;
}
//----------------------------------------------------------------------------
void StringTree::Save (FILE* pkFile, int iLevel, int iTabSize)
{
    // indent to proper location
    int i, iIndent = iLevel*iTabSize;
    if (iIndent > 0)
    {
        for (i = 0; i < iIndent; i++)
        {
            System::Fprintf(pkFile,"%c",' ');
        }
    }

    // label with level
    if (iLevel < 10)
    {
        System::Fprintf(pkFile,"%d:  ",iLevel);
    }
    else
    {
        System::Fprintf(pkFile,"%d: ",iLevel);
    }

    // header string
    if (m_kStrings.size() > 0)
    {
        System::Fprintf(pkFile,"%s\n",m_kStrings[0]);
    }
    else
    {
        System::Fprintf(pkFile,"<no header>\n");
    }

    // body strings
    iIndent += 4;

    int j;
    for (j = 1; j < (int)m_kStrings.size(); j++)
    {
        for (i = 0; i < iIndent; i++)
        {
            System::Fprintf(pkFile,"%c",' ');
        }
        System::Fprintf(pkFile,"%s\n",m_kStrings[j]);
    }

    iLevel++;
    for (j = 0; j < (int)m_kChildren.size(); j++)
    {
        m_kChildren[j]->Save(pkFile,iLevel,iTabSize);
    }
}
//----------------------------------------------------------------------------
void StringTree::FormatFloat (float fValue, size_t uiStringSize,
    char* acString)
{
    if (fValue > -Mathf::MAX_REAL)
    {
        if (fValue < Mathf::MAX_REAL)
        {
            System::Sprintf(acString,uiStringSize,"%f",fValue);
        }
        else
        {
            System::Strcpy(acString,uiStringSize,"INFINITY");
        }
    }
    else
    {
        System::Strcpy(acString,uiStringSize,"-INFINITY");
    }
}
//----------------------------------------------------------------------------
void StringTree::FormatDouble (double dValue, size_t uiStringSize,
    char* acString)
{
    if (dValue > -Mathd::MAX_REAL)
    {
        if (dValue < Mathd::MAX_REAL)
        {
            System::Sprintf(acString,uiStringSize,"%lf",dValue);
        }
        else
        {
            System::Strcpy(acString,uiStringSize,"INFINITY");
        }
    }
    else
    {
        System::Strcpy(acString,uiStringSize,"-INFINITY");
    }
}
//----------------------------------------------------------------------------
char* Wm4::Format (const Rtti* pkRTTI, const char* acName)
{
    assert(pkRTTI);
    const char* acRTTIName = pkRTTI->GetName();

    size_t uiLength;
    char* acString;
    if (acName)
    {
        uiLength = strlen(acRTTIName) + strlen(acName) + 4;
        acString = WM4_NEW char[uiLength];
        System::Sprintf(acString,uiLength,"%s <%s>",acRTTIName,acName);
    }
    else
    {
        uiLength = strlen(acRTTIName) + 2;
        acString = WM4_NEW char[uiLength];
        System::Sprintf(acString,uiLength,"%s",acRTTIName);
    }
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acString)
{
    size_t uiSize;
    char* acDuplicate;
    if (acString)
    {
        uiSize = strlen(acString) + 1;
        acDuplicate = WM4_NEW char[uiSize];
        System::Strcpy(acDuplicate,uiSize,acString);
    }
    else
    {
        uiSize = strlen("<no title>") + 1;
        acDuplicate = WM4_NEW char[uiSize];
        System::Strcpy(acDuplicate,uiSize,"<no title>");
    }
    return acDuplicate;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// formatting of native data
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, bool bValue)
{
    assert(acPrefix);
    size_t uiLength = strlen(acPrefix) + 2 + (bValue ? 4 : 5);
    char* acString = WM4_NEW char[uiLength];
    if (bValue)
    {
        System::Sprintf(acString,uiLength,"%s true",acPrefix);
    }
    else
    {
        System::Sprintf(acString,uiLength,"%s false",acPrefix);
    }
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, char cValue)
{
    assert(acPrefix);
    size_t uiLength = strlen(acPrefix) + 2 + 1;
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %c",acPrefix,cValue);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, unsigned char ucValue)
{
    assert(acPrefix);
    const size_t uiSize = 32;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"%u",(unsigned int)ucValue);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, short sValue)
{
    assert(acPrefix);
    const size_t uiSize = 32;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"%hd",sValue);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, unsigned short usValue)
{
    assert(acPrefix);
    const size_t uiSize = 32;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"%hu",usValue);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, int iValue)
{
    assert(acPrefix);
    const size_t uiSize = 32;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"%d",iValue);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, unsigned int uiValue)
{
    assert(acPrefix);
    const size_t uiSize = 32;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"%u",uiValue);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, long lValue)
{
    assert(acPrefix);
    const size_t uiSize = 32;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"%ld",lValue);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, unsigned long ulValue)
{
    assert(acPrefix);
    const size_t uiSize = 32;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"%lu",ulValue);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, float fValue)
{
    assert(acPrefix);
    const size_t uiDummySize = 256;
    char acDummy[uiDummySize];
    StringTree::FormatFloat(fValue,uiDummySize,acDummy);

    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, double dValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    StringTree::FormatDouble(dValue,uiSize,acDummy);

    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, void* pvValue)
{
    assert(acPrefix);
    const size_t uiSize = 32;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"%p",pvValue);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const char* acValue)
{
    assert(acPrefix && acValue);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acValue);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acValue);
    return acString;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// formatting of nonnative data
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const BoundingVolume* pkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize], acZ[uiSize], acR[uiSize];
    StringTree::FormatFloat(pkValue->GetCenter().X(),uiSize,acX);
    StringTree::FormatFloat(pkValue->GetCenter().Y(),uiSize,acY);
    StringTree::FormatFloat(pkValue->GetCenter().Z(),uiSize,acZ);
    StringTree::FormatFloat(pkValue->GetRadius(),uiSize,acR);

    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"(x: %s, y: %s, z: %s, r: %s)",acX,acY,
        acZ,acR);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const ColorRGBA& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"(r: %f, g: %f, b: %f, a: %f)",
        rkValue.R(),rkValue.G(),rkValue.B(),rkValue.A());
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const ColorRGB& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"(r: %f, g: %f, b: %f)",rkValue.R(),
        rkValue.G(),rkValue.B());
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Line3f& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,
        "(px: %f, py: %f, pz: %f, dx: %f, dy: %f, dz: %f)",
        rkValue.Origin.X(),rkValue.Origin.Y(),rkValue.Origin.Z(),
        rkValue.Direction.X(),rkValue.Direction.Y(),rkValue.Direction.Z());
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Matrix2f& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"[c0:(%f,%f),c1:(%f,%f)]",
        rkValue[0][0],rkValue[1][0],
        rkValue[0][1],rkValue[1][1]);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Matrix3f& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,
        "[c0:(%f,%f,%f),c1:(%f,%f,%f),c2:(%f,%f,%f)]",
        rkValue[0][0],rkValue[1][0],rkValue[2][0],
        rkValue[0][1],rkValue[1][1],rkValue[2][1],
        rkValue[0][2],rkValue[1][2],rkValue[2][2]);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Matrix4f& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[256];
    System::Sprintf(acDummy,uiSize,
    "[c0:(%f,%f,%f,%f),c1:(%f,%f,%f,%f),c2:(%f,%f,%f,%f),c3(%f,%f,%f,%f)]",
        rkValue[0][0],rkValue[1][0],rkValue[2][0],rkValue[3][0],
        rkValue[0][1],rkValue[1][1],rkValue[2][1],rkValue[3][1],
        rkValue[0][2],rkValue[1][2],rkValue[2][2],rkValue[3][2],
        rkValue[0][3],rkValue[1][3],rkValue[2][3],rkValue[3][3]);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Plane3f& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,
        "(nx: %f, ny: %f, nz: %f, c: %f)",rkValue.Normal.X(),
        rkValue.Normal.Y(),rkValue.Normal.Z(),rkValue.Constant);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Quaternionf& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,
        "(w: %.3f, x: %.3f, y: %.3f, z: %.3f)",rkValue.W(),
        rkValue.X(),rkValue.Y(),rkValue.Z());
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Vector2f& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize];
    StringTree::FormatFloat(rkValue.X(),uiSize,acX);
    StringTree::FormatFloat(rkValue.Y(),uiSize,acY);

    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"(x: %s, y: %s)",acX,acY);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Vector3f& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize], acZ[uiSize];
    StringTree::FormatFloat(rkValue.X(),uiSize,acX);
    StringTree::FormatFloat(rkValue.Y(),uiSize,acY);
    StringTree::FormatFloat(rkValue.Z(),uiSize,acZ);

    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"(x: %s, y: %s, z: %s)",acX,acY,acZ);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Vector4f& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize], acZ[uiSize], acW[uiSize];
    StringTree::FormatFloat(rkValue.X(),uiSize,acX);
    StringTree::FormatFloat(rkValue.Y(),uiSize,acY);
    StringTree::FormatFloat(rkValue.Z(),uiSize,acZ);
    StringTree::FormatFloat(rkValue.W(),uiSize,acW);

    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"(x: %s, y: %s, z: %s, w: %s)",acX,acY,
        acZ,acW);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Line3d& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,
        "(px: %lf, py: %lf, pz: %lf, dx: %lf, dy: %lf, dz: %lf)",
        rkValue.Origin.X(),rkValue.Origin.Y(),rkValue.Origin.Z(),
        rkValue.Direction.X(),rkValue.Direction.Y(),rkValue.Direction.Z());
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Matrix2d& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"[c0:(%lf,%lf),c1:(%lf,%lf)]",
        rkValue[0][0],rkValue[1][0],
        rkValue[0][1],rkValue[1][1]);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Matrix3d& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,
        "[c0:(%lf,%lf,%lf),c1:(%lf,%lf,%lf),c2:(%lf,%lf,%lf)]",
        rkValue[0][0],rkValue[1][0],rkValue[2][0],
        rkValue[0][1],rkValue[1][1],rkValue[2][1],
        rkValue[0][2],rkValue[1][2],rkValue[2][2]);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Matrix4d& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,
    "[c0:(%lf,%lf,%lf,%lf),c1:(%lf,%lf,%lf,%lf),c2:(%lf,%lf,%lf,%lf),"
    "c3(%lf,%lf,%lf,%lf)]",
        rkValue[0][0],rkValue[1][0],rkValue[2][0],rkValue[3][0],
        rkValue[0][1],rkValue[1][1],rkValue[2][1],rkValue[3][1],
        rkValue[0][2],rkValue[1][2],rkValue[2][2],rkValue[3][2],
        rkValue[0][3],rkValue[1][3],rkValue[2][3],rkValue[3][3]);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Plane3d& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,
        "(nx: %lf, ny: %lf, nz: %lf, c: %lf)",rkValue.Normal.X(),
        rkValue.Normal.Y(),rkValue.Normal.Z(),rkValue.Constant);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Quaterniond& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,
        "(w: %.3lf, x: %.3lf, y: %.3lf, z: %.3lf)",rkValue.W(),
        rkValue.X(),rkValue.Y(),rkValue.Z());
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Vector2d& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize];
    StringTree::FormatDouble(rkValue.X(),uiSize,acX);
    StringTree::FormatDouble(rkValue.Y(),uiSize,acY);

    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"(x: %s, y: %s)",acX,acY);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Vector3d& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize], acZ[uiSize];
    StringTree::FormatDouble(rkValue.X(),uiSize,acX);
    StringTree::FormatDouble(rkValue.Y(),uiSize,acY);
    StringTree::FormatDouble(rkValue.Z(),uiSize,acZ);

    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"(x: %s, y: %s, z: %s)",acX,acY,acZ);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* Wm4::Format (const char* acPrefix, const Vector4d& rkValue)
{
    assert(acPrefix);
    const size_t uiSize = 256;
    char acX[uiSize], acY[uiSize], acZ[uiSize], acW[uiSize];
    StringTree::FormatDouble(rkValue.X(),uiSize,acX);
    StringTree::FormatDouble(rkValue.Y(),uiSize,acY);
    StringTree::FormatDouble(rkValue.Z(),uiSize,acZ);
    StringTree::FormatDouble(rkValue.W(),uiSize,acW);

    char acDummy[uiSize];
    System::Sprintf(acDummy,uiSize,"(x: %s, y: %s, z: %s, w: %s)",acX,acY,
        acZ,acW);
    size_t uiLength = strlen(acPrefix) + 2 + strlen(acDummy);
    char* acString = WM4_NEW char[uiLength];
    System::Sprintf(acString,uiLength,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
