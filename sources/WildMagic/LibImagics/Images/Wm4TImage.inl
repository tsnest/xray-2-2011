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
TImage<T>::TImage (int iDimensions, int* aiBound, T* atData)
    :
    Lattice(iDimensions,aiBound)
{
    SetData(atData);
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::TImage (const TImage& rkImage)
    :
    Lattice(rkImage)
{
    m_atData = WM4_NEW T[m_iQuantity];
    size_t uiSize = m_iQuantity*sizeof(T);
    System::Memcpy(m_atData,uiSize,rkImage.m_atData,uiSize);
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::TImage (const char* acFilename)
{
    Load(acFilename);
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::TImage (int iDimensions)
    :
    Lattice(iDimensions)
{
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>::~TImage ()
{
    WM4_DELETE[] m_atData;
}
//----------------------------------------------------------------------------
template <class T>
void TImage<T>::SetData (T* atData)
{
    if (atData)
    {
        m_atData = atData;
    }
    else
    {
        assert(m_iQuantity > 0);
        m_atData = WM4_NEW T[m_iQuantity];
        memset(m_atData,0,m_iQuantity*sizeof(T));
    }
}
//----------------------------------------------------------------------------
template <class T>
T* TImage<T>::GetData () const
{
    return m_atData;
}
//----------------------------------------------------------------------------
template <class T>
T& TImage<T>::operator[] (int i) const
{
    // assert:  i < m_iQuantity
    return m_atData[i];
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>& TImage<T>::operator= (const TImage& rkImage)
{
    Lattice::operator=(rkImage);

    WM4_DELETE[] m_atData;
    m_atData = WM4_NEW T[m_iQuantity];
    size_t uiSize = m_iQuantity*sizeof(T);
    System::Memcpy(m_atData,uiSize,rkImage.m_atData,uiSize);

    return *this;
}
//----------------------------------------------------------------------------
template <class T>
TImage<T>& TImage<T>::operator= (T tValue)
{
    for (int i = 0; i < m_iQuantity; i++)
    {
        m_atData[i] = tValue;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::operator== (const TImage& rkImage) const
{
    if (Lattice::operator!=(rkImage))
    {
        return false;
    }

    return memcmp(m_atData,rkImage.m_atData,m_iQuantity*sizeof(T)) == 0;
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::operator!= (const TImage& rkImage) const
{
    return !operator==(rkImage);
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::Load (const char* acFilename)
{
    FILE* pkIFile = System::Fopen(acFilename,"rb");
    if (!pkIFile)
    {
        m_iDimensions = 0;
        m_iQuantity = 0;
        m_aiBound = 0;
        m_aiOffset = 0;
        m_atData = 0;
        return false;
    }

    if (!Lattice::Load(pkIFile))
    {
        m_atData = 0;
        System::Fclose(pkIFile);
        return false;
    }

    int iRTTI, iSizeOf;
    System::Read4le(pkIFile,1,&iRTTI);
    System::Read4le(pkIFile,1,&iSizeOf);

    m_atData = WM4_NEW T[m_iQuantity];
    if (iRTTI == T::GetRTTI())
    {
        const int iTypeSize = sizeof(T);
        switch (iTypeSize)
        {
        case 1:
            System::Read1(pkIFile,m_iQuantity,m_atData);
            break;
        case 2:
            System::Read2le(pkIFile,m_iQuantity,m_atData);
            break;
        case 4:
            System::Read4le(pkIFile,m_iQuantity,m_atData);
            break;
        case 8:
            System::Read8le(pkIFile,m_iQuantity,m_atData);
            break;
        default:
            assert(false);
        }
    }
    else
    {
        char* acFileData = WM4_NEW char[m_iQuantity*iSizeOf];
        switch (iSizeOf)
        {
        case 1:
            System::Read1(pkIFile,m_iQuantity,acFileData);
            break;
        case 2:
            System::Read2le(pkIFile,m_iQuantity,acFileData);
            break;
        case 4:
            System::Read4le(pkIFile,m_iQuantity,acFileData);
            break;
        case 8:
            System::Read8le(pkIFile,m_iQuantity,acFileData);
            break;
        default:
            assert(false);
        }
        ImageConvert(m_iQuantity,iRTTI,acFileData,T::GetRTTI(),m_atData);
        WM4_DELETE[] acFileData;
    }

    System::Fclose(pkIFile);
    return true;
}
//----------------------------------------------------------------------------
template <class T>
bool TImage<T>::Save (const char* acFilename) const
{
    FILE* pkOFile = System::Fopen(acFilename,"wb");
    if (!pkOFile)
    {
        return false;
    }

    if (!Lattice::Save(pkOFile))
    {
        System::Fclose(pkOFile);
        return false;
    }

    int iRTTI = T::GetRTTI();
    System::Write4le(pkOFile,1,&iRTTI);

    int iSizeOf = (int)(sizeof(T));
    System::Write4le(pkOFile,1,&iSizeOf);

    const int iTypeSize = sizeof(T);
    switch (iTypeSize)
    {
    case 1:
        System::Write1(pkOFile,m_iQuantity,m_atData);
        break;
    case 2:
        System::Write2le(pkOFile,m_iQuantity,m_atData);
        break;
    case 4:
        System::Write4le(pkOFile,m_iQuantity,m_atData);
        break;
    case 8:
        System::Write8le(pkOFile,m_iQuantity,m_atData);
        break;
    default:
        assert(false);
    }

    System::Fclose(pkOFile);
    return true;
}
//----------------------------------------------------------------------------
