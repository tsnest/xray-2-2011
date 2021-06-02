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
// Version: 4.0.1 (2007/06/16)

//----------------------------------------------------------------------------
inline const std::string& Program::GetProgramText () const
{
    return m_kProgramText;
}
//----------------------------------------------------------------------------
inline const Attributes& Program::GetInputAttributes () const
{
    return m_kInputAttributes;
}
//----------------------------------------------------------------------------
inline const Attributes& Program::GetOutputAttributes () const
{
    return m_kOutputAttributes;
}
//----------------------------------------------------------------------------
inline int Program::GetRendererConstantQuantity () const
{
    return (int)m_kRendererConstants.size();
}
//----------------------------------------------------------------------------
inline int Program::GetNumericalConstantQuantity () const
{
    return (int)m_kNumericalConstants.size();
}
//----------------------------------------------------------------------------
inline int Program::GetUserConstantQuantity () const
{
    return (int)m_kUserConstants.size();
}
//----------------------------------------------------------------------------
inline int Program::GetSamplerInformationQuantity () const
{
    return (int)m_kSamplerInformation.size();
}
//----------------------------------------------------------------------------
