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
// Version: 4.0.1 (2007/07/25)

#include "Wm4SoftRendererPCH.h"
#include "Wm4SoftSampler.h"
using namespace Wm4;

SoftSampler::Creator SoftSampler::ms_aoCreator[
    SamplerInformation::MAX_SAMPLER_TYPES];

//----------------------------------------------------------------------------
SoftSampler* SoftSampler::Create (const Texture* pkTexture)
{
    assert(pkTexture);

    const Image* pkImage = pkTexture->GetImage();
    int iDimension = pkImage->GetDimension();
    bool bIsRegularImage = !pkImage->IsCubeImage();
    if (1 <= iDimension && iDimension <= 3)
    {
        int i;
        for (i = 0; i < iDimension; i++)
        {
            unsigned int uiBound = (unsigned int)pkImage->GetBound(i);
            if (uiBound <= 1 || !IsPowerOfTwo(uiBound))
            {
                break;
            }
        }

        if (i == iDimension)
        {
            SoftSampler* pkSampler;
            if (bIsRegularImage)
            {
                pkSampler = ms_aoCreator[iDimension - 1](pkTexture);
            }
            else
            {
                pkSampler =
                    ms_aoCreator[SamplerInformation::SAMPLER_CUBE](pkTexture);
            }
            return pkSampler;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
SoftSampler::SoftSampler (const Texture* pkTexture)
{
    m_pkTexture = pkTexture;

    // The texture image is converted to RGBA8888 to avoid having to convert
    // on every interpolation call.  TO DO: Cubemap formats are handled by
    // the cubemap shader?
    m_aakImage[0] = m_pkTexture->GetImage()->CreateRGBA();
    memset(&m_aakImage[1],0,(MAX_LEVELS-1)*sizeof(ColorRGBA*));

    // The mipmap images are created by the derived classes.
    m_iLevels = 0;
    m_fCurrLevel = 0.0f;
    m_fDL = 0.0f;
    m_iLevel0 = 0;
    m_iLevel1 = 0;
    m_akImage0 = 0;
    m_akImage1 = 0;
    m_akCurrImage = 0;

    m_bAllowMipmapping = true;
}
//----------------------------------------------------------------------------
SoftSampler::~SoftSampler ()
{
    for (int i = 0; i < MAX_LEVELS; i++)
    {
        WM4_DELETE[] m_aakImage[i];
    }
}
//----------------------------------------------------------------------------
void SoftSampler::Recreate (const ColorRGBA* akCImage)
{
    // Copy the new base image to the sampler.  The derived classes are
    // responsible for recomputing mipmap images.
    const Image* pkImage = m_pkTexture->GetImage();
    assert(pkImage->GetDimension() == 2 && !pkImage->IsDepthImage());
    int iBound0 = pkImage->GetBound(0);
    int iBound1 = pkImage->GetBound(1);
    size_t uiSize = iBound0*sizeof(ColorRGBA);
    const ColorRGBA* akSrc = &akCImage[iBound0*(iBound1-1)];
    ColorRGBA* akTrg = m_aakImage[0];
    for (int iY = 0; iY < iBound1; iY++)
    {
        System::Memcpy(akTrg,uiSize,akSrc,uiSize);
        akSrc -= iBound0;
        akTrg += iBound0;
    }
}
//----------------------------------------------------------------------------
void SoftSampler::Recreate (const float* afDImage)
{
    // Copy the new base image to the sampler.  The derived classes are
    // responsible for recomputing mipmap images.
    const Image* pkImage = m_pkTexture->GetImage();
    assert(pkImage->GetDimension() == 2 && pkImage->IsDepthImage());
    int iBound0 = pkImage->GetBound(0);
    int iBound1 = pkImage->GetBound(1);
    const float* afSrcBase = &afDImage[iBound0*(iBound1-1)];
    ColorRGBA* akTrgBase = m_aakImage[0];
    for (int iY = 0; iY < iBound1; iY++)
    {
        const float* pfSrc = afSrcBase;
        ColorRGBA* pkTrg = akTrgBase;
        for (int iX = 0; iX < iBound0; iX++)
        {
            float fDepth = *pfSrc++;
            pkTrg->R() = fDepth;
            pkTrg->G() = fDepth;
            pkTrg->B() = fDepth;
            pkTrg->A() = 1.0f;
            pkTrg++;
        }
        afSrcBase -= iBound0;
        akTrgBase += iBound0;
    }
}
//----------------------------------------------------------------------------
bool SoftSampler::MipmappingEnabled () const
{
    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    return m_bAllowMipmapping
        && eFType != Texture::NEAREST
        && eFType != Texture::LINEAR;
}
//----------------------------------------------------------------------------
void SoftSampler::ComputeMipmapParameters (const int aiX[3],
    const int aiY[3], const float* aafVertex[3], const Attributes&, int)
{
    m_bAllowMipmapping = true;

    // Check for degeneracy (should not happen).
    int iX1mX0 = aiX[1] - aiX[0];
    int iX0mX2 = aiX[0] - aiX[2];
    int iY0mY1 = aiY[0] - aiY[1];
    int iY2mY0 = aiY[2] - aiY[0];
    int iDet = iX1mX0*iY2mY0 - iX0mX2*iY0mY1;
    assert(iDet != 0);

    // Compute intermediate terms.
    float fInvDet = 1.0f/(float)iDet;
    float fX0mX2 = (float)iX0mX2;
    float fX1mX0 = (float)iX1mX0;
    float fX2mX1 = (float)(aiX[2] - aiX[1]);
    float fY0mY1 = (float)iY0mY1;
    float fY1mY2 = (float)(aiY[1] - aiY[2]);
    float fY2mY0 = (float)iY2mY0;
    float fX0Y1mX1Y0 = (float)(aiX[0]*aiY[1] - aiX[1]*aiY[0]);
    float fX1Y2mX2Y1 = (float)(aiX[1]*aiY[2] - aiX[2]*aiY[1]);
    float fX2Y0mX0Y2 = (float)(aiX[2]*aiY[0] - aiX[0]*aiY[2]);
    float fW0 = aafVertex[0][3];
    float fW1 = aafVertex[1][3];
    float fW2 = aafVertex[2][3];
    float fW0W1 = fW0*fW1;
    float fW0W2 = fW0*fW2;
    float fW1W2 = fW1*fW2;
    float fW0mW1 = fW0 - fW1;
    float fW0mW2 = fW0 - fW2;
    float fW1W0mW2 = fW1*fW0mW2;
    float fW2W0mW1 = fW2*fW0mW1;
    m_fAMul0 = fW1W2*fY1mY2*fInvDet;
    m_fAMul1 = fW0W2*fY2mY0*fInvDet;
    m_fAMul2 = fW0W1*fY0mY1*fInvDet;
    m_fBMul0 = fW1W2*fX2mX1*fInvDet;
    m_fBMul1 = fW0W2*fX0mX2*fInvDet;
    m_fBMul2 = fW0W1*fX1mX0*fInvDet;
    m_fCMul0 = fW1W2*fX1Y2mX2Y1*fInvDet;
    m_fCMul1 = fW0W2*fX2Y0mX0Y2*fInvDet;
    m_fCMul2 = fW0W1*fX0Y1mX1Y0*fInvDet;
    m_fD = (fW2W0mW1*fY2mY0 + fW1W0mW2*fY0mY1)*fInvDet;
    m_fE = (fW2W0mW1*fX0mX2 + fW1W0mW2*fX1mX0)*fInvDet;
    m_fF = (fW2W0mW1*fX2Y0mX0Y2 + fW1W0mW2*fX0Y1mX1Y0)*fInvDet + fW1W2;
}
//----------------------------------------------------------------------------
float SoftSampler::GetImageCoordinate (int i, float fCoord, int iBound) const
{
    float fBound = (float)iBound;
    float fImCoord;

    Texture::WrapType eWType = m_pkTexture->GetWrapType(i);
    if (eWType == Texture::REPEAT)
    {
        fCoord -= Mathf::Floor(fCoord);
        fImCoord = fBound*fCoord - 0.5f;
    }
    else if (eWType == Texture::MIRRORED_REPEAT)
    {
        float fFloor = Mathf::Floor(fCoord);
        fCoord -= fFloor;
        int iFloor = (int)fFloor;
        if (iFloor & 1)
        {
            fCoord = 1.0f - fCoord;
        }
        fImCoord = fBound*fCoord - 0.5f;
    }
    else
    {
        fImCoord = fBound*fCoord - 0.5f;
        if (eWType == Texture::CLAMP)
        {
            if (fImCoord < -0.5f)
            {
                fImCoord = -0.5f;
            }
            else if (fImCoord > fBound - 0.5f)
            {
                fImCoord = fBound - 0.5f;
            }
        }
        else if (eWType == Texture::CLAMP_BORDER)
        {
            if (fImCoord < -1.0f)
            {
                fImCoord = -1.0f;
            }
            else if (fImCoord > fBound)
            {
                fImCoord = fBound;
            }
        }
        else // eWType == Texture::CLAMP_EDGE
        {
            if (fImCoord < 0.0f)
            {
                fImCoord = 0.0f;
            }
            else if (fImCoord > fBound - 1.0f)
            {
                fImCoord = fBound - 1.0f;
            }
        }
    }

    return fImCoord;
}
//----------------------------------------------------------------------------
void SoftSampler::ClampLevels ()
{
    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    if (eFType == Texture::NEAREST_NEAREST
    ||  eFType == Texture::LINEAR_NEAREST)
    {
        m_iLevel0 = (int)(m_fCurrLevel + 0.5);
        if (m_iLevel0 < 0)
        {
            m_iLevel0 = 0;
        }
        else if (m_iLevel0 >= m_iLevels)
        {
            m_iLevel0 = m_iLevels - 1;
        }
        m_akImage0 = m_aakImage[m_iLevel0];
    }
    else
    {
        m_iLevel0 = (int)Mathf::Floor(m_fCurrLevel);
        if (m_iLevel0 < 0)
        {
            m_iLevel0 = 0;
        }
        else if (m_iLevel0 >= m_iLevels)
        {
            m_iLevel0 = m_iLevels - 1;
        }
        m_akImage0 = m_aakImage[m_iLevel0];

        m_iLevel1 = (int)Mathf::Ceil(m_fCurrLevel);
        if (m_iLevel1 < 0)
        {
            m_iLevel1 = 0;
        }
        else if (m_iLevel1 >= m_iLevels)
        {
            m_iLevel1 = m_iLevels - 1;
        }
        m_akImage1 = m_aakImage[m_iLevel1];

        m_fDL = m_fCurrLevel - (float)m_iLevel0;
    }
}
//----------------------------------------------------------------------------
void SoftSampler::CreateMipmap1 (int iLevel, int* aiXBound, int iXBD2)
{
    m_aakImage[iLevel] = WM4_NEW ColorRGBA[iXBD2];
    aiXBound[iLevel] = iXBD2;
    RecreateMipmap1(iLevel,iXBD2);
}
//----------------------------------------------------------------------------
void SoftSampler::RecreateMipmap1 (int iLevel, int iXBD2)
{
    ColorRGBA* akPrev = m_aakImage[iLevel-1];
    ColorRGBA* akCurr = m_aakImage[iLevel];
    for (int iX = 0, iTX = 0; iX < iXBD2; iX++, iTX += 2)
    {
        akCurr[iX] = 0.5f*(akPrev[iTX] + akPrev[iTX+1]);
    }
}
//----------------------------------------------------------------------------
void SoftSampler::CreateMipmap2 (int iLevel, int* aiXBound, int* aiYBound,
    int* aiLog2XBound, int iXB, int iXBD2, int iYBD2, int iLXB, int iLXBM1)
{
    m_aakImage[iLevel] = WM4_NEW ColorRGBA[iXBD2*iYBD2];
    aiXBound[iLevel] = iXBD2;
    aiYBound[iLevel] = iYBD2;
    aiLog2XBound[iLevel] = iLXBM1;
    RecreateMipmap2(iLevel,iXB,iXBD2,iYBD2,iLXB,iLXBM1);
}
//----------------------------------------------------------------------------
void SoftSampler::RecreateMipmap2 (int iLevel, int iXB, int iXBD2, int iYBD2,
    int iLXB, int iLXBM1)
{
    ColorRGBA* akPrev = m_aakImage[iLevel-1];
    ColorRGBA* akCurr = m_aakImage[iLevel];
    for (int iY = 0, iTY = 0; iY < iYBD2; iY++, iTY += 2)
    {
        for (int iX = 0, iTX = 0; iX < iXBD2; iX++, iTX += 2)
        {
            int iCI = iX + (iY << iLXBM1);
            int iPI00 = iTX + (iTY << iLXB);
            int iPI10 = iPI00 + 1;
            int iPI01 = iPI00 + iXB;
            int iPI11 = iPI01 + 1;
            akCurr[iCI] = 0.25f*(
                akPrev[iPI00] +
                akPrev[iPI10] +
                akPrev[iPI01] +
                akPrev[iPI11]);
        }
    }
}
//----------------------------------------------------------------------------
void SoftSampler::CreateMipmap3 (int iLevel, int* aiXBound, int* aiYBound,
    int* aiZBound, int* aiLog2XBound, int* aiLog2YBound, int iXB, int iXBYB,
    int iXBD2, int iYBD2, int iZBD2, int iLXB, int iLYB, int iLXBM1,
    int iLYBM1)
{
    m_aakImage[iLevel] = WM4_NEW ColorRGBA[iXBD2*iYBD2*iZBD2];
    aiXBound[iLevel] = iXBD2;
    aiYBound[iLevel] = iYBD2;
    aiZBound[iLevel] = iZBD2;
    aiLog2XBound[iLevel] = iLXBM1;
    aiLog2YBound[iLevel] = iLYBM1;
    RecreateMipmap3(iLevel,iXB,iXBYB,iXBD2,iYBD2,iZBD2,iLXB,iLYB,iLXBM1,
        iLYBM1);
}
//----------------------------------------------------------------------------
void SoftSampler::RecreateMipmap3 (int iLevel, int iXB, int iXBYB, int iXBD2,
    int iYBD2, int iZBD2, int iLXB, int iLYB, int iLXBM1, int iLYBM1)
{
    ColorRGBA* akPrev = m_aakImage[iLevel-1];
    ColorRGBA* akCurr = m_aakImage[iLevel];
    for (int iZ = 0, iTZ = 0; iZ < iZBD2; iZ++, iTZ += 2)
    {
        for (int iY = 0, iTY = 0; iY < iYBD2; iY++, iTY += 2)
        {
            for (int iX = 0, iTX = 0; iX < iXBD2; iX++, iTX += 2)
            {
                int iCI = iX + ((iY + (iZ << iLYBM1)) << iLXBM1);
                int iPI000 = iTX + ((iTY + (iTZ << iLYB)) << iLXB);
                int iPI100 = iPI000 + 1;
                int iPI010 = iPI000 + iXB;
                int iPI110 = iPI010 + 1;
                int iPI001 = iPI000 + iXBYB;
                int iPI101 = iPI001 + 1;
                int iPI011 = iPI001 + iXB;
                int iPI111 = iPI011 + 1;
                akCurr[iCI] = 0.125f*(
                    akPrev[iPI000] +
                    akPrev[iPI100] +
                    akPrev[iPI010] +
                    akPrev[iPI110] +
                    akPrev[iPI001] +
                    akPrev[iPI101] +
                    akPrev[iPI011] +
                    akPrev[iPI111]);
            }
        }
    }
}
//----------------------------------------------------------------------------
