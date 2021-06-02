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

#ifndef WM4STRINGTREE_H
#define WM4STRINGTREE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Line3.h"
#include "Wm4Matrix2.h"
#include "Wm4Matrix3.h"
#include "Wm4Matrix4.h"
#include "Wm4Plane3.h"
#include "Wm4Quaternion.h"

namespace Wm4
{

class BoundingVolume;
class ColorRGB;
class ColorRGBA;
class Rtti;

class WM4_GRAPHICS_ITEM StringTree
{
public:
    // construction and destruction
    StringTree ();
    ~StringTree ();

    // strings
    int GetStringQuantity () const;
    void SetString (int i, char* acString);
    char* GetString (int i);
    void Append (char* acString);

    // children
    int GetChildQuantity () const;
    void SetChild (int i, StringTree* pkChild);
    StringTree* GetChild (int i);
    void Append (StringTree* pkChild);

    // streaming
    bool Save (const char* acFilename, int iTabSize = 4);

    static void FormatFloat (float fValue, size_t uiStringSize,
        char* acString);
    static void FormatDouble (double dValue, size_t uiStringSize,
        char* acString);

private:
    // streaming (recursive)
    void Save (FILE* pkOFile, int iLevel, int iTabSize);

    std::vector<char*> m_kStrings;
    std::vector<StringTree*> m_kChildren;
};

// string creation helpers (native types)
WM4_GRAPHICS_ITEM char* Format (const Rtti* pkRTTI, const char* acName);
WM4_GRAPHICS_ITEM char* Format (const char* acString);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, bool);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, char);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, unsigned char);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, short);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, unsigned short);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, int);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, unsigned int);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, long);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, unsigned long);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, float);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, double);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, void*);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const char*);

// string creation helpers (nonnative types)
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const BoundingVolume*);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const ColorRGBA&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const ColorRGB&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Line3f&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Matrix2f&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Matrix3f&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Matrix4f&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Plane3f&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Quaternionf&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Vector2f&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Vector3f&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Vector4f&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Line3d&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Matrix2d&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Matrix3d&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Matrix4d&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Plane3d&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Quaterniond&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Vector2d&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Vector3d&);
WM4_GRAPHICS_ITEM char* Format (const char* acPrefix, const Vector4d&);

template <class T>
StringTree* Format (const char* acTitle, int iQuantity, const T* atValue);

#include "Wm4StringTree.inl"

}

#endif
