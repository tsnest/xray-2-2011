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
// Version: 4.0.2 (2007/01/27)

#include "Wm4ApplicationPCH.h"
#include "TreeControl.h"
using namespace Wm4;

//----------------------------------------------------------------------------
unsigned int TreeControl::ms_auiBmpClosedB[240] =
{
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xffffffff,
0xffffffff,
0xfffff8ff,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xffffffff,
0xff0000ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff000000,
0xffffffff,
0xff0000ff,
0xff80ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff8080ff,
0xff8080ff,
0xff0080ff,
0xff000000,
0xffffffff,
0xff0000ff,
0xff80ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff8080ff,
0xff0080ff,
0xff000000,
0xffffffff,
0xff0000ff,
0xff80ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff0080ff,
0xff000000,
0xffffffff,
0xff0000ff,
0xff80ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff0080ff,
0xff000000,
0xffffffff,
0xff0000ff,
0xff80ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff0080ff,
0xff000000,
0xffffffff,
0xff0000ff,
0xff80ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff8080ff,
0xff00ffff,
0xff0080ff,
0xff000000,
0xffffffff,
0xff0000ff,
0xff80ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff8080ff,
0xff0080ff,
0xff000000,
0xffffffff,
0xff0000ff,
0xff80ffff,
0xff80ffff,
0xff80ffff,
0xff80ffff,
0xff80ffff,
0xff80ffff,
0xff80ffff,
0xff80ffff,
0xff80ffff,
0xff80ffff,
0xff80ffff,
0xff00ffff,
0xff0080ff,
0xff000000,
0xffffffff,
0xff0000ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0080ff,
0xff0000ff,
0xff0000ff,
0xff0000ff,
0xff0000ff,
0xff0000ff,
0xff0000ff,
0xfffff8ff,
0xffffffff,
0xfffff8ff,
0xff0000ff,
0xff80ffff,
0xff80ffff,
0xff00ffff,
0xff00ffff,
0xff00ffff,
0xff0000ff,
0xff000000,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xffffffff,
0xfffff8ff,
0xfffff8ff,
0xff0000ff,
0xff0000ff,
0xff0000ff,
0xff0000ff,
0xff0000ff,
0xff000000,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff
};
//----------------------------------------------------------------------------
unsigned int TreeControl::ms_auiBmpClosedP[240] =
{
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xffffffff,
0xffffffff,
0xfffff8ff,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xffffffff,
0xff800080,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff000000,
0xffffffff,
0xff800080,
0xffc0c0c0,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xff8080ff,
0xff8080ff,
0xff804000,
0xff000000,
0xffffffff,
0xff800080,
0xffc0c0c0,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xff8080ff,
0xff804000,
0xff000000,
0xffffffff,
0xff800080,
0xffc0c0c0,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xff804000,
0xff000000,
0xffffffff,
0xff800080,
0xffc0c0c0,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff804000,
0xff000000,
0xffffffff,
0xff800080,
0xffc0c0c0,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xff804000,
0xff000000,
0xffffffff,
0xff800080,
0xffc0c0c0,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff8080ff,
0xffff00ff,
0xff804000,
0xff000000,
0xffffffff,
0xff800080,
0xffc0c0c0,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xff8080ff,
0xff804000,
0xff000000,
0xffffffff,
0xff800080,
0xffc0c0c0,
0xffc0c0c0,
0xffc0c0c0,
0xffc0c0c0,
0xffc0c0c0,
0xffc0c0c0,
0xffc0c0c0,
0xffc0c0c0,
0xffc0c0c0,
0xffc0c0c0,
0xffc0c0c0,
0xffff00ff,
0xff804000,
0xff000000,
0xffffffff,
0xff800080,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff804000,
0xff800080,
0xff800080,
0xff800080,
0xff800080,
0xff800080,
0xff800080,
0xfffff8ff,
0xffffffff,
0xfffff8ff,
0xff800080,
0xffc0c0c0,
0xffc0c0c0,
0xffff00ff,
0xffff00ff,
0xffff00ff,
0xff800080,
0xff000000,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xffffffff,
0xfffff8ff,
0xfffff8ff,
0xff800080,
0xff800080,
0xff800080,
0xff800080,
0xff800080,
0xff000000,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff
};
//----------------------------------------------------------------------------
unsigned int TreeControl::ms_auiBmpClosedY[240] =
{
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xffffffff,
0xffffffff,
0xfffff8ff,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xffffffff,
0xff909000,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xff000000,
0xffffffff,
0xff909000,
0xfffff8cf,
0xffffc890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xffffc890,
0xffffc890,
0xffcfc860,
0xff000000,
0xffffffff,
0xff909000,
0xfffff8cf,
0xfffff890,
0xfffff890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xffffc890,
0xffcfc860,
0xff000000,
0xffffffff,
0xff909000,
0xfffff8cf,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xffcfc860,
0xff000000,
0xffffffff,
0xff909000,
0xfffff8cf,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffcfc860,
0xff000000,
0xffffffff,
0xff909000,
0xfffff8cf,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xffcfc860,
0xff000000,
0xffffffff,
0xff909000,
0xfffff8cf,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffffc890,
0xfffff890,
0xffcfc860,
0xff000000,
0xffffffff,
0xff909000,
0xfffff8cf,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xfffff890,
0xffffc890,
0xffcfc860,
0xff000000,
0xffffffff,
0xff909000,
0xfffff8cf,
0xfffff8cf,
0xfffff8cf,
0xfffff8cf,
0xfffff8cf,
0xfffff8cf,
0xfffff8cf,
0xfffff8cf,
0xfffff8cf,
0xfffff8cf,
0xfffff8cf,
0xfffff890,
0xffcfc860,
0xff000000,
0xffffffff,
0xff909000,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xffcfc860,
0xff909000,
0xff909000,
0xff909000,
0xff909000,
0xff909000,
0xff909000,
0xfffff8ff,
0xffffffff,
0xfffff8ff,
0xff909000,
0xfffff8cf,
0xfffff8cf,
0xfffff890,
0xfffff890,
0xfffff890,
0xff909000,
0xff000000,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xffffffff,
0xfffff8ff,
0xfffff8ff,
0xff909000,
0xff909000,
0xff909000,
0xff909000,
0xff909000,
0xff000000,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff,
0xfffff8ff
};
//----------------------------------------------------------------------------
unsigned int TreeControl::ms_auiBmpAttribute[240] =
{
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff000000,
0xff000000,
0xff000000,
0xff000000,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff
};
//----------------------------------------------------------------------------
std::map<HWND,std::pair<Application*,int> >* TreeControl::ms_pkWndAppMap = 0;
//----------------------------------------------------------------------------
TreeControl::TreeControl (Application* pkTheApp, int iExtraData,
    HINSTANCE hInstance, HWND hParentWnd, Stream& rkStream,
    const char* acName, int iX, int iY, int iW, int iH)
{
    m_pkTheApp = pkTheApp;
    m_iExtraData = iExtraData;
    m_hInstance = hInstance;
    m_hParentWnd = hParentWnd;

    size_t uiSize = strlen(acName) + 1;
    m_acName = WM4_NEW char[uiSize];
    System::Strcpy(m_acName,uiSize,acName);

    CreateWindows(iX,iY,iW,iH);
    CreateImageList();
    CreateTree(rkStream);

    ShowWindow(m_hTreeWnd,SW_SHOWDEFAULT);
    UpdateWindow(m_hTreeWnd);
}
//----------------------------------------------------------------------------
TreeControl::TreeControl (Application* pkTheApp, int iExtraData,
    HINSTANCE hInstance, HWND hParentWnd, Spatial* pkScene, int iX, int iY,
    int iW, int iH)
{
    m_pkTheApp = pkTheApp;
    m_iExtraData = iExtraData;
    m_hInstance = hInstance;
    m_hParentWnd = hParentWnd;

    const char* acName = pkScene->GetName().c_str();
    if (acName)
    {
        size_t uiSize = strlen(acName) + 1;
        m_acName = WM4_NEW char[uiSize];
        System::Strcpy(m_acName,uiSize,acName);
    }
    else
    {
        m_acName = WM4_NEW char[8];
        System::Strcpy(m_acName,8,"unnamed");
    }

    CreateWindows(iX,iY,iW,iH);
    CreateImageList();
    CreateTree(pkScene);

    ShowWindow(m_hTreeWnd,SW_SHOWDEFAULT);
    UpdateWindow(m_hTreeWnd);
}
//----------------------------------------------------------------------------
TreeControl::~TreeControl ()
{
    WM4_DELETE[] m_acName;

    DestroyTree();
    DestroyImageList();
    DestroyWindows();
}
//----------------------------------------------------------------------------
void TreeControl::CreateWindows (int iX, int iY, int iW, int iH)
{
    static char s_acWindowClassName[] = "Wm4TreeControl";

    // initialize tree view classes
#if defined(__MINGW32__) || defined(__CYGWIN__)
    // MINGW 4.1.0 and Cygwin 1.5.21_1 do not support the "Ex" version.
    InitCommonControls();
#else
    INITCOMMONCONTROLSEX kInitCC;
    kInitCC.dwSize = sizeof(INITCOMMONCONTROLSEX);
    kInitCC.dwICC = ICC_TREEVIEW_CLASSES;
    InitCommonControlsEx(&kInitCC);
#endif

    // register the window class
    WNDCLASS kWClass;
    kWClass.style = CS_HREDRAW | CS_VREDRAW;
    kWClass.lpfnWndProc = HandleMessage;
    kWClass.cbClsExtra = 0;
    kWClass.cbWndExtra = 0;
    kWClass.hInstance = m_hInstance;
    kWClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    kWClass.hCursor = LoadCursor(NULL,IDC_ARROW);
    kWClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    kWClass.lpszClassName = s_acWindowClassName;
    kWClass.lpszMenuName = NULL;
    RegisterClass(&kWClass);

    // create the tree view window
    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
        | WS_SIZEBOX;
    m_hTreeWnd = CreateWindow(s_acWindowClassName,m_acName,dwStyle,iX,iY,iW,
        iH,0,0,m_hInstance,NULL);

    // create the tree view
    dwStyle = WS_VISIBLE | WS_TABSTOP | WS_CHILD |
        TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;

    RECT kRect;
    GetClientRect(m_hTreeWnd,&kRect);

    m_hTreeView = CreateWindow(WC_TREEVIEW,"",dwStyle,0,0,
        kRect.right-kRect.left+1,kRect.bottom-kRect.top+1,m_hTreeWnd,0,
        m_hInstance,NULL);

    if (!ms_pkWndAppMap)
    {
        ms_pkWndAppMap = WM4_NEW std::map<HWND,std::pair<Application*,int> >;
    }
    (*ms_pkWndAppMap)[m_hTreeWnd] = std::make_pair(m_pkTheApp,m_iExtraData);
}
//----------------------------------------------------------------------------
void TreeControl::CreateImageList ()
{
    HDC hWindowDC = GetDC(m_hTreeView);
    HDC hMemoryDC = CreateCompatibleDC(hWindowDC);

    const size_t uiLength = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD*);
    char* acBitMapInfo = WM4_NEW char[uiLength];
    memset(acBitMapInfo,0,uiLength);
    BITMAPINFOHEADER& bmih = *(BITMAPINFOHEADER*)acBitMapInfo;
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = 16;
    bmih.biHeight = 15;
    bmih.biPlanes = 1;
    bmih.biBitCount = 32;
    bmih.biCompression = BI_RGB;
    bmih.biSizeImage = 240;
    bmih.biXPelsPerMeter = 0;
    bmih.biYPelsPerMeter = 0;
    bmih.biClrUsed = 0;
    bmih.biClrImportant = 0;

    unsigned int* auiColorBuffer = 0;
    HBITMAP hBitmap = 0;
    size_t uiSize = 240*sizeof(unsigned int);

    m_hImageList = ImageList_Create(16,15,ILC_COLOR32,4,1);

    hBitmap = CreateDIBSection(hMemoryDC,(CONST BITMAPINFO*)acBitMapInfo,
        DIB_RGB_COLORS,(void**)&auiColorBuffer,0,0);
    System::Memcpy(auiColorBuffer,uiSize,ms_auiBmpClosedB,uiSize);
    m_iClosedB = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);

    hBitmap = CreateDIBSection(hMemoryDC,(CONST BITMAPINFO*)acBitMapInfo,
        DIB_RGB_COLORS,(void**)&auiColorBuffer,0,0);
    System::Memcpy(auiColorBuffer,uiSize,ms_auiBmpClosedY,uiSize);
    m_iClosedY = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);

    hBitmap = CreateDIBSection(hMemoryDC,(CONST BITMAPINFO*)acBitMapInfo,
        DIB_RGB_COLORS,(void**)&auiColorBuffer,0,0);
    System::Memcpy(auiColorBuffer,uiSize,ms_auiBmpClosedP,uiSize);
    m_iClosedP = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);

    hBitmap = CreateDIBSection(hMemoryDC,(CONST BITMAPINFO*)acBitMapInfo,
        DIB_RGB_COLORS,(void**)&auiColorBuffer,0,0);
    System::Memcpy(auiColorBuffer,uiSize,ms_auiBmpAttribute,uiSize);
    m_iAttribute = ImageList_Add(m_hImageList,hBitmap,0);
    DeleteObject(hBitmap);

    WM4_DELETE[] acBitMapInfo;
    DeleteDC(hMemoryDC);
    ReleaseDC(m_hTreeView,hWindowDC);
}
//----------------------------------------------------------------------------
void TreeControl::CreateTree (Stream& rkStream)
{
    TreeView_SetImageList(m_hTreeView,m_hImageList,TVSIL_NORMAL);

    // items are to be inserted in-order
    TV_INSERTSTRUCT tvs;
    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    // add root item (name is the input filename)
    tvs.hParent = TVI_ROOT;
    tvs.item.pszText = m_acName;
    tvs.item.iImage = m_iClosedB;
    tvs.item.iSelectedImage = m_iClosedB;
    m_hTreeRoot = TreeView_InsertItem(m_hTreeView,&tvs);

    for (int i = 0; i < rkStream.GetObjectCount(); i++)
    {
        SpatialPtr spkObject = DynamicCast<Spatial>(rkStream.GetObjectAt(i));
        if (spkObject)
        {
            spkObject->UpdateGS();
            spkObject->UpdateRS();
            StringTree* pkRoot = spkObject->SaveStrings();
            CreateTreeRecursive(m_hTreeRoot,pkRoot,NT_CLASS);
            WM4_DELETE pkRoot;
        }
    }
}
//----------------------------------------------------------------------------
void TreeControl::CreateTree (Spatial* pkScene)
{
    TreeView_SetImageList(m_hTreeView,m_hImageList,TVSIL_NORMAL);

    // items are to be inserted in-order
    TV_INSERTSTRUCT tvs;
    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    // add root item (name is the input filename)
    tvs.hParent = TVI_ROOT;
    tvs.item.pszText = m_acName;
    tvs.item.iImage = m_iClosedB;
    tvs.item.iSelectedImage = m_iClosedB;
    m_hTreeRoot = TreeView_InsertItem(m_hTreeView,&tvs);

    if (pkScene)
    {
        pkScene->UpdateGS();
        pkScene->UpdateRS();
        StringTree* pkRoot = pkScene->SaveStrings();
        CreateTreeRecursive(m_hTreeRoot,pkRoot,NT_CLASS);
        WM4_DELETE pkRoot;
    }
}
//----------------------------------------------------------------------------
void TreeControl::CreateTreeRecursive (HTREEITEM hParent,
    StringTree* pkTree, NodeType eType)
{
    // items are to be inserted in-order
    TV_INSERTSTRUCT tvs;
    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    assert(pkTree->GetStringQuantity() > 0);
    if (strncmp(pkTree->GetString(0),"Wm4",3) == 0)
    {
        // string tree rooted at an Object-derived object

        // Add object.  Strip off the "WmX." prefix (the "+ 4" term).
        tvs.hParent = hParent;
        tvs.item.pszText = pkTree->GetString(0) + 4;
        if (eType == NT_CLASS)
        {
            tvs.item.iImage = m_iClosedB;
            tvs.item.iSelectedImage = m_iClosedB;
        }
        else if (eType == NT_SUBCLASS)
        {
            tvs.item.iImage = m_iClosedY;
            tvs.item.iSelectedImage = m_iClosedY;
        }
        else
        {
            assert(false);
        }

        tvs.hParent = TreeView_InsertItem(m_hTreeView,&tvs);

        // add subclass of object
        int iStart;
        if (strncmp(pkTree->GetString(0),"Wm4.Object",10) != 0)
        {
            CreateTreeRecursive(tvs.hParent,pkTree->GetChild(0),NT_SUBCLASS);
            iStart = 1;
        }
        else
        {
            iStart = 0;
        }

        // add strings
        int i;
        for (i = 1; i < pkTree->GetStringQuantity(); i++)
        {
            tvs.item.pszText = pkTree->GetString(i);
            tvs.item.iImage = m_iAttribute;
            tvs.item.iSelectedImage = m_iAttribute;
            TreeView_InsertItem(m_hTreeView,&tvs);
        }

        // add children
        for (i = iStart; i < pkTree->GetChildQuantity(); i++)
        {
            StringTree* pkCTree = pkTree->GetChild(i);
            assert( pkCTree->GetStringQuantity() > 0 );

            if (strncmp(pkCTree->GetString(0),"Wm4",3) == 0)
            {
                eType = NT_CLASS;
            }
            else
            {
                eType = NT_DATA;
            }

            CreateTreeRecursive(tvs.hParent,pkCTree,eType);
        }
    }
    else
    {
        // string tree represents a native type (for example, an array)

        // add object
        tvs.hParent = hParent;
        tvs.item.pszText = pkTree->GetString(0);
        tvs.item.iImage = m_iClosedP;
        tvs.item.iSelectedImage = m_iClosedP;
        tvs.hParent = TreeView_InsertItem(m_hTreeView,&tvs);

        // add strings
        int i;
        for (i = 1; i < pkTree->GetStringQuantity(); i++)
        {
            tvs.item.pszText = pkTree->GetString(i);
            tvs.item.iImage = m_iAttribute;
            tvs.item.iSelectedImage = m_iAttribute;
            TreeView_InsertItem(m_hTreeView,&tvs);
        }

        // add children
        for (i = 0; i < pkTree->GetChildQuantity(); i++)
        {
            StringTree* pkCTree = pkTree->GetChild(i);
            assert( pkCTree->GetStringQuantity() > 0 );

            if (strncmp(pkCTree->GetString(0),"Wm4",3) == 0)
            {
                eType = NT_CLASS;
            }
            else
            {
                eType = NT_DATA;
            }

            CreateTreeRecursive(tvs.hParent,pkCTree,eType);
        }
    }
}
//----------------------------------------------------------------------------
void TreeControl::DestroyWindows ()
{
    DestroyWindow(m_hTreeView);
    DestroyWindow(m_hTreeWnd);
    if (ms_pkWndAppMap)
    {
        ms_pkWndAppMap->erase(m_hTreeWnd);
        if (ms_pkWndAppMap->size() == 0)
        {
            WM4_DELETE ms_pkWndAppMap;
            ms_pkWndAppMap = 0;
        }
    }
}
//----------------------------------------------------------------------------
void TreeControl::DestroyImageList ()
{
    ImageList_Destroy(m_hImageList);
}
//----------------------------------------------------------------------------
void TreeControl::DestroyTree ()
{
    TreeView_DeleteAllItems(m_hTreeView);
}
//----------------------------------------------------------------------------
LRESULT CALLBACK TreeControl::HandleMessage (HWND hWnd, UINT uiMsg,
    WPARAM wParam, LPARAM lParam)
{
    if (uiMsg == WM_DESTROY)
    {
        if (ms_pkWndAppMap)
        {
            std::pair<Application*,int> kPair = (*ms_pkWndAppMap)[hWnd];
            Application* pkTheApp = kPair.first;
            if (pkTheApp)
            {
                int iExtraData = kPair.second;
                TreeControl* pkControl = 0;
                pkTheApp->SetExtraData(iExtraData,sizeof(TreeControl*),
                    &pkControl);
            }
            return 0;
        }
    }

    return DefWindowProc(hWnd,uiMsg,wParam,lParam);
}
//----------------------------------------------------------------------------
