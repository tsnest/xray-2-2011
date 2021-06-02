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
#include "CreateNormalMap.h"
using namespace std;

WM4_CONSOLE_APPLICATION(CreateNormalMap);

//----------------------------------------------------------------------------
void CreateNormalMap::Usage ()
{
    //cout << endl;
    //cout << "To convert Windows BITMAP format to Wild Magic Image" << endl;
    //cout << "Format (wmif), use" << endl;
    //cout << "    CreateNormalMap myfile.bmp [myfile.alpha.bmp]" << endl;
    //cout << "The BITMAP myfile.bmp is a 24-bit image that stores RGB" << endl;
    //cout << "components.  If you want an alpha channel, supply" << endl;
    //cout << "myfile.alpha.bmp, a 24-bit BITMAP image that is gray" << endl;
    //cout << "scale (R = G = B).  The common channel value is used as" << endl;
    //cout << "the alpha channel of the output image, myfile.wmif." << endl;
}
//----------------------------------------------------------------------------
int CreateNormalMap::GetImage (const char* acBMPName, int& riWidth,
    int& riHeight, unsigned char*& raucData)
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
    if (dibSection.dsBm.bmBitsPixel != 24)
    {
        return -3;
    }

    // Windows pads the rows of the BMP to be 4-byte aligned.
    int iRowSize = 3*riWidth;
    if ((iRowSize % 4) != 0)
    {
        iRowSize += 4 - (riWidth % 4);
    }

    // allocate the WMIF image
    raucData = WM4_NEW unsigned char[3*riWidth*riHeight];
    memset(raucData,0,3*riWidth*riHeight);

    // Windows BMP stores BGR, need to invert to RGB.
    unsigned char* aucOldRow = (unsigned char*)dibSection.dsBm.bmBits;
    unsigned char* aucNewRow = raucData;
    for (int iY = 0; iY < riHeight; iY++)
    {
        System::Memcpy(aucNewRow,3*riWidth,aucOldRow,3*riWidth);
        for (int iX = 0; iX < riWidth; iX++)
        {
            unsigned char ucB = aucNewRow[3*iX  ];
            unsigned char ucR = aucNewRow[3*iX+2];
            aucNewRow[3*iX  ] = ucR;
            aucNewRow[3*iX+2] = ucB;
        }
        aucOldRow += iRowSize;
        aucNewRow += 3*riWidth;
    }

    DeleteObject(hImage);

    return 0;
}
//----------------------------------------------------------------------------
void CreateNormalMap::SaveImage (const char* acBMPName, int iWidth,
    int iHeight, unsigned char* aucData)
{
    int iQuantity = iWidth*iHeight;
    int iDataBytes = 3*iQuantity;

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

    std::ofstream kOStr(acBMPName,ios::out|ios::binary);
    kOStr.write((const char*)&kFileHeader,sizeof(BITMAPFILEHEADER));
    kOStr.write((const char*)&kInfoHeader,sizeof(BITMAPINFOHEADER));
    kOStr.write((const char*)aucNewData,
        iNewDataBytes*sizeof(unsigned char));

    if (aucNewData != aucData)
    {
        WM4_DELETE[] aucNewData;
    }
}
//----------------------------------------------------------------------------
unsigned char* CreateNormalMap::GenerateNormals (int iXMax, int iYMax,
    float* afHeight, float fXScale, float fYScale)
{
    // The values afHeight[i] are required to be in [0,1].

    int iXMaxM1 = iXMax -1, iYMaxM1 = iYMax - 1;
    unsigned char* aucNormal = WM4_NEW unsigned char[3*iXMax*iYMax];

    for (int iY = 0; iY < iYMax; iY++)
    {
        // Use wrap-around.
        int iYm1 = (iY > 0 ? iY-1 : iYMaxM1);
        int iYp1 = (iY < iYMaxM1 ? iY+1 : 0);

        for (int iX = 0; iX < iXMax; iX++)
        {
            // Use wrap-around.
            int iXm1 = (iX > 0 ? iX-1 : iXMaxM1);
            int iXp1 = (iX < iXMaxM1 ? iX+1 : 0);

            // The approximation dH/dx is in [-xscale,xscale] and the
            // approximation dH/dy is in [-yscale,yscale] since H is in
            // [0,1].
            float fHXm1Y = afHeight[iXm1 + iY*iXMax];
            float fHXp1Y = afHeight[iXp1 + iY*iXMax];
            float fHXYm1 = afHeight[iX + iYm1*iXMax];
            float fHXYp1 = afHeight[iX + iYp1*iXMax];
            float fDHDX = fXScale*(fHXp1Y - fHXm1Y);
            float fDHDY = fYScale*(fHXYp1 - fHXYm1);

            // Left-handed (x,y,z) coordinates are used, so no minus sign
            // occurs in the first two components of the normal vector.
            Vector3f kNormal(fDHDX,fDHDY,1.0f);
            kNormal.Normalize();

            // Transform the normal vector from [-1,1]^3 to [0,255]^3 so it
            // can be stored as a color value.
            unsigned char* aucCompressed = &aucNormal[3*(iX+iY*iXMax)];
            aucCompressed[0] = (unsigned char)(127.5f*(kNormal[0]+1.0f));
            aucCompressed[1] = (unsigned char)(127.5f*(kNormal[1]+1.0f));
            aucCompressed[2] = (unsigned char)(127.5f*(kNormal[2]+1.0f));
        }
    }

    return aucNormal;
}
//----------------------------------------------------------------------------
int CreateNormalMap::Main (int iArgQuantity, char** aacArgument)
{
    if (iArgQuantity != 2)
    {
        Usage();
        return -1;
    }

    // Get RGB input file.  The filename must have extension ".bmp", so the
    // filename is at least 5 characters long.
    const char* acInFile = aacArgument[1];
    const size_t uiInFileSize = strlen(acInFile);
    if (uiInFileSize < 5)
    {
        return -2;
    }
    const char* acInFileExt = acInFile + uiInFileSize - 4;
    if (acInFileExt[0] != '.'
    ||  tolower(acInFileExt[1]) != 'b'
    ||  tolower(acInFileExt[2]) != 'm'
    ||  tolower(acInFileExt[3]) != 'p')
    {
        return -3;
    }

    std::string kInName(acInFile);

    // Extract the BMP data.  The output width is guaranteed to be a multiple
    // of four.
    int iXMax, iYMax;
    unsigned char* aucHeight;
    int iErrorCode = GetImage(acInFile,iXMax,iYMax,aucHeight);
    if (iErrorCode != 0)
    {
        WM4_DELETE[] aucHeight;
        return iErrorCode;
    }

    // Convert to gray scale: I = 0.2125*R + 0.7154*G + 0.0721*B
    int iQuantity = iXMax*iYMax;
    float* afIntensity = WM4_NEW float[iQuantity];
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        afIntensity[i] = 0.2125f*aucHeight[3*i] + 0.7154f*aucHeight[3*i+1] +
            0.0721f*aucHeight[3*i+2];
        unsigned char ucIntensity = (unsigned char)afIntensity[i];
        aucHeight[3*i] = ucIntensity;
        aucHeight[3*i+1] = ucIntensity;
        aucHeight[3*i+2] = ucIntensity;
        afIntensity[i] /= 255.0f;  // in [0,1]
    }

    std::string kIntensityName = std::string("intensity.") + kInName;
    SaveImage(kIntensityName.c_str(),iXMax,iYMax,aucHeight);

    // Compute normals.
    float fXScale = 4.0f, fYScale = 4.0f;
    unsigned char* aucNormal = GenerateNormals(iXMax,iYMax,afIntensity,
        fXScale,fYScale);

    std::string kNormalName = std::string("normal.") + kInName;
    SaveImage(kNormalName.c_str(),iXMax,iYMax,aucNormal);

    WM4_DELETE[] aucNormal;
    WM4_DELETE[] afIntensity;
    WM4_DELETE[] aucHeight;
    return 0;
}
//----------------------------------------------------------------------------
