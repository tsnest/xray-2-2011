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

#include "MapTextureToQuad.h"

WM4_WINDOW_APPLICATION(MapTextureToQuad);

//----------------------------------------------------------------------------
MapTextureToQuad::MapTextureToQuad ()
    :
    WindowApplication2("MapTextureToQuad",0,0,256,256,
        ColorRGBA(1.0f,1.0f,1.0f,1.0f))
{
    System::InsertDirectory("Data");

    m_pkMap = 0;
    m_pkImage = 0;
    m_bMouseDown = false;
    m_iSelected = -1;
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // source bitmap
    m_pkImage = Image::Load("MagicianSmallReflectY");
    if (!m_pkImage)
    {
        return false;
    }

    // The quadrilateral to which the image is perspectively mapped.  The
    // default is the original image rectangle (the initial mapping is the
    // identity).
    m_akVertex[0] = Vector2f(0.0f,0.0f);
    m_akVertex[1] = Vector2f(m_pkImage->GetBound(0)-1.0f,0.0f);
    m_akVertex[2] = Vector2f(m_pkImage->GetBound(0)-1.0f,
        m_pkImage->GetBound(1)-1.0f);
    m_akVertex[3] = Vector2f(0.0f,m_pkImage->GetBound(1)-1.0f);

    Vector2f kOffset(0.5f*(GetWidth()-m_pkImage->GetBound(0)),
        0.5f*(GetHeight()-m_pkImage->GetBound(1)));
    for (int i = 0; i < 4; i++)
    {
        m_akVertex[i] += kOffset;
    }

    CreateMapping();
    return true;
}
//----------------------------------------------------------------------------
void MapTextureToQuad::OnTerminate ()
{
    WM4_DELETE m_pkImage;
    WM4_DELETE m_pkMap;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnMouseClick (int iButton, int iState, int iX,
    int iY, unsigned int)
{
    if (iButton != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    if (iState == MOUSE_DOWN)
    {
        m_bMouseDown = true;
        SelectVertex(Vector2f((float)iX,(float)iY));
        return true;
    }

    if (iState == MOUSE_UP)
    {
        m_bMouseDown = false;
        if (m_iSelected >= 0
        &&  0 <= iX && iX < GetWidth()
        &&  0 <= iY && iY < GetHeight())
        {
            UpdateQuadrilateral(Vector2f((float)iX,(float)iY));
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnMotion (int, int iX, int iY, unsigned int)
{
    if (m_bMouseDown)
    {
        if (m_iSelected >= 0
        &&  0 <= iX && iX < GetWidth()
        &&  0 <= iY && iY < GetHeight())
        {
            UpdateQuadrilateral(Vector2f((float)iX,(float)iY));
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void MapTextureToQuad::CreateMapping ()
{
    // Create the new perspective mapping from the *target* quadrilateral to
    // the *source* square bitmap.  The mapping is in this direction to avoid
    // holes in the drawn quadrilateral.
    WM4_DELETE m_pkMap;
#ifdef USE_HM_QUAD_TO_SQR
    m_pkMap = WM4_NEW HmQuadToSqrf(m_akVertex[0],m_akVertex[1],m_akVertex[2],
        m_akVertex[3]);
#else
    m_pkMap = WM4_NEW BiQuadToSqrf(m_akVertex[0],m_akVertex[1],m_akVertex[2],
        m_akVertex[3]);
#endif

    // compute axis-aligned bounding box
    int iXMin = GetWidth(), iXMax = 0, iYMin = GetWidth(), iYMax = 0;
    for (int i = 0; i < 4; i++)
    {
        if (iXMin > (int)m_akVertex[i].X())
        {
            iXMin = (int)m_akVertex[i].X();
        }

        if (iXMax < (int)m_akVertex[i].X())
        {
            iXMax = (int)m_akVertex[i].X();
        }

        if (iYMin > (int)m_akVertex[i].Y())
        {
            iYMin = (int)m_akVertex[i].Y();
        }

        if (iYMax < (int)m_akVertex[i].Y())
        {
            iYMax = (int)m_akVertex[i].Y();
        }
    }

    // draw perspective mapping of image (...inefficient drawing...)
    ClearScreen();
    int iXSize = m_pkImage->GetBound(0);
    int iYSize = m_pkImage->GetBound(1);
    unsigned char* aucData = m_pkImage->GetData();
    for (int iTrgY = iYMin; iTrgY <= iYMax; iTrgY++)
    {
        Vector2f kQuad;
        kQuad.Y() = (float)iTrgY;

        int iXStart = iXMin;
        while (iXStart <= iXMax)
        {
            kQuad.X() = (float)iXStart;
            if (PointInPolygon2f(4,m_akVertex).ContainsQuadrilateral(kQuad))
            {
                break;
            }
            iXStart++;
        }

        int iXFinal = iXMax;
        while ( iXFinal >= iXMin )
        {
            kQuad.X() = (float)iXFinal;
            if (PointInPolygon2f(4,m_akVertex).ContainsQuadrilateral(kQuad))
            {
                break;
            }
            iXFinal--;
        }

        for (int iTrgX = iXStart; iTrgX <= iXFinal; iTrgX++)
        {
            // transform point to unit square
            kQuad.X() = (float)iTrgX;
            Vector2f kSquare = m_pkMap->Transform(kQuad);

            // convert to bitmap coordinates (using clamping)
            int iSrcX = (int)((iXSize-1)*kSquare.X());
            if (iSrcX < 0)
            {
                iSrcX = 0;
            }
            else if (iSrcX >= iXSize)
            {
                iSrcX = iXSize-1;
            }

            int iSrcY = (int)((iYSize-1)*kSquare.Y());
            if (iSrcY < 0)
            {
                iSrcY = 0;
            }
            else if (iSrcY >= iYSize)
            {
                iSrcY = iYSize-1;
            }

            int iSrcIndex = 3*(iSrcX+iXSize*iSrcY);
            unsigned char* aucSrcRGB = aucData + iSrcIndex;
            int iTrgIndex = iTrgX + GetWidth()*iTrgY;
            unsigned char* aucTrgRGB =
                (unsigned char*)(m_akScreen + iTrgIndex);

            for (int i = 0; i < 3; i++)
            {
                aucTrgRGB[i] = aucSrcRGB[2-i];
            }
        }
    }
    OnDisplay();
}
//----------------------------------------------------------------------------
void MapTextureToQuad::SelectVertex (const Vector2f& rkPos)
{
    // identify vertex within 5 pixels of mouse click
    const float fPixels = 5.0f;
    m_iSelected = -1;
    for (int i = 0; i < 4; i++)
    {
        Vector2f kDiff = rkPos - m_akVertex[i];
        if (kDiff.Length() <= fPixels)
        {
            m_iSelected = i;
            break;
        }
    }
}
//----------------------------------------------------------------------------
void MapTextureToQuad::UpdateQuadrilateral (const Vector2f& rkPos)
{
    // quadrilateral must remain convex
    int iPrev = (m_iSelected > 0 ? m_iSelected - 1 : 3);
    int iNext = (m_iSelected < 3 ? m_iSelected + 1 : 0);
    Vector2f kDiff1 = rkPos - m_akVertex[iPrev];
    Vector2f kDiff2 = m_akVertex[iNext] - rkPos;
    float fDet = kDiff1.X()*kDiff2.Y() - kDiff1.Y()*kDiff2.X();
    if (fDet > 0.0f)
    {
        m_akVertex[m_iSelected] = rkPos;
        CreateMapping();
    }
}
//----------------------------------------------------------------------------
