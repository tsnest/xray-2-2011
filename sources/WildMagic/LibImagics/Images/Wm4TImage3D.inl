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
// Version: 4.0.1 (2006/09/21)

//----------------------------------------------------------------------------
template <class T>
TImage3D<T>::TImage3D (int iXBound, int iYBound, int iZBound, T* atData)
    :
    TImage<T>(3)
{
    int* aiBound = WM4_NEW int[3];
    aiBound[0] = iXBound;
    aiBound[1] = iYBound;
    aiBound[2] = iZBound;
    this->SetBounds(aiBound);
    SetData(atData);
}
//----------------------------------------------------------------------------
template <class T>
TImage3D<T>::TImage3D (const TImage3D& rkImage)
    :
    TImage<T>(rkImage)
{
}
//----------------------------------------------------------------------------
template <class T>
TImage3D<T>::TImage3D (const char* acFilename)
    :
    TImage<T>(acFilename)
{
}
//----------------------------------------------------------------------------
template <class T>
T& TImage3D<T>::operator() (int iX, int iY, int iZ) const
{
    // assert:  x < bound[0] && y < bound[1] && z < bound[2]
    return m_atData[iX + this->m_aiBound[0]*(iY + this->m_aiBound[1]*iZ)];
}
//----------------------------------------------------------------------------
template <class T>
int TImage3D<T>::GetIndex (int iX, int iY, int iZ) const
{
    // assert:  x < bound[0] && y < bound[1] && z < bound[2]
    return iX + this->m_aiBound[0]*(iY + this->m_aiBound[1]*iZ);
}
//----------------------------------------------------------------------------
template <class T>
void TImage3D<T>::GetCoordinates (int iIndex, int& riX, int& riY, int& riZ)
    const
{
    // assert:  iIndex < m_iQuantity
    riX = iIndex % this->m_aiBound[0];
    iIndex /= this->m_aiBound[0];
    riY = iIndex % this->m_aiBound[1];
    riZ = iIndex / this->m_aiBound[1];
}
//----------------------------------------------------------------------------
template <class T>
TImage3D<T>& TImage3D<T>::operator= (const TImage3D& rkImage)
{
    return (TImage3D<T>&) TImage<T>::operator=(rkImage);
}
//----------------------------------------------------------------------------
template <class T>
TImage3D<T>& TImage3D<T>::operator= (T tValue)
{
    return (TImage3D<T>&) TImage<T>::operator=(tValue);
}
//----------------------------------------------------------------------------
