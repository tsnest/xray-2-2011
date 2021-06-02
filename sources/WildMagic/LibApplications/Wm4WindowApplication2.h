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
// Version: 4.0.0 (2006/06/28)

#ifndef WM4WINDOWAPPLICATION2_H
#define WM4WINDOWAPPLICATION2_H

#include "Wm4WindowApplication.h"

namespace Wm4
{

class WindowApplication2 : public WindowApplication
{
public:
    WindowApplication2 (const char* acWindowTitle, int iXPosition,
        int iYPosition, int iWidth, int iHeight,
        const ColorRGBA& rkBackgroundColor);

    virtual ~WindowApplication2 ();

    // event callbacks
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnResize (int iWidth, int iHeight);
    virtual void OnDisplay ();

    // Allows you to do additional drawing after the screen polygon is drawn.
    virtual void ScreenOverlay ();

    void ClearScreen ();

    class Color
    {
    public:
        Color (unsigned char ucR=0, unsigned char ucG=0, unsigned char ucB=0)
        {
            r = ucR;
            g = ucG;
            b = ucB;
        }

        bool operator== (Color kColor) const
        {
            return b == kColor.b && g == kColor.g && r == kColor.r;
        }

        bool operator!= (Color kColor) const
        {
            return b != kColor.b || g != kColor.g || r != kColor.r;
        }

        unsigned char b, g, r;
    };

    void SetPixel (int iX, int iY, Color kColor);
    void SetThickPixel (int iX, int iY, int iThick, Color kColor);
    Color GetPixel (int iX, int iY);
    void DrawLine (int iX0, int iY0, int iX1, int iY1, Color kColor);
    void DrawRectangle (int iXMin, int iYMin, int iXMax, int iYMax,
        Color kColor, bool bSolid = false);
    void DrawCircle (int iXCenter, int iYCenter, int iRadius, Color kColor,
        bool bSolid = false);
    void Fill (int iX, int iY, Color kFColor, Color kBColor);

    bool& ClampToWindow ();

    // For right-handed drawing.  You still draw to the left-handed screen,
    // but immediately before drawing the screen is copied into another buffer
    // with the rows reversed.  You need only call DoFlip(true) once for an
    // application.  The default is 'false'.
    void DoFlip (bool bDoFlip);


protected:
    int Index (int iX, int iY)
    {
        // left-handed screen coordinates
        return iX + m_iWidth*iY;
    }

    int m_iScrWidth, m_iScrHeight;
    Color* m_akScreen;
    bool m_bClampToWindow;

    // For right-handed drawing.  The array m_akScreen is copied to
    // m_akFlipScreen so that the rows are reversed.
    Color* m_akFlipScreen;
};

}

#endif
