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

#ifndef WM4PDEFILTER3_H
#define WM4PDEFILTER3_H

#include "Wm4ImagicsLIB.h"
#include "Wm4PdeFilter.h"

namespace Wm4
{

class WM4_IMAGICS_ITEM PdeFilter3 : public PdeFilter
{
public:
    // Abstract base class.
    PdeFilter3 (int iXBound, int iYBound, int iZBound, float fXSpacing,
        float fYSpacing, float fZSpacing, const float* afData,
        const bool* abMask, float fBorderValue, ScaleType eScaleType);

    virtual ~PdeFilter3 ();

    // Member access.  The internal 2D images for "data" and "mask" are
    // copies of the inputs to the constructor but padded with a 1-pixel
    // thick border to support filtering on the image boundary.  These images
    // are of size (xbound+2)-by-(ybound+2)-by-(zbound+2).  The inputs (x,y,z)
    // to GetData and GetMask are constrained to 0 <= x < xbound,
    // 0 <= y < ybound, and 0 <= z < zbound.  The correct lookups into the
    // padded arrays are handled internally.
    int GetXBound () const;
    int GetYBound () const;
    int GetZBound () const;
    float GetXSpacing () const;
    float GetYSpacing () const;
    float GetZSpacing () const;

    // Voxel access and derivative estimation.  The lookups into the padded
    // data are handled correctly.  The estimation involves only the
    // 3-by-3-by-3 neighborhood of (x,y,z), where 0 <= x < xbound,
    // 0 <= y < ybound, and 0 <= z < zbound.  [If larger neighborhoods are
    // desired at a later date, the padding and associated code must be
    // adjusted accordingly.]
    float GetU (int iX, int iY, int iZ) const;
    float GetUx (int iX, int iY, int iZ) const;
    float GetUy (int iX, int iY, int iZ) const;
    float GetUz (int iX, int iY, int iZ) const;
    float GetUxx (int iX, int iY, int iZ) const;
    float GetUxy (int iX, int iY, int iZ) const;
    float GetUxz (int iX, int iY, int iZ) const;
    float GetUyy (int iX, int iY, int iZ) const;
    float GetUyz (int iX, int iY, int iZ) const;
    float GetUzz (int iX, int iY, int iZ) const;
    bool GetMask (int iX, int iY, int iZ) const;

protected:
    // Assign values to the 1-voxel image border.
    void AssignDirichletImageBorder ();
    void AssignNeumannImageBorder ();

    // Assign values to the 1-voxel mask border.
    void AssignDirichletMaskBorder ();
    void AssignNeumannMaskBorder ();

    // This function recomputes the boundary values when Neumann conditions
    // are used.  If a derived class overrides this, it must call the
    // base-class OnPreUpdate first.
    virtual void OnPreUpdate ();

    // Iterate over all the pixels and call OnUpdate(x,y,z) for each voxel
    // that is not masked out.
    virtual void OnUpdate ();

    // If a derived class overrides this, it must call the base-class
    // OnPostUpdate last.  The base-class function swaps the buffers for the
    // next pass.
    virtual void OnPostUpdate ();

    // The per-pixel processing depends on the PDE algorithm.  The (x,y,z)
    // must be in padded coordinates: 1 <= x <= xbound, 1 <= y <= ybound, and
    // 1 <= z <= zbound.
    virtual void OnUpdate (int iX, int iY, int iZ) = 0;

    // Copy source data to temporary storage.
    void LookUp7 (int iX, int iY, int iZ);
    void LookUp27 (int iX, int iY, int iZ);

    // Image parameters.
    int m_iXBound, m_iYBound, m_iZBound;
    float m_fXSpacing;       // dx
    float m_fYSpacing;       // dy
    float m_fZSpacing;       // dz
    float m_fInvDx;          // 1/dx
    float m_fInvDy;          // 1/dy
    float m_fInvDz;          // 1/dz
    float m_fHalfInvDx;      // 1/(2*dx)
    float m_fHalfInvDy;      // 1/(2*dy)
    float m_fHalfInvDz;      // 1/(2*dz)
    float m_fInvDxDx;        // 1/(dx*dx)
    float m_fFourthInvDxDy;  // 1/(4*dx*dy)
    float m_fFourthInvDxDz;  // 1/(4*dx*dz)
    float m_fInvDyDy;        // 1/(dy*dy)
    float m_fFourthInvDyDz;  // 1/(4*dy*dz)
    float m_fInvDzDz;        // 1/(dz*dz)

    // Temporary storage for 3x3x3 neighborhood.
    float m_fUmmm, m_fUzmm, m_fUpmm;
    float m_fUmzm, m_fUzzm, m_fUpzm;
    float m_fUmpm, m_fUzpm, m_fUppm;
    float m_fUmmz, m_fUzmz, m_fUpmz;
    float m_fUmzz, m_fUzzz, m_fUpzz;
    float m_fUmpz, m_fUzpz, m_fUppz;
    float m_fUmmp, m_fUzmp, m_fUpmp;
    float m_fUmzp, m_fUzzp, m_fUpzp;
    float m_fUmpp, m_fUzpp, m_fUppp;

    // Successive iterations toggle between two buffers.
    float*** m_aaafSrc;
    float*** m_aaafDst;
    bool*** m_aaabMask;
};

#include "Wm4PdeFilter3.inl"

}

#endif
