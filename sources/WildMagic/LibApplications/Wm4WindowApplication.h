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
// Version: 4.0.1 (2007/02/18)

#ifndef WM4WINDOWAPPLICATION_H
#define WM4WINDOWAPPLICATION_H

#include "Wm4Application.h"

namespace Wm4
{

class WindowApplication : public Application
{
public:
    // abstract base class
    WindowApplication (const char* acWindowTitle, int iXPosition,
        int iYPosition, int iWidth, int iHeight,
        const ColorRGBA& rkBackgroundColor);
    virtual ~WindowApplication ();

    // Entry point to be implemented by the application.  The return value
    // is an exit code, if desired.
    virtual int Main (int iQuantity, char** apcArgument);

    // member access
    const char* GetWindowTitle () const;
    int GetXPosition () const;
    int GetYPosition () const;
    int GetWidth () const;
    int GetHeight () const;
    void SetRenderer (Renderer* pkRenderer);
    Renderer* GetRenderer ();
    void SetWindowID (int iWindowID);
    int GetWindowID () const;

    // event callbacks
    virtual bool OnPrecreate ();
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnMove (int iX, int iY);
    virtual void OnResize (int iWidth, int iHeight);
    virtual void OnDisplay ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);
    virtual bool OnKeyUp (unsigned char ucKey, int iX, int iY);
    virtual bool OnSpecialKeyDown (int iKey, int iX, int iY);
    virtual bool OnSpecialKeyUp (int iKey, int iX, int iY);
    virtual bool OnMouseClick (int iButton, int iState, int iX, int iY,
        unsigned int uiModifiers);
    virtual bool OnMotion (int iButton, int iX, int iY,
        unsigned int uiModifiers);
    virtual bool OnPassiveMotion (int iX, int iY);
    virtual bool OnMouseWheel (int iDelta, int iX, int iY,
        unsigned int uiModifiers);

    // mouse position
    void SetMousePosition (int iX, int iY);
    void GetMousePosition (int& riX, int& riY) const;

    // Font information.  These are platform-specific, so classes that
    // implement the WindowApplication interfaces must implement these
    // functions.  They are not defined by WindowApplication.
    int GetStringWidth (const char* acText) const;
    int GetCharacterWidth (const char cCharacter) const;
    int GetFontHeight () const;

    // Key identifiers.  These are platform-specific, so classes that
    // implement the WindowApplication interfaces must define these variables.
    // They are not defined by WindowApplication.
    int KEY_TERMINATE;  // default KEY_ESCAPE, redefine as desired
    static const int KEY_ESCAPE;
    static const int KEY_LEFT_ARROW;
    static const int KEY_RIGHT_ARROW;
    static const int KEY_UP_ARROW;
    static const int KEY_DOWN_ARROW;
    static const int KEY_HOME;
    static const int KEY_END;
    static const int KEY_PAGE_UP;
    static const int KEY_PAGE_DOWN;
    static const int KEY_INSERT;
    static const int KEY_DELETE;
    static const int KEY_F1;
    static const int KEY_F2;
    static const int KEY_F3;
    static const int KEY_F4;
    static const int KEY_F5;
    static const int KEY_F6;
    static const int KEY_F7;
    static const int KEY_F8;
    static const int KEY_F9;
    static const int KEY_F10;
    static const int KEY_F11;
    static const int KEY_F12;
    static const int KEY_BACKSPACE;
    static const int KEY_TAB;
    static const int KEY_ENTER;
    static const int KEY_RETURN;

    // keyboard modifiers
    static const int KEY_SHIFT;
    static const int KEY_CONTROL;
    static const int KEY_ALT;
    static const int KEY_COMMAND;

    // mouse buttons
    static const int MOUSE_LEFT_BUTTON;
    static const int MOUSE_MIDDLE_BUTTON;
    static const int MOUSE_RIGHT_BUTTON;

    // mouse state
    static const int MOUSE_UP;
    static const int MOUSE_DOWN;

    // mouse modifiers
    static const int MODIFIER_CONTROL;
    static const int MODIFIER_LBUTTON;
    static const int MODIFIER_MBUTTON;
    static const int MODIFIER_RBUTTON;
    static const int MODIFIER_SHIFT;

protected:
    // The hookup to the 'main' entry point into the executable.
    static int Run (int iQuantity, char** apcArgument);

    // constructor inputs
    const char* m_acWindowTitle;
    int m_iXPosition, m_iYPosition, m_iWidth, m_iHeight;
    ColorRGBA m_kBackgroundColor;

    // An identifier for the window (representation is platform-specific).
    int m_iWindowID;

    // The frame buffer parameters for the renderer creation.  You may set
    // these to values different than the defaults during your application
    // constructor call.
    FrameBuffer::FormatType m_eFormat;
    FrameBuffer::DepthType m_eDepth;
    FrameBuffer::StencilType m_eStencil;
    FrameBuffer::BufferingType m_eBuffering;
    FrameBuffer::MultisamplingType m_eMultisampling;

    // The renderer (used for 2D and 3D applications).
    Renderer* m_pkRenderer;
};

#include "Wm4WindowApplication.inl"

}

#endif
