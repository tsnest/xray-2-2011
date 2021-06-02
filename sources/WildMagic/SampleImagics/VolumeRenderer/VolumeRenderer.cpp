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

#include "VolumeRenderer.h"

WM4_WINDOW_APPLICATION(VolumeRenderer);

// The initial size of the application window is irrelevant.  OnPrecreate will
// change the size based on the selected image.

//----------------------------------------------------------------------------
VolumeRenderer::VolumeRenderer ()
    :
    WindowApplication2("VolumeRenderer",0,0,0,0,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_bButtonDown = false;
    m_iBound = 0;
    m_fGamma = 0.25f;

    // For the Mac, to select files to display since there is no concept of
    // command-line parameters.
    m_bLaunchFileDialog = true;
}
//----------------------------------------------------------------------------
bool VolumeRenderer::OnPrecreate ()
{
    if (!WindowApplication2::OnPrecreate())
    {
        return false;
    }

    char* acFilename = 0;
    Application::TheCommand->Filename(acFilename);
    if (!acFilename)
    {
        // input filename must be specified on the command line
        return false;
    }

    // load image, must be 3D and pixels must be unsigned char
    int iDimensions, iQuantity, iRTTI, iSizeOf;
    int* aiBound = 0;
    char* acData = 0;
    bool bLoaded = Lattice::LoadRaw(acFilename,iDimensions,aiBound,
        iQuantity,iRTTI,iSizeOf,acData);
    if (!bLoaded || iDimensions != 3 || iRTTI != Euchar::GetRTTI())
    {
        WM4_DELETE[] acData;
        WM4_DELETE[] acFilename;
        return false;
    }

    ImageUChar3D* pkImage = WM4_NEW ImageUChar3D(aiBound[0],aiBound[1],
        aiBound[2],(Euchar*)acData);

    // get maximum dimension size
    int iBMax = pkImage->GetBound(0);
    if (pkImage->GetBound(1) > iBMax)
    {
        iBMax = pkImage->GetBound(1);
    }
    if (pkImage->GetBound(2) > iBMax)
    {
        iBMax = pkImage->GetBound(2);
    }
    m_iBound = 2*iBMax;
    m_fHBound = (float)iBMax;

    m_pkRT = WM4_NEW RayTrace(pkImage,m_fGamma);
    WM4_DELETE pkImage;

    // resize application window
    m_iWidth = m_iBound;
    m_iHeight = m_iBound;

    WM4_DELETE[] acFilename;
    WM4_DELETE[] aiBound;
    return true;
}
//----------------------------------------------------------------------------
bool VolumeRenderer::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    m_pkRT->Trace(1);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void VolumeRenderer::OnTerminate ()
{
    WM4_DELETE m_pkRT;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void VolumeRenderer::OnDisplay ()
{
    m_pkRT->DrawWireFrame();

    for (int iY = 0; iY < m_iBound; iY++)
    {
        for (int iX = 0; iX < m_iBound; iX++)
        {
            int i = iX + m_iBound*iY;
            unsigned int uiValue = m_pkRT->GetRendered(i);
            unsigned char ucR = GetRed24(uiValue);
            unsigned char ucG = GetGreen24(uiValue);
            unsigned char ucB = GetBlue24(uiValue);
            SetPixel(iX,iY,Color(ucR,ucG,ucB));
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool VolumeRenderer::OnMouseClick (int iButton, int iState, int iX, int iY,
    unsigned int)
{
    if (iButton != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    if (iState == MOUSE_DOWN)
    {
        m_bButtonDown = true;
        m_fX0 = (iX-m_fHBound)/m_fHBound;
        m_fY0 = (iY-m_fHBound)/m_fHBound;
    }
    else
    {
        m_bButtonDown = false;
        m_fX1 = (iX-m_fHBound)/m_fHBound;
        m_fY1 = (iY-m_fHBound)/m_fHBound;

        if (m_pkRT->MoveTrackBall(m_fX0,m_fY0,m_fX1,m_fY1))
        {
            m_pkRT->Trace(1);
            OnDisplay();
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool VolumeRenderer::OnMotion (int, int iX, int iY, unsigned int)
{
    if (!m_bButtonDown)
    {
        return false;
    }

    // save old frame
    float m_aafSFrame[3][3];
    int iDX, iDY;
    for (iDY = 0; iDY < 3; iDY++)
    {
        for (iDX = 0; iDX < 3; iDX++)
        {
            m_aafSFrame[iDY][iDX] = m_pkRT->Frame(iDY,iDX);
        }
    }
        
    m_fX1 = (iX-m_fHBound)/m_fHBound;
    m_fY1 = (iY-m_fHBound)/m_fHBound;
    if (m_pkRT->MoveTrackBall(m_fX0,m_fY0,m_fX1,m_fY1))
    {
        // Trace every second ray, then fill in with nearest neighbor values.
        m_pkRT->Trace(2);
        OnDisplay();

        // restore old frame
        for (iDY = 0; iDY < 3; iDY++)
        {
            for (iDX = 0; iDX < 3; iDX++)
            {
                m_pkRT->Frame(iDY,iDX) = m_aafSFrame[iDY][iDX];
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool VolumeRenderer::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication2::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    switch (ucKey)
    {
    case '+':
        m_fGamma /= 1.1f;
        m_pkRT->Correction(m_fGamma);
        OnDisplay();
        return true;
    case '-':
        m_fGamma *= 1.1f;
        m_pkRT->Correction(m_fGamma);
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
