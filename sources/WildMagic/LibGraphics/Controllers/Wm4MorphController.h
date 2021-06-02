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

#ifndef WM4MORPHCONTROLLER_H
#define WM4MORPHCONTROLLER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Controller.h"
#include "Wm4FloatArray.h"
#include "Wm4Geometry.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM MorphController : public Controller
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.  The numbers of vertices, morph targets,
    // and the keys are fixed for the lifetime of the object.
    MorphController (int iVertexQuantity, int iTargetQuantity,
        int iKeyQuantity);
    virtual ~MorphController ();

    int GetVertexQuantity () const;
    int GetTargetQuantity () const;
    int GetKeyQuantity () const;

    void SetVertices (int i, Vector3fArray* pkVertices);
    Vector3fArray* GetVertices (int i) const;

    void SetTimes (FloatArray* pkTimes);
    FloatArray* GetTimes () const;
    void SetWeights (int i, FloatArray* pkWeights);
    FloatArray* GetWeights (int i) const;

    // animation update
    virtual bool Update (double dAppTime);

protected:
    // streaming support
    MorphController ();

    // lookup on bounding keys
    void GetKeyInfo (float fCtrlTime, float& rfTime, float& rfOmTime,
        int& ri0, int& ri1);

    // Target geometry.  The number of vertices per target must match the
    // number of vertices in the managed geometry object.  The array of
    // vertices at location 0 are those of one of the targets.  Based on the
    // comments about morph keys, the array at location i >= 1 is computed
    // as the difference between the i-th target and the 0-th target.
    int m_iVertexQuantity;
    int m_iTargetQuantity;
    Vector3fArrayPtr* m_aspkVertices;  // [target_quantity][vertex_quantity]

    // Morph keys.  The morphed object is a combination of N targets by
    // weights w[0] through w[N-1] with w[i] in [0,1] and sum_i w[i] = 1.
    // Each combination is sum_{i=0}^{N-1} w[i]*X[i] where X[i] is a vertex
    // of the i-th target.  This can be rewritten as a combination
    // X[0] + sum_{i=0}^{N-2} w[i] Y[i] where w'[i] = w[i+1] and
    // Y[i] = X[i+1] - X[0].  The weights stored in this class are the
    // w'[i] (to reduce storage).  This also reduces computation time by a
    // small amount (coefficient of X[0] is 1, so no multiplication must
    // occur).
    int m_iKeyQuantity;
    FloatArrayPtr m_spkTimes;  // [key_quantity]
    FloatArrayPtr* m_aspkWeights;  // [key_quantity][target_quantity-1]

    // for O(1) lookup on bounding keys
    int m_iLastIndex;
};

WM4_REGISTER_STREAM(MorphController);
typedef Pointer<MorphController> MorphControllerPtr;
#include "Wm4MorphController.inl"

}

#endif
