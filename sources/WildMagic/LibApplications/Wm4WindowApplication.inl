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
// Version: 4.0.2 (2007/05/06)

//----------------------------------------------------------------------------
inline const char* WindowApplication::GetWindowTitle () const
{
    return m_acWindowTitle;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetXPosition () const
{
    return m_iXPosition;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetYPosition () const
{
    return m_iYPosition;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetWidth () const
{
    return m_iWidth;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetHeight () const
{
    return m_iHeight;
}
//----------------------------------------------------------------------------
inline Renderer* WindowApplication::GetRenderer ()
{
    return m_pkRenderer;
}
//----------------------------------------------------------------------------
inline void WindowApplication::SetRenderer (Renderer* pkRenderer)
{
    m_pkRenderer = pkRenderer;
}
//----------------------------------------------------------------------------
inline void WindowApplication::SetWindowID (int iWindowID)
{
    m_iWindowID = iWindowID;
}
//----------------------------------------------------------------------------
inline int WindowApplication::GetWindowID () const
{
    return m_iWindowID;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnPrecreate ()
{
    // stub for derived classes
    return true;
}
//----------------------------------------------------------------------------
inline void WindowApplication::OnDisplay ()
{
    // stub for derived classes
}
//----------------------------------------------------------------------------
inline void WindowApplication::OnIdle ()
{
    // stub for derived classes
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnKeyDown (unsigned char, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnKeyUp (unsigned char, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnSpecialKeyDown (int, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnSpecialKeyUp (int, int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnMouseClick (int, int, int, int, unsigned int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnMotion (int, int, int, unsigned int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnPassiveMotion (int, int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
inline bool WindowApplication::OnMouseWheel (int, int, int, unsigned int)
{
    // stub for derived classes
    return false;
}
//----------------------------------------------------------------------------
