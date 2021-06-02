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
inline int SoftFrameBuffer::GetWidth () const
{
    return m_iWidth;
}
//----------------------------------------------------------------------------
inline int SoftFrameBuffer::GetHeight () const
{
    return m_iHeight;
}
//----------------------------------------------------------------------------
inline int SoftFrameBuffer::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline void SoftFrameBuffer::SetColorBufferUI (unsigned int* auiColorBuffer)
{
    m_auiColorBuffer = auiColorBuffer;
    m_bColorBufferOwner = false;
}
//----------------------------------------------------------------------------
inline unsigned int* SoftFrameBuffer::GetColorBufferUI () const
{
    return m_auiColorBuffer;
}
//----------------------------------------------------------------------------
inline ColorRGBA* SoftFrameBuffer::GetColorBufferRGBA () const
{
    return m_akColorBuffer;
}
//----------------------------------------------------------------------------
inline unsigned int SoftFrameBuffer::GetMaxDepthValue () const
{
    return m_uiMaxDepthValue;
}
//----------------------------------------------------------------------------
inline unsigned int* SoftFrameBuffer::GetDepthBufferUI () const
{
    return m_auiDepthBuffer;
}
//----------------------------------------------------------------------------
inline float* SoftFrameBuffer::GetDepthBufferF () const
{
    return m_afDepthBuffer;
}
//----------------------------------------------------------------------------
inline unsigned int SoftFrameBuffer::GetMaxStencilValue () const
{
    return m_uiMaxStencilValue;
}
//----------------------------------------------------------------------------
inline unsigned int* SoftFrameBuffer::GetStencilBuffer () const
{
    return m_auiStencilBuffer;
}
//----------------------------------------------------------------------------
