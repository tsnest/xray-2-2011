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

#ifndef WMIFVIEWER_H
#define WMIFVIEWER_H

#include "Wm4WindowApplication2.h"
using namespace Wm4;

class WmifViewer : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    WmifViewer ();

    virtual bool OnPrecreate ();
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnSpecialKeyDown (int iKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);
    virtual bool OnMotion (int iButton, int iX, int iY,
        unsigned int uiModifiers);
    virtual void ScreenOverlay ();

protected:
    static float FromHalf (unsigned short usValue);

    void ConvertRGB888 ();
    void ConvertRGBA8888 ();
    void ConvertDepth16 ();
    void ConvertDepth24 ();
    void ConvertDepth32 ();
    void ConvertCubeRGB888 ();
    void ConvertCubeRGBA8888 ();
    void ConvertRGB32F ();
    void ConvertRGBA32F ();
    void ConvertRGB16F ();
    void ConvertRGBA16F ();
    void ConvertRGB16I ();
    void ConvertRGBA16I ();
    void ConvertIntensity8I ();
    void ConvertIntensity16I ();
    void ConvertIntensity16F ();
    void ConvertIntensity32F ();
    void ConvertRGB565 ();
    void ConvertRGBA5551 ();
    void ConvertRGBA4444 ();
    typedef void (WmifViewer::*ConvertFunction)(void);
    ConvertFunction m_aoCFunction[Image::IT_QUANTITY];

    void SetStrRGB888 (int iX, int iY);
    void SetStrRGBA8888 (int iX, int iY);
    void SetStrDepth16 (int iX, int iY);
    void SetStrDepth24 (int iX, int iY);
    void SetStrDepth32 (int iX, int iY);
    void SetStrCubeRGB888 (int iX, int iY);
    void SetStrCubeRGBA8888 (int iX, int iY);
    void SetStrRGB32F (int iX, int iY);
    void SetStrRGBA32F (int iX, int iY);
    void SetStrRGB16F (int iX, int iY);
    void SetStrRGBA16F (int iX, int iY);
    void SetStrRGB16I (int iX, int iY);
    void SetStrRGBA16I (int iX, int iY);
    void SetStrIntensity8I (int iX, int iY);
    void SetStrIntensity16I (int iX, int iY);
    void SetStrIntensity16F (int iX, int iY);
    void SetStrIntensity32F (int iX, int iY);
    void SetStrRGB565 (int iX, int iY);
    void SetStrRGBA5551 (int iX, int iY);
    void SetStrRGBA4444 (int iX, int iY);
    typedef void (WmifViewer::*SetStrFunction)(int,int);
    SetStrFunction m_aoSFunction[Image::IT_QUANTITY];

    void CopySliceToScreen ();
    void ReadPixelValue (int iX, int iY);
    void WritePixelString ();

    int m_iQuantity, m_iSliceQuantity, m_iCubeFace;
    ImagePtr m_spkImage;
    Color* m_akColor;
    unsigned char* m_aucAlpha;
    bool m_bMouseDown, m_bAlphaActive;

    enum { PIXEL_STR_SIZE = 256 };
    char m_acPixelStr[PIXEL_STR_SIZE];
};

WM4_REGISTER_INITIALIZE(WmifViewer);

#endif
