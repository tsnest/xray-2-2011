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
// Version: 4.0.1 (2006/09/21)

#include "Wm4ImagicsPCH.h"
#include "Wm4Lattice.h"
using namespace Wm4;

const char* Lattice::ms_acHeader = "Magic Image";

//----------------------------------------------------------------------------
Lattice::Lattice (int iDimensions, int* aiBound)
{
#ifdef _DEBUG
    assert(iDimensions > 0 && aiBound);
    for (int i = 0; i < iDimensions; i++)
    {
        assert(aiBound[i] > 0);
    }
#endif

    m_iDimensions = iDimensions;
    m_aiBound = aiBound;
    m_aiOffset = WM4_NEW int[iDimensions];

    ComputeQuantityAndOffsets();
}
//----------------------------------------------------------------------------
Lattice::Lattice (const Lattice& rkLattice)
{
    m_iDimensions = rkLattice.m_iDimensions;
    m_iQuantity = rkLattice.m_iQuantity;
    m_aiBound = WM4_NEW int[m_iDimensions];
    m_aiOffset = WM4_NEW int[m_iDimensions];

    size_t uiSize = m_iDimensions*sizeof(int);
    System::Memcpy(m_aiBound,uiSize,rkLattice.m_aiBound,uiSize);
    System::Memcpy(m_aiOffset,uiSize,rkLattice.m_aiOffset,uiSize);
}
//----------------------------------------------------------------------------
Lattice::Lattice (int iDimensions)
{
    assert(iDimensions > 0);

    m_iDimensions = iDimensions;
    m_aiBound = 0;
    m_aiOffset = WM4_NEW int[iDimensions];
    memset(m_aiOffset,0,iDimensions*sizeof(int));
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
Lattice::Lattice ()
{
    m_iDimensions = 0;
    m_aiBound = 0;
    m_aiOffset = 0;
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
Lattice::~Lattice ()
{
    WM4_DELETE[] m_aiBound;
    WM4_DELETE[] m_aiOffset;
}
//----------------------------------------------------------------------------
void Lattice::SetBounds (int* aiBound)
{
#ifdef _DEBUG
    assert(aiBound);
    for (int i = 0; i < m_iDimensions; i++)
    {
        assert(aiBound[i] > 0);
    }
#endif

    m_aiBound = aiBound;
    ComputeQuantityAndOffsets();
}
//----------------------------------------------------------------------------
void Lattice::ComputeQuantityAndOffsets ()
{
    int i;

    // calculate number of lattice points
    m_iQuantity = 1;
    for (i = 0; i < m_iDimensions; i++)
    {
        m_iQuantity *= m_aiBound[i];
    }

    // calculate offset indices of neighboring lattice points
    m_aiOffset[0] = 1;
    for (i = 1; i < m_iDimensions; i++)
    {
        m_aiOffset[i] = m_aiBound[i-1]*m_aiOffset[i-1];
    }
}
//----------------------------------------------------------------------------
Lattice& Lattice::operator= (const Lattice& rkLattice)
{
    if (m_iDimensions != rkLattice.m_iDimensions)
    {
        WM4_DELETE[] m_aiBound;
        WM4_DELETE[] m_aiOffset;
        m_iDimensions = rkLattice.m_iDimensions;
        m_iQuantity = rkLattice.m_iQuantity;
        m_aiBound = WM4_NEW int[m_iDimensions];
        m_aiOffset = WM4_NEW int[m_iDimensions];
    }

    size_t uiSize = m_iDimensions*sizeof(int);
    System::Memcpy(m_aiBound,uiSize,rkLattice.m_aiBound,uiSize);
    System::Memcpy(m_aiOffset,uiSize,rkLattice.m_aiOffset,uiSize);

    return *this;
}
//----------------------------------------------------------------------------
bool Lattice::operator== (const Lattice& rkLattice) const
{
    if (m_iDimensions != rkLattice.m_iDimensions)
    {
        return false;
    }

    int iCompareSize = m_iDimensions*sizeof(int);
    return memcmp(m_aiBound,rkLattice.m_aiBound,iCompareSize) == 0;
}
//----------------------------------------------------------------------------
bool Lattice::operator!= (const Lattice& rkLattice) const
{
    return !operator==(rkLattice);
}
//----------------------------------------------------------------------------
int Lattice::GetIndex (const int* aiCoord) const
{
    // assert:  aiCoord is array of m_iDimensions elements
    int iIndex = aiCoord[0];
    for (int i = 1; i < m_iDimensions; i++)
    {
        iIndex += m_aiOffset[i]*aiCoord[i];
    }
    return iIndex;
}
//----------------------------------------------------------------------------
void Lattice::GetCoordinates (int iIndex, int* aiCoord) const
{
    // assert:  aiCoord is array of m_iDimensions elements
    for (int i = 0; i < m_iDimensions; i++)
    {
        aiCoord[i] = iIndex % m_aiBound[i];
        iIndex /= m_aiBound[i];
    }
}
//----------------------------------------------------------------------------
bool Lattice::Load (FILE* pkIFile)
{
    // Header is an ASCII string of 'char'.  No need to swap bytes on big
    // endian platforms.
    int iSize = (int)strlen(ms_acHeader) + 1;
    char* acBuffer = WM4_NEW char[iSize];
    System::Read1(pkIFile,iSize,acBuffer);
    acBuffer[iSize-1] = 0;
    if (strncmp(acBuffer,ms_acHeader,iSize) != 0)
    {
        WM4_DELETE[] acBuffer;
        m_iDimensions = 0;
        m_iQuantity = 0;
        m_aiBound = 0;
        m_aiOffset = 0;
        return false;
    }
    WM4_DELETE[] acBuffer;

    System::Read4le(pkIFile,1,&m_iDimensions);

    WM4_DELETE[] m_aiBound;
    m_aiBound = WM4_NEW int[m_iDimensions];
    System::Read4le(pkIFile,m_iDimensions,m_aiBound);

    WM4_DELETE[] m_aiOffset;
    m_aiOffset = WM4_NEW int[m_iDimensions];

    ComputeQuantityAndOffsets();

    return true;
}
//----------------------------------------------------------------------------
bool Lattice::Save (FILE* pkOFile) const
{
    System::Write1(pkOFile,(int)strlen(ms_acHeader)+1,ms_acHeader);
    System::Write4le(pkOFile,1,&m_iDimensions);
    System::Write4le(pkOFile,m_iDimensions,m_aiBound);
    return true;
}
//----------------------------------------------------------------------------
bool Lattice::LoadRaw (const char* acFilename, int& riDimensions,
    int*& raiBound, int& riQuantity, int& riRTTI, int& riSizeOf,
    char*& racData)
{
    FILE* pkIFile = System::Fopen(acFilename,"rb");
    if (!pkIFile)
    {
        return false;
    }

    // Header is an ASCII string of 'char'.  No need to swap bytes on big
    // endian platforms.
    int iSize = (int)strlen(ms_acHeader) + 1;
    char* acBuffer = WM4_NEW char[iSize];
    System::Read1(pkIFile,iSize,acBuffer);
    acBuffer[iSize-1] = 0;
    if (strncmp(acBuffer,ms_acHeader,iSize) != 0)
    {
        WM4_DELETE[] acBuffer;
        System::Fclose(pkIFile);
        return false;
    }
    WM4_DELETE[] acBuffer;

    System::Read4le(pkIFile,1,&riDimensions);

    raiBound = WM4_NEW int[riDimensions];
    System::Read4le(pkIFile,riDimensions,raiBound);

    riQuantity = 1;
    for (int i = 0; i < riDimensions; i++)
    {
        riQuantity *= raiBound[i];
    }

    System::Read4le(pkIFile,1,&riRTTI);
    System::Read4le(pkIFile,1,&riSizeOf);

    racData = WM4_NEW char[riQuantity*riSizeOf];
    switch (riSizeOf)
    {
    case 1:
        System::Read1(pkIFile,riQuantity,racData);
        break;
    case 2:
        System::Read2le(pkIFile,riQuantity,racData);
        break;
    case 4:
        System::Read4le(pkIFile,riQuantity,racData);
        break;
    case 8:
        System::Read8le(pkIFile,riQuantity,racData);
        break;
    default:
        assert(false);
    }

    return true;
}
//----------------------------------------------------------------------------
