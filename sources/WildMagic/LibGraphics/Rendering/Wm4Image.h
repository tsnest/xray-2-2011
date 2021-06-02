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
// Version: 4.0.4 (2008/12/10)

#ifndef WM4IMAGE_H
#define WM4IMAGE_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"
#include "Wm4ColorRGBA.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Image : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // NOTE:  Image dimensions must be a power of two only for the software
    // renderer (to be allowed, though in WM5).

    enum FormatMode
    {
        IT_RGB888,
        IT_RGBA8888,
        IT_DEPTH16,
        IT_DEPTH24,
        IT_DEPTH32,
        IT_CUBE_RGB888,
        IT_CUBE_RGBA8888,
        IT_RGB32F,
        IT_RGBA32F,
        IT_RGB16F,
        IT_RGBA16F,
        IT_RGB16I,
        IT_RGBA16I,
        IT_INTENSITY8I,
        IT_INTENSITY16I,
        IT_INTENSITY16F,
        IT_INTENSITY32F,
        IT_RGB565,   // (high)   B G R (low)
        IT_RGBA5551, // (high) A B G R (low)
        IT_RGBA4444, // (high) A B G R (low)
        IT_QUANTITY
    };

    // Construction and destruction.  Image accepts responsibility for
    // deleting the input array.  The acImageName field is required and
    // is used as a unique identifier for the image for purposes of sharing.

    // 1D image
    Image (FormatMode eFormat, int iBound0, unsigned char* aucData,
        const char* acImageName);

    // 2D image
    Image (FormatMode eFormat, int iBound0, int iBound1,
        unsigned char* aucData, const char* acImageName);

    // 3D image
    Image (FormatMode eFormat, int iBound0, int iBound1, int iBound2,
        unsigned char* aucData, const char* acImageName);

    virtual ~Image ();

    // Member access.
    FormatMode GetFormat () const;
    const std::string& GetFormatName () const;
    static std::string GetFormatName (int eFormat);
    bool IsDepthImage () const;
    bool IsCubeImage () const;
    int GetBytesPerPixel () const;
    static int GetBytesPerPixel (int eFormat);
    int GetDimension () const;
    int GetBound (int i) const;
    int GetQuantity () const;
    unsigned char* GetData () const;
    unsigned char* operator() (int i);

    // Deferred allocation of image data.
    void AllocateData ();

    // Streaming support.  The sharing system is automatically invoked by
    // these calls.  In Load, if an image corresponding to the filename is
    // already in memory, then that image is returned (i.e. shared).
    // Otherwise, a new image is created and returned.  The filename is used
    // as the image name.
    static Image* Load (const std::string &rkImageName,
        bool bAbsolutePath = false);

    // This is intended to support saving procedurally generated images or
    // for utilities that convert to WMIF from another format.  The filename
    // in this call does not replace the image name that might already exist.
    bool Save (const char* acFilename);

    // NOTE:  These are used by the software renderer.  They do not handle
    // all image formats.

    // Create an image of ColorRGBA values.  The function returns an image
    // of the same width and height for these formats.  The returned image
    // is dynamically allocated; the caller is responsible for deleting it.
    ColorRGBA* CreateRGBA () const;

    // Copy to an already existing image of ColorRGBA values.  The input
    // array must have the correct dimensions as the Image itself.
    void CopyRGBA (ColorRGBA* akCImage) const;

protected:
    // support for streaming
    Image ();

    FormatMode m_eFormat;
    int m_iDimension;
    int m_aiBound[3];
    int m_iQuantity;
    unsigned char* m_aucData;

    static int ms_aiBytesPerPixel[IT_QUANTITY];
    static std::string ms_akFormatName[IT_QUANTITY];
};

WM4_REGISTER_STREAM(Image);
typedef Pointer<Image> ImagePtr;
#include "Wm4Image.inl"

}

#endif
