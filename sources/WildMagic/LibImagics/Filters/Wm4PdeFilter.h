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

#ifndef WM4PDEFILTER_H
#define WM4PDEFILTER_H

#include "Wm4ImagicsLIB.h"
#include "Wm4System.h"

namespace Wm4
{

class WM4_IMAGICS_ITEM PdeFilter
{
public:
    enum ScaleType
    {
        // The data is processed as is.
        ST_NONE,

        // The data range is d in [min,max].  The scaled values are d'.

        // d' = (d-min)/(max-min) in [0,1]
        ST_UNIT,

        // d' = -1 + 2*(d-min)/(max-min) in [-1,1]
        ST_SYMMETRIC,

        // max > -min:  d' = d/max in [min/max,1]
        // max < -min:  d' = -d/min in [-1,-max/min]
        ST_PRESERVE_ZERO
    };

    // The abstract base class for all PDE-based filters.
    virtual ~PdeFilter ();

    // Member access.
    int GetQuantity () const;
    float GetBorderValue () const;
    ScaleType GetScaleType () const;

    // Access to the time step for the PDE solver.
    void SetTimeStep (float fTimeStep);
    float GetTimeStep () const;

    // This function calls OnPreUpdate, OnUpdate, and OnPostUpdate, in that
    // order.
    void Update ();

protected:
    PdeFilter (int iQuantity, const float* afData, float fBorderValue,
        ScaleType eScaleType);

    // The derived classes for 2D and 3D implement this to recompute the
    // boundary values when Neumann conditions are used.  If derived classes
    // built on top of the 2D or 3D classes implement this also, they must
    // call the base-class OnPreUpdate first.
    virtual void OnPreUpdate () = 0;

    // The derived classes for 2D and 3D implement this to iterate over the
    // image elements, updating an element only if it is not masked out.
    virtual void OnUpdate () = 0;

    // The derived classes for 2D and 3D implement this to swap the buffers
    // for the next pass.  If derived classes built on top of the 2D or 3D
    // classes implement this also, they must call the base-class OnPostUpdate
    // last. 
    virtual void OnPostUpdate () = 0;

    // The number of image elements.
    int m_iQuantity;

    // When set to Mathf::MAX_REAL, Neumann conditions are in use (zero-valued
    // derivatives on the image border).  Dirichlet conditions are used,
    // otherwise (image is constant on the border).
    float m_fBorderValue;

    // This member stores how the image data was transformed during the
    // constructor call.
    ScaleType m_eScaleType;
    float m_fMin, m_fOffset, m_fScale;

    // The time step for the PDE solver.  The stability of an algorithm
    // depends on the magnitude of the time step, but the magnitude itself
    // depends on the algorithm.
    float m_fTimeStep;
};

#include "Wm4PdeFilter.inl"

}

#endif
