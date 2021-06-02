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

#include "Wm4WindowApplication.h"
#include "Wm4GlxRenderer.h"
using namespace Wm4;

const int WindowApplication::KEY_ESCAPE             = 0x1B;
const int WindowApplication::KEY_HOME               = 0x95;
const int WindowApplication::KEY_LEFT_ARROW         = 0x96;
const int WindowApplication::KEY_UP_ARROW           = 0x97;
const int WindowApplication::KEY_RIGHT_ARROW        = 0x98;
const int WindowApplication::KEY_DOWN_ARROW         = 0x99;
const int WindowApplication::KEY_PAGE_UP            = 0x9A;
const int WindowApplication::KEY_PAGE_DOWN          = 0x9B;
const int WindowApplication::KEY_END                = 0x9C;
const int WindowApplication::KEY_INSERT             = 0x9E;
const int WindowApplication::KEY_DELETE             = 0x9F;
const int WindowApplication::KEY_F1                 = 0xBE;
const int WindowApplication::KEY_F2                 = 0xBF;
const int WindowApplication::KEY_F3                 = 0xC0;
const int WindowApplication::KEY_F4                 = 0xC1;
const int WindowApplication::KEY_F5                 = 0xC2;
const int WindowApplication::KEY_F6                 = 0xC3;
const int WindowApplication::KEY_F7                 = 0xC4;
const int WindowApplication::KEY_F8                 = 0xC5;
const int WindowApplication::KEY_F9                 = 0xC6;
const int WindowApplication::KEY_F10                = 0xC7;
const int WindowApplication::KEY_F11                = 0xC8;
const int WindowApplication::KEY_F12                = 0xC9;

// TO DO.  Check to see these are correct.
const int WindowApplication::KEY_BACKSPACE          = 0x08;
const int WindowApplication::KEY_TAB                = 0x09;
const int WindowApplication::KEY_ENTER              = 0x0D;
const int WindowApplication::KEY_RETURN             = 0x0D;
// END TO DO.

const int WindowApplication::KEY_SHIFT              = 0x0001;
const int WindowApplication::KEY_CONTROL            = 0x0004;
const int WindowApplication::KEY_ALT                = 0x0008;
const int WindowApplication::KEY_COMMAND            = 0x0040;

const int WindowApplication::MOUSE_LEFT_BUTTON      = 0x0001;
const int WindowApplication::MOUSE_MIDDLE_BUTTON    = 0x0002;
const int WindowApplication::MOUSE_RIGHT_BUTTON     = 0x0003;
const int WindowApplication::MOUSE_DOWN             = 0x0004;
const int WindowApplication::MOUSE_UP               = 0x0005;

const int WindowApplication::MODIFIER_CONTROL       = 0x0004;
const int WindowApplication::MODIFIER_LBUTTON       = 0x0001;
const int WindowApplication::MODIFIER_MBUTTON       = 0x0002;
const int WindowApplication::MODIFIER_RBUTTON       = 0x0003;
const int WindowApplication::MODIFIER_SHIFT         = 0x0001;

// Indices into the extra data storage for button up-down state and for key
// key modifier up-down state.
#define GLXAPP_BUTTONDOWN  0
#define GLXAPP_SHIFTDOWN   GLXAPP_BUTTONDOWN + 8*sizeof(bool)
#define GLXAPP_CONTROLDOWN GLXAPP_SHIFTDOWN + sizeof(bool)
#define GLXAPP_ALTDOWN     GLXAPP_CONTROLDOWN + sizeof(bool)
#define GLXAPP_COMMANDDOWN GLXAPP_ALTDOWN + sizeof(bool)

//----------------------------------------------------------------------------
void WindowApplication::SetMousePosition (int iX, int iY)
{
    WindowApplication* pkTheApp = (WindowApplication*)TheApplication;
    GlxRenderer* pkRenderer = (GlxRenderer*)pkTheApp->GetRenderer();
    Display* pkDisplay = pkRenderer->GetDisplay();
    Window ulWindow = pkRenderer->GetWindow();

    XWarpPointer(pkDisplay,0,ulWindow,0,0,0,0,iX,iY);
    XFlush(pkDisplay);
}
//----------------------------------------------------------------------------
void WindowApplication::GetMousePosition (int& riX, int& riY) const
{
    WindowApplication* pkTheApp = (WindowApplication*)TheApplication;
    GlxRenderer* pkRenderer = (GlxRenderer*)pkTheApp->GetRenderer();
    Display* pkDisplay = pkRenderer->GetDisplay();
    Window ulWindow = pkRenderer->GetWindow();

    Window ulRootWindow, ulChildWindow;
    int iRootX, iRootY;
    unsigned int uiModifier;
    XQueryPointer(pkDisplay,ulWindow,&ulRootWindow,&ulChildWindow,&iRootX,
        &iRootY,&riX,&riY,&uiModifier);
}
//----------------------------------------------------------------------------
int WindowApplication::GetStringWidth (const char* acText) const
{
    assert(acText && strlen(acText) > 0);
    return 8*(int)strlen(acText);
}
//----------------------------------------------------------------------------
int WindowApplication::GetCharacterWidth (const char) const
{
    return 8;
}
//----------------------------------------------------------------------------
int WindowApplication::GetFontHeight () const
{
    return 13;
}
//----------------------------------------------------------------------------
int WindowApplication::Main (int iQuantity, char** apcArgument)
{
    WindowApplication* pkTheApp = (WindowApplication*)TheApplication;
    pkTheApp->KEY_TERMINATE = WindowApplication::KEY_ESCAPE;

    // Allow work to be done before the window is created.
    if (!pkTheApp->OnPrecreate())
    {
        return -1;
    }

    // Connect to the X server.
    const char* acDisplayName = 0;
    Display* pkDisplay = XOpenDisplay(acDisplayName);
    if (!pkDisplay)
    {
        return -2;
    }

    // Make sure the X server has OpenGL GLX extensions.
    int iErrorBase, iEventBase;
    Bool bSuccess = glXQueryExtension(pkDisplay,&iErrorBase,&iEventBase);
    assert(bSuccess);
    if (!bSuccess)
    {
        return -3;
    }

    // Partial construction of a GLX renderer.  The window for the renderer
    // is not yet constructed.  When it is, the window identifier is supplied
    // to the renderer to complete its construction.
    GlxRenderer* pkGlxRenderer = WM4_NEW GlxRenderer(pkDisplay,m_eFormat,
        m_eDepth,m_eStencil,m_eBuffering,m_eMultisampling,
        pkTheApp->GetWidth(),pkTheApp->GetHeight());
    if (!pkGlxRenderer->IsConstructed())
    {
        return -4;
    }

    pkTheApp->SetRenderer(pkGlxRenderer);

    // Create an X Window with the visual information created by the
    // renderer constructor.
    XVisualInfo* pkVisual = pkGlxRenderer->GetVisual();
    
    // The visual information might not be the default, so create an X
    // colormap to use.
    Window ulRootWindow = RootWindow(pkDisplay,pkVisual->screen);
    Colormap kCMap = XCreateColormap(pkDisplay,ulRootWindow,
        pkVisual->visual,AllocNone);

    // Set the event mask to include exposure (paint), button presses (mouse),
    // and key presses (keyboard).
    XSetWindowAttributes kWindowAttributes;
    kWindowAttributes.colormap = kCMap;
    kWindowAttributes.border_pixel = 0;
    kWindowAttributes.event_mask =
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask |
        Button1MotionMask |
        Button2MotionMask |
        Button3MotionMask |
        KeyPressMask |
        KeyReleaseMask |
        ExposureMask |
        StructureNotifyMask;

    int iXPos = pkTheApp->GetXPosition();
    int iYPos = pkTheApp->GetYPosition();
    unsigned int uiWidth = (unsigned int)pkTheApp->GetWidth();
    unsigned int uiHeight = (unsigned int)pkTheApp->GetHeight();
    unsigned int uiBorderWidth = 0;
    unsigned long ulValueMask = CWBorderPixel | CWColormap | CWEventMask;
    Window ulWindow = XCreateWindow(pkDisplay,ulRootWindow,iXPos,iYPos,
        uiWidth,uiHeight,uiBorderWidth,pkVisual->depth,InputOutput,
        pkVisual->visual,ulValueMask,&kWindowAttributes);

    XSizeHints kHints;
    kHints.flags = PPosition | PSize;
    kHints.x = iXPos;
    kHints.y = iYPos;
    kHints.width = uiWidth;
    kHints.height = uiHeight;
    XSetNormalHints(pkDisplay,ulWindow,&kHints);

    const char* acIconName = pkTheApp->GetWindowTitle();
    Pixmap kIconPixmap = None;
    XSetStandardProperties(pkDisplay,ulWindow,pkTheApp->GetWindowTitle(),
        acIconName,kIconPixmap,apcArgument,iQuantity,&kHints);

    // Finish the construction of the renderer.
    pkGlxRenderer->FinishConstruction(ulWindow);
    if (!pkGlxRenderer->IsConstructed())
    {
        return -5;
    }

    if (pkTheApp->OnInitialize())
    {
        // display the window
        XMapWindow(pkDisplay,ulWindow);

        // start the message pump
        bool bDoPump = true;
        while (bDoPump)
        {
            if (!XPending(pkDisplay))
            {
                pkTheApp->OnIdle();
                continue;
            }

            XEvent kEvent;
            XNextEvent(pkDisplay,&kEvent);
            int iIndex;
            bool bState;

            if (kEvent.type == ButtonPress || kEvent.type == ButtonRelease)
            {
                pkTheApp->OnMouseClick(kEvent.xbutton.button,
                    kEvent.xbutton.type,kEvent.xbutton.x,kEvent.xbutton.y,
                    kEvent.xbutton.state);

                if (kEvent.type == ButtonPress)
                {
                    iIndex = GLXAPP_BUTTONDOWN + kEvent.xbutton.button;
                    bState = true;
                    pkTheApp->SetExtraData(iIndex,sizeof(bool),&bState);
                }
                else
                {
                    iIndex = GLXAPP_BUTTONDOWN + kEvent.xbutton.button;
                    bState = false;
                    pkTheApp->SetExtraData(iIndex,sizeof(bool),&bState);
                }

                continue;
            }

            if (kEvent.type == MotionNotify)
            {
                int iButton = 0;

                iIndex = GLXAPP_BUTTONDOWN + 1;
                pkTheApp->GetExtraData(iIndex,sizeof(bool),&bState);
                if (bState)
                {
                    iButton = WindowApplication::MOUSE_LEFT_BUTTON;
                }
                else
                {
                    iIndex = GLXAPP_BUTTONDOWN + 2;
                    pkTheApp->GetExtraData(iIndex,sizeof(bool),&bState);
                    if (bState)
                    {
                        iButton = WindowApplication::MOUSE_MIDDLE_BUTTON;
                    }
                    else
                    {
                        iIndex = GLXAPP_BUTTONDOWN + 3;
                        pkTheApp->GetExtraData(iIndex,sizeof(bool),&bState);
                        if (bState)
                        {
                            iButton = WindowApplication::MOUSE_RIGHT_BUTTON;
                        }
                    }
                }

                if (iButton > 0)
                {
                    pkTheApp->OnMotion(iButton,kEvent.xmotion.x,
                        kEvent.xmotion.y,kEvent.xmotion.state);
                }
                else
                {
                    pkTheApp->OnPassiveMotion(kEvent.xmotion.x,
                        kEvent.xmotion.y);
                }
                continue;
            }

            if (kEvent.type == KeyPress || kEvent.type == KeyRelease)
            {
                KeySym kKeySym = XKeycodeToKeysym(pkDisplay,
                    kEvent.xkey.keycode,0);

                int iKey = (kKeySym & 0x00FF);

                // quit application if the KEY_TERMINATE key is pressed
                if (iKey == pkTheApp->KEY_TERMINATE)
                {
                    XDestroyWindow(pkDisplay,ulWindow);
                    bDoPump = false;
                    continue;
                }

                // Adjust for special keys that exist on the key pad and on
                // the number pad.
                if ((kKeySym & 0xFF00) != 0)
                {
                    if (0x50 <= iKey && iKey <= 0x57)
                    {
                        // keypad Home, {L,U,R,D}Arrow, Pg{Up,Dn}, End
                        iKey += 0x45;
                    }
                    else if (iKey == 0x63)
                    {
                        // keypad Insert
                        iKey = 0x9e;
                    }
                    else if (iKey == 0xFF)
                    {
                        // keypad Delete
                        iKey = 0x9f;
                    }
                }

                if ((WindowApplication::KEY_HOME <= iKey
                &&  iKey <= WindowApplication::KEY_END)
                ||  (WindowApplication::KEY_F1 <= iKey
                &&  iKey <= WindowApplication::KEY_F12))
                {
                    if (kEvent.type == KeyPress )
                    {
                        pkTheApp->OnSpecialKeyDown(iKey,kEvent.xbutton.x,
                            kEvent.xbutton.y);
                    }
                    else
                    {
                        pkTheApp->OnSpecialKeyUp(iKey,kEvent.xbutton.x,
                            kEvent.xbutton.y);
                    }
                }
                else
                {
                    // Get key-modifier state.
                    bState = (kEvent.xbutton.state &
                        WindowApplication::KEY_SHIFT) != 0;
                    pkTheApp->SetExtraData(GLXAPP_SHIFTDOWN,sizeof(bool),
                        &bState);

                    // Adjust for shift state.
                    unsigned char ucKey = (unsigned char)iKey;
                    if (bState && 'a' <= ucKey && ucKey <= 'z')
                    {
                        ucKey = (unsigned char)(iKey - 32);
                    }

                    bState = (kEvent.xbutton.state &
                        WindowApplication::KEY_CONTROL) != 0;
                    pkTheApp->SetExtraData(GLXAPP_CONTROLDOWN,sizeof(bool),
                        &bState);
                    
                    bState = (kEvent.xbutton.state &
                        WindowApplication::KEY_ALT) != 0;
                    pkTheApp->SetExtraData(GLXAPP_ALTDOWN,sizeof(bool),
                        &bState);

                    bState = (kEvent.xbutton.state &
                        WindowApplication::KEY_COMMAND) != 0;
                    pkTheApp->SetExtraData(GLXAPP_COMMANDDOWN,sizeof(bool),
                        &bState);

                    if (kEvent.type == KeyPress)
                    {
                        pkTheApp->OnKeyDown(ucKey,kEvent.xbutton.x,
                            kEvent.xbutton.y);
                    }
                    else
                    {
                        pkTheApp->OnKeyUp(ucKey,kEvent.xbutton.x,
                            kEvent.xbutton.y);
                    }
                }
                continue;
            }

            if (kEvent.type == Expose)
            {
                pkTheApp->OnDisplay();
                continue;
            }

            if (kEvent.type == ConfigureNotify)
            {
                pkTheApp->OnMove(kEvent.xconfigure.x,kEvent.xconfigure.y);
                pkTheApp->OnResize(kEvent.xconfigure.width,
                    kEvent.xconfigure.height);
                continue;
            }
        }
    }

    pkTheApp->OnTerminate();
    XCloseDisplay(pkDisplay);
    return 0;
}
//----------------------------------------------------------------------------
void Application::LaunchTreeControl (Spatial*, int, int, int, int)
{
    // TO DO
}
//----------------------------------------------------------------------------
void Application::ShutdownTreeControl ()
{
    // TO DO
}
//----------------------------------------------------------------------------

