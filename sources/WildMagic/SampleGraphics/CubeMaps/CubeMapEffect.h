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
// Version: 4.0.2 (2008/02/18)

#ifndef CUBEMAPEFFECT_H
#define CUBEMAPEFFECT_H

#include "Wm4GraphicsLIB.h"
#include "Wm4ShaderEffect.h"
#include "Wm4Culler.h"

namespace Wm4
{

class Culler;

class CubeMapEffect : public ShaderEffect
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM_WITH_POSTLINK;

public:
    // Construction and destruction.  The caller is responsible for deleting
    // the input images if they were dynamically allocated.  The images must
    // be 2D, must be square with a common bound, and must have format
    // Image::IT_RGB888.
    CubeMapEffect (Image* pkXpImage, Image* pkXmImage, Image* pkYpImage,
        Image* pkYmImage, Image* pkZpImage, Image* pkZmImage,
        float fReflectivity, bool bAllowDynamicUpdates);

    virtual ~CubeMapEffect ();

    virtual void Draw (Renderer* pkRenderer, Spatial* pkGlobalObject,
        int iVisibleQuantity, VisibleObject* akVisible);

    void SetReflectivity (float fReflectivity);
    float GetReflectivity () const;

    // For dynamic updating of the cube map.  This function computes the new
    // faces only when bAllowDynamicUpdates was set to 'true' in the
    // constructor.
    bool CanDynamicallyUpdate () const;
    void UpdateFaces (Renderer* pkRenderer, Spatial* pkScene,
        const Vector3f& rkEnvOrigin, const Vector3f& rkEnvDVector,
        const Vector3f& rkEnvUVector, const Vector3f& rkEnvRVector);

protected:
    CubeMapEffect ();

    // For dynamic updates.
    void UpdateFace (int iFace, const Image* pkImage);

    // Four of the faces need a reflection of the image to correct for the
    // handedness of coordinates.
    static void ReflectXY (int iXSize, int iYSize, int iBytesPerPixel,
        unsigned char* aucData);

    // The reflectivity is stored at index 0.  The other values are unused.
    float m_afReflectivity[4];

    // Storage for the six cube face images.
    ImagePtr m_spkHexaImage;

    // For dynamic updates after construction of the effect.
    CameraPtr m_spkCamera;
    Culler* m_pkCuller;
    ImagePtr m_spkCubeFace;
    Texture* m_pkCubeFaceTexture;
    FrameBuffer* m_pkCubeFaceBuffer;
};

WM4_REGISTER_STREAM(CubeMapEffect);
typedef Pointer<CubeMapEffect> CubeMapEffectPtr;
#include "CubeMapEffect.inl"

}

#endif
