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
// Version: 4.0.2 (2007/07/25)

#include "Wm4SoftRendererPCH.h"
#include "Wm4SoftSamplerProj.h"
using namespace Wm4;

WM4_IMPLEMENT_INITIALIZE(SoftSamplerProj);

//----------------------------------------------------------------------------
void SoftSamplerProj::Initialize ()
{
    ms_aoCreator[SamplerInformation::SAMPLER_PROJ] =
        &SoftSamplerProj::CreateProj;
}
//----------------------------------------------------------------------------
SoftSampler* SoftSamplerProj::CreateProj (const Texture* pkTexture)
{
    return WM4_NEW SoftSamplerProj(pkTexture);
}
//----------------------------------------------------------------------------
SoftSamplerProj::SoftSamplerProj (const Texture* pkTexture)
    :
    SoftSampler(pkTexture)
{
    const Image* pkImage = m_pkTexture->GetImage();
    assert(pkImage->GetDimension() == 2);
    m_akCurrImage = m_aakImage[0];
    m_iCurrXBound = pkImage->GetBound(0);
    m_iCurrYBound = pkImage->GetBound(1);
    m_iCurrLog2XBound = (int)Log2OfPowerOfTwo((unsigned int)m_iCurrXBound);

    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    if (eFType == Texture::NEAREST || eFType == Texture::LINEAR)
    {
        m_iLevels = 1;
        m_aiXBound = WM4_NEW int[1];
        m_aiYBound = WM4_NEW int[1];
        m_aiLog2XBound = WM4_NEW int[1];
        m_aiXBound[0] = m_iCurrXBound;
        m_aiYBound[0] = m_iCurrYBound;
        m_aiLog2XBound[0] = m_iCurrLog2XBound;
    }
    else
    {
        CreateMipmaps();
    }
}
//----------------------------------------------------------------------------
SoftSamplerProj::~SoftSamplerProj ()
{
    WM4_DELETE[] m_aiXBound;
    WM4_DELETE[] m_aiYBound;
    WM4_DELETE[] m_aiLog2XBound;
}
//----------------------------------------------------------------------------
void SoftSamplerProj::Recreate (const ColorRGBA* akCImage)
{
    SoftSampler::Recreate(akCImage);

    const Image* pkImage = m_pkTexture->GetImage();
    m_akCurrImage = m_aakImage[0];
    m_iCurrXBound = pkImage->GetBound(0);
    m_iCurrYBound = pkImage->GetBound(1);
    m_iCurrLog2XBound = (int)Log2OfPowerOfTwo((unsigned int)m_iCurrXBound);

    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    if (eFType != Texture::NEAREST && eFType != Texture::LINEAR)
    {
        RecreateMipmaps();
    }
}
//----------------------------------------------------------------------------
void SoftSamplerProj::Recreate (const float* afDImage)
{
    SoftSampler::Recreate(afDImage);

    const Image* pkImage = m_pkTexture->GetImage();
    m_akCurrImage = m_aakImage[0];
    m_iCurrXBound = pkImage->GetBound(0);
    m_iCurrYBound = pkImage->GetBound(1);
    m_iCurrLog2XBound = (int)Log2OfPowerOfTwo((unsigned int)m_iCurrXBound);

    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    if (eFType != Texture::NEAREST && eFType != Texture::LINEAR)
    {
        RecreateMipmaps();
    }
}
//----------------------------------------------------------------------------
void SoftSamplerProj::ComputeMipmapParameters (const int aiX[3],
    const int aiY[3], const float* aafVertex[3], const Attributes& rkAttr,
    int iUnit)
{
    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    if (eFType == Texture::NEAREST || eFType == Texture::LINEAR)
    {
        m_akCurrImage = m_aakImage[0];
        m_iCurrXBound = m_aiXBound[0];
        m_iCurrYBound = m_aiYBound[0];
        m_iCurrLog2XBound = m_aiLog2XBound[0];
        return;
    }

    SoftSampler::ComputeMipmapParameters(aiX,aiY,aafVertex,rkAttr,iUnit);

    int iOffset = rkAttr.GetTCoordOffset(iUnit);
    float fU0 = aafVertex[0][iOffset];
    float fU1 = aafVertex[1][iOffset];
    float fU2 = aafVertex[2][iOffset];
    float fU1mU0 = fU1 - fU0;
    float fU2mU0 = fU2 - fU0;

    m_afA[0] = m_aiXBound[0]*(fU1mU0*m_fAMul0 + fU2mU0*m_fAMul1);
    m_afB[0] = m_aiXBound[0]*(fU1mU0*m_fBMul0 + fU2mU0*m_fBMul1);
    m_afC[0] = m_aiXBound[0]*(fU1mU0*m_fCMul0 + fU2mU0*m_fCMul1 + fU0);
    m_afAEmBD[0] = m_afA[0]*m_fE - m_afB[0]*m_fD;
    m_afAFmCD[0] = m_afA[0]*m_fF - m_afC[0]*m_fD;
    m_afBFmCE[0] = m_afB[0]*m_fF - m_afC[0]*m_fE;

    iOffset++;
    float fV0 = aafVertex[0][iOffset];
    float fV1 = aafVertex[1][iOffset];
    float fV2 = aafVertex[2][iOffset];
    float fV1mV0 = fV1 - fV0;
    float fV2mV0 = fV2 - fV0;

    m_afA[1] = m_aiYBound[0]*(fV1mV0*m_fAMul0 + fV2mV0*m_fAMul1);
    m_afB[1] = m_aiYBound[0]*(fV1mV0*m_fBMul0 + fV2mV0*m_fBMul1);
    m_afC[1] = m_aiYBound[0]*(fV1mV0*m_fCMul0 + fV2mV0*m_fCMul1 + fV0);
    m_afAEmBD[1] = m_afA[1]*m_fE - m_afB[1]*m_fD;
    m_afAFmCD[1] = m_afA[1]*m_fF - m_afC[1]*m_fD;
    m_afBFmCE[1] = m_afB[1]*m_fF - m_afC[1]*m_fE;
}
//----------------------------------------------------------------------------
void SoftSamplerProj::DisallowMipmapping ()
{
    m_bAllowMipmapping = false;
    m_akCurrImage = m_aakImage[0];
    m_iCurrXBound = m_aiXBound[0];
    m_iCurrYBound = m_aiYBound[0];
    m_iCurrLog2XBound = m_aiLog2XBound[0];
}
//----------------------------------------------------------------------------
void SoftSamplerProj::CurrentPixel (int iX, int iY)
{
    float fDenom = m_fD*iX + m_fE*iY + m_fF;
    float fInvDenomSqr = 1.0f/(fDenom*fDenom);

    m_fCurrLevel = 0.0f;
    for (int i = 0; i < 2; i++)
    {
        float fXDeriv = (+m_afAEmBD[i]*iY + m_afAFmCD[i])*fInvDenomSqr;
        float fYDeriv = (-m_afAEmBD[i]*iX + m_afBFmCE[i])*fInvDenomSqr;
        float fArg = fXDeriv*fXDeriv + fYDeriv*fYDeriv;
        if (fArg > 1.0f)
        {
            float fLevel = 0.5f*Mathf::Log2(fArg);
            if (fLevel > m_fCurrLevel)
            {
                m_fCurrLevel = fLevel;
            }
        }
    }

    ClampLevels();

    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    if (eFType == Texture::NEAREST_NEAREST
    ||  eFType == Texture::LINEAR_NEAREST)
    {
        m_akCurrImage = m_aakImage[m_iLevel0];
        m_iCurrXBound = m_aiXBound[m_iLevel0];
        m_iCurrYBound = m_aiYBound[m_iLevel0];
        m_iCurrLog2XBound = m_aiLog2XBound[m_iLevel0];
    }
    else
    {
        m_iXBound0 = m_aiXBound[m_iLevel0];
        m_iXBound1 = m_aiXBound[m_iLevel1];
        m_iYBound0 = m_aiYBound[m_iLevel0];
        m_iYBound1 = m_aiYBound[m_iLevel1];
        m_iLog2XBound0 = m_aiLog2XBound[m_iLevel0];
        m_iLog2XBound1 = m_aiLog2XBound[m_iLevel1];
    }
}
//----------------------------------------------------------------------------
void SoftSamplerProj::CreateMipmaps ()
{
    const Image* pkImage = m_pkTexture->GetImage();
    int iLXB = (int)Log2OfPowerOfTwo((unsigned int)pkImage->GetBound(0));
    int iLYB = (int)Log2OfPowerOfTwo((unsigned int)pkImage->GetBound(1));

    int iMaxLevel1, iMaxLevel2;
    if (pkImage->GetBound(0) >= pkImage->GetBound(1))
    {
        // xb >= yb
        m_iLevels = iLXB + 1;
        iMaxLevel1 = iLXB;
        iMaxLevel2 = iLYB;
    }
    else
    {
        // yb >= xb
        m_iLevels = iLYB + 1;
        iMaxLevel1 = iLYB;
        iMaxLevel2 = iLXB;
    }

    m_aiXBound = WM4_NEW int[m_iLevels];
    m_aiYBound = WM4_NEW int[m_iLevels];
    m_aiLog2XBound = WM4_NEW int[m_iLevels];
    m_aiXBound[0] = pkImage->GetBound(0);
    m_aiYBound[0] = pkImage->GetBound(1);
    m_aiLog2XBound[0] = iLXB;

    // 2D reduction
    int iXB = m_aiXBound[0];
    int iXBD2 = (iXB >> 1);
    int iYBD2 = (m_aiYBound[0] >> 1);
    int iLXBM1 = iLXB - 1;
    int i;
    for (i = 1; i <= iMaxLevel2; i++)
    {
        CreateMipmap2(i,m_aiXBound,m_aiYBound,m_aiLog2XBound,iXB,iXBD2,iYBD2,
            iLXB,iLXBM1);

        iXB = iXBD2;
        iXBD2 >>= 1;
        iYBD2 >>= 1;
        iLXB = iLXBM1;
        iLXBM1--;
    }

    // 1D reduction
    if (pkImage->GetBound(0) > pkImage->GetBound(1))
    {
        for (/**/; i <= iMaxLevel1; i++)
        {
            CreateMipmap1(i,m_aiXBound,iXBD2);
            m_aiYBound[i] = 1;
            m_aiLog2XBound[i] = iLXBM1;
            iXBD2 >>= 1;
            iLXBM1--;
        }
    }
    else if (pkImage->GetBound(0) < pkImage->GetBound(1))
    {
        for (/**/; i <= iMaxLevel1; i++)
        {
            CreateMipmap1(i,m_aiYBound,iYBD2);
            m_aiXBound[i] = 1;
            m_aiLog2XBound[i] = 0;
            iYBD2 >>= 1;
        }
    }
}
//----------------------------------------------------------------------------
void SoftSamplerProj::RecreateMipmaps ()
{
    const Image* pkImage = m_pkTexture->GetImage();
    int iLXB = (int)Log2OfPowerOfTwo((unsigned int)pkImage->GetBound(0));
    int iLYB = (int)Log2OfPowerOfTwo((unsigned int)pkImage->GetBound(1));

    int iMaxLevel1, iMaxLevel2;
    if (pkImage->GetBound(0) >= pkImage->GetBound(1))
    {
        // xb >= yb
        iMaxLevel1 = iLXB;
        iMaxLevel2 = iLYB;
    }
    else
    {
        // yb >= xb
        iMaxLevel1 = iLYB;
        iMaxLevel2 = iLXB;
    }

    // 2D reduction
    int iXB = m_aiXBound[0];
    int iXBD2 = (iXB >> 1);
    int iYBD2 = (m_aiYBound[0] >> 1);
    int iLXBM1 = iLXB - 1;
    int i;
    for (i = 1; i <= iMaxLevel2; i++)
    {
        RecreateMipmap2(i,iXB,iXBD2,iYBD2,iLXB,iLXBM1);

        iXB = iXBD2;
        iXBD2 >>= 1;
        iYBD2 >>= 1;
        iLXB = iLXBM1;
        iLXBM1--;
    }

    // 1D reduction
    if (pkImage->GetBound(0) > pkImage->GetBound(1))
    {
        for (/**/; i <= iMaxLevel1; i++)
        {
            RecreateMipmap1(i,iXBD2);
            iXBD2 >>= 1;
            iLXBM1--;
        }
    }
    else if (pkImage->GetBound(0) < pkImage->GetBound(1))
    {
        for (/**/; i <= iMaxLevel1; i++)
        {
            RecreateMipmap1(i,iYBD2);
            iYBD2 >>= 1;
        }
    }
}
//----------------------------------------------------------------------------
ColorRGBA SoftSamplerProj::operator() (const float* afCoord)
{
    float fInvQ = 1.0f/afCoord[3];
    float fS = afCoord[0]*fInvQ;
    float fT = afCoord[1]*fInvQ;

    float fXIm, fYIm;
    ColorRGBA kColor;

    Texture::FilterType eFType = m_pkTexture->GetFilterType();

    if (eFType == Texture::NEAREST || eFType == Texture::NEAREST_NEAREST)
    {
        fXIm = GetImageCoordinate(0,fS,m_iCurrXBound);
        fYIm = GetImageCoordinate(1,fT,m_iCurrYBound);
        kColor = GetNearestColor(fXIm,fYIm);
        return kColor;
    }

    if (eFType == Texture::LINEAR || eFType == Texture::LINEAR_NEAREST)
    {
        fXIm = GetImageCoordinate(0,fS,m_iCurrXBound);
        fYIm = GetImageCoordinate(1,fT,m_iCurrYBound);
        kColor = GetLinearColor(fXIm,fYIm);
        return kColor;
    }

    if (m_bAllowMipmapping)
    {
        ColorRGBA kC0, kC1;

        if (eFType == Texture::NEAREST_LINEAR)
        {
            m_akCurrImage = m_akImage0;
            m_iCurrXBound = m_iXBound0;
            m_iCurrYBound = m_iYBound0;
            m_iCurrLog2XBound = m_iLog2XBound0;
            fXIm = GetImageCoordinate(0,fS,m_iCurrXBound);
            fYIm = GetImageCoordinate(1,fT,m_iCurrYBound);
            kC0 = GetNearestColor(fXIm,fYIm);

            m_akCurrImage = m_akImage1;
            m_iCurrXBound = m_iXBound1;
            m_iCurrYBound = m_iYBound1;
            m_iCurrLog2XBound = m_iLog2XBound1;
            fXIm = GetImageCoordinate(0,fS,m_iCurrXBound);
            fYIm = GetImageCoordinate(1,fT,m_iCurrYBound);
            kC1 = GetNearestColor(fXIm,fYIm);

            kColor = kC0 + m_fDL*(kC1 - kC0);
            return kColor;
        }

        if (eFType == Texture::LINEAR_LINEAR)
        {
            m_akCurrImage = m_akImage0;
            m_iCurrXBound = m_iXBound0;
            m_iCurrYBound = m_iYBound0;
            m_iCurrLog2XBound = m_iLog2XBound0;
            fXIm = GetImageCoordinate(0,fS,m_iCurrXBound);
            fYIm = GetImageCoordinate(1,fT,m_iCurrYBound);
            kC0 = GetLinearColor(fXIm,fYIm);

            m_akCurrImage = m_akImage1;
            m_iCurrXBound = m_iXBound1;
            m_iCurrYBound = m_iYBound1;
            m_iCurrLog2XBound = m_iLog2XBound1;
            fXIm = GetImageCoordinate(0,fS,m_iCurrXBound);
            fYIm = GetImageCoordinate(1,fT,m_iCurrYBound);
            kC1 = GetLinearColor(fXIm,fYIm);

            kColor = kC0 + m_fDL*(kC1 - kC0);
            return kColor;
        }
    }
    else
    {
        if (eFType == Texture::NEAREST_LINEAR)
        {
            fXIm = GetImageCoordinate(0,fS,m_iCurrXBound);
            fYIm = GetImageCoordinate(1,fT,m_iCurrYBound);
            kColor = GetNearestColor(fXIm,fYIm);
            return kColor;
        }

        if (eFType == Texture::LINEAR_LINEAR)
        {
            fXIm = GetImageCoordinate(0,fS,m_iCurrXBound);
            fYIm = GetImageCoordinate(1,fT,m_iCurrYBound);
            kColor = GetLinearColor(fXIm,fYIm);
            return kColor;
        }
    }

    assert(false);
    return ColorRGBA::BLACK;
}
//----------------------------------------------------------------------------
ColorRGBA SoftSamplerProj::GetNearestColor (float fXIm, float fYIm) const
{
    int iX = (int)(fXIm + 0.5f);
    int iY = (int)(fYIm + 0.5f);
    return m_akCurrImage[Index(iX,iY)];
}
//----------------------------------------------------------------------------
ColorRGBA SoftSamplerProj::GetLinearColor (float fXIm, float fYIm) const
{
    float fX0 = Mathf::Floor(fXIm);
    float fX1 = Mathf::Ceil(fXIm);
    float fY0 = Mathf::Floor(fYIm);
    float fY1 = Mathf::Ceil(fYIm);
    float fDX = fXIm - fX0;
    float fDY = fYIm - fY0;
    int iX0 = (int)fX0;
    int iX1 = (int)fX1;
    int iY0 = (int)fY0;
    int iY1 = (int)fY1;
    int iIndex;

    if (m_pkTexture->GetWrapType(0) == Texture::REPEAT)
    {
        if (iX0 < 0)
        {
            iX0 = m_iCurrXBound - 1;
        }
        if (iX1 >= m_iCurrXBound)
        {
            iX1 = 0;
        }
    }

    if (m_pkTexture->GetWrapType(1) == Texture::REPEAT)
    {
        if (iY0 < 0)
        {
            iY0 = m_iCurrYBound - 1;
        }
        if (iY1 >= m_iCurrYBound)
        {
            iY1 = 0;
        }
    }

    const ColorRGBA& rkBorder = m_pkTexture->GetBorderColor();
    ColorRGBA kC00, kC01, kC10, kC11;
    if (iX0 >= 0)
    {
        iIndex = Index(iX0,iY0);
        kC00 = (iY0 >= 0 ? m_akCurrImage[iIndex] : rkBorder);
        iIndex = Index(iX0,iY1);
        kC01 = (iY1 < m_iCurrYBound ? m_akCurrImage[iIndex] : rkBorder);
    }
    else
    {
        kC00 = rkBorder;
        kC01 = rkBorder;
    }

    if (iX1 < m_iCurrXBound)
    {
        iIndex = Index(iX1,iY0);
        kC10 = (iY0 >= 0 ? m_akCurrImage[iIndex] : rkBorder);
        iIndex = Index(iX1,iY1);
        kC11 = (iY1 < m_iCurrYBound ? m_akCurrImage[iIndex] : rkBorder);
    }
    else
    {
        kC10 = rkBorder;
        kC11 = rkBorder;
    }

    ColorRGBA kC0 = kC00 + fDY*(kC01 - kC00);
    ColorRGBA kC1 = kC10 + fDY*(kC11 - kC10);
    ColorRGBA kC = kC0 + fDX*(kC1 - kC0);
    return kC;
}
//----------------------------------------------------------------------------
