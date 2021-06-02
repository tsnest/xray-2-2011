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
// Version: 4.0.0 (2006/09/21)

#ifndef WM4FASTMARCH_H
#define WM4FASTMARCH_H

#include "Wm4ImagicsLIB.h"
#include "Wm4Math.h"

namespace Wm4
{

class WM4_IMAGICS_ITEM FastMarch
{
public:
    // Abstract base class.
    virtual ~FastMarch ();

    // Member access.
    void SetTime (int i, float fTime);
    float GetTime (int i) const;
    int GetQuantity () const;
    void GetTimeExtremes (float& rfMin, float& rfMax) const;

    // Image element classification.
    bool IsValid (int i) const;
    bool IsTrial (int i) const;
    bool IsFar (int i) const;
    bool IsZeroSpeed (int i) const;
    bool IsInterior (int i) const;
    void GetInterior (std::vector<int>& rkInterior) const;
    virtual void GetBoundary (std::vector<int>& rkBoundary) const = 0;
    virtual bool IsBoundary (int i) const = 0;

    // Run one step of the fast marching algorithm.
    virtual void Iterate () = 0;

protected:
    // Constructors.
    FastMarch (int iQuantity, const float* afSpeed,
        const std::vector<int>& rkSeeds);
    FastMarch (int iQuantity, const float fSpeed,
        const std::vector<int>& rkSeeds);

    // Called by the constructors.
    void Initialize (int iQuantity, const std::vector<int>& rkSeeds);
    void InitializeSpeed (const float* afSpeed);
    void InitializeSpeed (const float fSpeed);

    int m_iQuantity;
    float* m_afInvSpeed;
    float* m_afTime;
    TMinHeap<int,float> m_kHeap;
    const TMinHeapRecord<int,float>** m_apkTrial;
};

#include "Wm4FastMarch.inl"

}

#endif
