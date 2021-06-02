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
// Version: 4.0.7 (2008/12/10)

#include "Wm4Dx9RendererPCH.h"
#include "Wm4Dx9Renderer.h"
#include "Wm4Dx9Resources.h"
#include "Wm4ShaderEffect.h"
using namespace Wm4;

D3DFORMAT Dx9Renderer::ms_aeImageFormat[Image::IT_QUANTITY] =
{
    D3DFMT_A8R8G8B8,      // Image::IT_RGB888
    D3DFMT_A8R8G8B8,      // Image::IT_RGBA8888
    D3DFMT_D16,           // Image::IT_DEPTH16
    D3DFMT_D24X8,         // Image::IT_DEPTH24
    D3DFMT_D32,           // Image::IT_DEPTH32
    D3DFMT_A8R8G8B8,      // Image::IT_CUBE_RGB888
    D3DFMT_A8R8G8B8,      // Image::IT_CUBE_RGBA8888
    D3DFMT_A32B32G32R32F, // Image::IT_RGB32F
    D3DFMT_A32B32G32R32F, // Image::IT_RGBA32F
    D3DFMT_A16B16G16R16F, // Image::IT_RGB16F
    D3DFMT_A16B16G16R16F, // Image::IT_RGBA16F
    D3DFMT_A16B16G16R16,  // Image::IT_RGB16I
    D3DFMT_A16B16G16R16,  // Image::IT_RGBA16I
    D3DFMT_L8,            // Image::IT_INTENSITY8I
    D3DFMT_L16,           // Image::IT_INTENSITY16I
    D3DFMT_R16F,          // Image::IT_INTENSITY16F
    D3DFMT_R32F,          // Image::IT_INTENSITY32F
    D3DFMT_R5G6B5,        // Image::IT_RGB565
    D3DFMT_A1R5G5B5,      // Image::IT_RGBA5551
    D3DFMT_A4R4G4B4       // Image::IT_RGBA4444
};

//----------------------------------------------------------------------------
void Dx9Renderer::OnLoadVProgram (ResourceIdentifier*& rpkID,
    VertexProgram* pkVProgram)
{
    // Compile the vertex program.
    VProgramID* pkResource = WM4_NEW VProgramID;
    rpkID = pkResource;

    const char* acProgramText = pkVProgram->GetProgramText().c_str();
    int iProgramLength = (int)strlen(acProgramText);

    LPD3DXBUFFER pkCompiledShader = 0;
    LPD3DXBUFFER pkErrors = 0;
    ms_hResult = D3DXAssembleShader(acProgramText,iProgramLength,
        0,0,0,&pkCompiledShader,&pkErrors);
    assert(SUCCEEDED(ms_hResult));
    assert(pkCompiledShader);

    // Create the vertex shader and binding information.
    ms_hResult = m_pqDevice->CreateVertexShader( 
        (DWORD*)(pkCompiledShader->GetBufferPointer()),&pkResource->ID);
    assert(SUCCEEDED(ms_hResult));

    // Release buffers, if necessary.
    if (pkCompiledShader)
    {
        pkCompiledShader->Release();
    }
    if (pkErrors)
    {
        pkErrors->Release();
    }
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnReleaseVProgram (ResourceIdentifier* pkID)
{
    VProgramID* pkResource = (VProgramID*)pkID;
    ms_hResult = pkResource->ID->Release();
    assert(SUCCEEDED(ms_hResult));
    WM4_DELETE pkResource;
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnLoadPProgram (ResourceIdentifier*& rpkID,
    PixelProgram* pkPProgram)
{
    // Compile the pixel program.
    PProgramID* pkResource = WM4_NEW PProgramID;
    rpkID = pkResource;

    const char* acProgramText = pkPProgram->GetProgramText().c_str();
    int iProgramLength = (int)strlen(acProgramText);

    LPD3DXBUFFER pkCompiledShader = 0;
    LPD3DXBUFFER pkErrors = 0;
    ms_hResult = D3DXAssembleShader(acProgramText,iProgramLength,
        0,0,0,&pkCompiledShader,&pkErrors);
    assert(SUCCEEDED(ms_hResult));
    assert(pkCompiledShader);

    // Create the pixel shader and binding information.
    ms_hResult = m_pqDevice->CreatePixelShader( 
        (DWORD*)(pkCompiledShader->GetBufferPointer()),&pkResource->ID);
    assert(SUCCEEDED(ms_hResult));

    // Release buffers, if necessary.
    if (pkCompiledShader)
    {
        pkCompiledShader->Release();
    }
    if (pkErrors)
    {
        pkErrors->Release();
    }
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnReleasePProgram (ResourceIdentifier* pkID)
{
    PProgramID* pkResource = (PProgramID*)pkID;
    ms_hResult = pkResource->ID->Release();
    assert(SUCCEEDED(ms_hResult));
    WM4_DELETE pkResource;
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnLoadTexture (ResourceIdentifier*& rpkID,
    Texture* pkTexture)
{
    // The texture is encountered the first time.  Set up the texture unit
    // in hardware that will manage this texture.
    TextureID* pkResource = WM4_NEW TextureID;
    pkResource->TextureObject = pkTexture;
    rpkID = pkResource;

    bool bOffscreen = pkTexture->IsOffscreenTexture();

    // Copy the image data from system memory to video memory.
    const Image* pkImage = pkTexture->GetImage();
    assert(pkImage);
    int iDimension = pkImage->GetDimension();
    bool bIsRegularImage = !pkImage->IsCubeImage();

    // Windows stores BGR (lowest byte to highest byte), but Wild Magic
    // stores RGB.  The byte ordering must be reversed.
    int iQuantity, iByteQuantity = 0;
    unsigned char* aucSrc = pkImage->GetData();
    unsigned char* aucRSrc = 0;
    bool bOwnRSrc = true;
    int i, iSrcBase = 0, iRSrcBase = 0;
    Image::FormatMode eFormat = pkImage->GetFormat();
    D3DFORMAT eD3DFormat = ms_aeImageFormat[eFormat];

    if (aucSrc)
    {
        switch (eFormat)
        {
        case Image::IT_RGB888:
            // Swap R and B and pad to an RGBA image.
            iQuantity = pkImage->GetQuantity();
            iByteQuantity = 4*iQuantity;
            aucRSrc = WM4_NEW unsigned char[iByteQuantity];
            for (i = 0; i < iQuantity; i++, iSrcBase += 3, iRSrcBase += 4)
            {
                aucRSrc[iRSrcBase    ] = aucSrc[iSrcBase + 2];
                aucRSrc[iRSrcBase + 1] = aucSrc[iSrcBase + 1];
                aucRSrc[iRSrcBase + 2] = aucSrc[iSrcBase    ];
                aucRSrc[iRSrcBase + 3] = 255;
            }
            break;

        case Image::IT_RGBA8888:
            iQuantity = pkImage->GetQuantity();
            iByteQuantity = 4*iQuantity;
            aucRSrc = WM4_NEW unsigned char[iByteQuantity];
            for (i = 0; i < iQuantity; i++, iSrcBase += 4, iRSrcBase += 4)
            {
                aucRSrc[iRSrcBase    ] = aucSrc[iSrcBase + 2];
                aucRSrc[iRSrcBase + 1] = aucSrc[iSrcBase + 1];
                aucRSrc[iRSrcBase + 2] = aucSrc[iSrcBase    ];
                aucRSrc[iRSrcBase + 3] = aucSrc[iSrcBase + 3];
            }
            break;

        case Image::IT_CUBE_RGB888:
            // Swap R and B and pad to an RGBA image.
            iQuantity = 6*pkImage->GetQuantity();
            iByteQuantity = 4*iQuantity;
            aucRSrc = WM4_NEW unsigned char[iByteQuantity];
            for (i = 0; i < iQuantity; i++, iSrcBase += 3, iRSrcBase += 4)
            {
                aucRSrc[iRSrcBase    ] = aucSrc[iSrcBase + 2];
                aucRSrc[iRSrcBase + 1] = aucSrc[iSrcBase + 1];
                aucRSrc[iRSrcBase + 2] = aucSrc[iSrcBase    ];
                aucRSrc[iRSrcBase + 3] = 255;
            }
            iByteQuantity = 4*pkImage->GetQuantity();
            break;

        case Image::IT_CUBE_RGBA8888:
            iQuantity = 6*pkImage->GetQuantity();
            iByteQuantity = 4*iQuantity;
            aucRSrc = WM4_NEW unsigned char[iByteQuantity];
            for (i = 0; i < iQuantity; i++, iSrcBase += 4, iRSrcBase += 4)
            {
                aucRSrc[iRSrcBase    ] = aucSrc[iSrcBase + 2];
                aucRSrc[iRSrcBase + 1] = aucSrc[iSrcBase + 1];
                aucRSrc[iRSrcBase + 2] = aucSrc[iSrcBase    ];
                aucRSrc[iRSrcBase + 3] = aucSrc[iSrcBase + 3];
            }
            iByteQuantity = 4*pkImage->GetQuantity();
            break;

        case Image::IT_RGB565:
        {
            // Swap R and B.
            iQuantity = pkImage->GetQuantity();
            iByteQuantity = 2*iQuantity;
            aucRSrc = WM4_NEW unsigned char[iByteQuantity];
            unsigned short* ausSrc = (unsigned short*)aucSrc;
            unsigned short* ausRSrc = (unsigned short*)aucRSrc;
            for (i = 0; i < iQuantity; i++)
            {
                unsigned short value = *ausSrc++;
                unsigned short blue = value & 0x001Fu;
                unsigned short green = value & 0x07E0u;
                unsigned short red = value & 0xF800u;
                value = (red >> 11) | green | (blue << 11);
                *ausRSrc++ = value;
            }
            break;
        }

        case Image::IT_RGBA5551:
        {
            // Swap R and B.
            iQuantity = pkImage->GetQuantity();
            iByteQuantity = 2*iQuantity;
            aucRSrc = WM4_NEW unsigned char[iByteQuantity];
            unsigned short* ausSrc = (unsigned short*)aucSrc;
            unsigned short* ausRSrc = (unsigned short*)aucRSrc;
            for (i = 0; i < iQuantity; i++)
            {
                unsigned short value = *ausSrc++;
                unsigned short blue = value & 0x001Fu;
                unsigned short green = value & 0x03E0u;
                unsigned short red = value & 0x7C00u;
                unsigned short alpha = value & 0x8000u;
                value = (red >> 10) | green | (blue << 10) | alpha;
                *ausRSrc++ = value;
            }
            break;
        }

        case Image::IT_RGBA4444:
        {
            // Swap R and B.
            iQuantity = pkImage->GetQuantity();
            iByteQuantity = 2*iQuantity;
            aucRSrc = WM4_NEW unsigned char[iByteQuantity];
            unsigned short* ausSrc = (unsigned short*)aucSrc;
            unsigned short* ausRSrc = (unsigned short*)aucRSrc;
            for (i = 0; i < iQuantity; i++)
            {
                unsigned short value = *ausSrc++;
                unsigned short blue = value & 0x000Fu;
                unsigned short green = value & 0x00F0u;
                unsigned short red = value & 0x0F00u;
                unsigned short alpha = value & 0xF000u;
                value = (red >> 8) | green | (blue << 8) | alpha;
                *ausRSrc++ = value;
            }
            break;
        }

        default:
            // There is no need to preprocess depth or intensity images.  The
            // floating-point formats and the 16/32-bit integer formats are
            // already RGB/RGBA.
            aucRSrc = aucSrc;
            bOwnRSrc = false;
            iByteQuantity = pkImage->GetBytesPerPixel()*pkImage->GetQuantity();
            break;
        }
    }

    DWORD dwUsage;
    D3DPOOL kPool;
    if (bOffscreen)
    {
        dwUsage = D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP;
        kPool = D3DPOOL_DEFAULT;
    }
    else
    {
        if (iDimension < 3)
        {
            dwUsage = D3DUSAGE_AUTOGENMIPMAP;
        }
        else
        {
            dwUsage = 0;
        }
        kPool = D3DPOOL_MANAGED;
    }

    D3DLOCKED_RECT kLockRect;

    switch (iDimension)
    {
    case 1:
    {
        // DirectX appears not to have support for 1D textures, but the
        // pkImage data works anyway because it is equivalent to an n-by-1
        // 2D image.
        LPDIRECT3DTEXTURE9 pkDXTexture;
        ms_hResult = D3DXCreateTexture(m_pqDevice,pkImage->GetBound(0),1,
            0,dwUsage,eD3DFormat,kPool,&pkDXTexture);
        assert(SUCCEEDED(ms_hResult));

        if (aucRSrc)
        {
            ms_hResult = pkDXTexture->LockRect(0,&kLockRect,0,0);
            assert(SUCCEEDED(ms_hResult));
            memcpy(kLockRect.pBits,aucRSrc,iByteQuantity);
            ms_hResult = pkDXTexture->UnlockRect(0);
            assert(SUCCEEDED(ms_hResult));
        }

        pkResource->ID = pkDXTexture;
        break;
    }
    case 2:
    {
        if (bIsRegularImage)
        {
            LPDIRECT3DTEXTURE9 pkDXTexture;

            ms_hResult = D3DXCreateTexture(m_pqDevice,pkImage->GetBound(0),
                pkImage->GetBound(1),0,dwUsage,eD3DFormat,kPool,&pkDXTexture);
            assert(SUCCEEDED(ms_hResult));

            if (!pkTexture->IsOffscreenTexture() && aucRSrc)
            {
                ms_hResult = pkDXTexture->LockRect(0,&kLockRect,0,0);
                assert(SUCCEEDED(ms_hResult));
                memcpy(kLockRect.pBits,aucRSrc,iByteQuantity);
                ms_hResult = pkDXTexture->UnlockRect(0);
                assert(SUCCEEDED(ms_hResult));
            }

            pkResource->ID = pkDXTexture;
        }
        else
        {
            LPDIRECT3DCUBETEXTURE9 pkDXTexture;
            ms_hResult = D3DXCreateCubeTexture(m_pqDevice,pkImage->GetBound(0),
                0,dwUsage,eD3DFormat,kPool,&pkDXTexture);
            assert(SUCCEEDED(ms_hResult));

            LPDIRECT3DSURFACE9 pkFace;
            unsigned char* aucRFace;

            if (aucRSrc)
            {
                aucRFace = aucRSrc;
                ms_hResult = pkDXTexture->GetCubeMapSurface(
                    D3DCUBEMAP_FACE_POSITIVE_X,0,&pkFace);
                assert(SUCCEEDED(ms_hResult));
                ms_hResult = pkFace->LockRect(&kLockRect,0,0);
                assert(SUCCEEDED(ms_hResult));
                memcpy(kLockRect.pBits,aucRFace,iByteQuantity);
                ms_hResult =
                    pkDXTexture->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X,0);
                assert(SUCCEEDED(ms_hResult));
                pkFace->Release();

                aucRFace = aucRSrc + iByteQuantity;
                ms_hResult = pkDXTexture->GetCubeMapSurface(
                    D3DCUBEMAP_FACE_NEGATIVE_X,0,&pkFace);
                assert(SUCCEEDED(ms_hResult));
                ms_hResult = pkFace->LockRect(&kLockRect,0,0);
                assert(SUCCEEDED(ms_hResult));
                memcpy(kLockRect.pBits,aucRFace,iByteQuantity);
                ms_hResult =
                    pkDXTexture->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_X,0);
                assert(SUCCEEDED(ms_hResult));
                pkFace->Release();

                aucRFace = aucRSrc + 2*iByteQuantity;
                ms_hResult = pkDXTexture->GetCubeMapSurface(
                    D3DCUBEMAP_FACE_POSITIVE_Y,0,&pkFace);
                assert(SUCCEEDED(ms_hResult));
                ms_hResult = pkFace->LockRect(&kLockRect,0,0);
                assert(SUCCEEDED(ms_hResult));
                memcpy(kLockRect.pBits,aucRFace,iByteQuantity);
                ms_hResult =
                    pkDXTexture->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Y,0);
                assert(SUCCEEDED(ms_hResult));
                pkFace->Release();

                aucRFace = aucRSrc + 3*iByteQuantity;
                ms_hResult = pkDXTexture->GetCubeMapSurface(
                    D3DCUBEMAP_FACE_NEGATIVE_Y,0,&pkFace);
                assert(SUCCEEDED(ms_hResult));
                ms_hResult = pkFace->LockRect(&kLockRect,0,0);
                assert(SUCCEEDED(ms_hResult));
                memcpy(kLockRect.pBits,aucRFace,iByteQuantity);
                ms_hResult =
                    pkDXTexture->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Y,0);
                assert(SUCCEEDED(ms_hResult));
                pkFace->Release();

                aucRFace = aucRSrc + 4*iByteQuantity;
                ms_hResult = pkDXTexture->GetCubeMapSurface(
                    D3DCUBEMAP_FACE_POSITIVE_Z,0,&pkFace);
                assert(SUCCEEDED(ms_hResult));
                ms_hResult = pkFace->LockRect(&kLockRect,0,0);
                assert(SUCCEEDED(ms_hResult));
                memcpy(kLockRect.pBits,aucRFace,iByteQuantity);
                ms_hResult =
                    pkDXTexture->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Z,0);
                assert(SUCCEEDED(ms_hResult));
                pkFace->Release();

                aucRFace = aucRSrc + 5*iByteQuantity;
                ms_hResult = pkDXTexture->GetCubeMapSurface(
                    D3DCUBEMAP_FACE_NEGATIVE_Z,0,&pkFace);
                assert(SUCCEEDED(ms_hResult));
                ms_hResult = pkFace->LockRect(&kLockRect,0,0);
                assert(SUCCEEDED(ms_hResult));
                memcpy(kLockRect.pBits,aucRFace,iByteQuantity);
                ms_hResult =
                    pkDXTexture->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Z,0);
                assert(SUCCEEDED(ms_hResult));
                pkFace->Release();
            }

            pkResource->ID = pkDXTexture;
        }
        break;
    }
    case 3:
    {
        LPDIRECT3DVOLUMETEXTURE9 pkDXTexture;
        ms_hResult = D3DXCreateVolumeTexture(m_pqDevice,pkImage->GetBound(0),
            pkImage->GetBound(1),pkImage->GetBound(2),0,dwUsage,eD3DFormat,
            kPool,&pkDXTexture);
        assert(SUCCEEDED(ms_hResult));

        if (aucRSrc)
        {
            D3DLOCKED_BOX kLockBox;
            ms_hResult = pkDXTexture->LockBox(0,&kLockBox,0,0);
            assert(SUCCEEDED(ms_hResult));
            memcpy(kLockBox.pBits,aucRSrc,iByteQuantity);
            ms_hResult = pkDXTexture->UnlockBox(0);
            assert(SUCCEEDED(ms_hResult));
        }

        pkResource->ID = pkDXTexture;
        break;
    }
    default:
        assert(false);
        break;
    }

    if (bOwnRSrc)
    {
        WM4_DELETE[] aucRSrc;
    }
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnReleaseTexture (ResourceIdentifier* pkID)
{
    TextureID* pkResource = (TextureID*)pkID;
    ms_hResult = pkResource->ID->Release();
    assert(SUCCEEDED(ms_hResult));
    WM4_DELETE pkResource;
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnLoadVBuffer (ResourceIdentifier*& rpkID,
    const Attributes& rkIAttr, const Attributes& rkOAttr,
    VertexBuffer* pkVBuffer)
{
    VBufferID* pkResource = WM4_NEW VBufferID;
    rpkID = pkResource;
    pkResource->IAttr = rkIAttr;
    pkResource->OAttr = rkOAttr;

    std::vector<D3DVERTEXELEMENT9> kElements;
    D3DVERTEXELEMENT9 kElement;
    kElement.Stream = 0;
    kElement.Method = D3DDECLMETHOD_DEFAULT;

    int iVertexSize = 0, iChannels;

    if (rkIAttr.HasPosition())
    {
        iChannels = rkIAttr.GetPositionChannels();
        kElement.Offset = (WORD)iVertexSize;
        iVertexSize += iChannels*sizeof(float);
        kElement.Type = (BYTE)(D3DDECLTYPE_FLOAT1 + iChannels - 1);
        kElement.Usage = D3DDECLUSAGE_POSITION;
        kElement.UsageIndex = 0;
        kElements.push_back(kElement);
    }

    if (rkIAttr.HasBlendWeight())
    {
        iChannels = rkIAttr.GetBlendWeightChannels();
        kElement.Offset = (WORD)iVertexSize;
        iVertexSize += iChannels*sizeof(float);
        kElement.Type = (BYTE)(D3DDECLTYPE_FLOAT1 + iChannels - 1);
        kElement.Usage = D3DDECLUSAGE_BLENDWEIGHT;
        kElement.UsageIndex = 0;
        kElements.push_back(kElement);
    }

    if (rkIAttr.HasNormal())
    {
        iChannels = rkIAttr.GetNormalChannels();
        kElement.Offset = (WORD)iVertexSize;
        iVertexSize += iChannels*sizeof(float);
        kElement.Type = (BYTE)(D3DDECLTYPE_FLOAT1 + iChannels - 1);
        kElement.Usage = D3DDECLUSAGE_NORMAL;
        kElement.UsageIndex = 0;
        kElements.push_back(kElement);
    }

    int iUnit;
    for (iUnit = 0; iUnit < rkIAttr.GetMaxColors(); iUnit++)
    {
        if (rkIAttr.HasColor(iUnit))
        {
            kElement.Offset = (WORD)iVertexSize;
            iVertexSize += sizeof(DWORD);
            kElement.Type = D3DDECLTYPE_D3DCOLOR;
            kElement.Usage = D3DDECLUSAGE_COLOR;
            kElement.UsageIndex = (BYTE)iUnit;
            kElements.push_back(kElement);
        }
    }

    if (rkIAttr.HasFog())
    {
        kElement.Offset = (WORD)iVertexSize;
        iVertexSize += sizeof(float);
        kElement.Type = D3DDECLTYPE_FLOAT1;
        kElement.Usage = D3DDECLUSAGE_FOG;
        kElement.UsageIndex = 0;
        kElements.push_back(kElement);
    }

    if (rkIAttr.HasPSize())
    {
        kElement.Offset = (WORD)iVertexSize;
        iVertexSize += sizeof(float);
        kElement.Type = D3DDECLTYPE_FLOAT1;
        kElement.Usage = D3DDECLUSAGE_PSIZE;
        kElement.UsageIndex = 0;
        kElements.push_back(kElement);
    }

    if (rkIAttr.HasBlendIndices())
    {
        iChannels = rkIAttr.GetBlendIndicesChannels();
        kElement.Offset = (WORD)iVertexSize;
        iVertexSize += iChannels*sizeof(float);
        kElement.Type = (BYTE)(D3DDECLTYPE_FLOAT1 + iChannels - 1);
        kElement.Usage = D3DDECLUSAGE_BLENDINDICES;
        kElement.UsageIndex = 0;
        kElements.push_back(kElement);
    }

    for (iUnit = 0; iUnit < rkIAttr.GetMaxTCoords(); iUnit++)
    {
        if (rkIAttr.HasTCoord(iUnit))
        {
            iChannels = rkIAttr.GetTCoordChannels(iUnit);
            kElement.Offset = (WORD)iVertexSize;
            iVertexSize += iChannels*sizeof(float);
            kElement.Type = (BYTE)(D3DDECLTYPE_FLOAT1 + iChannels - 1);
            kElement.Usage = D3DDECLUSAGE_TEXCOORD;
            kElement.UsageIndex = (BYTE)iUnit;
            kElements.push_back(kElement);
        }
    }

    if (rkIAttr.HasTangent())
    {
        iChannels = rkIAttr.GetTangentChannels();
        kElement.Offset = (WORD)iVertexSize;
        iVertexSize += iChannels*sizeof(float);
        kElement.Type = (BYTE)(D3DDECLTYPE_FLOAT1 + iChannels - 1);
        kElement.Usage = D3DDECLUSAGE_TANGENT;
        kElement.UsageIndex = 0;
        kElements.push_back(kElement);
    }

    if (rkIAttr.HasBitangent())
    {
        iChannels = rkIAttr.GetBitangentChannels();
        kElement.Offset = (WORD)iVertexSize;
        iVertexSize += iChannels*sizeof(float);
        kElement.Type = (BYTE)(D3DDECLTYPE_FLOAT1 + iChannels - 1);
        kElement.Usage = D3DDECLUSAGE_BINORMAL;
        kElement.UsageIndex = 0;
        kElements.push_back(kElement);
    }

    D3DVERTEXELEMENT9 kSentinel = D3DDECL_END();
    kElements.push_back(kSentinel);

    ms_hResult = m_pqDevice->CreateVertexDeclaration(&kElements[0],
        &pkResource->Declaration);
    assert(SUCCEEDED(ms_hResult));

    // Copy the vertex buffer data to an array.  NOTE:  The output iChannels
    // will be smaller than vertexSize*vertexQuantity for pkVBuffer whenever
    // the vertex buffer attributes have colors.  This is because a ColorRGBA
    // value is 4 floats but is packed into 1 float.
    float* afCompatible = 0;  // allocated by BuildCompatibleArray
    pkVBuffer->BuildCompatibleArray(rkIAttr,true,iChannels,afCompatible);

    // Create the vertex buffer.
    unsigned int uiVBSize = (unsigned int)(iChannels*sizeof(float));
    LPDIRECT3DVERTEXBUFFER9 pqVBuffer;
    ms_hResult = m_pqDevice->CreateVertexBuffer(uiVBSize,0,0,D3DPOOL_MANAGED,
        &pqVBuffer,0);
    assert(SUCCEEDED(ms_hResult));

    // Copy the vertex buffer data from system memory to video memory.
    float* afVBData;
    ms_hResult = pqVBuffer->Lock(0,uiVBSize,(void**)(&afVBData),0);
    assert(SUCCEEDED(ms_hResult));
    System::Memcpy(afVBData,uiVBSize,afCompatible,uiVBSize);
    ms_hResult = pqVBuffer->Unlock();
    assert(SUCCEEDED(ms_hResult));

    WM4_DELETE[] afCompatible;

    // Generate the binding information and save it.
    pkResource->ID = pqVBuffer;
    pkResource->VertexSize = iVertexSize;
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnReleaseVBuffer (ResourceIdentifier* pkID)
{
    VBufferID* pkResource = (VBufferID*)pkID;
    pkResource->ID->Release();
    pkResource->Declaration->Release();
    WM4_DELETE pkResource;
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnLoadIBuffer (ResourceIdentifier*& rpkID,
    IndexBuffer* pkIBuffer)
{
    // The index buffer is encountered the first time.
    IBufferID* pkResource = WM4_NEW IBufferID;
    rpkID = pkResource;

    int iIQuantity = pkIBuffer->GetIndexQuantity();
    int* aiIndex = pkIBuffer->GetData();
    unsigned int uiIBSize = (unsigned int)(iIQuantity*sizeof(int));

    // Create the index buffer.
    LPDIRECT3DINDEXBUFFER9 pqIBuffer;
    ms_hResult = m_pqDevice->CreateIndexBuffer(uiIBSize,D3DUSAGE_WRITEONLY,
        D3DFMT_INDEX32,D3DPOOL_MANAGED,&pqIBuffer,0);
    assert(SUCCEEDED(ms_hResult));

    // Copy the index buffer data from system memory to video memory.
    char* pcIndices;
    ms_hResult = pqIBuffer->Lock(0,uiIBSize,(void**)(&pcIndices),0);
    assert(SUCCEEDED(ms_hResult));

    System::Memcpy(pcIndices,uiIBSize,aiIndex,iIQuantity*sizeof(int));

    ms_hResult = pqIBuffer->Unlock();
    assert(SUCCEEDED(ms_hResult));

    // Generate the binding information and save it.
    pkResource->ID = pqIBuffer;
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnReleaseIBuffer (ResourceIdentifier* pkID)
{
    IBufferID* pkResource = (IBufferID*)pkID;
    pkResource->ID->Release();
    WM4_DELETE pkResource;
}
//----------------------------------------------------------------------------
