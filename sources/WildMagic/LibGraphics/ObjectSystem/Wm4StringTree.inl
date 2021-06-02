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
inline int StringTree::GetStringQuantity () const
{
    return (int)m_kStrings.size();
}
//----------------------------------------------------------------------------
inline void StringTree::SetString (int i, char* acString)
{
    assert(0 <= i && i < (int)m_kStrings.size());
    m_kStrings[i] = acString;
}
//----------------------------------------------------------------------------
inline char* StringTree::GetString (int i)
{
    assert(0 <= i && i < (int)m_kStrings.size());
    return m_kStrings[i];
}
//----------------------------------------------------------------------------
inline void StringTree::Append (char* acString)
{
    m_kStrings.push_back(acString);
}
//----------------------------------------------------------------------------
inline int StringTree::GetChildQuantity () const
{
    return (int)m_kChildren.size();
}
//----------------------------------------------------------------------------
inline void StringTree::SetChild (int i, StringTree* pkChild)
{
    assert(0 <= i && i < (int)m_kStrings.size());
    m_kChildren[i] = pkChild;
}
//----------------------------------------------------------------------------
inline StringTree* StringTree::GetChild (int i)
{
    assert(0 <= i && i < (int)m_kChildren.size());
    return m_kChildren[i];
}
//----------------------------------------------------------------------------
inline void StringTree::Append (StringTree* pkChild)
{
    m_kChildren.push_back(pkChild);
}
//----------------------------------------------------------------------------
template <class T>
StringTree* Format (const char* acTitle, int iQuantity, const T* atValue)
{
    StringTree* pkTree = WM4_NEW StringTree;
    pkTree->Append(Format(acTitle));

    pkTree->Append(Format("quantity =",iQuantity));
    for (int i = 0; i < iQuantity; i++)
    {
        const size_t uiSize = 256;
        char acPrefix[uiSize];
        System::Sprintf(acPrefix,uiSize,"%d: ",i);
        T tValue = atValue[i];
        pkTree->Append(Format(acPrefix,tValue));
    }

    return pkTree;
}
//----------------------------------------------------------------------------
