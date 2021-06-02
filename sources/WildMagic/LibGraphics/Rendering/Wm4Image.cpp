// Wild Magic Source 77Code
// David Eberly
// http://www.geometrictools.com
// Copyright (c) 1998-2007
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// either of the locations:
//     http://www.gnu.org/copyleft/lgpl.html
//     http://www.geometrictools.com/License/WildMagicLicense.pdf
//
// Version: 4.0.9 (2009/02/27)

#include "Wm4GraphicsPCH.h"
#include "Wm4Image.h"
#include "Wm4BitHacks.h"
#include "Wm4Catalog.h"
#include "Wm4ImageVersion.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Image,Object);
WM4_IMPLEMENT_STREAM(Image);
WM4_IMPLEMENT_DEFAULT_NAME_ID(Image,Object);

int Image::ms_aiBytesPerPixel[Image::IT_QUANTITY] =
{
    3,  // IT_RGB888
    4,  // IT_RGBA8888
    4,  // IT_DEPTH16
    4,  // IT_DEPTH24
    4,  // IT_DEPTH32
    3,  // IT_CUBE_RGB888
    4,  // IT_CUBE_RGBA8888
    12, // IT_RGB32F
    16, // IT_RGBA32F
    6,  // IT_RGB16F
    8,  // IT_RGBA16F
    6,  // IT_RGB16I
    8,  // IT_RGBA16I
    1,  // IT_INTENSITY8I
    2,  // IT_INTENSITY16I
    2,  // IT_INTENSITY16F
    4,  // IT_INTENSITY32F
    2,  // IT_RGB565
    2,  // IT_RGBA5551
    2   // IT_RGBA4444
};

std::string Image::ms_akFormatName[Image::IT_QUANTITY] =
{
    std::string("IT_RGB888"),
    std::string("IT_RGBA8888"),
    std::string("IT_DEPTH16"),
    std::string("IT_DEPTH24"),
    std::string("IT_DEPTH32"),
    std::string("IT_CUBE_RGB888"),
    std::string("IT_CUBE_RGBA8888"),
    std::string("IT_RGB32F"),
    std::string("IT_RGBA32F"),
    std::string("IT_RGB16F"),
    std::string("IT_RGBA16F"),
    std::string("IT_RGB16I"),
    std::string("IT_RGBA16I"),
    std::string("IT_INTENSITY8I"),
    std::string("IT_INTENSITY16I"),
    std::string("IT_INTENSITY16F"),
    std::string("IT_INTENSITY32F"),
    std::string("IT_RGB565"),
    std::string("IT_RGBA5551"),
    std::string("IT_RGBA4444")
};

//----------------------------------------------------------------------------
Image::Image (FormatMode eFormat, int iBound0, unsigned char* aucData,
    const char* acImageName)
{
    // The 'assert' is commented out to allow non-power-of-two framebuffer
    // objects.  However, the software renderer still needs power-of-two
    // until I modify it to handle non-power-of-two.
    //
    //assert(IsPowerOfTwo((unsigned int)iBound0));
    assert(acImageName);

    m_eFormat = eFormat;
    m_iDimension = 1;
    m_aiBound[0] = iBound0;
    m_aiBound[1] = 1;
    m_aiBound[2] = 1;
    m_iQuantity = iBound0;
    m_aucData = aucData;
    SetName(acImageName);
    Catalog<Image>::GetActive()->Insert(this);
}
//----------------------------------------------------------------------------
Image::Image (FormatMode eFormat, int iBound0, int iBound1,
    unsigned char* aucData, const char* acImageName)
{
    // The 'assert' is commented out to allow non-power-of-two framebuffer
    // objects.  However, the software renderer still needs power-of-two
    // until I modify it to handle non-power-of-two.
    //
    //assert(IsPowerOfTwo((unsigned int)iBound0)
    //    && IsPowerOfTwo((unsigned int)iBound1));
    assert(acImageName);

    m_eFormat = eFormat;
    m_iDimension = 2;
    m_aiBound[0] = iBound0;
    m_aiBound[1] = iBound1;
    m_aiBound[2] = 1;
    m_iQuantity = iBound0*iBound1;
    m_aucData = aucData;
    SetName(acImageName);
    Catalog<Image>::GetActive()->Insert(this);
}
//----------------------------------------------------------------------------
Image::Image (FormatMode eFormat, int iBound0, int iBound1, int iBound2,
    unsigned char* aucData, const char* acImageName)
{
    // The 'assert' is commented out to allow non-power-of-two framebuffer
    // objects.  However, the software renderer still needs power-of-two
    // until I modify it to handle non-power-of-two.
    //
    //assert(IsPowerOfTwo((unsigned int)iBound0)
    //    && IsPowerOfTwo((unsigned int)iBound1)
    //    && IsPowerOfTwo((unsigned int)iBound2));
    assert(acImageName);

    m_eFormat = eFormat;
    m_iDimension = 3;
    m_aiBound[0] = iBound0;
    m_aiBound[1] = iBound1;
    m_aiBound[2] = iBound2;
    m_iQuantity = iBound0*iBound1*iBound2;
    m_aucData = aucData;
    SetName(acImageName);
    Catalog<Image>::GetActive()->Insert(this);
}
//----------------------------------------------------------------------------
Image::Image ()
{
    m_eFormat = IT_QUANTITY;
    m_iDimension = 0;
    m_aiBound[0] = 0;
    m_aiBound[1] = 0;
    m_aiBound[2] = 0;
    m_iQuantity = 0;
    m_aucData = 0;
}
//----------------------------------------------------------------------------
Image::~Image ()
{
    WM4_DELETE[] m_aucData;

    Catalog<Image>::RemoveAll(this);
}
//----------------------------------------------------------------------------
void Image::AllocateData ()
{
    if (!m_aucData)
    {
        int iNumBytes = m_iQuantity*ms_aiBytesPerPixel[m_eFormat];
        m_aucData = WM4_NEW unsigned char[iNumBytes];
    }
}
//----------------------------------------------------------------------------
Image* Image::Load (const std::string& rkImageName, bool bAbsolutePath)
{
    const char* acDecorated;
    if (bAbsolutePath)
    {
        acDecorated = rkImageName.c_str();
    }
    else
    {
        std::string kFilename = rkImageName + std::string(".wmif");
        acDecorated = System::GetPath(kFilename.c_str(),
        System::SM_READ);
    }
    if (!acDecorated)
    {
        return 0;
    }

    char* acBuffer;
    int iSize;
    bool bLoaded = System::Load(acDecorated,acBuffer,iSize);
    if (!bLoaded)
    {
        // file does not exist
        return 0;
    }
    if (iSize < ImageVersion::LENGTH)
    {
        // file not large enough to store version string
        WM4_DELETE[] acBuffer;
        return 0;
    }

    // read the file version
    ImageVersion kVersion(acBuffer);
    if (!kVersion.IsValid())
    {
        WM4_DELETE[] acBuffer;
        return 0;
    }

    char* pcCurrent = acBuffer + ImageVersion::LENGTH;

    // read the image format and dimensions
    int iFormat, iDimension, aiBound[3];
    pcCurrent += System::Read4le(pcCurrent,1,&iFormat);
    if (kVersion >= ImageVersion(3,1))
    {
        pcCurrent += System::Read4le(pcCurrent,1,&iDimension);
    }
    else
    {
        iDimension = 2;
    }
    pcCurrent += System::Read4le(pcCurrent,1,&aiBound[0]);
    pcCurrent += System::Read4le(pcCurrent,1,&aiBound[1]);
    if (kVersion >= ImageVersion(3,1))
    {
        pcCurrent += System::Read4le(pcCurrent,1,&aiBound[2]);
    }
    else
    {
        aiBound[2] = 1;
    }

    FormatMode eFormat = (FormatMode)iFormat;
    int iQuantity = aiBound[0]*aiBound[1]*aiBound[2];

    // read the image data
    int iDataSize = ms_aiBytesPerPixel[eFormat]*iQuantity;
    if (eFormat == Image::IT_CUBE_RGB888
    ||  eFormat == Image::IT_CUBE_RGBA8888)
    {
        iDataSize *= 6;
    }
    unsigned char* aucData = WM4_NEW unsigned char[iDataSize];
    System::Read1(pcCurrent,iDataSize,aucData);

    Image* pkImage = 0;
    switch (iDimension)
    {
    case 1:
        pkImage = WM4_NEW Image(eFormat,aiBound[0],aucData,
            rkImageName.c_str());
        break;
    case 2:
        pkImage = WM4_NEW Image(eFormat,aiBound[0],aiBound[1],aucData,
            rkImageName.c_str());
        break;
    case 3:
        pkImage = WM4_NEW Image(eFormat,aiBound[0],aiBound[1],aiBound[2],
            aucData,rkImageName.c_str());
        break;
    default:
        assert(false);
    }

    WM4_DELETE[] acBuffer;
    return pkImage;
}
//----------------------------------------------------------------------------
bool Image::Save (const char* acFilename)
{
    if (!acFilename)
    {
        return false;
    }

    FILE* pkFile = System::Fopen(acFilename,"wb");
    if (!pkFile)
    {
        return false;
    }

    // write the file version
    System::Write1(pkFile,ImageVersion::LENGTH,ImageVersion::LABEL);

    // write the image format and dimensions
    int iFormat = (int)m_eFormat;
    System::Write4le(pkFile,1,&iFormat);
    System::Write4le(pkFile,1,&m_iDimension);
    System::Write4le(pkFile,1,&m_aiBound[0]);
    System::Write4le(pkFile,1,&m_aiBound[1]);
    System::Write4le(pkFile,1,&m_aiBound[2]);

    // write the image data
    int iDataSize = ms_aiBytesPerPixel[m_eFormat]*m_iQuantity;
    if (IsCubeImage())
    {
        iDataSize *= 6;
    }
    System::Write1(pkFile,iDataSize,m_aucData);

    System::Fclose(pkFile);
    return true;
}
//----------------------------------------------------------------------------
ColorRGBA* Image::CreateRGBA () const
{
    if (!IsCubeImage())
    {
        ColorRGBA* akCImage = WM4_NEW ColorRGBA[m_iQuantity];
        CopyRGBA(akCImage);
        return akCImage;
    }

    // Cube maps are handled as six separate images, so there is no need to
    // create an RGBA image here.
    return 0;
}
//----------------------------------------------------------------------------
void Image::CopyRGBA (ColorRGBA* akCImage) const
{
    const float fInv255 = 1.0f/255.0f;
    ColorRGBA* pkCValue;
    const unsigned char* pucValue;
    int i;

    if (m_eFormat == IT_RGB888)
    {
        pkCValue = akCImage;
        pucValue = (const unsigned char*)m_aucData;
        for (i = 0; i < m_iQuantity; i++, pkCValue++)
        {
            pkCValue->R() = fInv255*(float)(*pucValue++);
            pkCValue->G() = fInv255*(float)(*pucValue++);
            pkCValue->B() = fInv255*(float)(*pucValue++);
            pkCValue->A() = 1.0f;
        }
    }
    else if (m_eFormat == IT_RGBA8888)
    {
        pkCValue = akCImage;
        pucValue = (const unsigned char*)m_aucData;
        for (i = 0; i < m_iQuantity; i++, pkCValue++)
        {
            pkCValue->R() = fInv255*(float)(*pucValue++);
            pkCValue->G() = fInv255*(float)(*pucValue++);
            pkCValue->B() = fInv255*(float)(*pucValue++);
            pkCValue->A() = fInv255*(float)(*pucValue++);
        }
    }
    else if (IsDepthImage())
    {
        pkCValue = akCImage;
        const float* pfValue = (const float*)m_aucData;
        for (i = 0; i < m_iQuantity; i++, pkCValue++, pfValue++)
        {
            float fValue = *pfValue;
            pkCValue->R() = fValue;
            pkCValue->G() = fValue;
            pkCValue->B() = fValue;
            pkCValue->A() = 1.0f;
        }
    }
    else
    {
        // Cube images are handled as six 2D images in the software renderer,
        // so no conversion is needed.

        // TODO. Handle RGB16F, RGBA16F, RGB32F, RGBA32F, RGB16I, RGBA16I,
        // INTENSITY8I, INTENSITY16I, INTENSITY16F, INTENSITY32F.
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Image::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    int iTmp;
    rkStream.Read(iTmp);
    m_eFormat = (FormatMode)iTmp;
    rkStream.Read(m_iDimension);
    rkStream.Read(m_aiBound[0]);
    rkStream.Read(m_aiBound[1]);
    rkStream.Read(m_aiBound[2]);
    rkStream.Read(m_iQuantity);
    int iBytes = ms_aiBytesPerPixel[m_eFormat]*m_iQuantity;
    m_aucData = WM4_NEW unsigned char[iBytes];
    rkStream.Read(iBytes,m_aucData);

    Catalog<Image>::GetActive()->Insert(this);

    WM4_END_DEBUG_STREAM_LOAD(Image);
}
//----------------------------------------------------------------------------
void Image::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Image::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void Image::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write((int)m_eFormat);
    rkStream.Write(m_iDimension);
    rkStream.Write(m_aiBound[0]);
    rkStream.Write(m_aiBound[1]);
    rkStream.Write(m_aiBound[2]);
    rkStream.Write(m_iQuantity);
    int iBytes = ms_aiBytesPerPixel[m_eFormat]*m_iQuantity;
    rkStream.Write(iBytes,m_aucData);

    WM4_END_DEBUG_STREAM_SAVE(Image);
}
//----------------------------------------------------------------------------
int Image::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = Object::GetDiskUsed(rkVersion) +
        sizeof(int) + // m_eFormat
        sizeof(m_iDimension) +
        sizeof(m_aiBound[0]) +
        sizeof(m_aiBound[1]) +
        sizeof(m_aiBound[2]) +
        sizeof(m_iQuantity);

    int iBytes = ms_aiBytesPerPixel[m_eFormat]*m_iQuantity;
    iSize += iBytes*sizeof(m_aucData[0]);

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* Image::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("format =",ms_akFormatName[m_eFormat].c_str()));
    pkTree->Append(Format("dimension = ",m_iDimension));

    const size_t uiTitleSize = 16;
    char acTitle[uiTitleSize];
    for (int i = 0; i < m_iDimension; i++)
    {
        System::Sprintf(acTitle,uiTitleSize,"bound[%d] =",i);
        pkTree->Append(Format(acTitle,m_aiBound[i]));
    }

    // children
    pkTree->Append(Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
