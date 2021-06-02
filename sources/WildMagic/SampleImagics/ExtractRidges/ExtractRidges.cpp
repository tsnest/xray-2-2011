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
// Version: 4.7.0 (2008/04/27)

#include "ExtractRidges.h"
using namespace Wm4;

WM4_CONSOLE_APPLICATION(ExtractRidges);

//----------------------------------------------------------------------------
int ExtractRidges::Main (int, char**)
{
    ImageDouble2D kImage(System::GetPath("../../Data/Im/","Head.im"));

    // Normalize the image values to be in [0,1].
    int iQuantity = kImage.GetQuantity();
    double dMin = kImage[0], dMax = dMin;
    int i;
    for (i = 1; i < iQuantity; i++)
    {
        if (kImage[i] < dMin)
        {
            dMin = kImage[i];
        }
        else if (kImage[i] > dMax)
        {
            dMax = kImage[i];
        }
    }
    double dInvRange = 1.0/(dMax - dMin);
    for (i = 0; i < iQuantity; i++)
    {
        kImage[i] = (kImage[i] - dMin)*dInvRange;
    }

    // Use first-order centered finite differences to estimate the image
    // derivatives.  The gradient is DF = (df/dx, df/dy) and the Hessian
    // is D^2F = {{d^2f/dx^2, d^2f/dxdy}, {d^2f/dydx, d^2f/dy^2}}.
    int iXBound = kImage.GetBound(0);
    int iYBound = kImage.GetBound(1);
    ImageDouble2D kDX(iXBound, iYBound);
    ImageDouble2D kDY(iXBound, iYBound);
    ImageDouble2D kDXX(iXBound, iYBound);
    ImageDouble2D kDXY(iXBound, iYBound);
    ImageDouble2D kDYY(iXBound, iYBound);
    int iX, iY;
    for (iY = 1; iY < iYBound-1; iY++)
    {
        for (iX = 1; iX < iXBound-1; iX++)
        {
            kDX(iX,iY) = 0.5*(kImage(iX+1,iY) - kImage(iX-1,iY));
            kDY(iX,iY) = 0.5*(kImage(iX,iY+1) - kImage(iX,iY-1));
            kDXX(iX,iY) = kImage(iX+1,iY) - 2.0*kImage(iX,iY)
                + kImage(iX-1,iY);
            kDXY(iX,iY) = 0.25*(kImage(iX+1,iY+1) + kImage(iX-1,iY-1)
                - kImage(iX+1,iY-1) - kImage(iX-1,iY+1));
            kDYY(iX,iY) = kImage(iX,iY+1) - 2.0*kImage(iX,iY)
                + kImage(iX,iY+1);
        }
    }
    kDX.Save("dx.im");
    kDY.Save("dy.im");
    kDXX.Save("dxx.im");
    kDXY.Save("dxy.im");
    kDYY.Save("dyy.im");

    // The eigensolver produces eigenvalues a and b and corresponding
    // eigenvectors U and V:  D^2F*U = a*U, D^2F*V = b*V.  Define
    // P = Dot(U,DF) and Q = Dot(V,DF).  The classification is as follows.
    //   ridge:   P = 0 with a < 0
    //   valley:  Q = 0 with b > 0
    ImageDouble2D kAImage(iXBound, iYBound);
    ImageDouble2D kBImage(iXBound, iYBound);
    ImageDouble2D kPImage(iXBound, iYBound);
    ImageDouble2D kQImage(iXBound, iYBound);
    for (iY = 1; iY < iYBound-1; iY++)
    {
        for (iX = 1; iX < iXBound-1; iX++)
        {
            Vector2d kGrad(kDX(iX,iY),kDY(iX,iY));
            Matrix2d kHess(kDXX(iX,iY),kDXY(iX,iY),kDXY(iX,iY),kDYY(iX,iY));
            Eigend kES(kHess);
            kES.IncrSortEigenStuff2();
            kAImage(iX,iY) = kES.GetEigenvalue(0);
            kBImage(iX,iY) = kES.GetEigenvalue(1);
            Vector2d kU, kV;
            kES.GetEigenvector(0, kU);
            kES.GetEigenvector(1, kV);
            kPImage(iX,iY) = kU.Dot(kGrad);
            kQImage(iX,iY) = kV.Dot(kGrad);
        }
    }
    kAImage.Save("a.im");
    kBImage.Save("b.im");
    kPImage.Save("p.im");
    kQImage.Save("q.im");

    // Use a cheap classification of the pixels by testing for sign changes
    // between neighboring pixels.
    ImageRGB82D kResult(iXBound,iYBound);
    for (iY = 1; iY < iYBound-1; iY++)
    {
        for (iX = 1; iX < iXBound-1; iX++)
        {
            unsigned char ucGray = (unsigned char)(255.0f*kImage(iX,iY));

            double dPValue = kPImage(iX,iY);
            bool bIsRidge = false;
            if (dPValue*kPImage(iX-1,iY) < 0.0
            ||  dPValue*kPImage(iX+1,iY) < 0.0
            ||  dPValue*kPImage(iX,iY-1) < 0.0
            ||  dPValue*kPImage(iX,iY+1) < 0.0)
            {
                if (kAImage(iX,iY) < 0.0)
                {
                    bIsRidge = true;
                }
            }

            double dQValue = kQImage(iX,iY);
            bool bIsValley = false;
            if (dQValue*kQImage(iX-1,iY) < 0.0
            ||  dQValue*kQImage(iX+1,iY) < 0.0
            ||  dQValue*kQImage(iX,iY-1) < 0.0
            ||  dQValue*kQImage(iX,iY+1) < 0.0)
            {
                if (kBImage(iX,iY) > 0.0)
                {
                    bIsValley = true;
                }
            }

            if (bIsRidge)
            {
                if (bIsValley)
                {
                    kResult(iX,iY) = GetColor24(ucGray,0,ucGray);
                }
                else
                {
                    kResult(iX,iY) = GetColor24(ucGray,0,0);
                }
            }
            else if (bIsValley)
            {
                kResult(iX,iY) = GetColor24(0,0,ucGray);
            }
            else
            {
                kResult(iX,iY) = GetColor24(ucGray,ucGray,ucGray);
            }
        }
    }
    kResult.Save("result.im");

    return 0;
}
//----------------------------------------------------------------------------
