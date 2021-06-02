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
// Version: 4.7.0 (2008/09/15)

//----------------------------------------------------------------------------
template <class T>
UnorderedSet<T>::UnorderedSet (int iMaxQuantity, int iGrow)
{
    if (iMaxQuantity == 0)
    {
        iMaxQuantity = DEFAULT_GROW;
    }

    if (iGrow == 0)
    {
        iGrow = DEFAULT_GROW;
    }

    m_iQuantity = 0;
    m_iMaxQuantity = iMaxQuantity;
    m_iGrow = iGrow;
    m_atElement = WM4_NEW T[m_iMaxQuantity];
}
//----------------------------------------------------------------------------
template <class T>
UnorderedSet<T>::UnorderedSet (const UnorderedSet& rkSet)
{
    m_atElement = 0;
    *this = rkSet;
}
//----------------------------------------------------------------------------
template <class T>
UnorderedSet<T>::~UnorderedSet ()
{
    WM4_DELETE[] m_atElement;
}
//----------------------------------------------------------------------------
template <class T>
void UnorderedSet<T>::Clear ()
{
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
template <class T>
void UnorderedSet<T>::Reset (int iMaxQuantity, int iGrow)
{
    WM4_DELETE[] m_atElement;

    if (iMaxQuantity == 0)
    {
        iMaxQuantity = DEFAULT_GROW;
    }

    if (iGrow == 0)
    {
        iGrow = DEFAULT_GROW;
    }

    m_iQuantity = 0;
    m_iMaxQuantity = iMaxQuantity;
    m_iGrow = iGrow;
    m_atElement = WM4_NEW T[m_iMaxQuantity];
}
//----------------------------------------------------------------------------
template <class T>
UnorderedSet<T>& UnorderedSet<T>::operator= (const UnorderedSet& rkSet)
{
    m_iQuantity = rkSet.m_iQuantity;
    m_iMaxQuantity = rkSet.m_iMaxQuantity;
    m_iGrow = rkSet.m_iGrow;
    WM4_DELETE[] m_atElement;
    m_atElement = WM4_NEW T[m_iMaxQuantity];
    for (int i = 0; i < m_iMaxQuantity; i++)
    {
        m_atElement[i] = rkSet.m_atElement[i];
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class T>
int UnorderedSet<T>::GetMaxQuantity () const
{
    return m_iMaxQuantity;
}
//----------------------------------------------------------------------------
template <class T>
int UnorderedSet<T>::GetGrow () const
{
    return m_iGrow;
}
//----------------------------------------------------------------------------
template <class T>
int UnorderedSet<T>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class T>
const T& UnorderedSet<T>::Get (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_atElement[i];
}
//----------------------------------------------------------------------------
template <class T>
T& UnorderedSet<T>::operator[] (int i)
{
    assert(0 <= i && i < m_iQuantity);
    return m_atElement[i];
}
//----------------------------------------------------------------------------
template <class T>
bool UnorderedSet<T>::Exists (const T& rtElement) const
{
    for (int i = 0; i < m_iQuantity; i++)
    {
        if (rtElement == m_atElement[i])
        {
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <class T>
bool UnorderedSet<T>::Insert (const T& rtElement)
{
    int i;
    for (i = 0; i < m_iQuantity; i++)
    {
        if (rtElement == m_atElement[i])
        {
            return false;
        }
    }

    if (m_iQuantity == m_iMaxQuantity)
    {
        int iNewMaxQuantity = m_iMaxQuantity + m_iGrow;
        T* atNewElement = WM4_NEW T[iNewMaxQuantity];
        for (i = 0; i < m_iMaxQuantity; i++)
        {
            atNewElement[i] = m_atElement[i];
        }
        WM4_DELETE[] m_atElement;
        m_iMaxQuantity = iNewMaxQuantity;
        m_atElement = atNewElement;
    }

    m_atElement[m_iQuantity++] = rtElement;
    return true;
}
//----------------------------------------------------------------------------
template <class T>
int UnorderedSet<T>::Append (const T& rtElement)
{
    if (m_iQuantity == m_iMaxQuantity)
    {
        int iNewMaxQuantity = m_iMaxQuantity + m_iGrow;
        T* atNewElement = WM4_NEW T[iNewMaxQuantity];
        for (int i = 0; i < m_iMaxQuantity; i++)
        {
            atNewElement[i] = m_atElement[i];
        }
        WM4_DELETE[] m_atElement;
        m_iMaxQuantity = iNewMaxQuantity;
        m_atElement = atNewElement;
    }

    int iLocation = m_iQuantity++;
    m_atElement[iLocation] = rtElement;
    return iLocation;
}
//----------------------------------------------------------------------------
template <class T>
bool UnorderedSet<T>::Remove (const T& rtElement, int* piOld, int* piNew)
{
    for (int i = 0; i < m_iQuantity; i++)
    {
        if (rtElement == m_atElement[i])
        {
            m_iQuantity--;
            if (piOld)
            {
                *piOld = m_iQuantity;
            }

            if (i != m_iQuantity)
            {
                m_atElement[i] = m_atElement[m_iQuantity];
                if (piNew)
                {
                    *piNew = i;
                }
            }
            else
            {
                if (piNew)
                {
                    *piNew = -1;
                }
            }
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <class T>
bool UnorderedSet<T>::RemoveAt (int i, int* piOld, int* piNew)
{
    if (0 <= i && i < m_iQuantity)
    {
        m_iQuantity--;
        if (piOld)
        {
            *piOld = m_iQuantity;
        }

        if (i != m_iQuantity)
        {
            m_atElement[i] = m_atElement[m_iQuantity];
            if (piNew)
            {
                *piNew = i;
            }
        }
        else
        {
            if (piNew)
            {
                *piNew = -1;
            }
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class T>
void UnorderedSet<T>::Compactify ()
{
    if (m_iQuantity > 0)
    {
        T* atNewElement = WM4_NEW T[m_iQuantity];
        for (int i = 0; i < m_iQuantity; i++)
        {
            atNewElement[i] = m_atElement[i];
        }
        WM4_DELETE[] m_atElement;
        m_iMaxQuantity = m_iQuantity;
        m_atElement = atNewElement;
    }
    else
    {
        m_iQuantity = 0;
        m_iMaxQuantity = DEFAULT_GROW;
        m_iGrow = DEFAULT_GROW;
        WM4_DELETE[] m_atElement;
        m_atElement = WM4_NEW int[m_iMaxQuantity];
    }
}
//----------------------------------------------------------------------------
