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

#ifndef WM4SKINCONTROLLER_H
#define WM4SKINCONTROLLER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Controller.h"
#include "Wm4Vector3.h"

namespace Wm4
{

class Node;

class WM4_GRAPHICS_ITEM SkinController : public Controller
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.  SkinController accepts responsibility
    // for deleting the input arrays.  The weight and offset arrays must have
    // been allocated using the template function Allocate found in
    // Wm4System.h.
    SkinController (int iVertexQuantity, int iBoneQuantity, Node** apkBones,
        float** aafWeight, Vector3f** aakOffset);
    virtual ~SkinController ();

    // member access
    int GetVertexQuantity () const;
    int GetBoneQuantity () const;
    Node* GetBone (int iBone) const;
    float& Weight (int iVertex, int iBone);
    Vector3f& Offset (int iVertex, int iBone);

    // animation update
    virtual bool Update (double dAppTime);

protected:
    SkinController ();

    int m_iVertexQuantity;    // vq
    int m_iBoneQuantity;      // bq
    Node** m_apkBones;        // bones[bq]
    float** m_aafWeight;      // weight[vq][bq]
    Vector3f** m_aakOffset;   // offset[vq][bq]
};

WM4_REGISTER_STREAM(SkinController);
typedef Pointer<SkinController> SkinControllerPtr;
#include "Wm4SkinController.inl"

}

#endif
