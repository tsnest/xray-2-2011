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
// Version: 4.0.1 (2006/08/30)

#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include "Wm4WindowApplication2.h"
using namespace Wm4;

class Triangulation : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    Triangulation ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void ClearAll ();

    void DoExample0 ();
    void DoExample1 ();
    void DoExample2 ();
    void DoExample3 ();
    void DoExample4 ();

    std::vector<Vector2f> m_kPositions;
    std::vector<int> m_kOuter, m_kInner0, m_kInner1;
    std::vector<std::vector<int>*> m_kInners;
    TriangulateEC<float>::Tree* m_pkRoot;
    std::vector<Vector2f> m_kFillSeed;
    std::vector<int> m_kTriangles;

    Query::Type m_eType;
    float m_fEpsilon;
    int m_iExample;
};

WM4_REGISTER_INITIALIZE(Triangulation);

#endif
