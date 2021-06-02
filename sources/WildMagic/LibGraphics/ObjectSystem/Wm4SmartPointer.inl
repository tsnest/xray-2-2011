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
template <class T>
Pointer<T>::Pointer (T* pkObject)
{
    m_pkObject = pkObject;
    if (m_pkObject)
    {
        m_pkObject->IncrementReferences();
    }
}
//----------------------------------------------------------------------------
template <class T>
Pointer<T>::Pointer (const Pointer& rkPointer)
{
    m_pkObject = rkPointer.m_pkObject;
    if (m_pkObject)
    {
        m_pkObject->IncrementReferences();
    }
}
//----------------------------------------------------------------------------
template <class T>
Pointer<T>::~Pointer ()
{
    if (m_pkObject)
    {
        m_pkObject->DecrementReferences();
    }
}
//----------------------------------------------------------------------------
template <class T>
Pointer<T>::operator T* () const
{
    return m_pkObject;
}
//----------------------------------------------------------------------------
template <class T>
T& Pointer<T>::operator* () const
{
    return *m_pkObject;
}
//----------------------------------------------------------------------------
template <class T>
T* Pointer<T>::operator-> () const
{
    return m_pkObject;
}
//----------------------------------------------------------------------------
template <class T>
Pointer<T>& Pointer<T>::operator= (T* pkObject)
{
    if (m_pkObject != pkObject)
    {
        if (pkObject)
        {
            pkObject->IncrementReferences();
        }

        if (m_pkObject)
        {
            m_pkObject->DecrementReferences();
        }

        m_pkObject = pkObject;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <class T>
Pointer<T>& Pointer<T>::operator= (const Pointer& rkPointer)
{
    if (m_pkObject != rkPointer.m_pkObject)
    {
        if (rkPointer.m_pkObject)
        {
            rkPointer.m_pkObject->IncrementReferences();
        }

        if (m_pkObject)
        {
            m_pkObject->DecrementReferences();
        }

        m_pkObject = rkPointer.m_pkObject;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <class T>
bool Pointer<T>::operator== (T* pkObject) const
{
    return m_pkObject == pkObject;
}
//----------------------------------------------------------------------------
template <class T>
bool Pointer<T>::operator!= (T* pkObject) const
{
    return m_pkObject != pkObject;
}
//----------------------------------------------------------------------------
template <class T>
bool Pointer<T>::operator== (const Pointer& rkPointer) const
{
    return m_pkObject == rkPointer.m_pkObject;
}
//----------------------------------------------------------------------------
template <class T>
bool Pointer<T>::operator!= (const Pointer& rkPointer) const
{
    return m_pkObject != rkPointer.m_pkObject;
}
//----------------------------------------------------------------------------
