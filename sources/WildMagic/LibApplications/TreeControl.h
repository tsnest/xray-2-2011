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
// Version: 4.0.1 (2007/01/27)

#ifndef TREECONTROL_H
#define TREECONTROL_H

#include <windows.h>
#include <commctrl.h>
#include "Wm4Application.h"
namespace Wm4
{

class TreeControl
{
public:
    TreeControl (Application* pkTheApp, int iExtraData, HINSTANCE hInstance,
        HWND hParentWnd, Stream& rkStream, const char* acName, int iX,
        int iY, int iW, int iH);

    TreeControl (Application* pkTheApp, int iExtraData, HINSTANCE hInstance,
        HWND hParentWnd, Spatial* pkScene, int iX, int iY, int iW, int iH);

    ~TreeControl ();

protected:
    void CreateWindows (int iX, int iY, int iW, int iH);
    void CreateImageList ();
    void CreateTree (Stream& rkStream);
    void CreateTree (Spatial* pkScene);

    enum NodeType
    {
        NT_CLASS,
        NT_SUBCLASS,
        NT_DATA
    };

    void CreateTreeRecursive (HTREEITEM hParent, StringTree* pkTree,
        NodeType eType);

    void DestroyWindows ();
    void DestroyImageList ();
    void DestroyTree ();

    static LRESULT CALLBACK HandleMessage (HWND hWnd, UINT uiMsg,
        WPARAM wParam, LPARAM lParam);

    Application* m_pkTheApp;
    int m_iExtraData;
    HINSTANCE m_hInstance;
    HWND m_hParentWnd;
    HWND m_hTreeWnd;
    HWND m_hTreeView;
    HTREEITEM m_hTreeRoot;
    HIMAGELIST m_hImageList;
    char* m_acName;
    int m_iClosedB, m_iClosedY, m_iClosedP, m_iAttribute;

    static unsigned int ms_auiBmpClosedB[240];
    static unsigned int ms_auiBmpClosedP[240];
    static unsigned int ms_auiBmpClosedY[240];
    static unsigned int ms_auiBmpAttribute[240];

    static std::map<HWND,std::pair<Application*,int> >* ms_pkWndAppMap;
};

}

#endif
