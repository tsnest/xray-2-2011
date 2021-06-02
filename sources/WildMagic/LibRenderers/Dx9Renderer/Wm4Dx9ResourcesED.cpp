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
// Version: 4.0.6 (2008/01/05)

#include "Wm4Dx9RendererPCH.h"
#include "Wm4Dx9Renderer.h"
#include "Wm4Dx9Resources.h"
#include "Wm4ShaderEffect.h"
using namespace Wm4;

DWORD Dx9Renderer::ms_adwTexMinFilter[Texture::MAX_FILTER_TYPES] =
{
    D3DTEXF_POINT,  // Texture::MM_NEAREST
    D3DTEXF_LINEAR, // Texture::MM_LINEAR
    D3DTEXF_POINT,  // Texture::MM_NEAREST_NEAREST
    D3DTEXF_POINT,  // Texture::MM_NEAREST_LINEAR
    D3DTEXF_LINEAR, // Texture::MM_LINEAR_NEAREST
    D3DTEXF_LINEAR, // Texture::MM_LINEAR_LINEAR
};

DWORD Dx9Renderer::ms_adwTexMipFilter[Texture::MAX_FILTER_TYPES] =
{
    D3DTEXF_NONE,   // Texture::MM_NEAREST
    D3DTEXF_NONE,   // Texture::MM_LINEAR
    D3DTEXF_POINT,  // Texture::MM_NEAREST_NEAREST
    D3DTEXF_LINEAR, // Texture::MM_NEAREST_LINEAR
    D3DTEXF_POINT,  // Texture::MM_LINEAR_NEAREST
    D3DTEXF_LINEAR, // Texture::MM_LINEAR_LINEAR
};

DWORD Dx9Renderer::ms_adwTexWrapMode[Texture::MAX_WRAP_TYPES] =
{
    D3DTADDRESS_CLAMP,      // Texture::CLAMP
    D3DTADDRESS_WRAP,       // Texture::REPEAT
    D3DTADDRESS_MIRROR,     // Texture::MIRRORED_REPEAT
    D3DTADDRESS_BORDER,     // Texture::CLAMP_BORDER
    D3DTADDRESS_CLAMP,      // Texture::CLAMP_EDGE
};

//----------------------------------------------------------------------------
void Dx9Renderer::SetVProgramConstant (int, int iBaseRegister,
    int iRegisterQuantity, float* afData)
{
    ms_hResult = m_pqDevice->SetVertexShaderConstantF(iBaseRegister,afData,
        iRegisterQuantity);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::SetPProgramConstant (int, int iBaseRegister,
    int iRegisterQuantity, float* afData)
{
    ms_hResult = m_pqDevice->SetPixelShaderConstantF(iBaseRegister,afData,
        iRegisterQuantity);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnEnableVProgram (ResourceIdentifier* pkID)
{
    VProgramID* pkResource = (VProgramID*)pkID;
    ms_hResult = m_pqDevice->SetVertexShader(pkResource->ID);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnDisableVProgram (ResourceIdentifier*)
{
    ms_hResult = m_pqDevice->SetVertexShader(0);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnEnablePProgram (ResourceIdentifier* pkID)
{
    PProgramID* pkResource = (PProgramID*)pkID;
    ms_hResult = m_pqDevice->SetPixelShader(pkResource->ID);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnDisablePProgram (ResourceIdentifier*)
{
    ms_hResult = m_pqDevice->SetPixelShader(0);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnEnableTexture (ResourceIdentifier* pkID)
{
    TextureID* pkResource = (TextureID*)pkID;
    Texture* pkTexture = pkResource->TextureObject;

    SamplerInformation* pkSI = m_apkActiveSamplers[m_iCurrentSampler];
    SamplerInformation::Type eSType = pkSI->GetType();
    int iTextureUnit = pkSI->GetTextureUnit();

    // Anisotropic filtering value.
    float fAnisotropy = pkTexture->GetAnisotropyValue();
    if (1.0f < fAnisotropy && fAnisotropy <= ms_fMaxAnisotropy)
    {
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_MAXANISOTROPY,(DWORD)fAnisotropy);
        assert(SUCCEEDED(ms_hResult));
    }
    else
    {
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_MAXANISOTROPY,(DWORD)1.0f);
        assert(SUCCEEDED(ms_hResult));
    }

    // Set the filter mode.
    Texture::FilterType eFType = pkTexture->GetFilterType();
    if (eFType == Texture::NEAREST)
    {
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_MAGFILTER,D3DTEXF_POINT);
        assert(SUCCEEDED(ms_hResult));
    }
    else
    {
        if (1.0f < fAnisotropy && fAnisotropy <= ms_fMaxAnisotropy)
        {
            ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
                D3DSAMP_MAGFILTER,D3DTEXF_ANISOTROPIC);
            assert(SUCCEEDED(ms_hResult));
        }
        else
        {
            ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
                D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
            assert(SUCCEEDED(ms_hResult));
        }
    }

    // Set the mipmap mode.
    if (1.0f < fAnisotropy && fAnisotropy <= ms_fMaxAnisotropy)
    {
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_MINFILTER,D3DTEXF_ANISOTROPIC);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_MIPFILTER,D3DTEXF_ANISOTROPIC);
        assert(SUCCEEDED(ms_hResult));
    }
    else
    {
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_MINFILTER,ms_adwTexMinFilter[eFType]);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_MIPFILTER,ms_adwTexMipFilter[eFType]);
        assert(SUCCEEDED(ms_hResult));
    }

    // Set the border color (for clamp to border).
    const ColorRGBA& rkBorderColor = pkTexture->GetBorderColor();
    ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
        D3DSAMP_BORDERCOLOR,D3DCOLOR_COLORVALUE(rkBorderColor.R(),
        rkBorderColor.G(),rkBorderColor.B(),rkBorderColor.A()));
    assert(SUCCEEDED(ms_hResult));

    switch (eSType)
    {
    case SamplerInformation::SAMPLER_1D:
    {
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_ADDRESSU,ms_adwTexWrapMode[pkTexture->GetWrapType(0)]);
        assert(SUCCEEDED(ms_hResult));
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_ADDRESSV,ms_adwTexWrapMode[pkTexture->GetWrapType(0)]);
        assert(SUCCEEDED(ms_hResult));
        break;
    }
    case SamplerInformation::SAMPLER_2D:
    case SamplerInformation::SAMPLER_PROJ:
    case SamplerInformation::SAMPLER_CUBE:
    {
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_ADDRESSU,ms_adwTexWrapMode[pkTexture->GetWrapType(0)]);
        assert(SUCCEEDED(ms_hResult));
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_ADDRESSV,ms_adwTexWrapMode[pkTexture->GetWrapType(1)]);
        assert(SUCCEEDED(ms_hResult));
        break;
    }
    case SamplerInformation::SAMPLER_3D:
    {
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_ADDRESSU,ms_adwTexWrapMode[pkTexture->GetWrapType(0)]);
        assert(SUCCEEDED(ms_hResult));
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_ADDRESSV,ms_adwTexWrapMode[pkTexture->GetWrapType(1)]);
        assert(SUCCEEDED(ms_hResult));
        ms_hResult = m_pqDevice->SetSamplerState(iTextureUnit,
            D3DSAMP_ADDRESSW,ms_adwTexWrapMode[pkTexture->GetWrapType(2)]);
        assert(SUCCEEDED(ms_hResult));
        break;
    }
    default:
        assert(false);
        break;
    }

    ms_hResult = m_pqDevice->SetTexture(iTextureUnit,pkResource->ID);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnDisableTexture (ResourceIdentifier*)
{
    SamplerInformation* pkSI = m_apkActiveSamplers[m_iCurrentSampler];
    int iTextureUnit = pkSI->GetTextureUnit();

    ms_hResult = m_pqDevice->SetTextureStageState(iTextureUnit,
        D3DTSS_COLOROP,D3DTOP_DISABLE);
    assert(SUCCEEDED(ms_hResult));

    ms_hResult = m_pqDevice->SetTexture(iTextureUnit,0);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void* Dx9Renderer::Lock (DynamicTexture* pkTexture, unsigned int)
{
    assert(pkTexture);
    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    assert(pkID);
    TextureID* pkResource = (TextureID*)pkID;

    const Image* pkImage = pkTexture->GetImage();
    assert(pkImage && !pkImage->IsCubeImage());
    int iDimension = pkImage->GetDimension();

    void* pvData = 0;
    switch (iDimension)
    {
    case 1:
    case 2:
    {
        LPDIRECT3DTEXTURE9 pkDXTexture = (LPDIRECT3DTEXTURE9)pkResource->ID;
        D3DLOCKED_RECT kLockRect;
        ms_hResult = pkDXTexture->LockRect(0,&kLockRect,0,0);
        assert(SUCCEEDED(ms_hResult));
        pvData = kLockRect.pBits;
        break;
    }
    case 3:
    {
        LPDIRECT3DVOLUMETEXTURE9 pkDXTexture =
            (LPDIRECT3DVOLUMETEXTURE9)pkResource->ID;
        D3DLOCKED_BOX kLockBox;
        ms_hResult = pkDXTexture->LockBox(0,&kLockBox,0,0);
        assert(SUCCEEDED(ms_hResult));
        pvData = kLockBox.pBits;
        break;
    }
    default:
        assert(false);
        pvData = 0;
        break;
    }

    m_pkLockedTexture = pkTexture;
    m_iLockedFace = 0;
    return pvData;
}
//----------------------------------------------------------------------------
void Dx9Renderer::Unlock (DynamicTexture* pkTexture)
{
    assert(pkTexture == m_pkLockedTexture && m_iLockedFace == 0);
    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    assert(pkID);
    TextureID* pkResource = (TextureID*)pkID;

    const Image* pkImage = pkTexture->GetImage();
    assert(pkImage && !pkImage->IsCubeImage());
    int iDimension = pkImage->GetDimension();

    switch (iDimension)
    {
    case 1:
    case 2:
    {
        LPDIRECT3DTEXTURE9 pkDXTexture = (LPDIRECT3DTEXTURE9)pkResource->ID;
        ms_hResult = pkDXTexture->UnlockRect(0);
        break;
    }
    case 3:
    {
        LPDIRECT3DVOLUMETEXTURE9 pkDXTexture =
            (LPDIRECT3DVOLUMETEXTURE9)pkResource->ID;
        ms_hResult = pkDXTexture->UnlockBox(0);
        break;
    }
    default:
        assert(false);
        break;
    }

    m_pkLockedTexture = 0;
    m_iLockedFace = -1;
}
//----------------------------------------------------------------------------
void* Dx9Renderer::Lock (int iFace, DynamicTexture* pkTexture, unsigned int)
{
    assert(0 <= iFace && iFace < 6 && pkTexture);
    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    assert(pkID);
    TextureID* pkResource = (TextureID*)pkID;

    const Image* pkImage = pkTexture->GetImage();
    assert(pkImage && pkImage->IsCubeImage());
    (void)pkImage;  // Avoid release build warning.

    LPDIRECT3DCUBETEXTURE9 pkDXTexture =
        (LPDIRECT3DCUBETEXTURE9)pkResource->ID;
    D3DLOCKED_RECT kLockRect;
    ms_hResult = pkDXTexture->GetCubeMapSurface(
        (D3DCUBEMAP_FACES)(D3DCUBEMAP_FACE_POSITIVE_X + iFace),0,
        &m_pkLockedFace);
    assert(SUCCEEDED(ms_hResult));
    ms_hResult = m_pkLockedFace->LockRect(&kLockRect,0,0);
    assert(SUCCEEDED(ms_hResult));
    return kLockRect.pBits;
}
//----------------------------------------------------------------------------
void Dx9Renderer::Unlock (int iFace, DynamicTexture* pkTexture)
{
    assert(pkTexture && pkTexture == m_pkLockedTexture);
    assert(iFace == m_iLockedFace && 0 <= iFace && iFace < 6);
    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    assert(pkID);
    TextureID* pkResource = (TextureID*)pkID;

    const Image* pkImage = pkTexture->GetImage();
    assert(pkImage && pkImage->IsCubeImage());
    (void)pkImage;  // Avoid release build warning.

    LPDIRECT3DCUBETEXTURE9 pkDXTexture =
        (LPDIRECT3DCUBETEXTURE9)pkResource->ID;
    ms_hResult = pkDXTexture->UnlockRect(
        (D3DCUBEMAP_FACES)(D3DCUBEMAP_FACE_POSITIVE_X + iFace),0);
    assert(SUCCEEDED(ms_hResult));
    m_pkLockedFace->Release();

    m_pkLockedTexture = 0;
    m_iLockedFace = -1;
    m_pkLockedFace = 0;
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnEnableVBuffer (ResourceIdentifier* pkID)
{
    VBufferID* pkResource = (VBufferID*)pkID;
    ms_hResult = m_pqDevice->SetStreamSource(0,pkResource->ID,0,
        pkResource->VertexSize);
    assert(SUCCEEDED(ms_hResult));
    ms_hResult = m_pqDevice->SetVertexDeclaration(pkResource->Declaration);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnDisableVBuffer (ResourceIdentifier*)
{
    // Nothing to do.
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnEnableIBuffer (ResourceIdentifier* pkID)
{
    // Bind the current index buffer.
    IBufferID* pkResource = (IBufferID*)pkID;
    ms_hResult = m_pqDevice->SetIndices(pkResource->ID);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
void Dx9Renderer::OnDisableIBuffer (ResourceIdentifier*)
{
    // Nothing to do.
}
//----------------------------------------------------------------------------
