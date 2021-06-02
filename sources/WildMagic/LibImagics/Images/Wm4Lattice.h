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

#ifndef WM4LATTICE_H
#define WM4LATTICE_H

#include "Wm4ImagicsLIB.h"
#include "Wm4System.h"

namespace Wm4
{

class WM4_IMAGICS_ITEM Lattice
{
public:
    // abstract base class for TImage
    virtual ~Lattice ();

    // data access
    int GetDimensions () const;
    const int* GetBounds () const;
    int GetBound (int i) const;
    int GetQuantity () const;
    const int* GetOffsets () const;
    int GetOffset (int i) const;

    // assignment
    Lattice& operator= (const Lattice& rkLattice);

    // comparisons
    bool operator== (const Lattice& rkLattice) const;
    bool operator!= (const Lattice& rkLattice) const;

    // Conversions between n-dim and 1-dim structures.  The coordinate arrays
    // must have the same number of elements as the dimensions of the lattice.
    int GetIndex (const int* aiCoord) const;
    void GetCoordinates (int iIndex, int* aiCoord) const;

    // streaming
    bool Load (FILE* pkIFile);
    bool Save (FILE* pkOFile) const;

    static bool LoadRaw (const char* acFilename, int& riDimensions,
        int*& raiBound, int& riQuantity, int& riRTTI, int& riSizeOf,
        char*& racData);

protected:
    // Construction.  Lattice accepts responsibility for deleting the
    // bound array.
    Lattice (int iDimensions, int* aiBound);
    Lattice (const Lattice& rkLattice);
    Lattice ();

    // For deferred creation of bounds.  Lattice accepts responsibility
    // for deleting the bound array.
    Lattice (int iDimensions);
    void SetBounds (int* aiBound);
    void ComputeQuantityAndOffsets ();

    int m_iDimensions;
    int* m_aiBound;
    int m_iQuantity;
    int* m_aiOffset;

    // streaming
    static const char* ms_acHeader;
};

#include "Wm4Lattice.inl"

}

#endif
