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
// Version: 4.0.3 (2007/07/25)

#include "Wm4SoftRendererPCH.h"
#include "Wm4SoftSampler3.h"
using namespace Wm4;

WM4_IMPLEMENT_INITIALIZE(SoftSampler3);

//----------------------------------------------------------------------------
void SoftSampler3::Initialize ()
{
    ms_aoCreator[SamplerInformation::SAMPLER_3D] = &SoftSampler3::Create3;
}
//----------------------------------------------------------------------------
SoftSampler* SoftSampler3::Create3 (const Texture* pkTexture)
{
    return WM4_NEW SoftSampler3(pkTexture);
}
//----------------------------------------------------------------------------
SoftSampler3::SoftSampler3 (const Texture* pkTexture)
    :
    SoftSampler(pkTexture)
{
    const Image* pkImage = m_pkTexture->GetImage();
    assert(pkImage->GetDimension() == 3);
    m_akCurrImage = m_aakImage[0];
    m_iCurrXBound = pkImage->GetBound(0);
    m_iCurrYBound = pkImage->GetBound(1);
    m_iCurrZBound = pkImage->GetBound(2);
    m_iCurrLog2XBound = (int)Log2OfPowerOfTwo((unsigned int)m_iCurrXBound);
    m_iCurrLog2YBound = (int)Log2OfPowerOfTwo((unsigned int)m_iCurrYBound);

    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    if (eFType == Texture::NEAREST || eFType == Texture::LINEAR)
    {
        m_iLevels = 1;
        m_aiXBound = WM4_NEW int[1];
        m_aiYBound = WM4_NEW int[1];
        m_aiZBound = WM4_NEW int[1];
        m_aiLog2XBound = WM4_NEW int[1];
        m_aiLog2YBound = WM4_NEW int[1];
        m_aiXBound[0] = m_iCurrXBound;
        m_aiYBound[0] = m_iCurrYBound;
        m_aiZBound[0] = m_iCurrZBound;
        m_aiLog2XBound[0] = m_iCurrLog2XBound;
        m_aiLog2YBound[0] = m_iCurrLog2YBound;
    }
    else
    {
        CreateMipmaps();
    }
}
//----------------------------------------------------------------------------
SoftSampler3::~SoftSampler3 ()
{
}
//----------------------------------------------------------------------------
void SoftSampler3::Recreate (const ColorRGBA* akCImage)
{
    SoftSampler::Recreate(akCImage);

    const Image* pkImage = m_pkTexture->GetImage();
    m_akCurrImage = m_aakImage[0];
    m_iCurrXBound = pkImage->GetBound(0);
    m_iCurrYBound = pkImage->GetBound(1);
    m_iCurrZBound = pkImage->GetBound(2);
    m_iCurrLog2XBound = (int)Log2OfPowerOfTwo((unsigned int)m_iCurrXBound);
    m_iCurrLog2YBound = (int)Log2OfPowerOfTwo((unsigned int)m_iCurrYBound);

    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    if (eFType != Texture::NEAREST && eFType != Texture::LINEAR)
    {
        RecreateMipmaps();
    }
}
//----------------------------------------------------------------------------
void SoftSampler3::ComputeMipmapParameters (const int aiX[3],
    const int aiY[3], const float* aafVertex[3], const Attributes& rkAttr,
    int iUnit)
{
    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    if (eFType == Texture::NEAREST || eFType == Texture::LINEAR)
    {
        m_akCurrImage = m_aakImage[0];
        m_iCurrXBound = m_aiXBound[0];
        m_iCurrYBound = m_aiYBound[0];
        m_iCurrZBound = m_aiZBound[0];
        m_iCurrLog2XBound = m_aiLog2XBound[0];
        m_iCurrLog2YBound = m_aiLog2YBound[0];
        return;
    }

    SoftSampler::ComputeMipmapParameters(aiX,aiY,aafVertex,rkAttr,iUnit);

    int iOffset = rkAttr.GetTCoordOffset(iUnit) + 4 -
        rkAttr.GetPositionChannels();
    float fU0 = aafVertex[0][iOffset];
    float fU1 = aafVertex[1][iOffset];
    float fU2 = aafVertex[2][iOffset];
    m_afA[0] = m_aiXBound[0]*(m_fAMul0*fU0 + m_fAMul1*fU1 + m_fAMul2*fU2);
    m_afB[0] = m_aiXBound[0]*(m_fBMul0*fU0 + m_fBMul1*fU1 + m_fBMul2*fU2);
    m_afC[0] = m_aiXBound[0]*(m_fCMul0*fU0 + m_fCMul1*fU1 + m_fCMul2*fU2);
    m_afAEmBD[0] = m_afA[0]*m_fE - m_afB[0]*m_fD;
    m_afAFmCD[0] = m_afA[0]*m_fF - m_afC[0]*m_fD;
    m_afBFmCE[0] = m_afB[0]*m_fF - m_afC[0]*m_fE;

    iOffset++;
    float fV0 = aafVertex[0][iOffset];
    float fV1 = aafVertex[1][iOffset];
    float fV2 = aafVertex[2][iOffset];
    m_afA[1] = m_aiYBound[0]*(m_fAMul0*fV0 + m_fAMul1*fV1 + m_fAMul2*fV2);
    m_afB[1] = m_aiYBound[0]*(m_fBMul0*fV0 + m_fBMul1*fV1 + m_fBMul2*fV2);
    m_afC[1] = m_aiYBound[0]*(m_fCMul0*fV0 + m_fCMul1*fV1 + m_fCMul2*fV2);
    m_afAEmBD[1] = m_afA[1]*m_fE - m_afB[1]*m_fD;
    m_afAFmCD[1] = m_afA[1]*m_fF - m_afC[1]*m_fD;
    m_afBFmCE[1] = m_afB[1]*m_fF - m_afC[1]*m_fE;

    iOffset++;
    float fW0 = aafVertex[0][iOffset];
    float fW1 = aafVertex[1][iOffset];
    float fW2 = aafVertex[2][iOffset];
    m_afA[2] = m_aiZBound[0]*(m_fAMul0*fW0 + m_fAMul1*fW1 + m_fAMul2*fW2);
    m_afB[2] = m_aiZBound[0]*(m_fBMul0*fW0 + m_fBMul1*fW1 + m_fBMul2*fW2);
    m_afC[2] = m_aiZBound[0]*(m_fCMul0*fW0 + m_fCMul1*fW1 + m_fCMul2*fW2);
    m_afAEmBD[2] = m_afA[2]*m_fE - m_afB[2]*m_fD;
    m_afAFmCD[2] = m_afA[2]*m_fF - m_afC[2]*m_fD;
    m_afBFmCE[2] = m_afB[2]*m_fF - m_afC[2]*m_fE;
}
//----------------------------------------------------------------------------
void SoftSampler3::DisallowMipmapping ()
{
    m_bAllowMipmapping = false;
    m_akCurrImage = m_aakImage[0];
    m_iCurrXBound = m_aiXBound[0];
    m_iCurrYBound = m_aiYBound[0];
    m_iCurrZBound = m_aiZBound[0];
    m_iCurrLog2XBound = m_aiLog2XBound[0];
    m_iCurrLog2YBound = m_aiLog2YBound[0];
}
//----------------------------------------------------------------------------
void SoftSampler3::CurrentPixel (int iX, int iY)
{
    float fDenom = m_fD*iX + m_fE*iY + m_fF;
    float fInvDenomSqr = 1.0f/(fDenom*fDenom);

    m_fCurrLevel = 0.0f;
    for (int i = 0; i < 3; i++)
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
        m_iCurrZBound = m_aiZBound[m_iLevel0];
        m_iCurrLog2XBound = m_aiLog2XBound[m_iLevel0];
        m_iCurrLog2YBound = m_aiLog2YBound[m_iLevel0];
    }
    else
    {
        m_iXBound0 = m_aiXBound[m_iLevel0];
        m_iXBound1 = m_aiXBound[m_iLevel1];
        m_iYBound0 = m_aiYBound[m_iLevel0];
        m_iYBound1 = m_aiYBound[m_iLevel1];
        m_iZBound0 = m_aiZBound[m_iLevel0];
        m_iZBound1 = m_aiZBound[m_iLevel1];
        m_iLog2XBound0 = m_aiLog2XBound[m_iLevel0];
        m_iLog2XBound1 = m_aiLog2XBound[m_iLevel1];
        m_iLog2YBound0 = m_aiLog2YBound[m_iLevel0];
        m_iLog2YBound1 = m_aiLog2YBound[m_iLevel1];
    }
}
//----------------------------------------------------------------------------
void SoftSampler3::CreateMipmaps ()
{
    const Image* pkImage = m_pkTexture->GetImage();
    int iLXB = (int)Log2OfPowerOfTwo((unsigned int)pkImage->GetBound(0));
    int iLYB = (int)Log2OfPowerOfTwo((unsigned int)pkImage->GetBound(1));
    int iLZB = (int)Log2OfPowerOfTwo((unsigned int)pkImage->GetBound(2));

    int iMaxLevel1, iMaxLevel2, iMaxLevel3, iMax, iMid, iMin;
    if (pkImage->GetBound(0) >= pkImage->GetBound(1))
    {
        if (pkImage->GetBound(0) >= pkImage->GetBound(2))
        {
            if (pkImage->GetBound(1) >= pkImage->GetBound(2))
            {
                // xb >= yb >= zb
                m_iLevels = iLXB + 1;
                iMaxLevel1 = iLXB;
                iMaxLevel2 = iLYB;
                iMaxLevel3 = iLZB;
                iMax = 0;
                iMid = 1;
                iMin = 2;
            }
            else
            {
                // xb >= zb >= yb
                m_iLevels = iLXB + 1;
                iMaxLevel1 = iLXB;
                iMaxLevel2 = iLZB;
                iMaxLevel3 = iLYB;
                iMax = 0;
                iMid = 2;
                iMin = 1;
            }
        }
        else
        {
            // zb >= xb >= yb
            m_iLevels = iLZB + 1;
            iMaxLevel1 = iLZB;
            iMaxLevel2 = iLXB;
            iMaxLevel3 = iLYB;
            iMax = 2;
            iMid = 0;
            iMin = 1;
        }
    }
    else
    {
        if (pkImage->GetBound(1) >= pkImage->GetBound(2))
        {
            if (pkImage->GetBound(0) >= pkImage->GetBound(2))
            {
                // yb >= xb >= zb
                m_iLevels = iLYB + 1;
                iMaxLevel1 = iLYB;
                iMaxLevel2 = iLXB;
                iMaxLevel3 = iLZB;
                iMax = 1;
                iMid = 0;
                iMin = 2;
            }
            else
            {
                // yb >= zb >= xb
                m_iLevels = iLYB + 1;
                iMaxLevel1 = iLYB;
                iMaxLevel2 = iLZB;
                iMaxLevel3 = iLXB;
                iMax = 1;
                iMid = 2;
                iMin = 0;
            }
        }
        else
        {
            // zb >= yb >= xb
            m_iLevels = iLZB + 1;
            iMaxLevel1 = iLZB;
            iMaxLevel2 = iLYB;
            iMaxLevel3 = iLXB;
            iMax = 2;
            iMid = 1;
            iMin = 0;
        }
    }

    // 3D reduction
    int iXB = m_aiXBound[0];
    int iYB = m_aiYBound[0];
    int iXBYB = iXB*iYB;
    int iXBD2 = (iXB >> 1);
    int iYBD2 = (iYB >> 1);
    int iZBD2 = (m_aiZBound[0] >> 1);
    int iLXBM1 = iLXB - 1;
    int iLYBM1 = iLYB - 1;
    int i;
    for (i = 1; i <= iMaxLevel3; i++)
    {
        CreateMipmap3(i,m_aiXBound,m_aiYBound,m_aiZBound,m_aiLog2XBound,
            m_aiLog2YBound,iXB,iXBYB,iXBD2,iYBD2,iZBD2,iLXB,iLYB,iLXBM1,
            iLYBM1);
        iXB = iXBD2;
        iYB = iYBD2;
        iXBYB >>= 2;
        iXBD2 >>= 1;
        iYBD2 >>= 1;
        iZBD2 >>= 1;
        iLXB = iLXBM1;
        iLYB = iLYBM1;
        iLXBM1--;
        iLYBM1--;
    }

    // 2D reduction
    if (iMin == 0)
    {
        // current reduced image is (xb,yb,zb) = (1,2^{m2-m0},2^{m1-m0})
        for (/**/; i <= iMaxLevel2; i++)
        {
            CreateMipmap2(i,m_aiYBound,m_aiZBound,m_aiLog2YBound,iYB,iYBD2,
                iZBD2,iLYB,iLYBM1);
            m_aiXBound[i] = 1;
            m_aiLog2XBound[i] = 0;
            iYB = iYBD2;
            iYBD2 >>= 1;
            iZBD2 >>= 1;
        }
    }
    else if (iMin == 1)
    {
        // current reduced image is (xb,yb,zb) = (2^{m2-m0},1,2^{m1-m0})
        for (/**/; i <= iMaxLevel2; i++)
        {
            CreateMipmap2(i,m_aiXBound,m_aiZBound,m_aiLog2XBound,iXB,iXBD2,
                iZBD2,iLXB,iLXBM1);
            m_aiYBound[i] = 1;
            m_aiLog2YBound[i] = 0;
            iXB = iXBD2;
            iXBD2 >>= 1;
            iZBD2 >>= 1;
            iLXB = iLXBM1;
            iLXBM1--;
        }
    }
    else // iMin == 2
    {
        // current reduced image is (xb,yb,zb) = (2^{m2-m0},2^{m1-m0},1)
        for (/**/; i <= iMaxLevel2; i++)
        {
            CreateMipmap2(i,m_aiXBound,m_aiYBound,m_aiLog2XBound,iXB,iXBD2,
                iYBD2,iLXB,iLXBM1);
            m_aiZBound[i] = 1;
            iXB = iXBD2;
            iXBD2 >>= 1;
            iYBD2 >>= 1;
            iLXB = iLXBM1;
            iLXBM1--;
        }
    }

    // 1D reduction
    if (iMax == 0)
    {
        // current reduced image is (xb,yb,zb) = (2^{m2-m1},1,1)
        for (/**/; i <= iMaxLevel1; i++)
        {
            CreateMipmap1(i,m_aiXBound,iXBD2);
            m_aiYBound[i] = 1;
            m_aiZBound[i] = 1;
            m_aiLog2XBound[i] = iLXBM1;
            m_aiLog2YBound[i] = 0;
            iXBD2 >>= 1;
            iLXBM1--;
        }
    }
    else if (iMax == 1)
    {
        // current reduced image is (xb,yb,zb) = (1,2^{m2-m1},1)
        for (/**/; i <= iMaxLevel1; i++)
        {
            CreateMipmap1(i,m_aiYBound,iYBD2);
            m_aiXBound[i] = 1;
            m_aiZBound[i] = 1;
            m_aiLog2XBound[i] = 0;
            m_aiLog2YBound[i] = iLYBM1;
            iYBD2 >>= 1;
            iLYBM1--;
        }
    }
    else // iMax == 2
    {
        // current reduced image is (xb,yb,zb) = (1,1,2^{m2-m1})
        for (/**/; i <= iMaxLevel1; i++)
        {
            CreateMipmap1(i,m_aiZBound,iZBD2);
            m_aiXBound[i] = 1;
            m_aiYBound[i] = 1;
            m_aiLog2XBound[i] = 0;
            m_aiLog2YBound[i] = 0;
            iZBD2 >>= 1;
        }
    }
}
//----------------------------------------------------------------------------
void SoftSampler3::RecreateMipmaps ()
{
    const Image* pkImage = m_pkTexture->GetImage();
    int iLXB = (int)Log2OfPowerOfTwo((unsigned int)pkImage->GetBound(0));
    int iLYB = (int)Log2OfPowerOfTwo((unsigned int)pkImage->GetBound(1));
    int iLZB = (int)Log2OfPowerOfTwo((unsigned int)pkImage->GetBound(2));

    int iMaxLevel1, iMaxLevel2, iMaxLevel3, iMax, iMid, iMin;
    if (pkImage->GetBound(0) >= pkImage->GetBound(1))
    {
        if (pkImage->GetBound(0) >= pkImage->GetBound(2))
        {
            if (pkImage->GetBound(1) >= pkImage->GetBound(2))
            {
                // xb >= yb >= zb
                iMaxLevel1 = iLXB;
                iMaxLevel2 = iLYB;
                iMaxLevel3 = iLZB;
                iMax = 0;
                iMid = 1;
                iMin = 2;
            }
            else
            {
                // xb >= zb >= yb
                iMaxLevel1 = iLXB;
                iMaxLevel2 = iLZB;
                iMaxLevel3 = iLYB;
                iMax = 0;
                iMid = 2;
                iMin = 1;
            }
        }
        else
        {
            // zb >= xb >= yb
            iMaxLevel1 = iLZB;
            iMaxLevel2 = iLXB;
            iMaxLevel3 = iLYB;
            iMax = 2;
            iMid = 0;
            iMin = 1;
        }
    }
    else
    {
        if (pkImage->GetBound(1) >= pkImage->GetBound(2))
        {
            if (pkImage->GetBound(0) >= pkImage->GetBound(2))
            {
                // yb >= xb >= zb
                iMaxLevel1 = iLYB;
                iMaxLevel2 = iLXB;
                iMaxLevel3 = iLZB;
                iMax = 1;
                iMid = 0;
                iMin = 2;
            }
            else
            {
                // yb >= zb >= xb
                iMaxLevel1 = iLYB;
                iMaxLevel2 = iLZB;
                iMaxLevel3 = iLXB;
                iMax = 1;
                iMid = 2;
                iMin = 0;
            }
        }
        else
        {
            // zb >= yb >= xb
            iMaxLevel1 = iLZB;
            iMaxLevel2 = iLYB;
            iMaxLevel3 = iLXB;
            iMax = 2;
            iMid = 1;
            iMin = 0;
        }
    }

    // 3D reduction
    int iXB = m_aiXBound[0];
    int iYB = m_aiYBound[0];
    int iXBYB = iXB*iYB;
    int iXBD2 = (iXB >> 1);
    int iYBD2 = (iYB >> 1);
    int iZBD2 = (m_aiZBound[0] >> 1);
    int iLXBM1 = iLXB - 1;
    int iLYBM1 = iLYB - 1;
    int i;
    for (i = 1; i <= iMaxLevel3; i++)
    {
        RecreateMipmap3(i,iXB,iXBYB,iXBD2,iYBD2,iZBD2,iLXB,iLYB,iLXBM1,
            iLYBM1);
        iXB = iXBD2;
        iYB = iYBD2;
        iXBYB >>= 2;
        iXBD2 >>= 1;
        iYBD2 >>= 1;
        iZBD2 >>= 1;
        iLXB = iLXBM1;
        iLYB = iLYBM1;
        iLXBM1--;
        iLYBM1--;
    }

    // 2D reduction
    if (iMin == 0)
    {
        // current reduced image is (xb,yb,zb) = (1,2^{m2-m0},2^{m1-m0})
        for (/**/; i <= iMaxLevel2; i++)
        {
            RecreateMipmap2(i,iYB,iYBD2,iZBD2,iLYB,iLYBM1);
            iYB = iYBD2;
            iYBD2 >>= 1;
            iZBD2 >>= 1;
        }
    }
    else if (iMin == 1)
    {
        // current reduced image is (xb,yb,zb) = (2^{m2-m0},1,2^{m1-m0})
        for (/**/; i <= iMaxLevel2; i++)
        {
            RecreateMipmap2(i,iXB,iXBD2,iZBD2,iLXB,iLXBM1);
            iXB = iXBD2;
            iXBD2 >>= 1;
            iZBD2 >>= 1;
            iLXB = iLXBM1;
            iLXBM1--;
        }
    }
    else // iMin == 2
    {
        // current reduced image is (xb,yb,zb) = (2^{m2-m0},2^{m1-m0},1)
        for (/**/; i <= iMaxLevel2; i++)
        {
            RecreateMipmap2(i,iXB,iXBD2,iYBD2,iLXB,iLXBM1);
            iXB = iXBD2;
            iXBD2 >>= 1;
            iYBD2 >>= 1;
            iLXB = iLXBM1;
            iLXBM1--;
        }
    }

    // 1D reduction
    if (iMax == 0)
    {
        // current reduced image is (xb,yb,zb) = (2^{m2-m1},1,1)
        for (/**/; i <= iMaxLevel1; i++)
        {
            RecreateMipmap1(i,iXBD2);
            iXBD2 >>= 1;
            iLXBM1--;
        }
    }
    else if (iMax == 1)
    {
        // current reduced image is (xb,yb,zb) = (1,2^{m2-m1},1)
        for (/**/; i <= iMaxLevel1; i++)
        {
            RecreateMipmap1(i,iYBD2);
            iYBD2 >>= 1;
            iLYBM1--;
        }
    }
    else // iMax == 2
    {
        // current reduced image is (xb,yb,zb) = (1,1,2^{m2-m1})
        for (/**/; i <= iMaxLevel1; i++)
        {
            RecreateMipmap1(i,iZBD2);
            iZBD2 >>= 1;
        }
    }
}
//----------------------------------------------------------------------------
ColorRGBA SoftSampler3::operator() (const float* afCoord)
{
    float fXIm, fYIm, fZIm;
    ColorRGBA kColor;

    Texture::FilterType eFType = m_pkTexture->GetFilterType();
    if (eFType == Texture::NEAREST || eFType == Texture::NEAREST_NEAREST)
    {
        fXIm = GetImageCoordinate(0,afCoord[0],m_iCurrXBound);
        fYIm = GetImageCoordinate(1,afCoord[1],m_iCurrYBound);
        fZIm = GetImageCoordinate(2,afCoord[2],m_iCurrZBound);
        kColor = GetNearestColor(fXIm,fYIm,fZIm);
        return kColor;
    }

    if (eFType == Texture::LINEAR || eFType == Texture::LINEAR_NEAREST)
    {
        fXIm = GetImageCoordinate(0,afCoord[0],m_iCurrXBound);
        fYIm = GetImageCoordinate(1,afCoord[1],m_iCurrYBound);
        fZIm = GetImageCoordinate(2,afCoord[2],m_iCurrZBound);
        kColor = GetLinearColor(fXIm,fYIm,fZIm);
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
            m_iCurrZBound = m_iZBound0;
            m_iCurrLog2XBound = m_iLog2XBound0;
            m_iCurrLog2YBound = m_iLog2YBound0;
            fXIm = GetImageCoordinate(0,afCoord[0],m_iCurrXBound);
            fYIm = GetImageCoordinate(1,afCoord[1],m_iCurrYBound);
            fZIm = GetImageCoordinate(2,afCoord[2],m_iCurrZBound);
            kC0 = GetNearestColor(fXIm,fYIm,fZIm);

            m_akCurrImage = m_akImage1;
            m_iCurrXBound = m_iXBound1;
            m_iCurrYBound = m_iYBound1;
            m_iCurrZBound = m_iZBound1;
            m_iCurrLog2XBound = m_iLog2XBound1;
            m_iCurrLog2YBound = m_iLog2YBound1;
            fXIm = GetImageCoordinate(0,afCoord[0],m_iCurrXBound);
            fYIm = GetImageCoordinate(1,afCoord[1],m_iCurrYBound);
            fZIm = GetImageCoordinate(2,afCoord[2],m_iCurrZBound);
            kC1 = GetNearestColor(fXIm,fYIm,fZIm);

            kColor = kC0 + m_fDL*(kC1 - kC0);
            return kColor;
        }

        if (eFType == Texture::LINEAR_LINEAR)
        {
            m_akCurrImage = m_akImage0;
            m_iCurrXBound = m_iXBound0;
            m_iCurrYBound = m_iYBound0;
            m_iCurrZBound = m_iZBound0;
            m_iCurrLog2XBound = m_iLog2XBound0;
            m_iCurrLog2YBound = m_iLog2YBound0;
            fXIm = GetImageCoordinate(0,afCoord[0],m_iCurrXBound);
            fYIm = GetImageCoordinate(1,afCoord[1],m_iCurrYBound);
            fZIm = GetImageCoordinate(2,afCoord[2],m_iCurrZBound);
            kC0 = GetLinearColor(fXIm,fYIm,fZIm);

            m_akCurrImage = m_akImage1;
            m_iCurrXBound = m_iXBound1;
            m_iCurrYBound = m_iYBound1;
            m_iCurrZBound = m_iZBound1;
            m_iCurrLog2XBound = m_iLog2XBound1;
            m_iCurrLog2YBound = m_iLog2YBound1;
            fXIm = GetImageCoordinate(0,afCoord[0],m_iCurrXBound);
            fYIm = GetImageCoordinate(1,afCoord[1],m_iCurrYBound);
            fZIm = GetImageCoordinate(2,afCoord[2],m_iCurrZBound);
            kC1 = GetLinearColor(fXIm,fYIm,fZIm);

            kColor = kC0 + m_fDL*(kC1 - kC0);
            return kColor;
        }
    }
    else
    {
        if (eFType == Texture::NEAREST_LINEAR)
        {
            fXIm = GetImageCoordinate(0,afCoord[0],m_iCurrXBound);
            fYIm = GetImageCoordinate(1,afCoord[1],m_iCurrYBound);
            fZIm = GetImageCoordinate(2,afCoord[2],m_iCurrZBound);
            kColor = GetNearestColor(fXIm,fYIm,fZIm);
            return kColor;
        }

        if (eFType == Texture::LINEAR_LINEAR)
        {
            fXIm = GetImageCoordinate(0,afCoord[0],m_iCurrXBound);
            fYIm = GetImageCoordinate(1,afCoord[1],m_iCurrYBound);
            fZIm = GetImageCoordinate(2,afCoord[2],m_iCurrZBound);
            kColor = GetLinearColor(fXIm,fYIm,fZIm);
            return kColor;
        }
    }

    assert(false);
    return ColorRGBA::BLACK;
}
//----------------------------------------------------------------------------
ColorRGBA SoftSampler3::GetNearestColor (float fXIm, float fYIm, float fZIm)
    const
{
    int iX = (int)(fXIm + 0.5f);
    int iY = (int)(fYIm + 0.5f);
    int iZ = (int)(fZIm + 0.5f);
    return m_akCurrImage[Index(iX,iY,iZ)];
}
//----------------------------------------------------------------------------
ColorRGBA SoftSampler3::GetLinearColor (float fXIm, float fYIm, float fZIm)
    const
{
    float fX0 = Mathf::Floor(fXIm);
    float fX1 = Mathf::Ceil(fXIm);
    float fY0 = Mathf::Floor(fYIm);
    float fY1 = Mathf::Ceil(fYIm);
    float fZ0 = Mathf::Floor(fZIm);
    float fZ1 = Mathf::Ceil(fZIm);
    float fDX = fXIm - fX0;
    float fDY = fYIm - fY0;
    float fDZ = fZIm - fZ0;
    int iX0 = (int)fX0;
    int iX1 = (int)fX1;
    int iY0 = (int)fY0;
    int iY1 = (int)fY1;
    int iZ0 = (int)fZ0;
    int iZ1 = (int)fZ1;
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

    if (m_pkTexture->GetWrapType(2) == Texture::REPEAT)
    {
        if (iZ0 < 0)
        {
            iZ0 = m_iCurrZBound - 1;
        }
        if (iZ1 >= m_iCurrZBound)
        {
            iZ1 = 0;
        }
    }

    const ColorRGBA& rkBorder = m_pkTexture->GetBorderColor();
    ColorRGBA kC000, kC010, kC100, kC110, kC001, kC011, kC101, kC111;
    if (iX0 >= 0)
    {
        if (iY0 >= 0)
        {
            iIndex = Index(iX0,iY0,iZ0);
            kC000 = (iZ0 >= 0 ? m_akCurrImage[iIndex] : rkBorder);
            iIndex = Index(iX0,iY0,iZ1);
            kC001 = (iZ1 < m_iCurrZBound ? m_akCurrImage[iIndex] : rkBorder);
        }
        else
        {
            kC000 = rkBorder;
            kC001 = rkBorder;
        }

        if (iY1 < m_iCurrYBound)
        {
            iIndex = Index(iX0,iY1,iZ0);
            kC010 = (iZ0 >= 0 ? m_akCurrImage[iIndex] : rkBorder);
            iIndex = Index(iX0,iY1,iZ1);
            kC011 = (iZ1 < m_iCurrZBound ? m_akCurrImage[iIndex] : rkBorder);
        }
        else
        {
            kC010 = rkBorder;
            kC011 = rkBorder;
        }
    }
    else
    {
        kC000 = rkBorder;
        kC001 = rkBorder;
        kC010 = rkBorder;
        kC011 = rkBorder;
    }

    if (iX1 < m_iCurrXBound)
    {
        if (iY0 >= 0)
        {
            iIndex = Index(iX1,iY0,iZ0);
            kC100 = (iZ0 >= 0 ? m_akCurrImage[iIndex] : rkBorder);
            iIndex = Index(iX1,iY0,iZ1);
            kC101 = (iZ1 < m_iCurrZBound ? m_akCurrImage[iIndex] : rkBorder);
        }
        else
        {
            kC100 = rkBorder;
            kC101 = rkBorder;
        }

        if (iY1 < m_iCurrYBound)
        {
            iIndex = Index(iX1,iY1,iZ0);
            kC110 = (iZ0 >= 0 ? m_akCurrImage[iIndex] : rkBorder);
            iIndex = Index(iX1,iY1,iZ1);
            kC111 = (iZ1 < m_iCurrZBound ? m_akCurrImage[iIndex] : rkBorder);
        }
        else
        {
            kC110 = rkBorder;
            kC111 = rkBorder;
        }
    }
    else
    {
        kC100 = rkBorder;
        kC101 = rkBorder;
        kC110 = rkBorder;
        kC111 = rkBorder;
    }

    ColorRGBA kC00 = kC000 + fDZ*(kC001 - kC000);
    ColorRGBA kC01 = kC010 + fDZ*(kC011 - kC010);
    ColorRGBA kC10 = kC100 + fDZ*(kC101 - kC100);
    ColorRGBA kC11 = kC110 + fDZ*(kC111 - kC110);
    ColorRGBA kC0 = kC00 + fDY*(kC01 - kC00);
    ColorRGBA kC1 = kC10 + fDY*(kC11 - kC10);
    ColorRGBA kC = kC0 + fDX*(kC1 - kC0);
    return kC;
}
//----------------------------------------------------------------------------
