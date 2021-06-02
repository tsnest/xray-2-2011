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

#include "ClodPolyline.h"

WM4_WINDOW_APPLICATION(ClodPolyline);

const int g_iSize = 256;

//----------------------------------------------------------------------------
ClodPolyline::ClodPolyline ()
    :
    WindowApplication2("ClodPolyline",0,0,g_iSize,g_iSize,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    m_pkPolyline = 0;
}
//----------------------------------------------------------------------------
bool ClodPolyline::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // generate points on unit circle, then adjust the distances to center
    int iVQuantity = 16;
    Vector3f* akVertex = WM4_NEW Vector3f[iVQuantity];
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        float fAngle = Mathf::TWO_PI*i/iVQuantity;
        akVertex[i].X() = Mathf::Cos(fAngle);
        akVertex[i].Y() = Mathf::Sin(fAngle);
        akVertex[i].Z() = 0.0f;

        float fAdjust = 1.0f + 0.25f*Mathf::SymmetricRandom();
        akVertex[i] *= fAdjust;
    }

    m_pkPolyline = WM4_NEW Polyline3(iVQuantity,akVertex,true);

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void ClodPolyline::OnTerminate ()
{
    WM4_DELETE m_pkPolyline;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void ClodPolyline::OnDisplay ()
{
    ClearScreen();

    int iEQuantity = m_pkPolyline->GetEdgeQuantity();
    const int* aiEdge = m_pkPolyline->GetEdges();
    const Vector3f* akVertex = m_pkPolyline->GetVertices();

    Vector3f kV;
    int i;

    for (i = 0; i < m_pkPolyline->GetVertexQuantity(); i++)
    {
        kV = akVertex[i];
        int iX = int(0.25f*g_iSize*(kV.X()+2.0f));
        int iY = g_iSize - 1 - (int)(0.25f*g_iSize*(kV.Y()+2.0f));
        for (int iDY = -1; iDY <= 1; iDY++)
        {
            for (int iDX = -1; iDX <= 1; iDX++)
            {
                SetPixel(iX+iDX,iY+iDY,Color(0,0,0));
            }
        }
    }

    for (i = 0; i < iEQuantity; i++)
    {
        kV = akVertex[aiEdge[2*i]];
        int iX0 = int(0.25f*g_iSize*(kV.X()+2.0f));
        int iY0 = g_iSize - 1 - (int)(0.25f*g_iSize*(kV.Y()+2.0f));

        kV = akVertex[aiEdge[2*i+1]];
        int iX1 = int(0.25*g_iSize*(kV.X()+2.0));
        int iY1 = g_iSize - 1 - (int)(0.25f*g_iSize*(kV.Y()+2.0f));
        DrawLine(iX0,iY0,iX1,iY1,Color(0,0,0));
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool ClodPolyline::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication2::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    int iLOD, iMaxLOD;

    switch (ucKey)
    {
    case '+':  // increase level of detail
    case '=':
        iLOD = m_pkPolyline->GetLevelOfDetail();
        iMaxLOD = m_pkPolyline->GetMaxLevelOfDetail();
        if (iLOD < iMaxLOD)
        {
            m_pkPolyline->SetLevelOfDetail(iLOD+1);
            OnDisplay();
        }
        return true;
    case '-':  // decrease level of detail
    case '_':
        iLOD = m_pkPolyline->GetLevelOfDetail();
        if (iLOD > 0)
        {
            m_pkPolyline->SetLevelOfDetail(iLOD-1);
            OnDisplay();
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
