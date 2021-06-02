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
// Version: 4.0.1 (2006/09/07)

#include "ExtractLevelCurves.h"

WM4_CONSOLE_APPLICATION(ExtractLevelCurves);

// comment this out for LINEAR
#define USE_BILINEAR

ImageRGB82D* ExtractLevelCurves::ms_pkColor = 0;
unsigned int ExtractLevelCurves::ms_uiColor = 0;
float ExtractLevelCurves::ms_fMultiply = 1.0f/(float)MAGNIFY;

//----------------------------------------------------------------------------
void ExtractLevelCurves::DrawPixel (int iX, int iY)
{
    if (0 <= iX && iX < ms_pkColor->GetBound(0)
    &&  0 <= iY && iY < ms_pkColor->GetBound(1))
    {
        (*ms_pkColor)(iX,iY) = ms_uiColor;
    }
}
//----------------------------------------------------------------------------
bool ExtractLevelCurves::Extract ()
{
    ms_pkColor = WM4_NEW ImageRGB82D(MAGNIFY*SIZE,MAGNIFY*SIZE);

    // load image for level set extraction
    ImageInt2D kImage(System::GetPath("../../Data/Im/","Head.im"));
    if (kImage.GetDimensions() == 0)
    {
        // cannot load the image
        return false;
    }

    // get the extremes
    int iMin = kImage[0], iMax = iMin;
    for (int i = 1; i < kImage.GetQuantity(); i++)
    {
        int iValue = kImage[i];
        if (iValue < iMin)
        {
            iMin = iValue;
        }
        else if (iValue > iMax)
        {
            iMax = iValue;
        }
    }
    int iRange = iMax - iMin;

    // Generate a color subimage to superimpose level sets.  Process the
    // subimage with upper left corner (100,100) and of size 32x32.
    for (int iY = 0; iY < SIZE; iY++)
    {
        for (int iX = 0; iX < SIZE; iX++)
        {
            for (int iDY = 0; iDY < MAGNIFY; iDY++)
            {
                float fY = YPOS + iY + ms_fMultiply*iDY;
                for (int iDX = 0; iDX < MAGNIFY; iDX++)
                {
                    float fX = XPOS + iX + ms_fMultiply*iDX;
#ifdef USE_BILINEAR
                    float fInterp = BilinearInterpolate(kImage,fX,fY);
#else
                    float fInterp = LinearInterpolate(kImage,fX,fY);
#endif
                    fInterp = (fInterp - iMin)/(float)iRange;
                    unsigned char ucGrey = (unsigned char)(255.0f*fInterp);
                    (*ms_pkColor)(MAGNIFY*iX+iDX,MAGNIFY*iY+iDY) =
                        GetColor24(ucGrey,ucGrey,ucGrey);
                }
            }
        }
    }

    // extract the level set
    std::vector<Vector2f> kVArray;
    std::vector<EdgeKey> kEArray;
#ifdef USE_BILINEAR
    ExtractCurveSquares kLSE(kImage.GetBound(0),kImage.GetBound(1),
        (int*)kImage.GetData());
#else
    ExtractCurveTris kLSE(kImage.GetBound(0),kImage.GetBound(1),
        (int*)kImage.GetData());
#endif
    kLSE.ExtractContour(512,kVArray,kEArray);
    kLSE.MakeUnique(kVArray,kEArray);

    // draw the edges in the subimage
    ms_uiColor = GetColor24(0,255,0);
    for (int iE = 0; iE < (int)kEArray.size(); iE++)
    {
        const EdgeKey& rkEdge = kEArray[iE];

        Vector2f kV0 = kVArray[rkEdge.V[0]];
        int iX0 = (int)((kV0[0] - XPOS)*MAGNIFY);
        int iY0 = (int)((kV0[1] - YPOS)*MAGNIFY);

        Vector2f kV1 = kVArray[rkEdge.V[1]];
        int iX1 = (int)((kV1[0] - XPOS)*MAGNIFY);
        int iY1 = (int)((kV1[1] - YPOS)*MAGNIFY);

        Line2D(iX0,iY0,iX1,iY1,DrawPixel);
    }

    // draw the vertices in the subimage
    ms_uiColor = GetColor24(255,0,0);
    for (int iV = 0; iV < (int)kVArray.size(); iV++)
    {
        Vector2f kV = kVArray[iV];
        int iX = (int)((kV[0] - XPOS)*MAGNIFY);
        int iY = (int)((kV[1] - YPOS)*MAGNIFY);
        DrawPixel(iX,iY);
    }

#ifdef USE_BILINEAR
    ms_pkColor->Save("BilinearZoom.im");
#else
    ms_pkColor->Save("LinearZoom.im");
#endif

    WM4_DELETE ms_pkColor;
    return true;
}
//----------------------------------------------------------------------------
float ExtractLevelCurves::LinearInterpolate (const ImageInt2D& rkImage,
    float fX, float fY) const
{
    int iX = (int) fX;
    if (iX < 0 || iX >= rkImage.GetBound(0))
    {
        return 0.0f;
    }

    int iY = (int) fY;
    if (iY < 0 || iY >= rkImage.GetBound(1))
    {
        return 0.0f;
    }

    float fDX = fX - iX, fDY = fY - iY;

    int iXBound = rkImage.GetBound(0);
    const int* aiData = (const int*)rkImage.GetData();
    int i00 = iX + iXBound*iY;
    int i10 = i00 + 1;
    int i01 = i00 + iXBound;
    int i11 = i10 + iXBound;
    float fF00 = (float)aiData[i00];
    float fF10 = (float)aiData[i10];
    float fF01 = (float)aiData[i01];
    float fF11 = (float)aiData[i11];
    float fInterp;

    int iXParity = (iX & 1), iYParity = (iY & 1);
    if (iXParity == iYParity)
    {
        if (fDX + fDY <= 1.0f)
        {
            fInterp = fF00 + fDX*(fF10-fF00) + fDY*(fF01-fF00);
        }
        else
        {
            fInterp = fF10+fF01-fF11 + fDX*(fF11-fF01) + fDY*(fF11-fF10);
        }
    }
    else
    {
        if (fDY <= fDX)
        {
            fInterp = fF00 + fDX*(fF10-fF00) + fDY*(fF11-fF10);
        }
        else
        {
            fInterp = fF00 + fDX*(fF11-fF01) + fDY*(fF01-fF00);
        }
    }

    return fInterp;
}
//----------------------------------------------------------------------------
float ExtractLevelCurves::BilinearInterpolate (const ImageInt2D& rkImage,
    float fX, float fY) const
{
    int iX = (int) fX;
    if (iX < 0 || iX >= rkImage.GetBound(0))
    {
        return 0.0;
    }

    int iY = (int) fY;
    if (iY < 0 || iY >= rkImage.GetBound(1))
    {
        return 0.0;
    }

    float fDX = fX - iX, fDY = fY - iY;
    float fOmDX = 1.0f - fDX, fOmDY = 1.0f - fDY;

    int iXBound = rkImage.GetBound(0);
    const int* aiData = (const int*)rkImage.GetData();
    int i00 = iX + iXBound*iY;
    int i10 = i00 + 1;
    int i01 = i00 + iXBound;
    int i11 = i10 + iXBound;
    float fF00 = (float)aiData[i00];
    float fF10 = (float)aiData[i10];
    float fF01 = (float)aiData[i01];
    float fF11 = (float)aiData[i11];

    float fInterp = fOmDX*(fOmDY*fF00+fDY*fF01)+fDX*(fOmDY*fF10+fDY*fF11);
    return fInterp;
}
//----------------------------------------------------------------------------
int ExtractLevelCurves::Main (int, char**)
{
    return (Extract() ? 0 : -1);
}
//----------------------------------------------------------------------------
