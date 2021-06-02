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

#include "QuadraticFreeForm2D.h"

WM4_WINDOW_APPLICATION(QuadraticFreeForm2D);

static const int gs_iSize = 512;

//----------------------------------------------------------------------------
QuadraticFreeForm2D::QuadraticFreeForm2D ()
    :
    WindowApplication2("QuadraticFreeForm2D",0,0,gs_iSize,gs_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_pkImage = 0;
    m_bMouseDown = false;
    m_iRow = -1;
    m_iCol = -1;
}
//----------------------------------------------------------------------------
bool QuadraticFreeForm2D::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // source bitmap
    m_pkImage = Image::Load("Magician");
    assert(m_pkImage && m_pkImage->GetFormat() == Image::IT_RGB888);

    // The default control points produce the identity function.
    int iX = gs_iSize/4, iY = gs_iSize/4;
    m_aaiCtrlX[0][0] =   iX;  m_aaiCtrlY[0][0] =   iY;
    m_aaiCtrlX[0][1] =   iX;  m_aaiCtrlY[0][1] = 2*iY;
    m_aaiCtrlX[0][2] =   iX;  m_aaiCtrlY[0][2] = 3*iY;
    m_aaiCtrlX[1][0] = 2*iX;  m_aaiCtrlY[1][0] =   iY;
    m_aaiCtrlX[1][1] = 2*iX;  m_aaiCtrlY[1][1] = 2*iY;
    m_aaiCtrlX[1][2] = 2*iX;  m_aaiCtrlY[1][2] = 3*iY;
    m_aaiCtrlX[2][0] = 3*iX;  m_aaiCtrlY[2][0] =   iY;
    m_aaiCtrlX[2][1] = 3*iX;  m_aaiCtrlY[2][1] = 2*iY;
    m_aaiCtrlX[2][2] = 3*iX;  m_aaiCtrlY[2][2] = 3*iY;
    m_aakCtrl[0][0] = Vector2f(0.0f,0.0f);
    m_aakCtrl[0][1] = Vector2f(0.0f,0.5f);
    m_aakCtrl[0][2] = Vector2f(0.0f,1.0f);
    m_aakCtrl[1][0] = Vector2f(0.5f,0.0f);
    m_aakCtrl[1][1] = Vector2f(0.5f,0.5f);
    m_aakCtrl[1][2] = Vector2f(0.5f,1.0f);
    m_aakCtrl[2][0] = Vector2f(1.0f,0.0f);
    m_aakCtrl[2][1] = Vector2f(1.0f,0.5f);
    m_aakCtrl[2][2] = Vector2f(1.0f,1.0f);

    DoFlip(true);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void QuadraticFreeForm2D::OnTerminate ()
{
    WM4_DELETE m_pkImage;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void QuadraticFreeForm2D::OnDisplay ()
{
    ClearScreen();

    int iWImage = m_pkImage->GetBound(0);
    int iHImage = m_pkImage->GetBound(1);
    unsigned char* aucData = m_pkImage->GetData();

    const int iNumSamples = 2*gs_iSize;
    const float fInvNumSamples = 1.0f/(float)iNumSamples;
    Vector2f kParam;
    for (int iS = 0; iS < iNumSamples; iS++)
    {
        int iXImage = iS*iWImage/iNumSamples;
        kParam.X() = fInvNumSamples*iS;
        for (int iT = 0; iT < iNumSamples; iT++)
        {
            int iYImage = iT*iHImage/iNumSamples;
            kParam.Y() = fInvNumSamples*iT;
            Vector2f kResult = Evaluate(kParam);

            int iX = ControlToScreen(kResult.X());
            int iY = ControlToScreen(kResult.Y());

            int iIndex = 3*(iXImage + iWImage*iYImage);
            unsigned char ucR = aucData[iIndex++];
            unsigned char ucG = aucData[iIndex++];
            unsigned char ucB = aucData[iIndex++];

            SetPixel(iX,iY,Color(ucR,ucG,ucB));
        }
    }

    // draw the control points
    const int iThick = 2;
    for (int iRow = 0; iRow <= 2; iRow++)
    {
        for (int iCol = 0; iCol <= 2; iCol++)
        {
            for (int iDY = -iThick; iDY <= iThick; iDY++)
            {
                for (int iDX = -iThick; iDX <= iThick; iDX++)
                {
                    SetPixel(m_aaiCtrlX[iRow][iCol]+iDX,
                        m_aaiCtrlY[iRow][iCol]+iDY,Color(0,255,0));
                }
            }
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool QuadraticFreeForm2D::OnMouseClick (int iButton, int iState, int iX,
    int iY, unsigned int)
{
    if (iButton != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    // Handle the flip in y that is used for drawing.
    iY = gs_iSize - 1 - iY;

    if (iState == MOUSE_DOWN)
    {
        m_bMouseDown = true;
        SelectVertex(iX,iY);
        return true;
    }

    if (iState == MOUSE_UP)
    {
        m_bMouseDown = false;
        if (m_iRow >= 0 && m_iCol >= 0
        &&  0 <= iX && iX < GetWidth() && 0 <= iY && iY < GetHeight())
        {
            m_aaiCtrlX[m_iRow][m_iCol] = iX;
            m_aaiCtrlY[m_iRow][m_iCol] = iY;
            m_aakCtrl[m_iRow][m_iCol].X() = ScreenToControl(iX);
            m_aakCtrl[m_iRow][m_iCol].Y() = ScreenToControl(iY);
            OnDisplay();
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool QuadraticFreeForm2D::OnMotion (int, int iX, int iY, unsigned int)
{
    if (m_bMouseDown)
    {
        // Handle the flip in y that is used for drawing.
        iY = gs_iSize - 1 - iY;

        if (m_iRow >= 0 && m_iCol >= 0
        &&  0 <= iX && iX < GetWidth() && 0 <= iY && iY < GetHeight())
        {
            m_aaiCtrlX[m_iRow][m_iCol] = iX;
            m_aaiCtrlY[m_iRow][m_iCol] = iY;
            m_aakCtrl[m_iRow][m_iCol].X() = ScreenToControl(iX);
            m_aakCtrl[m_iRow][m_iCol].Y() = ScreenToControl(iY);
            OnDisplay();
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
float QuadraticFreeForm2D::ScreenToControl (int iScreen) const
{
    return 2*iScreen/(float)gs_iSize - 0.5f;
}
//----------------------------------------------------------------------------
int QuadraticFreeForm2D::ControlToScreen (float fControl) const
{
    return (int)(gs_iSize*(0.5f*fControl + 0.25f));
}
//----------------------------------------------------------------------------
Vector2f QuadraticFreeForm2D::Evaluate (const Vector2f& rkParam) const
{
    float fS = rkParam.X();
    float fT = rkParam.Y();
    float fOmS = 1.0f - fS;
    float fOmT = 1.0f - fT;
    float fB0s = fOmS*fOmS;
    float fB0t = fOmT*fOmT;
    float fB1s = 2.0f*fOmS*fS;
    float fB1t = 2.0f*fOmT*fT;
    float fB2s = fS*fS;
    float fB2t = fT*fT;

    Vector2f kResult =
        fB0s*(fB0t*m_aakCtrl[0][0]+fB1t*m_aakCtrl[0][1]+fB2t*m_aakCtrl[0][2])+
        fB1s*(fB0t*m_aakCtrl[1][0]+fB1t*m_aakCtrl[1][1]+fB2t*m_aakCtrl[1][2])+
        fB2s*(fB0t*m_aakCtrl[2][0]+fB1t*m_aakCtrl[2][1]+fB2t*m_aakCtrl[2][2]);

    return kResult;
}
//----------------------------------------------------------------------------
void QuadraticFreeForm2D::SelectVertex (int iX, int iY)
{
    // Identify vertex within 5 pixels of mouse click.
    const int iPixels = 5;
    m_iRow = -1;
    m_iCol = -1;
    for (int iRow = 0; iRow <= 2; iRow++)
    {
        for (int iCol = 0; iCol <= 2; iCol++)
        {
            int iDX = iX - m_aaiCtrlX[iRow][iCol];
            int iDY = iY - m_aaiCtrlY[iRow][iCol];
            if (abs(iDX) <= iPixels && abs(iDY) <= iPixels)
            {
                m_iRow = iRow;
                m_iCol = iCol;
                return;
            }
        }
    }
}
//----------------------------------------------------------------------------
