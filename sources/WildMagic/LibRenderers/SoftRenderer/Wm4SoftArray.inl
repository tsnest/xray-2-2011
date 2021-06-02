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
template <typename Type>
SoftArray<Type>::SoftArray (int iMaxQuantity)
{
    assert(iMaxQuantity > 0);
    if (iMaxQuantity <= 0)
    {
        iMaxQuantity = 1;
    }

    m_iQuantity = 0;
    m_iMaxQuantity = iMaxQuantity;
    m_atData = WM4_NEW Type[m_iMaxQuantity];
}
//----------------------------------------------------------------------------
template <typename Type>
SoftArray<Type>::SoftArray (const SoftArray& rkObject)
{
    m_atData = 0;
    *this = rkObject;
}
//----------------------------------------------------------------------------
template <typename Type>
SoftArray<Type>::~SoftArray ()
{
    WM4_DELETE[] m_atData;
}
//----------------------------------------------------------------------------
template <typename Type>
SoftArray<Type>& SoftArray<Type>::operator= (const SoftArray& rkObject)
{
    m_iQuantity = rkObject.m_iQuantity;
    m_iMaxQuantity = rkObject.m_iMaxQuantity;

    WM4_DELETE[] m_atData;
    if (m_iMaxQuantity > 0)
    {
        const size_t uiSize = m_iMaxQuantity*sizeof(Type);
        m_atData = WM4_NEW Type[m_iMaxQuantity];
        System::Memcpy(m_atData,uiSize,rkObject.m_atData,uiSize);
    }
    else
    {
        m_atData = 0;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename Type>
int SoftArray<Type>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <typename Type>
int SoftArray<Type>::GetMaxQuantity () const
{
    return m_iMaxQuantity;
}
//----------------------------------------------------------------------------
template <typename Type>
Type* SoftArray<Type>::GetData ()
{
    return m_atData;
}
//----------------------------------------------------------------------------
template <typename Type>
const Type* SoftArray<Type>::GetData () const
{
    return m_atData;
}
//----------------------------------------------------------------------------
template <typename Type>
void SoftArray<Type>::EnsureCapacity (int iRequestedMaxQuantity)
{
    if (iRequestedMaxQuantity > m_iMaxQuantity)
    {
        const size_t uiOldSize = m_iMaxQuantity*sizeof(Type);
        const size_t uiNewSize = iRequestedMaxQuantity*sizeof(Type);
        Type* atNewArray = WM4_NEW Type[iRequestedMaxQuantity];
        System::Memcpy(atNewArray,uiNewSize,m_atData,uiOldSize);
        WM4_DELETE[] m_atData;
        m_atData = atNewArray;
        m_iMaxQuantity = iRequestedMaxQuantity;
    }
}
//----------------------------------------------------------------------------
template <typename Type>
void SoftArray<Type>::Clear ()
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Type>
void SoftArray<Type>::Append (Type tValue)
{
    int i = m_iQuantity++;
    assert(m_iQuantity <= m_iMaxQuantity);
    m_atData[i] = tValue;
}
//----------------------------------------------------------------------------
template <typename Type>
Type SoftArray<Type>::Get (int i) const
{
    assert(0 <= i && i < m_iMaxQuantity);
    return m_atData[i];
}
//----------------------------------------------------------------------------
