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

#include <windows.h>
#include "WmifToBmp24.h"
using namespace std;

WM4_CONSOLE_APPLICATION(WmifToBmp24);

//----------------------------------------------------------------------------
void WmifToBmp24::Usage ()
{
    cout << endl;
    cout << "To convert from the Wild Magic Image Format (wmif) to" << endl;
    cout << "one (or two) Windows BITMAP images, use" << endl;
    cout << "    WmifToBmp24 myfile.wmif" << endl;
    cout << "If the wmif file is RGB888, the output file is a" << endl;
    cout << "24-bit BITMAP image named myfile.bmp.  If the wmif" << endl;
    cout << "file is RGBA8888, two output files are generated." << endl;
    cout << "The file myfile.bmp is a 24-bit BITMAP image that" << endl;
    cout << "contains the RGB portion of myfile.wmif.  The file" << endl;
    cout << "myfile.alpha.bmp is also a 24-bit BITMAP image, is" << endl;
    cout << "gray scale (R = G = B), and contains the A portion" << endl;
    cout << "of myfile.wmif.  The converter only supports Wild" << endl;
    cout << "Magic RGB888 and RBGA8888 for now." << endl;
}
//----------------------------------------------------------------------------
void WmifToBmp24::SaveRGBImage (ofstream& rkRGBStr, const Image* pkImage)
{
    int iWidth = pkImage->GetBound(0);
    int iHeight = pkImage->GetBound(1);
    int iQuantity = iWidth*iHeight;
    int iDataBytes = 3*iQuantity;
    unsigned char* aucData = pkImage->GetData();

    // Reverse byte order (Windows stores BGR (lowest byte to highest byte,
    // MIF stores RGB).
    for (int i = 0; i < iQuantity; i++)
    {
        unsigned char ucB = aucData[3*i  ];
        unsigned char ucR = aucData[3*i+2];
        aucData[3*i  ] = ucR;
        aucData[3*i+2] = ucB;
    }

    // Windows expects 4-byte alignment for rows of the bitmap.  Add padding
    // if the input image does not satisfy this constraint.
    int iNewWidth, iNewDataBytes;
    unsigned char* aucNewData;
    if ((iWidth % 4) != 0)
    {
        int iExtra = 4 - (iWidth % 4);
        iNewWidth = iWidth + iExtra;
        iNewDataBytes = 3*iNewWidth*iHeight;
        aucNewData = WM4_NEW unsigned char[iNewDataBytes];
        memset(aucNewData,0,iNewDataBytes*sizeof(unsigned char));
        unsigned char* aucOldRow = aucData;
        unsigned char* aucNewRow = aucNewData;
        for (int iY = 0; iY < iHeight; iY++)
        {
            System::Memcpy(aucNewRow,3*iNewWidth,aucOldRow,3*iWidth);
            aucOldRow += 3*iWidth;
            aucNewRow += 3*iNewWidth;
        }
    }
    else
    {
        iNewWidth = iWidth;
        iNewDataBytes = iDataBytes;
        aucNewData = aucData;
    }

    BITMAPFILEHEADER kFileHeader;
    kFileHeader.bfType = 0x4d42;  // "BM"
    kFileHeader.bfSize =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        iNewDataBytes;
    kFileHeader.bfReserved1 = 0;
    kFileHeader.bfReserved2 = 0;
    kFileHeader.bfOffBits =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER kInfoHeader;
    kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    kInfoHeader.biWidth = iNewWidth;
    kInfoHeader.biHeight = iHeight;
    kInfoHeader.biPlanes = 1;
    kInfoHeader.biBitCount = 24;
    kInfoHeader.biCompression = BI_RGB;
    kInfoHeader.biSizeImage = 0;
    kInfoHeader.biXPelsPerMeter = 0;
    kInfoHeader.biYPelsPerMeter = 0;
    kInfoHeader.biClrUsed = 0;
    kInfoHeader.biClrImportant = 0;

    rkRGBStr.write((const char*)&kFileHeader,sizeof(BITMAPFILEHEADER));
    rkRGBStr.write((const char*)&kInfoHeader,sizeof(BITMAPINFOHEADER));
    rkRGBStr.write((const char*)aucNewData,
        iNewDataBytes*sizeof(unsigned char));

    if (aucNewData != aucData)
    {
        WM4_DELETE[] aucNewData;
    }
}
//----------------------------------------------------------------------------
void WmifToBmp24::SaveRGBAImage (ofstream& rkRGBStr, ofstream& rkAStr,
    const Image* pkImage)
{
    int iWidth = pkImage->GetBound(0);
    int iHeight = pkImage->GetBound(1);
    int iQuantity = iWidth*iHeight;
    int iDataBytes = 3*iQuantity;
    const unsigned char* aucData = pkImage->GetData();

    // Reverse byte order (Windows stores BGR (lowest byte to highest byte,
    // MIF stores RGB).
    unsigned char* aucRGBData = new unsigned char[iDataBytes];
    unsigned char* aucAData = new unsigned char[iDataBytes];
    for (int i = 0; i < iQuantity; i++)
    {
        unsigned char ucB = aucData[4*i  ];
        unsigned char ucG = aucData[4*i+1];
        unsigned char ucR = aucData[4*i+2];
        unsigned char ucA = aucData[4*i+3];
        aucRGBData[3*i  ] = ucR;
        aucRGBData[3*i+1] = ucG;
        aucRGBData[3*i+2] = ucB;
        aucAData[3*i  ] = ucA;
        aucAData[3*i+1] = ucA;
        aucAData[3*i+2] = ucA;
    }

    BITMAPFILEHEADER kFileHeader;
    kFileHeader.bfType = 0x4d42;  // "BM"
    kFileHeader.bfSize =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        iDataBytes;
    kFileHeader.bfReserved1 = 0;
    kFileHeader.bfReserved2 = 0;
    kFileHeader.bfOffBits =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER kInfoHeader;
    kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    kInfoHeader.biWidth = iWidth;
    kInfoHeader.biHeight = iHeight;
    kInfoHeader.biPlanes = 1;
    kInfoHeader.biBitCount = 24;
    kInfoHeader.biCompression = BI_RGB;
    kInfoHeader.biSizeImage = 0;
    kInfoHeader.biXPelsPerMeter = 0;
    kInfoHeader.biYPelsPerMeter = 0;
    kInfoHeader.biClrUsed = 0;
    kInfoHeader.biClrImportant = 0;

    rkRGBStr.write((const char*)&kFileHeader,sizeof(BITMAPFILEHEADER));
    rkRGBStr.write((const char*)&kInfoHeader,sizeof(BITMAPINFOHEADER));
    rkRGBStr.write((const char*)aucRGBData,iDataBytes*sizeof(unsigned char));
    delete[] aucRGBData;

    rkAStr.write((const char*)&kFileHeader,sizeof(BITMAPFILEHEADER));
    rkAStr.write((const char*)&kInfoHeader,sizeof(BITMAPINFOHEADER));
    rkAStr.write((const char*)aucAData,iDataBytes*sizeof(unsigned char));
    delete[] aucAData;
}
//----------------------------------------------------------------------------
int WmifToBmp24::Main (int iArgQuantity, char** aacArgument)
{
    if (iArgQuantity <= 1)
    {
        Usage();
        return -1;
    }

    // input file name
    const char* acInputFile = aacArgument[1];

    // split the path to obtain the filename
    char acDrive[_MAX_DRIVE], acDir[_MAX_DIR], acName[_MAX_FNAME];
    char acExt[_MAX_EXT];
#if _MSC_VER >= 1400
    _splitpath_s(acInputFile,acDrive,_MAX_DRIVE,acDir,_MAX_DIR,acName,
        _MAX_FNAME,acExt,_MAX_EXT);
#else
    _splitpath(acInputFile,acDrive,acDir,acName,acExt);
#endif

    Image* pkImage = Image::Load(acName);
    if (!pkImage)
    {
        return -3;
    }

    switch (pkImage->GetFormat())
    {
    case Image::IT_RGB888:
    {
        char acRGBFile[_MAX_PATH];
        System::Sprintf(acRGBFile,_MAX_PATH,"%s.bmp",acName);
        ofstream kRGBStr(acRGBFile,ios::out|ios::binary);
        if (!kRGBStr)
        {
            delete pkImage;
            return -4;
        }
        SaveRGBImage(kRGBStr,pkImage);
        kRGBStr.close();
        break;
    }
    case Image::IT_RGBA8888:
    {
        char acRGBFile[_MAX_PATH];
        System::Sprintf(acRGBFile,_MAX_PATH,"%s.bmp",acName);
        ofstream kRGBStr(acRGBFile,ios::out|ios::binary);
        if (!kRGBStr)
        {
            delete pkImage;
            return -4;
        }

        char acAFile[_MAX_PATH];
        System::Sprintf(acAFile,_MAX_PATH,"%s.alpha.bmp",acName);
        ofstream kAStr(acAFile,ios::out|ios::binary);
        if (!kAStr)
        {
            delete pkImage;
            return -5;
        }

        SaveRGBAImage(kRGBStr,kAStr,pkImage);
        kRGBStr.close();
        kAStr.close();
        break;
    }
    default:
        return -6;
    }

    delete pkImage;
    return 0;
}
//----------------------------------------------------------------------------
