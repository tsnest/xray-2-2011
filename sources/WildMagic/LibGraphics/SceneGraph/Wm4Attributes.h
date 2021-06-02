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
// Version: 4.0.1 (2007/06/16)

#ifndef WM4ATTRIBUTES_H
#define WM4ATTRIBUTES_H

#include "Wm4GraphicsLIB.h"
#include "Wm4System.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Attributes
{
public:
    Attributes ();
    ~Attributes ();

    // Set the attributes you want.  The currently supported attributes are
    //    position:               1 to 4 channels
    //    blendweight:            1 to 4 channels
    //    normal vector:          1 to 4 channels
    //    color[i], 0 <= i < M:   1 to 4 channels (M = 2 for now)
    //    fog:                    1 channel
    //    psize:                  1 channel
    //    blendindices:           1 to 4 channels
    //    tcoord[i], 0 <= i < M:  1 to 4 channels (M depends on shader model)
    //    tangent vector:         1 to 4 channels
    //    bitangent vector:       1 to 4 channels
    //
    // The attrbitues are organized internally according to the
    // aforementioned list.

    void SetPositionChannels (int iPositionChannels);
    void SetBlendWeightChannels (int iBlendWeightChannels);
    void SetNormalChannels (int iNormalChannels);
    void SetColorChannels (int iUnit, int iColorChannels);
    void SetFogChannels (int iFogChannels);
    void SetPSizeChannels (int iPSizeChannels);
    void SetBlendIndicesChannels (int iBlendIndicesChannels);
    void SetTCoordChannels (int iUnit, int iTCoordChannels);
    void SetTangentChannels (int iTangentChannels);
    void SetBitangentChannels (int iBitangentChannels);

    // The number of 'float' channels used by all the attributes.
    int GetChannelQuantity () const;

    // Access to position information.
    int GetPositionOffset () const;
    int GetPositionChannels () const;
    bool HasPosition () const;

    // Access to blend-weight information.
    int GetBlendWeightOffset () const;
    int GetBlendWeightChannels () const;
    bool HasBlendWeight () const;

    // Access to normal information.
    int GetNormalOffset () const;
    int GetNormalChannels () const;
    bool HasNormal () const;

    // Access to color information.
    int GetMaxColors () const;
    int GetColorOffset (int iUnit) const;
    int GetColorChannels (int iUnit) const;
    bool HasColor (int iUnit) const;

    // Access to fog information.
    int GetFogOffset () const;
    int GetFogChannels () const;
    bool HasFog () const;

    // Access to psize information.
    int GetPSizeOffset () const;
    int GetPSizeChannels () const;
    bool HasPSize () const;

    // Access to blend-indices information.
    int GetBlendIndicesOffset () const;
    int GetBlendIndicesChannels () const;
    bool HasBlendIndices () const;

    // Access to texture coordinate information.
    int GetMaxTCoords () const;
    int GetTCoordOffset (int iUnit) const;
    int GetTCoordChannels (int iUnit) const;
    bool HasTCoord (int iUnit) const;

    // Access to tangent information.
    int GetTangentOffset () const;
    int GetTangentChannels () const;
    bool HasTangent () const;

    // Access to bitangent information.
    int GetBitangentOffset () const;
    int GetBitangentChannels () const;
    bool HasBitangent () const;

    // Support for comparing vertex program outputs with pixel program inputs.
    bool Matches (const Attributes& rkAttr, bool bIncludePosition,
        bool bIncludeBlendWeight, bool bIncludeNormal, bool bIncludeColor,
        bool bIncludeFog, bool bIncludePSize, bool bIncludeBlendIndices,
        bool bIncludeTCoord, bool bIncludeTangent, bool bIncludeBitangent)
        const;

    // Support for comparing vertex buffer attributes with vertex program
    // input attributes.
    bool operator== (const Attributes& rkAttr) const;
    bool operator!= (const Attributes& rkAttr) const;

private:
    void UpdateOffsets ();

    // The number of 'float' channels used by all attributes.
    int m_iChannelQuantity;

    // positions
    int m_iPositionOffset;
    int m_iPositionChannels;

    // blend weights
    int m_iBlendWeightOffset;
    int m_iBlendWeightChannels;

    // normals
    int m_iNormalOffset;
    int m_iNormalChannels;

    // colors
    std::vector<int> m_kColorOffset;
    std::vector<int> m_kColorChannels;

    // fog
    int m_iFogOffset;
    int m_iFogChannels;

    // psize
    int m_iPSizeOffset;
    int m_iPSizeChannels;

    // blend indices
    int m_iBlendIndicesOffset;
    int m_iBlendIndicesChannels;

    // texture coordinates
    std::vector<int> m_kTCoordOffset;
    std::vector<int> m_kTCoordChannels;

    // tangents
    int m_iTangentOffset;
    int m_iTangentChannels;

    // bitangents
    int m_iBitangentOffset;
    int m_iBitangentChannels;
};

#include "Wm4Attributes.inl"

}

#endif
