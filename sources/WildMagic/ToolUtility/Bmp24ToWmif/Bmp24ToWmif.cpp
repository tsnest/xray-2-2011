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
// Version: 4.0.1 (2008/11/14)

#include <windows.h>
#include "Bmp24ToWmif.h"
using namespace std;

WM4_CONSOLE_APPLICATION(Bmp24ToWmif);

//----------------------------------------------------------------------------
void Bmp24ToWmif::Usage ()
{
    cout << endl;
    cout << "To convert Windows BITMAP format to Wild Magic Image" << endl;
    cout << "Format (wmif), use" << endl;
    cout << "    Bmp24ToWmif myfile.bmp [myfile.alpha.bmp]" << endl;
    cout << "If the BITMAP myfile.bmp is a 32-bit image that stores " << endl;
    cout << "RGBA, then the output myfile.wmif is Image::RGBA8888." << endl;
    cout << "If the BITMAP myfile.bmp is a 24-bit image that stores " << endl;
    cout << "RGB, then the output myfile.wmif is Image::RGB888." << endl;
    cout << "If you want an alpha channel to be appended to the RGB " << endl;
    cout << "image, then supply myfile.alpha.bmp, a 24-bit BITMAP " << endl;
    cout << "image that is gray scale (R = G = B).  The common " << endl;
    cout << "channel is used as the alpha channel of the output " << endl;
    cout << "image myfile.wmif, which is Image::RGBA8888." << endl;
}
//----------------------------------------------------------------------------
int Bmp24ToWmif::GetImage (const char* acBMPName, int& riWidth, int& riHeight,
    unsigned char*& raucData, bool& rbIs24Bit)
{
    riWidth = 0;
    riHeight = 0;
    raucData = 0;

    HBITMAP hImage = (HBITMAP) LoadImage(NULL,acBMPName,IMAGE_BITMAP,0,0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (!hImage)
    {
        return -2;
    }

    DIBSECTION dibSection;
    GetObject(hImage,sizeof(DIBSECTION),&dibSection);

    riWidth = dibSection.dsBm.bmWidth;
    riHeight = dibSection.dsBm.bmHeight;

    int iNumChannels;
    if (dibSection.dsBm.bmBitsPixel == 24)
    {
        iNumChannels = 3;
        rbIs24Bit = true;
    }
    else if (dibSection.dsBm.bmBitsPixel == 32)
    {
        iNumChannels = 4;
        rbIs24Bit = false;
    }
    else
    {
        return -3;
    }

    // Windows pads the rows of the BMP to be 4-byte aligned.
    int iRowSize = iNumChannels*riWidth;
    if ((iRowSize % 4) != 0)
    {
        iRowSize += 4 - (riWidth % 4);
    }

    // Allocate the WMIF image.
    raucData = WM4_NEW unsigned char[iNumChannels*riWidth*riHeight];
    memset(raucData,0,iNumChannels*riWidth*riHeight);

    // Windows BMP stores BGR, need to invert to RGB.
    int iNumRowBytes = iNumChannels*riWidth;
    unsigned char* aucOldRow = (unsigned char*)dibSection.dsBm.bmBits;
    unsigned char* aucNewRow = raucData;
    for (int iY = 0; iY < riHeight; iY++)
    {
        System::Memcpy(aucNewRow,iNumRowBytes,aucOldRow,iNumRowBytes);
        for (int iX = 0; iX < riWidth; iX++)
        {
            unsigned char ucB = aucNewRow[iNumChannels*iX  ];
            unsigned char ucR = aucNewRow[iNumChannels*iX+2];
            aucNewRow[iNumChannels*iX  ] = ucR;
            aucNewRow[iNumChannels*iX+2] = ucB;
        }
        aucOldRow += iRowSize;
        aucNewRow += iNumChannels*riWidth;
    }

    DeleteObject(hImage);

    return 0;
}
//----------------------------------------------------------------------------
int Bmp24ToWmif::Main (int iArgQuantity, char** aacArgument)
{
    if (iArgQuantity <= 1)
    {
        Usage();
        return -1;
    }

    // Get RGB input file.  The filename must have extension ".bmp", so the
    // filename is at least 5 characters long.
    const char* acRGBFile = aacArgument[1];
    const size_t uiRGBFileSize = strlen(acRGBFile);
    if (uiRGBFileSize < 5)
    {
        return -2;
    }
    const char* acRGBFileExt = acRGBFile + uiRGBFileSize - 4;
    if (acRGBFileExt[0] != '.'
    ||  tolower(acRGBFileExt[1]) != 'b'
    ||  tolower(acRGBFileExt[2]) != 'm'
    ||  tolower(acRGBFileExt[3]) != 'p')
    {
        return -3;
    }

    // Generate the image name without an extension.
    std::string kImageName;
    int i;
    for (i = 0; i < (int)uiRGBFileSize - 4; i++)
    {
        kImageName += acRGBFile[i];
    }

    int iRGBWidth, iRGBHeight;
    unsigned char* aucRGBData;
    bool bIs24Bit;
    int iErrorCode = GetImage(acRGBFile,iRGBWidth,iRGBHeight,aucRGBData,
        bIs24Bit);
    if (iErrorCode != 0)
    {
        WM4_DELETE[] aucRGBData;
        return iErrorCode;
    }

    // The input file is SomeFile.bmp.  Allocate enough space to change
    // this to SomeFile.wmif.
    const size_t uiDstSize = strlen(acRGBFile) + 2;
    char* acWmifName = new char[uiDstSize];
    System::Strcpy(acWmifName,uiDstSize,acRGBFile);
    char* acWmifFileExt = acWmifName + uiDstSize - 5;
    acWmifFileExt[0] = 'w';
    acWmifFileExt[1] = 'm';
    acWmifFileExt[2] = 'i';
    acWmifFileExt[3] = 'f';
    acWmifFileExt[4] = 0;

    if (!bIs24Bit)
    {
        // The incoming BMP is 32-bit RGBA.
        Image* pkRGBAImage = WM4_NEW Image(Image::IT_RGBA8888,iRGBWidth,
            iRGBHeight,aucRGBData,acRGBFile);
        if (!pkRGBAImage)
        {
            WM4_DELETE[] aucRGBData;
            return -11;
        }
        if (!pkRGBAImage->Save(acWmifName))
        {
            WM4_DELETE pkRGBAImage;
            return -12;
        }
        WM4_DELETE pkRGBAImage;
        return 0;
    }

    if (iArgQuantity <= 2)
    {
        // create and save the RGB image to disk
        Image* pkRGBImage = WM4_NEW Image(Image::IT_RGB888,iRGBWidth,
            iRGBHeight,aucRGBData,acRGBFile);
        if (!pkRGBImage)
        {
            WM4_DELETE[] aucRGBData;
            return -6;
        }
        if (!pkRGBImage->Save(acWmifName))
        {
            WM4_DELETE pkRGBImage;
            return -7;
        }
        WM4_DELETE pkRGBImage;
        return 0;
    }

    // get A input file
    const char* acAFile = aacArgument[2];
    int iAWidth, iAHeight;
    unsigned char* aucAData;
    iErrorCode = GetImage(acAFile,iAWidth,iAHeight,aucAData,bIs24Bit);
    if (iErrorCode != 0 || !bIs24Bit)
    {
        WM4_DELETE[] aucRGBData;
        WM4_DELETE[] aucAData;
        return iErrorCode;
    }
    if (iAWidth != iRGBWidth || iAHeight != iRGBHeight)
    {
        WM4_DELETE[] aucRGBData;
        WM4_DELETE[] aucAData;
        return -8;
    }

    int iWidth = iRGBWidth, iHeight = iRGBHeight;
    int iQuantity = iWidth*iHeight;
    unsigned char* aucData = WM4_NEW unsigned char[4*iQuantity];
    for (i = 0; i < iQuantity; i++)
    {
        aucData[4*i  ] = aucRGBData[3*i  ];
        aucData[4*i+1] = aucRGBData[3*i+1];
        aucData[4*i+2] = aucRGBData[3*i+2];
        aucData[4*i+3] = aucAData[3*i];  // use the R channel for alpha
    }
    WM4_DELETE[] aucRGBData;
    WM4_DELETE[] aucAData;

    Image* pkRGBAImage = WM4_NEW Image(Image::IT_RGBA8888,iWidth,iHeight,
        aucData,acRGBFile);
    if (!pkRGBAImage)
    {
        WM4_DELETE[] aucData;
        return -9;
    }
    if (!pkRGBAImage->Save(acWmifName))
    {
        WM4_DELETE pkRGBAImage;
        return -10;
    }
    WM4_DELETE pkRGBAImage;
    return 0;
}
//----------------------------------------------------------------------------
