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
// Version: 4.0.1 (2008/09/07)

#ifndef BSPLINECURVEFITTER_H
#define BSPLINECURVEFITTER_H

#include "Wm4WindowApplication3.h"
using namespace Wm4;

class BSplineCurveFitter : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    BSplineCurveFitter ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void CreateScene ();
    void CreateBSplinePolyline ();

    NodePtr m_spkScene;
    VertexColor3EffectPtr m_spkEffect;
    Culler m_kCuller;

    int m_iSampleQuantity;
    Vector3f* m_akSample;
    int m_iDegree;
    int m_iCtrlQuantity;
    BSplineCurveFitf* m_pkBSpline;

    enum { MESSAGE_SIZE = 128 };
    char m_acMessage[MESSAGE_SIZE];
};

WM4_REGISTER_INITIALIZE(BSplineCurveFitter);

#endif
