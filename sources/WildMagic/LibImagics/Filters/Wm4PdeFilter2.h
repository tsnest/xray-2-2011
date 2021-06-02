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

#ifndef WM4PDEFILTER2_H
#define WM4PDEFILTER2_H

#include "Wm4ImagicsLIB.h"
#include "Wm4PdeFilter.h"

namespace Wm4
{

class WM4_IMAGICS_ITEM PdeFilter2 : public PdeFilter
{
public:
    // Abstract base class.
    PdeFilter2 (int iXBound, int iYBound, float fXSpacing, float fYSpacing,
        const float* afData, const bool* abMask, float fBorderValue,
        ScaleType eScaleType);

    virtual ~PdeFilter2 ();

    // Member access.  The internal 2D images for "data" and "mask" are
    // copies of the inputs to the constructor but padded with a 1-pixel
    // thick border to support filtering on the image boundary.  These images
    // are of size (xbound+2)-by-(ybound+2).  The inputs (x,y) to GetData and
    // GetMask are constrained to 0 <= x < xbound and 0 <= y < ybound.  The
    // correct lookups into the padded arrays are handled internally.
    int GetXBound () const;
    int GetYBound () const;
    float GetXSpacing () const;
    float GetYSpacing () const;

    // Pixel access and derivative estimation.  The lookups into the padded
    // data are handled correctly.  The estimation involves only the 3-by-3
    // neighborhood of (x,y), where 0 <= x < xbound and 0 <= y < ybound.
    // [If larger neighborhoods are desired at a later date, the padding and
    // associated code must be adjusted accordingly.]
    float GetU (int iX, int iY) const;
    float GetUx (int iX, int iY) const;
    float GetUy (int iX, int iY) const;
    float GetUxx (int iX, int iY) const;
    float GetUxy (int iX, int iY) const;
    float GetUyy (int iX, int iY) const;
    bool GetMask (int iX, int iY) const;

protected:
    // Assign values to the 1-pixel image border.
    void AssignDirichletImageBorder ();
    void AssignNeumannImageBorder ();

    // Assign values to the 1-pixel mask border.
    void AssignDirichletMaskBorder ();
    void AssignNeumannMaskBorder ();

    // This function recomputes the boundary values when Neumann conditions
    // are used.  If a derived class overrides this, it must call the
    // base-class OnPreUpdate first.
    virtual void OnPreUpdate ();

    // Iterate over all the pixels and call OnUpdate(x,y) for each pixel that
    // is not masked out.
    virtual void OnUpdate ();

    // If a derived class overrides this, it must call the base-class
    // OnPostUpdate last.  The base-class function swaps the buffers for the
    // next pass.
    virtual void OnPostUpdate ();

    // The per-pixel processing depends on the PDE algorithm.  The (x,y) must
    // be in padded coordinates: 1 <= x <= xbound and 1 <= y <= ybound.
    virtual void OnUpdate (int iX, int iY) = 0;

    // Copy source data to temporary storage.
    void LookUp5 (int iX, int iY);
    void LookUp9 (int iX, int iY);

    // Image parameters.
    int m_iXBound, m_iYBound;
    float m_fXSpacing;       // dx
    float m_fYSpacing;       // dy
    float m_fInvDx;          // 1/dx
    float m_fInvDy;          // 1/dy
    float m_fHalfInvDx;      // 1/(2*dx)
    float m_fHalfInvDy;      // 1/(2*dy)
    float m_fInvDxDx;        // 1/(dx*dx)
    float m_fFourthInvDxDy;  // 1/(4*dx*dy)
    float m_fInvDyDy;        // 1/(dy*dy)

    // Temporary storage for 3x3 neighborhood.  In the notation m_fUxy, the
    // x and y indices are in {m,z,p}, referring to subtract 1 (m), no change
    // (z), or add 1 (p) to the appropriate index.
    float m_fUmm, m_fUzm, m_fUpm;
    float m_fUmz, m_fUzz, m_fUpz;
    float m_fUmp, m_fUzp, m_fUpp;

    // Successive iterations toggle between two buffers.
    float** m_aafSrc;
    float** m_aafDst;
    bool** m_aabMask;
};

#include "Wm4PdeFilter2.inl"

}

#endif
