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

//----------------------------------------------------------------------------
inline FrameBuffer::FormatType FrameBuffer::GetFormatType () const
{
    return m_eFormat;
}
//----------------------------------------------------------------------------
inline FrameBuffer::DepthType FrameBuffer::GetDepthType () const
{
    return m_eDepth;
}
//----------------------------------------------------------------------------
inline FrameBuffer::StencilType FrameBuffer::GetStencilType () const
{
    return m_eStencil;
}
//----------------------------------------------------------------------------
inline FrameBuffer::BufferingType FrameBuffer::GetBufferingType () const
{
    return m_eBuffering;
}
//----------------------------------------------------------------------------
inline FrameBuffer::MultisamplingType FrameBuffer::GetMultisamplingType ()
    const
{
    return m_eMultisampling;
}
//----------------------------------------------------------------------------
inline Renderer* FrameBuffer::GetRenderer ()
{
    return m_pkRenderer;
}
//----------------------------------------------------------------------------
inline Texture* FrameBuffer::GetTarget ()
{
    return m_pkTarget;
}
//----------------------------------------------------------------------------
