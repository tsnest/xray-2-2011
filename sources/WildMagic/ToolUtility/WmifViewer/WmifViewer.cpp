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
// Version: 4.5.1 (2008/12/12)

#include "WmifViewer.h"

WM4_WINDOW_APPLICATION(WmifViewer);

//----------------------------------------------------------------------------
WmifViewer::WmifViewer ()
    :
    WindowApplication2("WmifViewer",0,0,256,256,
        ColorRGBA(0.9f,0.9f,0.9f,1.0f))
{
    m_iQuantity = 0;
    m_iSliceQuantity = 0;
    m_iCubeFace = 0;
    m_akColor = 0;
    m_aucAlpha = 0;
    m_bMouseDown = false;
    m_bAlphaActive = false;
    memset(m_acPixelStr,0,PIXEL_STR_SIZE);

#ifdef __APPLE__
    // For the Mac, to select files to display since there is no concept of
    // command-line parameters.
    m_bLaunchFileDialog = true;
#endif

    m_aoCFunction[Image::IT_RGB888] = &WmifViewer::ConvertRGB888;
    m_aoCFunction[Image::IT_RGBA8888] = &WmifViewer::ConvertRGBA8888;
    m_aoCFunction[Image::IT_DEPTH16] = &WmifViewer::ConvertDepth16;
    m_aoCFunction[Image::IT_DEPTH24] = &WmifViewer::ConvertDepth24;
    m_aoCFunction[Image::IT_DEPTH32] = &WmifViewer::ConvertDepth32;
    m_aoCFunction[Image::IT_CUBE_RGB888] = &WmifViewer::ConvertRGB888;
    m_aoCFunction[Image::IT_CUBE_RGBA8888] = &WmifViewer::ConvertRGBA8888;
    m_aoCFunction[Image::IT_RGB32F] = &WmifViewer::ConvertRGB32F;
    m_aoCFunction[Image::IT_RGBA32F] = &WmifViewer::ConvertRGBA32F;
    m_aoCFunction[Image::IT_RGB16F] = &WmifViewer::ConvertRGB16F;
    m_aoCFunction[Image::IT_RGBA16F] = &WmifViewer::ConvertRGBA16F;
    m_aoCFunction[Image::IT_RGB16I] = &WmifViewer::ConvertRGB16I;
    m_aoCFunction[Image::IT_RGBA16I] = &WmifViewer::ConvertRGBA16I;
    m_aoCFunction[Image::IT_INTENSITY8I] = &WmifViewer::ConvertIntensity8I;
    m_aoCFunction[Image::IT_INTENSITY16I] = &WmifViewer::ConvertIntensity16I;
    m_aoCFunction[Image::IT_INTENSITY16F] = &WmifViewer::ConvertIntensity16F;
    m_aoCFunction[Image::IT_INTENSITY32F] = &WmifViewer::ConvertIntensity32F;
    m_aoCFunction[Image::IT_RGB565] = &WmifViewer::ConvertRGB565;
    m_aoCFunction[Image::IT_RGBA5551] = &WmifViewer::ConvertRGBA5551;
    m_aoCFunction[Image::IT_RGBA4444] = &WmifViewer::ConvertRGBA4444;

    m_aoSFunction[Image::IT_RGB888] = &WmifViewer::SetStrRGB888;
    m_aoSFunction[Image::IT_RGBA8888] = &WmifViewer::SetStrRGBA8888;
    m_aoSFunction[Image::IT_DEPTH16] = &WmifViewer::SetStrDepth16;
    m_aoSFunction[Image::IT_DEPTH24] = &WmifViewer::SetStrDepth24;
    m_aoSFunction[Image::IT_DEPTH32] = &WmifViewer::SetStrDepth32;
    m_aoSFunction[Image::IT_CUBE_RGB888] = &WmifViewer::SetStrRGB888;
    m_aoSFunction[Image::IT_CUBE_RGBA8888] = &WmifViewer::SetStrRGBA8888;
    m_aoSFunction[Image::IT_RGB32F] = &WmifViewer::SetStrRGB32F;
    m_aoSFunction[Image::IT_RGBA32F] = &WmifViewer::SetStrRGBA32F;
    m_aoSFunction[Image::IT_RGB16F] = &WmifViewer::SetStrRGB16F;
    m_aoSFunction[Image::IT_RGBA16F] = &WmifViewer::SetStrRGBA16F;
    m_aoSFunction[Image::IT_RGB16I] = &WmifViewer::SetStrRGB16I;
    m_aoSFunction[Image::IT_RGBA16I] = &WmifViewer::SetStrRGBA16I;
    m_aoSFunction[Image::IT_INTENSITY8I] = &WmifViewer::SetStrIntensity8I;
    m_aoSFunction[Image::IT_INTENSITY16I] = &WmifViewer::SetStrIntensity16I;
    m_aoSFunction[Image::IT_INTENSITY16F] = &WmifViewer::SetStrIntensity16F;
    m_aoSFunction[Image::IT_INTENSITY32F] = &WmifViewer::SetStrIntensity32F;
    m_aoSFunction[Image::IT_RGB565] = &WmifViewer::SetStrRGB565;
    m_aoSFunction[Image::IT_RGBA5551] = &WmifViewer::SetStrRGBA5551;
    m_aoSFunction[Image::IT_RGBA4444] = &WmifViewer::SetStrRGBA4444;
}
//----------------------------------------------------------------------------
bool WmifViewer::OnPrecreate ()
{
    // This function allows the application window to be of an appropriate
    // size so that the input image fits.

    if (!WindowApplication2::OnPrecreate())
    {
        return false;
    }

    char* acFilename = 0;
    Application::TheCommand->Filename(acFilename);
    if (!acFilename)
    {
        // The input filename must be specified on the command line.
        return false;
    }

    // Load the image.
#ifdef __APPLE__
    bool bAbsolutePath = true;
#else
    bool bAbsolutePath = true;
    //size_t uiLength = strlen(acFilename);
    //if (uiLength > 5)
    //{
    //    if (acFilename[uiLength-5] == '.'
    //    &&  acFilename[uiLength-4] == 'w'
    //    &&  acFilename[uiLength-3] == 'm'
    //    &&  acFilename[uiLength-2] == 'i'
    //    &&  acFilename[uiLength-1] == 'f')
    //    {
    //        acFilename[uiLength-5] = 0;
    //    }
    //}
#endif
    m_spkImage = Image::Load(acFilename,bAbsolutePath);
    WM4_DELETE[] acFilename;
    if (!m_spkImage)
    {
        return false;
    }

    m_iWidth = m_spkImage->GetBound(0);
    m_iHeight = m_spkImage->GetBound(1);

    // The image is stored in right-handed screen coordinates, where the
    // origin is the lower-left corner of the screen.  Reflect the image
    // to display in left-handed coordinates.
    unsigned char aucSave[16];  // 16 = maximum number of bytes per pixel
    unsigned char* aucData = m_spkImage->GetData();
    int iBPP = m_spkImage->GetBytesPerPixel();
    for (int iY = 0; iY < m_iHeight/2; iY++)
    {
        for (int iX = 0; iX < m_iWidth; iX++)
        {
            int iSrc = iBPP*(iX + m_iWidth*iY);
            int iTrg = iBPP*(iX + m_iWidth*(m_iHeight - 1 - iY));
            unsigned char* pucSrc = &aucData[iSrc];
            unsigned char* pucTrg = &aucData[iTrg];
            memcpy(aucSave, pucSrc, iBPP);
            memcpy(pucSrc, pucTrg, iBPP);
            memcpy(pucTrg, aucSave, iBPP);
        }
    }

    // Set window size based on image size.  Adjust height to allow for
    // status bar.  The window width is chosen so that rows are multiples
    // of 4 bytes.
    if (m_iHeight < 128)
    {
        m_iHeight = 128;
    }

    int iStatusHeight = 20;
    m_iHeight += iStatusHeight;

    if (m_iWidth < 512)
    {
        m_iWidth = 512;
    }

    int iRem = (m_iWidth % 4);
    if (iRem > 0)
    {
        m_iWidth += 4-iRem;
    }

    return true;
}
//----------------------------------------------------------------------------
bool WmifViewer::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // The application image is stored so that the RGB data is in one
    // array and the A data is in another.
    m_iQuantity = m_spkImage->GetQuantity();
    m_iSliceQuantity = m_spkImage->GetBound(0)*m_spkImage->GetBound(1);
    m_akColor = WM4_NEW Color[m_iQuantity];
    m_aucAlpha = WM4_NEW unsigned char[m_iQuantity];

    // Transform the image data to a displayable format.
    (this->*m_aoCFunction[m_spkImage->GetFormat()])();

    CopySliceToScreen();
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void WmifViewer::OnTerminate ()
{
    m_spkImage = 0;
    WM4_DELETE[] m_akColor;
    WM4_DELETE[] m_aucAlpha;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
bool WmifViewer::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    switch(ucKey)
    {
    case 'a':
    case 'A':
        m_bAlphaActive = !m_bAlphaActive;
        CopySliceToScreen();
        if (m_bMouseDown)
        {
            ReadPixelValue(iX,iY);
        }
        OnDisplay();
        return true;
    }

    return WindowApplication2::OnKeyDown(ucKey,iX,iY);
}
//----------------------------------------------------------------------------
bool WmifViewer::OnSpecialKeyDown (int iKey, int iX, int iY)
{
    if (!m_spkImage->IsCubeImage())
    {
        return false;
    }

    if (iKey == KEY_UP_ARROW)
    {
        // Up-arrow pressed, go to next cube face.
        if (m_iCubeFace < 5)
        {
            m_iCubeFace++;
            CopySliceToScreen();
            if (m_bMouseDown)
            {
                ReadPixelValue(iX,iY);
            }
            OnDisplay();
        }
        return true;
    }

    if (iKey == KEY_DOWN_ARROW)
    {
        // Down-arrow pressed, go to previous cube face.
        if (m_iCubeFace > 0)
        {
            m_iCubeFace--;
            CopySliceToScreen();
            if (m_bMouseDown)
            {
                ReadPixelValue(iX,iY);
            }
            OnDisplay();
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool WmifViewer::OnMouseClick (int iButton, int iState, int iX, int iY,
    unsigned int)
{
    if (iButton == MOUSE_LEFT_BUTTON)
    {
        if (iState == MOUSE_DOWN)
        {
            m_bMouseDown = true;
            ReadPixelValue(iX,iY);
            OnDisplay();
            return true;
        }
        if (iState == MOUSE_UP)
        {
            m_bMouseDown = false;
            ReadPixelValue(-1,-1);
            OnDisplay();
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
bool WmifViewer::OnMotion (int, int iX, int iY, unsigned int)
{
    if (m_bMouseDown)
    {
        ReadPixelValue(iX,iY);
        OnDisplay();
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void WmifViewer::CopySliceToScreen ()
{
    ClearScreen();

    int iX, iY;

    if (m_bAlphaActive)
    {
        unsigned char* aucSlice = &m_aucAlpha[m_iCubeFace*m_iSliceQuantity];
        for (iY = 0; iY < m_spkImage->GetBound(1); iY++)
        {
            for (iX = 0; iX < m_spkImage->GetBound(0); iX++)
            {
                int i = iX + m_spkImage->GetBound(0)*iY;
                int j = Index(iX,iY);
                m_akScreen[j].b = aucSlice[i];
                m_akScreen[j].g = aucSlice[i];
                m_akScreen[j].r = aucSlice[i];
            }
        }
    }
    else
    {
        Color* akSlice = &m_akColor[m_iCubeFace*m_iSliceQuantity];
        for (iY = 0; iY < m_spkImage->GetBound(1); iY++)
        {
            for (iX = 0; iX < m_spkImage->GetBound(0); iX++)
            {
                int i = iX + m_spkImage->GetBound(0)*iY;
                int j = Index(iX,iY);
                m_akScreen[j] = akSlice[i];
            }
        }
    }
}
//----------------------------------------------------------------------------
void WmifViewer::ReadPixelValue (int iX, int iY)
{
    if (0 <= iX && iX < m_spkImage->GetBound(0)
    &&  0 <= iY && iY < m_spkImage->GetBound(1))
    {
        (this->*m_aoSFunction[m_spkImage->GetFormat()])(iX,iY);
    }
    else
    {
        size_t uiLength = strlen(m_acPixelStr);
        if(uiLength)
        {
            memset(m_acPixelStr,' ',uiLength);
        }
    }
}
//----------------------------------------------------------------------------
void WmifViewer::ScreenOverlay ()
{
    if (m_bMouseDown)
    {
        m_pkRenderer->Draw(4,m_iHeight-4,ColorRGBA::BLACK,m_acPixelStr);
    }
}
//----------------------------------------------------------------------------
#include <float.h>
float WmifViewer::FromHalf (unsigned short usValue)
{
    // From the OpenGL ARB specification for GL_ARB_half_float_pixel:
    //
    // A 16-bit floating-point number has a 1-bit sign (S), a 5-bit
    // exponent (E), and a 10-bit mantissa (M).  The value of a 16-bit
    // floating-point number is determined by the following:
    //     (-1)^S * 0.0,                        if E == 0 and M == 0,
    //     (-1)^S * 2^-14 * (M / 2^10),         if E == 0 and M != 0,
    //     (-1)^S * 2^(E-15) * (1 + M/2^10),    if 0 < E < 31,
    //     (-1)^S * INF,                        if E == 31 and M == 0, or
    //     NaN,                                 if E == 31 and M != 0,
    // where
    //     S = floor((N mod 65536) / 32768),
    //     E = floor((N mod 32768) / 1024), and
    //     M = N mod 1024.

    // This is not the most efficient conversion from half-float to float.
    static float s_fTwoPowMinus14 = Mathf::Pow(2.0f, -14.0f);
    static float s_fTwoPowMinus10 = Mathf::Pow(2.0f, -10.0f);
    static int s_iMax = INT_MAX;
    static float s_fNAN = *(float*)&s_iMax;

    bool bNegative = ((usValue & 0x8000) != 0);
    unsigned int uiExponent = ((usValue & 0x7C00) >> 10);
    unsigned int uiMantissa = (usValue & 0x03FF);

    float fValue;
    if (uiExponent > 0)
    {
        if (uiExponent < 31)
        {
            fValue = Mathf::Pow(2.0f, (float)uiExponent - 15.0f)*
                (1.0f + s_fTwoPowMinus10*(float)uiMantissa);
        }
        else
        {
            if (uiMantissa == 0)
            {
                fValue = Mathf::MAX_REAL;
            }
            else
            {
                fValue = s_fNAN;
            }
        }
    }
    else
    {
        if (uiMantissa == 0)
        {
            fValue = 0.0f;
        }
        else
        {
            fValue = s_fTwoPowMinus14*(float)uiMantissa;
        }
    }

    if(bNegative)
    {
        fValue = -fValue;
    }

    return fValue;
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGB888 ()
{
    const unsigned char* pucSrcData = m_spkImage->GetData();
    Color* pkTrgRGB = m_akColor;
    for (int i = 0; i < m_iQuantity; i++)
    {
        pkTrgRGB->r = *pucSrcData++;
        pkTrgRGB->g = *pucSrcData++;
        pkTrgRGB->b = *pucSrcData++;
        pkTrgRGB++;
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGBA8888 ()
{
    const unsigned char* pucSrcData = m_spkImage->GetData();
    Color* pkTrgRGB = m_akColor;
    unsigned char* pucTrgA = m_aucAlpha;
    for (int i = 0; i < m_iQuantity; i++)
    {
        pkTrgRGB->r = *pucSrcData++;
        pkTrgRGB->g = *pucSrcData++;
        pkTrgRGB->b = *pucSrcData++;
        pkTrgRGB++;

        *pucTrgA++ = *pucSrcData++;
    }
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertDepth16 ()
{
    const float* pfSrcData = (const float*)m_spkImage->GetData();

    float fMin = *pfSrcData++;
    float fMax = fMin;
    int i;
    for (i = 1; i < m_iQuantity; i++)
    {
        float fValue = *pfSrcData++;
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        pfSrcData = (const float*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fDepth = (*pfSrcData++ - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fDepth);
            pkTrgRGB->g = pkTrgRGB->r;
            pkTrgRGB->b = pkTrgRGB->r;
            pkTrgRGB++;
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertDepth24 ()
{
    const float* pfSrcData = (const float*)m_spkImage->GetData();

    float fMin = *pfSrcData++;
    float fMax = fMin;
    int i;
    for (i = 1; i < m_iQuantity; i++)
    {
        float fValue = *pfSrcData++;
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        pfSrcData = (const float*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fDepth = (*pfSrcData++ - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fDepth);
            pkTrgRGB->g = pkTrgRGB->r;
            pkTrgRGB->b = pkTrgRGB->r;
            pkTrgRGB++;
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertDepth32 ()
{
    const float* pfSrcData = (const float*)m_spkImage->GetData();

    float fMin = *pfSrcData++;
    float fMax = fMin;
    int i;
    for (i = 1; i < m_iQuantity; i++)
    {
        float fValue = *pfSrcData++;
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        pfSrcData = (const float*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fDepth = (*pfSrcData++ - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fDepth);
            pkTrgRGB->g = pkTrgRGB->r;
            pkTrgRGB->b = pkTrgRGB->r;
            pkTrgRGB++;
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGB32F ()
{
    const float* pfSrcData = (const float*)m_spkImage->GetData();

    float fMin = *pfSrcData++;
    float fMax = fMin;
    int i;
    for (i = 1; i < 3*m_iQuantity; i++)
    {
        float fValue = *pfSrcData++;
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        pfSrcData = (const float*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fRed = (*pfSrcData++ - fMin)*fInvRange;
            float fGreen = (*pfSrcData++ - fMin)*fInvRange;
            float fBlue = (*pfSrcData++ - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fRed);
            pkTrgRGB->g = (unsigned char)(255.0f*fGreen);
            pkTrgRGB->b = (unsigned char)(255.0f*fBlue);
            pkTrgRGB++;
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGBA32F ()
{
    const float* pfSrcData = (const float*)m_spkImage->GetData();

    float fMin = *pfSrcData++;
    float fMax = fMin;
    int i;
    for (i = 1; i < 4*m_iQuantity; i++)
    {
        float fValue = *pfSrcData++;
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        unsigned char* pucTrgA = m_aucAlpha;
        pfSrcData = (const float*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fRed = (*pfSrcData++ - fMin)*fInvRange;
            float fGreen = (*pfSrcData++ - fMin)*fInvRange;
            float fBlue = (*pfSrcData++ - fMin)*fInvRange;
            float fAlpha = (*pfSrcData++ - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fRed);
            pkTrgRGB->g = (unsigned char)(255.0f*fGreen);
            pkTrgRGB->b = (unsigned char)(255.0f*fBlue);
            pkTrgRGB++;
            *pucTrgA++ = (unsigned char)(255.0f*fAlpha);
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
        memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
    }
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGB16F ()
{
    const unsigned short* pusSrcData =
        (const unsigned short*)m_spkImage->GetData();

    float fMin = FromHalf(*pusSrcData++);
    float fMax = fMin;
    int i;
    for (i = 1; i < 3*m_iQuantity; i++)
    {
        float fValue = FromHalf(*pusSrcData++);
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        pusSrcData = (const unsigned short*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fRed = (FromHalf(*pusSrcData++) - fMin)*fInvRange;
            float fGreen = (FromHalf(*pusSrcData++) - fMin)*fInvRange;
            float fBlue = (FromHalf(*pusSrcData++) - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fRed);
            pkTrgRGB->g = (unsigned char)(255.0f*fGreen);
            pkTrgRGB->b = (unsigned char)(255.0f*fBlue);
            pkTrgRGB++;
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGBA16F ()
{
    const unsigned short* pusSrcData = 
        (const unsigned short*)m_spkImage->GetData();

    float fMin = FromHalf(*pusSrcData++);
    float fMax = fMin;
    int i;
    for (i = 1; i < 4*m_iQuantity; i++)
    {
        float fValue = FromHalf(*pusSrcData++);
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        unsigned char* pucTrgA = m_aucAlpha;
        pusSrcData = (const unsigned short*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fRed = (FromHalf(*pusSrcData++) - fMin)*fInvRange;
            float fGreen = (FromHalf(*pusSrcData++) - fMin)*fInvRange;
            float fBlue = (FromHalf(*pusSrcData++) - fMin)*fInvRange;
            float fAlpha = (FromHalf(*pusSrcData++) - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fRed);
            pkTrgRGB->g = (unsigned char)(255.0f*fGreen);
            pkTrgRGB->b = (unsigned char)(255.0f*fBlue);
            pkTrgRGB++;
            *pucTrgA++ = (unsigned char)(255.0f*fAlpha);
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGB16I ()
{
    const unsigned short* pusSrcData =
        (const unsigned short*)m_spkImage->GetData();

    float fMin = (float)(*pusSrcData++);
    float fMax = fMin;
    int i;
    for (i = 1; i < 3*m_iQuantity; i++)
    {
        float fValue = (float)(*pusSrcData++);
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        pusSrcData = (const unsigned short*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fRed = ((float)(*pusSrcData++) - fMin)*fInvRange;
            float fGreen = ((float)(*pusSrcData++) - fMin)*fInvRange;
            float fBlue = ((float)(*pusSrcData++) - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fRed);
            pkTrgRGB->g = (unsigned char)(255.0f*fGreen);
            pkTrgRGB->b = (unsigned char)(255.0f*fBlue);
            pkTrgRGB++;
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGBA16I ()
{
    const unsigned short* pusSrcData =
        (const unsigned short*)m_spkImage->GetData();

    float fMin = (float)(*pusSrcData++);
    float fMax = fMin;
    int i;
    for (i = 1; i < 4*m_iQuantity; i++)
    {
        float fValue = (float)(*pusSrcData++);
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        unsigned char* pucTrgA = m_aucAlpha;
        pusSrcData = (const unsigned short*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fRed = ((float)(*pusSrcData++) - fMin)*fInvRange;
            float fGreen = ((float)(*pusSrcData++) - fMin)*fInvRange;
            float fBlue = ((float)(*pusSrcData++) - fMin)*fInvRange;
            float fAlpha = ((float)(*pusSrcData++) - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fRed);
            pkTrgRGB->g = (unsigned char)(255.0f*fGreen);
            pkTrgRGB->b = (unsigned char)(255.0f*fBlue);
            pkTrgRGB++;
            *pucTrgA++ = (unsigned char)(255.0f*fAlpha);
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertIntensity8I ()
{
    const unsigned char* pucSrcData = m_spkImage->GetData();
    Color* pkTrgRGB = m_akColor;
    for (int i = 0; i < m_iQuantity; i++)
    {
        pkTrgRGB->r = *pucSrcData++;
        pkTrgRGB->g = pkTrgRGB->r;
        pkTrgRGB->b = pkTrgRGB->r;
        pkTrgRGB++;
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertIntensity16I ()
{
    const unsigned short* pusSrcData =
        (const unsigned short*)m_spkImage->GetData();

    float fMin = (float)(*pusSrcData++);
    float fMax = fMin;
    int i;
    for (i = 1; i < m_iQuantity; i++)
    {
        float fValue = (float)(*pusSrcData++);
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        pusSrcData = (const unsigned short*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fIntensity = ((float)(*pusSrcData++) - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fIntensity);
            pkTrgRGB->g = pkTrgRGB->r;
            pkTrgRGB->b = pkTrgRGB->r;
            pkTrgRGB++;
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertIntensity16F ()
{
    const unsigned short* pusSrcData =
        (const unsigned short*)m_spkImage->GetData();

    float fMin = FromHalf(*pusSrcData++);
    float fMax = fMin;
    int i;
    for (i = 1; i < m_iQuantity; i++)
    {
        float fValue = FromHalf(*pusSrcData++);
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        pusSrcData = (const unsigned short*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fIntensity = (FromHalf(*pusSrcData++) - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fIntensity);
            pkTrgRGB->g = pkTrgRGB->r;
            pkTrgRGB->b = pkTrgRGB->r;
            pkTrgRGB++;
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertIntensity32F ()
{
    const float* pfSrcData = (const float*)m_spkImage->GetData();

    float fMin = *pfSrcData++;
    float fMax = fMin;
    int i;
    for (i = 1; i < m_iQuantity; i++)
    {
        float fValue = *pfSrcData++;
        if (fValue < fMin)
        {
            fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (fMax > fMin)
    {
        float fInvRange = 1.0f/(fMax - fMin);
        Color* pkTrgRGB = m_akColor;
        pfSrcData = (const float*)m_spkImage->GetData();
        for (i = 0; i < m_iQuantity; i++)
        {
            float fIntensity = (*pfSrcData++ - fMin)*fInvRange;
            pkTrgRGB->r = (unsigned char)(255.0f*fIntensity);
            pkTrgRGB->g = pkTrgRGB->r;
            pkTrgRGB->b = pkTrgRGB->r;
            pkTrgRGB++;
        }
    }
    else
    {
        memset(m_akColor,0,m_iQuantity*sizeof(Color));
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGB565 ()
{
    const unsigned short* pusSrcData =
        (const unsigned short*)m_spkImage->GetData();
    Color* pkTrgRGB = m_akColor;
    for (int i = 0; i < m_iQuantity; i++)
    {
        unsigned int value = (unsigned int)(*pusSrcData++);
        unsigned int uiRed =   (value & 0x0000001Fu);
        unsigned int uiGreen = (value & 0x000007E0u) >> 5;
        unsigned int uiBlue =  (value & 0x0000F800u) >> 11;
        pkTrgRGB->r = (unsigned char)(255.0f*uiRed/31.0f);
        pkTrgRGB->g = (unsigned char)(255.0f*uiGreen/63.0f);
        pkTrgRGB->b = (unsigned char)(255.0f*uiBlue/31.0f);
        pkTrgRGB++;
    }

    memset(m_aucAlpha,0xFF,m_iQuantity*sizeof(unsigned char));
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGBA5551 ()
{
    const unsigned short* pusSrcData =
        (const unsigned short*)m_spkImage->GetData();
    Color* pkTrgRGB = m_akColor;
    unsigned char* pucTrgA = m_aucAlpha;
    for (int i = 0; i < m_iQuantity; i++)
    {
        unsigned int value = (unsigned int)(*pusSrcData++);
        unsigned int uiRed =   (value & 0x0000001Fu);
        unsigned int uiGreen = (value & 0x000003E0u) >> 5;
        unsigned int uiBlue =  (value & 0x00007C00u) >> 10;
        unsigned int uiAlpha = (value & 0x00008000u) >> 15;
        pkTrgRGB->r = (unsigned char)(255.0f*uiRed/31.0f);
        pkTrgRGB->g = (unsigned char)(255.0f*uiGreen/31.0f);
        pkTrgRGB->b = (unsigned char)(255.0f*uiBlue/31.0f);
        pkTrgRGB++;

        *pucTrgA++ = (uiAlpha > 0 ? 255 : 0);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::ConvertRGBA4444 ()
{
    const unsigned short* pusSrcData =
        (const unsigned short*)m_spkImage->GetData();
    Color* pkTrgRGB = m_akColor;
    unsigned char* pucTrgA = m_aucAlpha;
    for (int i = 0; i < m_iQuantity; i++)
    {
        unsigned int value = (unsigned int)(*pusSrcData++);
        unsigned int uiRed =   (value & 0x0000000Fu);
        unsigned int uiGreen = (value & 0x000000F0u) >> 4;
        unsigned int uiBlue =  (value & 0x00000F00u) >> 8;
        unsigned int uiAlpha = (value & 0x0000F000u) >> 12;
        pkTrgRGB->r = (unsigned char)(255.0f*uiRed/15.0f);
        pkTrgRGB->g = (unsigned char)(255.0f*uiGreen/15.0f);
        pkTrgRGB->b = (unsigned char)(255.0f*uiBlue/15.0f);
        pkTrgRGB++;

        *pucTrgA++ = (unsigned char)(255.0f*uiAlpha/15.0f);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGB888 (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const unsigned char* aucData = m_spkImage->GetData();
        int i = 3*(iX + m_spkImage->GetBound(0)*iY);
        unsigned int uiRed = (unsigned int)aucData[i++];
        unsigned int uiGreen = (unsigned int)aucData[i++];
        unsigned int uiBlue = (unsigned int)aucData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %3u , g = %3u , b = %3u",iX,iY,
            uiRed,uiGreen,uiBlue);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGBA8888 (int iX, int iY)
{
    const unsigned char* aucData = m_spkImage->GetData();
    int i = 4*(iX + m_spkImage->GetBound(0)*iY);

    if(m_bAlphaActive)
    {
        unsigned int uiAlpha = (unsigned int)aucData[i + 3];
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) a = %3u",iX,iY,
            uiAlpha);
    }
    else
    {
        unsigned int uiRed = (unsigned int)aucData[i++];
        unsigned int uiGreen = (unsigned int)aucData[i++];
        unsigned int uiBlue = (unsigned int)aucData[i++];
        unsigned int uiAlpha = (unsigned int)aucData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %3u , g = %3u , b = %3u, a = %3u",iX,iY,
            uiRed,uiGreen,uiBlue,uiAlpha);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrDepth16 (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const float* afData = (const float*)m_spkImage->GetData();
        int i = iX + m_spkImage->GetBound(0)*iY;
        float fDepth = afData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) d = %f",iX,iY,
            fDepth);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrDepth24 (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const float* afData = (const float*)m_spkImage->GetData();
        int i = iX + m_spkImage->GetBound(0)*iY;
        float fDepth = afData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) d = %f",iX,iY,
            fDepth);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrDepth32 (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const float* afData = (const float*)m_spkImage->GetData();
        int i = iX + m_spkImage->GetBound(0)*iY;
        float fDepth = afData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) d = %f",iX,iY,
            fDepth);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGB32F (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const float* afData = (const float*)m_spkImage->GetData();
        int i = 3*(iX + m_spkImage->GetBound(0)*iY);
        float fRed = afData[i++];
        float fGreen = afData[i++];
        float fBlue = afData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %f , g = %f , b = %f",iX,iY,
            fRed,fGreen,fBlue);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGBA32F (int iX, int iY)
{
    const float* afData = (const float *)m_spkImage->GetData();
    int i = 4*(iX + m_spkImage->GetBound(0)*iY);

    if(m_bAlphaActive)
    {
        float fAlpha = afData[i + 3];
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) a = %f",iX,iY,
            fAlpha);
    }
    else
    {
        float fRed = afData[i++];
        float fGreen = afData[i++];
        float fBlue = afData[i++];
        float fAlpha = afData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %f , g = %f , b = %f, a = %f",iX,iY,
            fRed,fGreen,fBlue,fAlpha);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGB16F (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const unsigned short* ausData =
            (const unsigned short*)m_spkImage->GetData();
        int i = 3*(iX + m_spkImage->GetBound(0)*iY);
        float fRed = FromHalf(ausData[i++]);
        float fGreen = FromHalf(ausData[i++]);
        float fBlue = FromHalf(ausData[i]);

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %f , g = %f , b = %f",iX,iY,
            fRed,fGreen,fBlue);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGBA16F (int iX, int iY)
{
    const unsigned short* ausData =
        (const unsigned short *)m_spkImage->GetData();
    int i = 4*(iX + m_spkImage->GetBound(0)*iY);

    if(m_bAlphaActive)
    {
        float fAlpha = FromHalf(ausData[i + 3]);
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) a = %f",iX,iY,
            fAlpha);
    }
    else
    {
        float fRed = FromHalf(ausData[i++]);
        float fGreen = FromHalf(ausData[i++]);
        float fBlue = FromHalf(ausData[i++]);
        float fAlpha = FromHalf(ausData[i]);

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %f , g = %f , b = %f, a = %f",iX,iY,
            fRed,fGreen,fBlue,fAlpha);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGB16I (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const unsigned short* ausData =
            (const unsigned short*)m_spkImage->GetData();
        int i = 3*(iX + m_spkImage->GetBound(0)*iY);
        unsigned int uiRed = (unsigned int)ausData[i++];
        unsigned int uiGreen = (unsigned int)ausData[i++];
        unsigned int uiBlue = (unsigned int)ausData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %u , g = %u , b = %u",iX,iY,
            uiRed,uiGreen,uiBlue);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGBA16I (int iX, int iY)
{
    const unsigned short* ausData =
        (const unsigned short *)m_spkImage->GetData();
    int i = 4*(iX + m_spkImage->GetBound(0)*iY);

    if(m_bAlphaActive)
    {
        unsigned int usAlpha = (unsigned int)ausData[i + 3];
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) a = %u",iX,iY,
            usAlpha);
    }
    else
    {
        unsigned int uiRed = (unsigned int)(ausData[i++]);
        unsigned int uiGreen = (unsigned int)(ausData[i++]);
        unsigned int uiBlue = (unsigned int)(ausData[i++]);
        unsigned int uiAlpha = (unsigned int)(ausData[i++]);

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %u , g = %u , b = %u, a = %u",iX,iY,
            uiRed,uiGreen,uiBlue,uiAlpha);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrIntensity8I (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const unsigned char* aucData = m_spkImage->GetData();
        int i = iX + m_spkImage->GetBound(0)*iY;
        unsigned int uiIntensity = (unsigned int)aucData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) i = %3u",iX,iY,
            uiIntensity);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrIntensity16I (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const unsigned short* ausData =
            (const unsigned short*)m_spkImage->GetData();
        int i = iX + m_spkImage->GetBound(0)*iY;
        unsigned int uiIntensity = (unsigned int)ausData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) i = %u",iX,iY,
            uiIntensity);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrIntensity16F (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const unsigned short* ausData =
            (const unsigned short*)m_spkImage->GetData();
        int i = iX + m_spkImage->GetBound(0)*iY;
        float fIntensity = FromHalf(ausData[i]);

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) i = %f",iX,iY,
            fIntensity);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrIntensity32F (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const float* afData = (const float*)m_spkImage->GetData();
        int i = iX + m_spkImage->GetBound(0)*iY;
        float fIntensity = afData[i];

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) i = %f",iX,iY,
            fIntensity);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGB565 (int iX, int iY)
{
    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) no alpha",iX,iY);
    }
    else
    {
        const unsigned short* ausData =
            (const unsigned short*)m_spkImage->GetData();
        int i = iX + m_spkImage->GetBound(0)*iY;
        unsigned int value = ausData[i];
        unsigned int uiRed =   (value & 0x0000001Fu);
        unsigned int uiGreen = (value & 0x000007E0u) >> 5;
        unsigned int uiBlue =  (value & 0x0000F800u) >> 11;

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %3u , g = %3u , b = %3u",iX,iY,
            uiRed,uiGreen,uiBlue);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGBA5551 (int iX, int iY)
{
    const unsigned short* ausData =
        (const unsigned short*)m_spkImage->GetData();
    int i = iX + m_spkImage->GetBound(0)*iY;
    unsigned int value = ausData[i];
    unsigned int uiAlpha = (value & 0x00008000u) >> 15;

    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) a = %3u",iX,iY,
            uiAlpha);
    }
    else
    {
        unsigned int uiRed =   (value & 0x0000001Fu);
        unsigned int uiGreen = (value & 0x000003E0u) >> 5;
        unsigned int uiBlue =  (value & 0x00007C00u) >> 10;

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %3u , g = %3u , b = %3u",iX,iY,
            uiRed,uiGreen,uiBlue);
    }
}
//----------------------------------------------------------------------------
void WmifViewer::SetStrRGBA4444 (int iX, int iY)
{
    const unsigned short* ausData =
        (const unsigned short*)m_spkImage->GetData();
    int i = iX + m_spkImage->GetBound(0)*iY;
    unsigned int value = ausData[i];
    unsigned int uiAlpha = (value & 0x0000F000u) >> 12;

    if(m_bAlphaActive)
    {
        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) a = %3u",iX,iY,
            uiAlpha);
    }
    else
    {
        unsigned int uiRed =   (value & 0x0000000Fu);
        unsigned int uiGreen = (value & 0x000000F0u) >> 4;
        unsigned int uiBlue =  (value & 0x00000F00u) >> 8;

        System::Sprintf(m_acPixelStr,PIXEL_STR_SIZE,
            "(%d,%d) r = %3u , g = %3u , b = %3u",iX,iY,
            uiRed,uiGreen,uiBlue);
    }
}
//----------------------------------------------------------------------------
