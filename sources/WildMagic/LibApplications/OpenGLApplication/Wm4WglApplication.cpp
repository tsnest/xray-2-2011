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
// Version: 4.0.3 (2007/05/06)

#include "Wm4ApplicationPCH.h"

#include <windows.h>
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#include "Wm4WindowApplication.h"
#include "Wm4WglRenderer.h"
#include "TreeControl.h"
using namespace Wm4;

const int WindowApplication::KEY_ESCAPE = VK_ESCAPE;
const int WindowApplication::KEY_LEFT_ARROW = VK_LEFT;
const int WindowApplication::KEY_RIGHT_ARROW = VK_RIGHT;
const int WindowApplication::KEY_UP_ARROW = VK_UP;
const int WindowApplication::KEY_DOWN_ARROW = VK_DOWN;
const int WindowApplication::KEY_HOME = VK_HOME;
const int WindowApplication::KEY_END = VK_END;
const int WindowApplication::KEY_PAGE_UP = VK_PRIOR;
const int WindowApplication::KEY_PAGE_DOWN = VK_NEXT;
const int WindowApplication::KEY_INSERT = VK_INSERT;
const int WindowApplication::KEY_DELETE = VK_DELETE;
const int WindowApplication::KEY_F1 = VK_F1;
const int WindowApplication::KEY_F2 = VK_F2;
const int WindowApplication::KEY_F3 = VK_F3;
const int WindowApplication::KEY_F4 = VK_F4;
const int WindowApplication::KEY_F5 = VK_F5;
const int WindowApplication::KEY_F6 = VK_F6;
const int WindowApplication::KEY_F7 = VK_F7;
const int WindowApplication::KEY_F8 = VK_F8;
const int WindowApplication::KEY_F9 = VK_F9;
const int WindowApplication::KEY_F10 = VK_F10;
const int WindowApplication::KEY_F11 = VK_F11;
const int WindowApplication::KEY_F12 = VK_F12;
const int WindowApplication::KEY_BACKSPACE = VK_BACK;
const int WindowApplication::KEY_TAB = VK_TAB;
const int WindowApplication::KEY_ENTER = VK_RETURN;
const int WindowApplication::KEY_RETURN = VK_RETURN;

const int WindowApplication::KEY_SHIFT = MK_SHIFT;
const int WindowApplication::KEY_CONTROL = MK_CONTROL;
const int WindowApplication::KEY_ALT = 0;      // not currently handled
const int WindowApplication::KEY_COMMAND = 0;  // not currently handled

const int WindowApplication::MOUSE_LEFT_BUTTON = 0;
const int WindowApplication::MOUSE_MIDDLE_BUTTON = 1;
const int WindowApplication::MOUSE_RIGHT_BUTTON = 2;
const int WindowApplication::MOUSE_UP = 0;
const int WindowApplication::MOUSE_DOWN = 1;

const int WindowApplication::MODIFIER_CONTROL = MK_CONTROL;
const int WindowApplication::MODIFIER_LBUTTON = MK_LBUTTON;
const int WindowApplication::MODIFIER_MBUTTON = MK_MBUTTON;
const int WindowApplication::MODIFIER_RBUTTON = MK_RBUTTON;
const int WindowApplication::MODIFIER_SHIFT = MK_SHIFT;

// These are in the VC7.1 basetsd.h, but not the VC6 basetsd.h.
#ifndef IntToPtr
#define IntToPtr(i) ((VOID*)(INT_PTR)((int)i))
#endif

#ifndef PtrToUint
#define PtrToUint(p) ((UINT)(UINT_PTR)(p))
#endif

#ifndef PtrToInt
#define PtrToInt(p)  ((INT)(INT_PTR)(p))
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//----------------------------------------------------------------------------
void WindowApplication::SetMousePosition (int iX, int iY)
{
    HWND hWnd = (HWND)IntToPtr(m_iWindowID);
    POINT kPoint;
    kPoint.x = (LONG)iX;
    kPoint.y = (LONG)iY;
    ClientToScreen(hWnd,&kPoint);
    SetCursorPos(kPoint.x,kPoint.y);
}
//----------------------------------------------------------------------------
void WindowApplication::GetMousePosition (int& riX, int& riY) const
{
    HWND hWnd = (HWND)IntToPtr(m_iWindowID);
    POINT kPoint;
    GetCursorPos(&kPoint);
    ScreenToClient(hWnd,&kPoint);
    riX = (int)kPoint.x;
    riY = (int)kPoint.y;
}
//----------------------------------------------------------------------------
int WindowApplication::GetStringWidth (const char* acText) const
{
    if (!acText || strlen(acText) == 0)
    {
        return 0;
    }

    HWND hWnd = (HWND)IntToPtr(m_iWindowID);
    HDC hDC = GetDC(hWnd);
    SIZE kSize;
    GetTextExtentPoint32(hDC,acText,(int)strlen(acText),&kSize);
    ReleaseDC(hWnd,hDC);
    return (int)kSize.cx;
}
//----------------------------------------------------------------------------
int WindowApplication::GetCharacterWidth (const char cCharacter) const
{
    HWND hWnd = (HWND)IntToPtr(m_iWindowID);
    HDC hDC = GetDC(hWnd);
    SIZE kSize;
    GetTextExtentPoint32(hDC,&cCharacter,1,&kSize);
    ReleaseDC(hWnd,hDC);
    return (int)kSize.cx;
}
//----------------------------------------------------------------------------
int WindowApplication::GetFontHeight () const
{
    HWND hWnd = (HWND)IntToPtr(m_iWindowID);
    HDC hDC = GetDC(hWnd);
    TEXTMETRIC kMetric;
    GetTextMetrics(hDC,&kMetric);
    ReleaseDC(hWnd,hDC);
    return (int)kMetric.tmHeight;
}
//----------------------------------------------------------------------------
LRESULT CALLBACK MsWindowEventHandler (HWND hWnd, UINT uiMsg, WPARAM wParam,
    LPARAM lParam)
{
    WindowApplication* pkTheApp =
        (WindowApplication*)Application::TheApplication;

    if (!pkTheApp || !pkTheApp->GetWindowID())
    {
        return DefWindowProc(hWnd,uiMsg,wParam,lParam);
    }

    switch (uiMsg) 
    {
        case WM_PAINT:
        {
            PAINTSTRUCT kPS;
            BeginPaint(hWnd,&kPS);
            pkTheApp->OnDisplay();
            EndPaint(hWnd,&kPS);
            return 0;
        }
        case WM_ERASEBKGND:
        {
            // This tells Windows not to erase the background (and that the
            // application is doing so).
            return 1;
        }
        case WM_MOVE:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMove(iXPos,iYPos);
            return 0;
        }
        case WM_SIZE:
        {
            int iWidth = int(LOWORD(lParam));
            int iHeight = int(HIWORD(lParam));
            pkTheApp->OnResize(iWidth,iHeight);
            return 0;
        }
        case WM_CHAR:
        {
            unsigned char ucKey = (unsigned char)(char)wParam;

            // quit application if the KEY_TERMINATE key is pressed
            if (ucKey == pkTheApp->KEY_TERMINATE)
            {
                PostQuitMessage(0);
                return 0;
            }

            // get the cursor position in client coordinates
            POINT kPoint;
            GetCursorPos(&kPoint);
            ScreenToClient(hWnd,&kPoint);
            int iXPos = (int)kPoint.x;
            int iYPos = (int)kPoint.y;

            pkTheApp->OnKeyDown(ucKey,iXPos,iYPos);
            return 0;
        }
        case WM_KEYDOWN:
        {
            int iVirtKey = int(wParam);

            // get cursor position client coordinates
            POINT kPoint;
            GetCursorPos(&kPoint);
            ScreenToClient(hWnd,&kPoint);
            int iXPos = (int)kPoint.x;
            int iYPos = (int)kPoint.y;

            if ((VK_F1 <= iVirtKey && iVirtKey <= VK_F12)
            ||  (VK_PRIOR <= iVirtKey && iVirtKey <= VK_DOWN)
            ||  (iVirtKey == VK_INSERT) || (iVirtKey == VK_DELETE))
            {
                pkTheApp->OnSpecialKeyDown(iVirtKey,iXPos,iYPos);
            }
            return 0;
        }
        case WM_KEYUP:
        {
            int iVirtKey = int(wParam);

            // get the cursor position in client coordinates
            POINT kPoint;
            GetCursorPos(&kPoint);
            ScreenToClient(hWnd,&kPoint);
            int iXPos = (int)kPoint.x;
            int iYPos = (int)kPoint.y;

            if ((VK_F1 <= iVirtKey && iVirtKey <= VK_F12)
            ||  (VK_PRIOR <= iVirtKey && iVirtKey <= VK_DOWN)
            ||  (iVirtKey == VK_INSERT) || (iVirtKey == VK_DELETE))
            {
                pkTheApp->OnSpecialKeyUp(iVirtKey,iXPos,iYPos);
            }
            else
            {
                pkTheApp->OnKeyUp((unsigned char)iVirtKey,iXPos,iYPos);
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_LEFT_BUTTON,
                WindowApplication::MOUSE_DOWN,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_LBUTTONUP:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_LEFT_BUTTON,
                WindowApplication::MOUSE_UP,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_MBUTTONDOWN:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_MIDDLE_BUTTON,
                WindowApplication::MOUSE_DOWN,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_MBUTTONUP:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_MIDDLE_BUTTON,
                WindowApplication::MOUSE_UP,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_RIGHT_BUTTON,
                WindowApplication::MOUSE_DOWN,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_RBUTTONUP:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            pkTheApp->OnMouseClick(WindowApplication::MOUSE_RIGHT_BUTTON,
                WindowApplication::MOUSE_UP,iXPos,iYPos,PtrToUint(wParam));
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));

            int iButton = -1;
            if (wParam & MK_LBUTTON)
            {
                iButton = WindowApplication::MOUSE_LEFT_BUTTON;
            }
            else if (wParam & MK_MBUTTON)
            {
                iButton = WindowApplication::MOUSE_MIDDLE_BUTTON;
            }
            else if (wParam & MK_RBUTTON)
            {
                iButton = WindowApplication::MOUSE_RIGHT_BUTTON;
            }

            if (iButton >= 0)
            {
                pkTheApp->OnMotion(iButton,iXPos,iYPos,PtrToUint(wParam));
            }
            else
            {
                pkTheApp->OnPassiveMotion(iXPos,iYPos);
            }

            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            short sWParam = (short)(HIWORD(wParam));
            int iDelta = ((int)sWParam)/WHEEL_DELTA;
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            unsigned int uiModifiers = (unsigned int)(LOWORD(wParam));
            pkTheApp->OnMouseWheel(iDelta,iXPos,iYPos,uiModifiers);
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hWnd,uiMsg,wParam,lParam);
}
//----------------------------------------------------------------------------
int WindowApplication::Main (int, char**)
{
    WindowApplication* pkTheApp = (WindowApplication*)TheApplication;
    pkTheApp->KEY_TERMINATE = WindowApplication::KEY_ESCAPE;

    // Allow work to be done before the window is created.
    if (!pkTheApp->OnPrecreate())
    {
        return -1;
    }

    // Register the window class.
    static char s_acWindowClass[] = "Wild Magic Application";
    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = MsWindowEventHandler;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = 0;
    wc.hIcon         = LoadIcon(0,IDI_APPLICATION);
    wc.hCursor       = LoadCursor(0,IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = s_acWindowClass;
    wc.lpszMenuName  = 0;
    RegisterClass(&wc);

    // Require the window to have the specified client area.
    RECT kRect = { 0, 0, pkTheApp->GetWidth()-1, pkTheApp->GetHeight()-1 };
    AdjustWindowRect(&kRect,WS_OVERLAPPEDWINDOW,false);

    // Create the application window.
    HWND hWnd = CreateWindow(s_acWindowClass,pkTheApp->GetWindowTitle(),
        WS_OVERLAPPEDWINDOW,pkTheApp->GetXPosition(),pkTheApp->GetYPosition(),
        kRect.right-kRect.left+1,kRect.bottom-kRect.top+1,0,0,0,0);

    // Create the renderer.
    int iPixelFormat = 0;
    WglRenderer* pkRenderer = WM4_NEW WglRenderer(hWnd,m_eFormat,m_eDepth,
        m_eStencil,m_eBuffering,m_eMultisampling,m_iWidth,m_iHeight,
        iPixelFormat);

    // Check if multisampling is requested.  If so, the window and renderer
    // must be recreated with a pixel format for multisampling.
    if (m_eMultisampling != FrameBuffer::MT_SAMPLING_NONE)
    {
        int aiAttributes[256], iPos = 0;
        aiAttributes[iPos++] = WGL_SUPPORT_OPENGL_ARB;
        aiAttributes[iPos++] = 1;
        aiAttributes[iPos++] = WGL_DRAW_TO_WINDOW_ARB;
        aiAttributes[iPos++] = 1;
        aiAttributes[iPos++] = WGL_ACCELERATION_ARB;
        aiAttributes[iPos++] = WGL_FULL_ACCELERATION_ARB;
        aiAttributes[iPos++] = WGL_PIXEL_TYPE_ARB;
        aiAttributes[iPos++] = WGL_TYPE_RGBA_ARB;
        aiAttributes[iPos++] = WGL_RED_BITS_ARB;
        aiAttributes[iPos++] = 8;
        aiAttributes[iPos++] = WGL_GREEN_BITS_ARB;
        aiAttributes[iPos++] = 8;
        aiAttributes[iPos++] = WGL_BLUE_BITS_ARB;
        aiAttributes[iPos++] = 8;
        aiAttributes[iPos++] = WGL_ALPHA_BITS_ARB;
        aiAttributes[iPos++] = 8;

        // Request a depth buffer.
        if (m_eDepth != FrameBuffer::DT_DEPTH_NONE)
        {
            aiAttributes[iPos++] = WGL_DEPTH_BITS_ARB;
            if (m_eDepth == FrameBuffer::DT_DEPTH_16)
            {
                aiAttributes[iPos++] = 16;
            }
            else if (m_eDepth == FrameBuffer::DT_DEPTH_24)
            {
                aiAttributes[iPos++] = 24;
            }
            else // m_eDepth == FrameBuffer::DT_DEPTH_32
            {
                aiAttributes[iPos++] = 32;
            }
        }

        // Request a stencil buffer.
        if (m_eStencil == FrameBuffer::ST_STENCIL_8)
        {
            aiAttributes[iPos++] = WGL_STENCIL_BITS_ARB;
            aiAttributes[iPos++] = 8;
        }

        // Request buffering.
        if (m_eBuffering == FrameBuffer::BT_BUFFERED_DOUBLE)
        {
            aiAttributes[iPos++] = WGL_DOUBLE_BUFFER_ARB;
            aiAttributes[iPos++] = 1;
        }

        // Request sampling.
        aiAttributes[iPos++] = WGL_SAMPLE_BUFFERS_ARB;
        aiAttributes[iPos++] = 1;
        aiAttributes[iPos++] = WGL_SAMPLES_ARB;
        if (m_eMultisampling == FrameBuffer::MT_SAMPLING_2)
        {
            aiAttributes[iPos++] = 2;
        }
        else // m_eMultisampling == FrameBuffer::MT_SAMPLING_4
        {
            aiAttributes[iPos++] = 4;
        }

        aiAttributes[iPos++] = 0; // list is zero-terminated

        unsigned int uiNumFormats = 0;
        BOOL bSuccessful = wglChoosePixelFormatARB(pkRenderer->GetDC(),
            aiAttributes,0,1,&iPixelFormat,&uiNumFormats);
        if (bSuccessful && uiNumFormats > 0)
        {
            // The card supports multisampling with the requested number of
            // samples.  Recreate the window and renderer.
            WM4_DELETE pkRenderer;

            bSuccessful = DestroyWindow(hWnd);
            assert(bSuccessful);

            hWnd = CreateWindow(s_acWindowClass,pkTheApp->GetWindowTitle(),
                WS_OVERLAPPEDWINDOW,pkTheApp->GetXPosition(),
                pkTheApp->GetYPosition(),kRect.right-kRect.left+1,
                kRect.bottom-kRect.top+1,0,0,0,0);

            pkRenderer = WM4_NEW WglRenderer(hWnd,m_eFormat,m_eDepth,
                m_eStencil,m_eBuffering,m_eMultisampling,m_iWidth,m_iHeight,
                iPixelFormat);
        }
    }

    pkTheApp->SetWindowID(PtrToInt(hWnd));
    pkTheApp->SetRenderer(pkRenderer);

    if (pkTheApp->OnInitialize())
    {
        // display the window
        ShowWindow(hWnd,SW_SHOW);
        UpdateWindow(hWnd);

        // start the message pump
        bool bApplicationRunning = true;
        while (bApplicationRunning)
        {
            MSG kMsg;
            if (PeekMessage(&kMsg,0,0,0,PM_REMOVE))
            {
                if (kMsg.message == WM_QUIT)
                {
                    bApplicationRunning = false;
                    continue;
                }

                HACCEL hAccel = 0;
                if (!TranslateAccelerator(hWnd,hAccel,&kMsg))
                {
                    TranslateMessage(&kMsg);
                    DispatchMessage(&kMsg);
                }
            }
            else
            {
                pkTheApp->OnIdle();
            }
        }
    }

    pkTheApp->OnTerminate();
    return 0;
}
//----------------------------------------------------------------------------
void Application::LaunchTreeControl (Spatial* pkScene, int iXPos, int iYPos,
    int iXSize, int iYSize)
{
    TreeControl* pkControl;
    GetExtraData(0,sizeof(TreeControl*),&pkControl);
    if (!pkControl)
    {
        pkControl = WM4_NEW TreeControl(this,0,GetModuleHandle(0),0,pkScene,
            iXPos,iYPos,iXSize,iYSize);
        SetExtraData(0,sizeof(TreeControl*),&pkControl);
    }
}
//----------------------------------------------------------------------------
void Application::ShutdownTreeControl ()
{
    TreeControl* pkControl;
    GetExtraData(0,sizeof(TreeControl*),&pkControl);
    if (pkControl)
    {
        WM4_DELETE pkControl;
        pkControl = 0;
        SetExtraData(0,sizeof(TreeControl*),&pkControl);
    }
}
//----------------------------------------------------------------------------
