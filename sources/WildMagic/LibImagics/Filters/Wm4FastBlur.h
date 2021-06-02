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
// Version: 4.0.1 (2006/09/21)

#ifndef WM4FASTBLUR_H
#define WM4FASTBLUR_H


// The idea is to represent the blurred image as f(x,s) in terms of position
// x and scale s.  Gaussian blurring is accomplished by using the input image
// I(x,s0) as the initial image (of scale s0 > 0) for the partial differential
// equation
//   s*df/ds = s^2*Laplacian(f)
// where the Laplacian operator is
//   Laplacian = (d/dx)^2, dimension 1
//   Laplacian = (d/dx)^2+(d/dy)^2, dimension 2
//   Laplacian = (d/dx)^2+(d/dy)^2+(d/dz)^2, dimension 3
//
// The term s*df/ds is approximated by
//   s*df(x,s)/ds = (f(x,b*s)-f(x,s))/ln(b)
// for b > 1, but close to 1, where ln(b) is the natural logarithm of b.  If
// you take the limit of the right-hand side as b approaches 1, you get the
// left-hand side.
//
// The term s^2*((d/dx)^2)f is approximated by
//   s^2*((d/dx)^2)f = (f(x+h*s,s)-2*f(x,s)+f(x-h*s,s))/h^2
// for h > 0, but close to zero.
//
// Equating the approximations for the left-hand side and the right-hand side
// of the partial differential equation leads to the numerical method used in
// this code.
//
// For iterative application of these functions, the caller is responsible
// for constructing a geometric sequence of scales,
//   s0, s1 = s0*b, s2 = s1*b = s0*b^2, ...
// where the base b satisfies 1 < b < exp(0.5*d) where d is the dimension of
// the image.  The upper bound on b guarantees stability of the finite
// difference method used to approximate the partial differential equation.
// The method assumes a pixel size of h = 1.
//
//
// Sample usage in 2D:
//
// const int iXBound = 256, iYBound = 256;
// float** aakImage = WM4_NEW float*[iYBound];
// float** aakTemp = WM4_NEW float*[iYBound];
// int iX, iY;
// for (iY = 0; iY < iYBound; iY++)
// {
//     aakImage[y] = WM4_NEW float[iXBound];
//     aakTemp[y] = WM4_NEW float[iXBound];
// }
// <initialization of aakImage[iY][iX] goes here...>
// const int iMax = <number of passes to blur>;
// double dScale = 1.0, dLogBase = 0.125, dBase = exp(0.125);
// for (int i = 1; i <= iMax; i++, dScale *= dBase)
// {
//     FastBlur2(iXBound,iYBound,aakImage,aakTemp,dScale,dLogBase);
//     <use the blurred image aakImage here if desired>;
// }
// for (iY = 0; iY < iYBound; iY++)
// {
//     WM4_DELETE[] aakImage[iY];
//     WM4_DELETE[] aakTemp[iY];
// }
// WM4_DELETE[] aakImage;
// WM4_DELETE[] aakTemp;

#include "Wm4ImagicsLIB.h"
#include "Wm4System.h"

namespace Wm4
{

// Explicit instantiation of the template code is done for PixelType of:
// short, int, float, or double.  The algorithm requires a temporary image
// of the same size as the original image.  The caller is responsible for
// providing this.

template <class PixelType>
WM4_IMAGICS_ITEM void FastBlur1 (int iXBound, PixelType* akImage,
    PixelType* akTemp, double dScale, double dLogBase);

template <class PixelType>
WM4_IMAGICS_ITEM void FastBlur2 (int iXBound, int iYBound,
    PixelType** aakImage, PixelType** aakTemp, double dScale,
    double dLogBase);

template <class PixelType>
WM4_IMAGICS_ITEM void FastBlur3 (int iXBound, int iYBound, int iZBound,
    PixelType*** aaakImage, PixelType*** aaakTemp, double dScale,
    double dLogBase);

}

#endif
