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
// Version: 4.0.0 (2006/09/06)

#ifndef WM4UTILITYREALM_H
#define WM4UTILITYREALM_H

#include "Wm4GraphicsLIB.h"
#include "Wm4RealM.h"
#include "Wm4UtilityRealV.h"

namespace Wm4
{
// Set all the matrix entries to zero.
template <typename Real, int Rows, int Cols>
void MakeZero (RealM<Real,Rows,Cols>& rkM);

// Create the tensor-product matrix M = U*V^T, where U is an r-by-1 vector
// and V is an c-by-1 vector.  The product is an r-by-c matrix.
template <typename Real, int Rows, int Cols>
void MakeTensorProduct (const RealV<Real,Rows>& rkU,
    const RealV<Real,Cols>& rkV, RealM<Real,Rows,Cols>& rkM);

// Create the n-by-n identity matrix.
template <typename Real, int N>
void MakeIdentity (RealM<Real,N,N>& rkM);

// Set or get rows or columns of matrices.
template <typename Real, int Rows, int Cols>
void SetRow (RealM<Real,Rows,Cols>& rkM, int iRow,
    const RealV<Real,Cols>& rkV);

template <typename Real, int Rows, int Cols>
RealV<Real,Cols> GetRow (const RealM<Real,Rows,Cols>& rkM, int iRow);

template <typename Real, int Rows, int Cols>
void SetColumn (RealM<Real,Rows,Cols>& rkM, int iCol,
    const RealV<Real,Rows>& rkV);

template <typename Real, int Rows, int Cols>
RealV<Real,Rows> GetColumn (const RealM<Real,Rows,Cols>& rkM, int iCol);

// Construct the transpose M^T of the input matrix M.
template <typename Real, int Rows, int Cols>
RealM<Real,Cols,Rows> Transpose (const RealM<Real,Rows,Cols>& rkM);

// Compute U = M*V, where M is an r-by-c matrix, V is a c-by-1 vector, and
// U is an r-by-1 vector.
template <typename Real, int Rows, int Cols>
RealV<Real,Rows> operator* (const RealM<Real,Rows,Cols>& rkM,
    const RealV<Real,Cols>& rkV);

// Compute U = V*M, where M is an r-by-c matrix, V is a 1-by-r vector, and
// U is a 1-by-c vector.
template <typename Real, int Rows, int Cols>
RealV<Real,Cols> operator* (const RealV<Real,Rows>& rkV,
    const RealM<Real,Rows,Cols>& rkM);

// Compute U^T*M*V, where M is an r-by-c matrix, U is an r-by-1 vector, and
// V is a c-by-1 vector.
template <typename Real, int Rows, int Cols>
Real QuadraticForm (const RealV<Real,Rows>& rkU,
    const RealM<Real,Rows,Cols>& rkM, const RealV<Real,Cols>& rkV);

// Compute M = A*B, where all matrices are n-by-n.
template <typename Real, int N>
RealM<Real,N,N> operator* (const RealM<Real,N,N>& rkA,
    const RealM<Real,N,N>& rkB);

// Compute M = A*B, where A is r-by-m, B is m-by-c, and M is r-by-c.
template <typename Real, int Rows, int Mids, int Cols>
RealM<Real,Rows,Cols> MulAB (const RealM<Real,Rows,Mids>& rkA,
    const RealM<Real,Mids,Cols>& rkB);

// Compute M = A*B^T, where A is r-by-m, B is c-by-m, and M is r-by-c.
template <typename Real, int Rows, int Mids, int Cols>
RealM<Real,Rows,Cols> MulABTrn (const RealM<Real,Rows,Mids>& rkA,
    const RealM<Real,Cols,Mids>& rkB);

// Compute M = A^T*B, where A is m-by-r, B is m-by-c, and M is r-by-c.
template <typename Real, int Rows, int Mids, int Cols>
RealM<Real,Rows,Cols> MulATrnB (const RealM<Real,Mids,Rows>& rkA,
    const RealM<Real,Mids,Cols>& rkB);

// Compute M = A^T*B^T, where A is m-by-r, B is c-by-m, and M is r-by-c.
template <typename Real, int Rows, int Mids, int Cols>
RealM<Real,Rows,Cols> MulATrnBTrn (const RealM<Real,Mids,Rows>& rkA,
    const RealM<Real,Cols,Mids>& rkB);

// Compute M*D, where M is an n-by-n matrix and D is an n-by-n diagonal
// matrix.  The diagonal matrix is stored as an n-vector.
template <typename Real, int N>
RealM<Real,N,N> MulMD (const RealM<Real,N,N>& rkM, const RealV<Real,N>& rkD);

// Compute D*M, where D is an n-by-n diagonal matrix and M is an n-by-n
// matrix.  The diagonal matrix is stored as an n-vector.
template <typename Real, int N>
RealM<Real,N,N> MulDM (const RealV<Real,N>& rkD, const RealM<Real,N,N>& rkM);

#include "Wm4UtilityRealM.inl"
}

#endif
