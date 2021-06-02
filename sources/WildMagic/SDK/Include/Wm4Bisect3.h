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

#ifndef WM4BISECT3_H
#define WM4BISECT3_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM Bisect3
{
public:
    typedef Real (*Function)(Real,Real,Real);

    Bisect3 (Function oF, Function oG, Function oH, int iMaxLevel,
        Real fTolerance);

    bool Bisect (Real fX0, Real fY0, Real fZ0, Real fX1,
        Real fY1, Real fZ1, Real& rfXRoot, Real& rfYRoot,
        Real& rfZRoot);

private:
    class WM4_FOUNDATION_ITEM BisectNode
    {
    public:
        Real X, Y, Z, F, G, H;
        BisectNode* XNext;
        BisectNode* YNext;
        BisectNode* ZNext;
    };

    bool ZeroTest (Real fX, Real fY, Real fZ, Real& rfF, Real& rfG,
        Real& rfH, Real& rfXRoot, Real& rfYRoot, Real& rfZRoot);

    static BisectNode* AddNode (Real fX, Real fY, Real fZ, Real fF, Real fG,
        Real fH);

    // input data and functions
    Function m_oF, m_oG, m_oH;
    int m_iLevel, m_iMaxLevel;
    Real m_fTolerance;
    Real m_fXRoot, m_fYRoot, m_fZRoot;

    // vertex and midpoint locations
    Real m_fX0, m_fXm, m_fX1, m_fY0, m_fYm, m_fY1, m_fZ0, m_fZm, m_fZ1;

    // vertices
    Real m_fF000, m_fF100, m_fF010, m_fF110;
    Real m_fF001, m_fF101, m_fF011, m_fF111;
    Real m_fG000, m_fG100, m_fG010, m_fG110;
    Real m_fG001, m_fG101, m_fG011, m_fG111;
    Real m_fH000, m_fH100, m_fH010, m_fH110;
    Real m_fH001, m_fH101, m_fH011, m_fH111;

    // edges
    Real m_fF00m, m_fF10m, m_fF01m, m_fF11m, m_fF0m0, m_fF1m0, m_fF0m1;
    Real m_fF1m1, m_fFm00, m_fFm10, m_fFm01, m_fFm11;
    Real m_fG00m, m_fG10m, m_fG01m, m_fG11m, m_fG0m0, m_fG1m0, m_fG0m1;
    Real m_fG1m1, m_fGm00, m_fGm10, m_fGm01, m_fGm11;
    Real m_fH00m, m_fH10m, m_fH01m, m_fH11m, m_fH0m0, m_fH1m0, m_fH0m1;
    Real m_fH1m1, m_fHm00, m_fHm10, m_fHm01, m_fHm11;

    // faces
    Real m_fF0mm, m_fFm0m, m_fFmm0, m_fF1mm, m_fFm1m, m_fFmm1;
    Real m_fG0mm, m_fGm0m, m_fGmm0, m_fG1mm, m_fGm1m, m_fGmm1;
    Real m_fH0mm, m_fHm0m, m_fHmm0, m_fH1mm, m_fHm1m, m_fHmm1;

    // center
    Real m_fFmmm, m_fGmmm, m_fHmmm;

    int m_iNetSign;

    // the graph and recursion routine for building it
    BisectNode* m_pkGraph;
    bool BisectRecurse (BisectNode* m_pkN000);
};

typedef Bisect3<float> Bisect3f;
typedef Bisect3<double> Bisect3d;

}

#endif
