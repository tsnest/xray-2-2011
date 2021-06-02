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

#include "DrawImplicitSurface.h"
#include "Function.h"

WM4_WINDOW_APPLICATION(DrawImplicitSurface);

const int g_iSize = 256;

//----------------------------------------------------------------------------
DrawImplicitSurface::DrawImplicitSurface ()
    :
    WindowApplication2("DrawImplicitSurface",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f)),
    m_kRT(F,DF,g_iSize,g_iSize)
{
    m_iMaxSample = 100;
    m_bBlur = false;
}
//----------------------------------------------------------------------------
bool DrawImplicitSurface::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // initialize camera and view frustum
    m_kRT.Location() = Vector3f(2.0f,0.0f,0.0f);
    m_kRT.Direction() = Vector3f(-1.0f,0.0f,0.0f);
    m_kRT.Up() = Vector3f(0.0f,1.0f,0.0f);
    m_kRT.Right() = m_kRT.Direction().Cross(m_kRT.Up());
    m_kRT.Near() = 0.1f;
    m_kRT.Far() = 10.0f;
    m_kRT.HalfWidth() = 2.0f*m_kRT.Near();  // 90 degree horizontal FOV
    m_kRT.HalfHeight() = 2.0f*m_kRT.Near();  // 90 degree vertical FOV

    // The light direction will be the camera direction so that we can see
    // the surface from all camera locations.

    // draw level surface
    m_kRT.DrawSurface(m_iMaxSample,m_kRT.Direction(),m_bBlur);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void DrawImplicitSurface::OnDisplay ()
{
    ClearScreen();

    const float* afImage = m_kRT.GetImage();
    for (int iY = 0, i = 0; iY < m_kRT.GetHeight(); iY++)
    {
        for (int iX = 0; iX < m_kRT.GetWidth(); iX++)
        {
            int iValue = (int)(255.0f*afImage[i++]);
            SetPixel(iX,iY,Color(iValue,iValue,iValue));
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool DrawImplicitSurface::OnSpecialKeyDown (int iKey, int, int)
{
    // TO DO:  These are chosen for the specific functions in this
    // application.  Allow the application to modify these, either by key
    // strokes or automatically.
    const float fTrnDelta = 0.25f, fRotDelta = 0.1f;

    float fLength;
    Matrix3f kRot;

    bool bMoved = false;

    if (iKey == KEY_UP_ARROW)
    {
        // translate forward in camera direction
        m_kRT.Location() += fTrnDelta*m_kRT.Direction();
        bMoved = true;
    }
    else if (iKey == KEY_DOWN_ARROW)
    {
        // translate backward in camera direction
        m_kRT.Location() -= fTrnDelta*m_kRT.Direction();
        bMoved = true;
    }
    else if (iKey == KEY_F1)
    {
        // rotate about camera right, move up on view sphere
        fLength = m_kRT.Location().Length();
        kRot.FromAxisAngle(m_kRT.Right(),fRotDelta);
        m_kRT.Direction() = kRot*m_kRT.Direction();
        m_kRT.Up() = kRot*m_kRT.Up();
        m_kRT.Location() = -fLength*m_kRT.Direction();
        bMoved = true;
    }
    else if (iKey == KEY_F2)
    {
        // rotate about camera right, move down on view sphere
        fLength = m_kRT.Location().Length();
        kRot.FromAxisAngle(m_kRT.Right(),-fRotDelta);
        m_kRT.Direction() = kRot*m_kRT.Direction();
        m_kRT.Up() = kRot*m_kRT.Up();
        m_kRT.Location() = -fLength*m_kRT.Direction();
        bMoved = true;
    }
    else if (iKey == KEY_F3)
    {
        // rotate about camera up, move right on view sphere
        fLength = m_kRT.Location().Length();
        kRot.FromAxisAngle(m_kRT.Up(),fRotDelta);
        m_kRT.Direction() = kRot*m_kRT.Direction();
        m_kRT.Right() = kRot*m_kRT.Right();
        m_kRT.Location() = -fLength*m_kRT.Direction();
        bMoved = true;
    }
    else if (iKey == KEY_F4)
    {
        // rotate about camera up, move left on view sphere
        fLength = m_kRT.Location().Length();
        kRot.FromAxisAngle(m_kRT.Up(),-fRotDelta);
        m_kRT.Direction() = kRot*m_kRT.Direction();
        m_kRT.Right() = kRot*m_kRT.Right();
        m_kRT.Location() = -fLength*m_kRT.Direction();
        bMoved = true;
    }
    else if (iKey == KEY_F5)
    {
        // rotate about camera direction, roll counterclockwise
        fLength = m_kRT.Location().Length();
        kRot.FromAxisAngle(m_kRT.Direction(),fRotDelta);
        m_kRT.Up() = kRot*m_kRT.Up();
        m_kRT.Right() = kRot*m_kRT.Right();
        bMoved = true;
    }
    else if (iKey == KEY_F6)
    {
        // rotate about camera direction, roll clockwise
        fLength = m_kRT.Location().Length();
        kRot.FromAxisAngle(m_kRT.Direction(),-fRotDelta);
        m_kRT.Up() = kRot*m_kRT.Up();
        m_kRT.Right() = kRot*m_kRT.Right();
        bMoved = true;
    }

    if (bMoved)
    {
        m_kRT.DrawSurface(m_iMaxSample,m_kRT.Direction(),m_bBlur);
        OnDisplay();
    }

    return true;
}
//----------------------------------------------------------------------------
bool DrawImplicitSurface::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication2::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case '+':  // increase ray sample size
    case '=':
        m_iMaxSample += 100;
        m_kRT.DrawSurface(m_iMaxSample,m_kRT.Direction(),m_bBlur);
        OnDisplay();
        return true;
    case '-':  // decrease ray sample size
    case '_':
        if (m_iMaxSample > 100)
        {
            m_iMaxSample -= 100;
            m_kRT.DrawSurface(m_iMaxSample,m_kRT.Direction(),m_bBlur);
            OnDisplay();
        }
        return true;
    case 'b':  // toggle blur of output image
    case 'B':
        m_bBlur = !m_bBlur;
        m_kRT.DrawSurface(m_iMaxSample,m_kRT.Direction(),m_bBlur);
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
