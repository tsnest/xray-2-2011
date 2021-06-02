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

//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
void MakeZero (RealM<Real,Rows,Cols>& rkM)
{
    Real* afEntry = (Real*)rkM;
    RMLOOP_RETN(afEntry[i] = (Real)0)
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
void MakeTensorProduct (const RealV<Real,Rows>& rkU,
    const RealV<Real,Cols>& rkV, RealM<Real,Rows,Cols>& rkM)
{
    for (int iRow = 0; iRow < Rows; iRow++)
    {
        for (int iCol = 0; iCol < Cols; iCol++)
        {
            rkM[iRow][iCol] = rkU[iRow]*rkV[iCol];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real, int N>
void MakeIdentity (RealM<Real,N,N>& rkM)
{
    for (int iRow = 0; iRow < N; iRow++)
    {
        for (int iCol = 0; iCol < N; iCol++)
        {
            if (iRow == iCol)
            {
                rkM[iRow][iCol] = (Real)1;
            }
            else
            {
                rkM[iRow][iCol] = (Real)0;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
void SetRow (RealM<Real,Rows,Cols>& rkM, int iRow,
    const RealV<Real,Cols>& rkV)
{
    for (int iCol = 0; iCol < Cols; iCol++)
    {
        rkM[iRow][iCol] = rkV[iCol];
    }
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealV<Real,Cols> GetRow (const RealM<Real,Rows,Cols>& rkM, int iRow)
{
    RealV<Real,Cols> kV;
    for (int iCol = 0; iCol < Cols; iCol++)
    {
        kV[iCol] = rkM[iRow][iCol];
    }
    return kV;
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
void SetColumn (RealM<Real,Rows,Cols>& rkM, int iCol,
    const RealV<Real,Rows>& rkV)
{
    for (int iRow = 0; iRow < Rows; iRow++)
    {
        rkM[iRow][iCol] = rkV[iRow];
    }
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealV<Real,Rows> GetColumn (const RealM<Real,Rows,Cols>& rkM, int iCol)
{
    RealV<Real,Cols> kV;
    for (int iRow = 0; iRow < Rows; iRow++)
    {
        kV[iRow] = rkM[iRow][iCol];
    }
    return kV;
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealM<Real,Cols,Rows> Transpose (const RealM<Real,Rows,Cols>& rkM)
{
    RealM<Real,Cols,Rows> kTranspose;
    for (int iRow = 0; iRow < Rows; iRow++)
    {
        for (int iCol = 0; iCol < Cols; iCol++)
        {
            kTranspose[iCol][iRow] = rkM[iRow][iCol];
        }
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealV<Real,Rows> operator* (const RealM<Real,Rows,Cols>& rkM,
    const RealV<Real,Cols>& rkV)
{
    RealV<Real,Rows> kMV;
    for (int iRow = 0; iRow < Rows; iRow++)
    {
        Real fSum = (Real)0;
        for (int iCol = 0; iCol < Cols; iCol++)
        {
            fSum += rkM[iRow][iCol]*rkV[iCol];
        }
        kMV[iRow] = fSum;
    }
    return kMV;
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
RealV<Real,Cols> operator* (const RealV<Real,Rows>& rkV,
    const RealM<Real,Rows,Cols>& rkM)
{
    RealV<Real,Cols> kVM;
    for (int iCol = 0; iCol < Cols; iCol++)
    {
        Real fSum = (Real)0;
        for (int iRow = 0; iRow < Rows; iRow++)
        {
            fSum += rkV[iRow]*rkM[iRow][iCol];
        }
        kVM[iCol] = fSum;
    }
    return kVM;
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Cols>
Real QuadraticForm (const RealV<Real,Rows>& rkU,
    const RealM<Real,Rows,Cols>& rkM, const RealV<Real,Cols>& rkV)
{
    RealV<Real,Rows> kMV = rkM*rkV;
    return Dot<Real,Rows>(rkU,kMV);
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealM<Real,N,N> operator* (const RealM<Real,N,N>& rkA,
    const RealM<Real,N,N>& rkB)
{
    RealM<Real,N,N> kAB;
    for (int iRow = 0; iRow < N; iRow++)
    {
        for (int iCol = 0; iCol < N; iCol++)
        {
            Real fSum = (Real)0;
            for (int iMid = 0; iMid < N; iMid++)
            {
                fSum += rkA[iRow][iMid]*rkB[iMid][iCol];
            }
            kAB[iRow][iCol] = fSum;
        }
    }
    return kAB;
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Mids, int Cols>
RealM<Real,Rows,Cols> MulAB (const RealM<Real,Rows,Mids>& rkA,
    const RealM<Real,Mids,Cols>& rkB)
{
    RealM<Real,Rows,Cols> kAB;
    for (int iRow = 0; iRow < Rows; iRow++)
    {
        for (int iCol = 0; iCol < Cols; iCol++)
        {
            Real fSum = (Real)0;
            for (int iMid = 0; iMid < Mids; iMid++)
            {
                fSum += rkA[iRow][iMid]*rkB[iMid][iCol];
            }
            kAB[iRow][iCol] = fSum;
        }
    }
    return kAB;
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Mids, int Cols>
RealM<Real,Rows,Cols> MulABTrn (const RealM<Real,Rows,Mids>& rkA,
    const RealM<Real,Cols,Mids>& rkB)
{
    RealM<Real,Rows,Cols> kABTrn;
    for (int iRow = 0; iRow < Rows; iRow++)
    {
        for (int iCol = 0; iCol < Cols; iCol++)
        {
            Real fSum = (Real)0;
            for (int iMid = 0; iMid < Mids; iMid++)
            {
                fSum += rkA[iRow][iMid]*rkB[iCol][iMid];
            }
            kABTrn[iRow][iCol] = fSum;
        }
    }
    return kABTrn;
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Mids, int Cols>
RealM<Real,Rows,Cols> MulATrnB (const RealM<Real,Mids,Rows>& rkA,
    const RealM<Real,Mids,Cols>& rkB)
{
    RealM<Real,Rows,Cols> kATrnB;
    for (int iRow = 0; iRow < Rows; iRow++)
    {
        for (int iCol = 0; iCol < Cols; iCol++)
        {
            Real fSum = (Real)0;
            for (int iMid = 0; iMid < Mids; iMid++)
            {
                fSum += rkA[iMid][iRow]*rkB[iMid][iCol];
            }
            kATrnB[iRow][iCol] = fSum;
        }
    }
    return kATrnB;
}
//----------------------------------------------------------------------------
template <typename Real, int Rows, int Mids, int Cols>
RealM<Real,Rows,Cols> MulATrnBTrn (const RealM<Real,Mids,Rows>& rkA,
    const RealM<Real,Cols,Mids>& rkB)
{
    RealM<Real,Rows,Cols> kATrnBTrn;
    for (int iRow = 0; iRow < Rows; iRow++)
    {
        for (int iCol = 0; iCol < Cols; iCol++)
        {
            Real fSum = (Real)0;
            for (int iMid = 0; iMid < Mids; iMid++)
            {
                fSum += rkA[iMid][iRow]*rkB[iCol][iMid];
            }
            kATrnBTrn[iRow][iCol] = fSum;
        }
    }
    return kATrnBTrn;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealM<Real,N,N> MulMD (const RealM<Real,N,N>& rkM, const RealV<Real,N>& rkD)
{
    RealM<Real,N,N> kMD;
    for (int iRow = 0; iRow < N; iRow++)
    {
        for (int iCol = 0; iCol < N; iCol++)
        {
            kMD[iRow][iCol] = rkM[iRow][iCol]*rkD[iCol];
        }
    }
    return kMD;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
RealM<Real,N,N> MulDM (const RealV<Real,N>& rkD, const RealM<Real,N,N>& rkM)
{
    RealM<Real,N,N> kDM;
    for (int iRow = 0; iRow < N; iRow++)
    {
        for (int iCol = 0; iCol < N; iCol++)
        {
            kDM[iRow][iCol] = rkD[iRow]*rkM[iRow][iCol];
        }
    }
    return kDM;
}
//----------------------------------------------------------------------------
