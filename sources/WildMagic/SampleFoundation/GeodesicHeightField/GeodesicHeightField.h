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
// Version: 4.0.1 (2006/08/07)

#ifndef GEODESICHEIGHTFIELD_H
#define GEODESICHEIGHTFIELD_H

#include "Wm4WindowApplication3.h"
#include "Wm4BSplineGeodesic.h"
using namespace Wm4;

class GeodesicHeightField : public WindowApplication3
{
    WM4_DECLARE_INITIALIZE;

public:
    GeodesicHeightField ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);

protected:
    void CreateScene ();
    void DrawLine (int iX0, int iY0, int iX1, int iY1,
        int iWidth, unsigned char* aucData);
    void ComputeDistanceCurvature ();
    static void RefineCallback ();

    NodePtr m_spkScene;
    WireframeStatePtr m_spkWireframe;
    TriMeshPtr m_spkMesh;
    ImagePtr m_spkImage;
    TextureEffectPtr m_spkEffect;
    Culler m_kCuller;

    Picker m_kPicker;

    BSplineRectangled* m_pkSurface;
    BSplineGeodesicd* m_pkGeodesic;

    int m_iSelected;
    int m_aiXIntr[2], m_aiYIntr[2];
    GVectord m_akPoint[2];
    int m_iPQuantity;
    GVectord* m_akPath;

    double m_dDistance, m_dCurvature;
};

WM4_REGISTER_INITIALIZE(GeodesicHeightField);

#endif
