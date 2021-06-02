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
// Version: 4.0.1 (2006/10/28)

#include <windows.h>
#include <cassert>
#include <fstream>
using namespace std;

//----------------------------------------------------------------------------
void FormatByte (unsigned char ucByte, unsigned char aucFormatted[5])
{
    aucFormatted[0] = '0';
    aucFormatted[1] = 'x';

    int iNibble = ((ucByte & 0xF0) >> 4);
    if (iNibble <= 9)
    {
        aucFormatted[2] = '0' + iNibble;
    }
    else
    {
        assert(10 <= iNibble && iNibble <= 15);
        iNibble -= 10;
        aucFormatted[2] = 'A' + iNibble;
    }

    iNibble = (ucByte & 0x0F);
    if (iNibble <= 9)
    {
        aucFormatted[3] = '0' + iNibble;
    }
    else
    {
        assert(10 <= iNibble && iNibble <= 15);
        iNibble -= 10;
        aucFormatted[3] = 'A' + iNibble;
    }

    aucFormatted[4] = 0;
}
//----------------------------------------------------------------------------
void CreateFontSource (const char* acName, HDC hDC)
{
    TEXTMETRIC kTM;
    BOOL bResult = GetTextMetrics(hDC,&kTM);
    int iCHeight = kTM.tmHeight;

    const size_t uiSize = 256;
    char acFName[uiSize];
#if _MSC_VER >= 1400
    sprintf_s(acFName,uiSize,"%s.cpp",acName);
#else
    sprintf(acFName,"%s.cpp",acName);
#endif
    ofstream kOStr(acFName);

    kOStr << "// Geometric Tools" << endl;
    kOStr << "// http://www.geometrictools.com" << endl;
    kOStr << "// Copyright (c) 1998-2006.  All Rights Reserved" << endl;
    kOStr << "//" << endl;
    kOStr << "// The Wild Magic Version 4 Restricted Libraries source code is supplied " << endl;
    kOStr << "// under the terms of the license agreement" << endl;
    kOStr << "//     http://www.geometrictools.com/License/Wm4RestrictedLicense.pdf" << endl;
    kOStr << "// and may not be copied or disclosed except in accordance with the terms" << endl;
    kOStr << "// of that agreement." << endl;
    kOStr << endl;
    kOStr << "#include \"Wm4BitmapFont.h\"" << endl;
    kOStr << endl;
    kOStr << "namespace Wm4" << endl;
    kOStr << "{" << endl << endl;

    // Allocate a temporary array for storing the bitmaps.
    int i, iCWidth;
    int iMaxCWidth = 0;
    for (i = 0; i < 256; i++)
    {
        GetCharWidth32(hDC,i,i,&iCWidth);
        if (iCWidth > iMaxCWidth)
        {
            iMaxCWidth = iCWidth;
        }
    }
    int iBQuantity = iMaxCWidth*iCHeight;
    unsigned char* aucBitmap = new unsigned char[iBQuantity];

    for (i = 0; i < 256; i++)
    {
        kOStr << "static const unsigned char gs_aucChar" << i << "[] = {";

        // Get the character width and determine how many bytes wide the
        // bitmap should be.
        char cChar = (char)i;
        GetCharWidth32(hDC,i,i,&iCWidth);
        int iBWidth = iCWidth / 8;
        if ((iCWidth % 8) != 0)
        {
            iBWidth++;
        }

        // Treat aucBitmap as a bwidth-by-cheight array.
        memset(aucBitmap,0,iBQuantity);

        // Draw the character and read the set bits.
        TextOut(hDC,0,0,&cChar,1);
        int iRow, iCol, iX, iY;
        for (iRow = 0; iRow < iCHeight; iRow++)
        {
            for (iX = 0, iCol = 0; iX < iCWidth; iX++)
            {
                unsigned char* pucBitmap = &aucBitmap[iCol+iBWidth*iRow];
                int iMod = (iX % 8);

                iY = iCHeight-1-iRow;
                COLORREF kColor = GetPixel(hDC,iX,iY);
                if (kColor == RGB(0,0,0))
                {
                    *pucBitmap |= (1 << (7 - iMod));
                }

                if (iMod == 7)
                {
                    iCol++;
                }
            }
        }

        // Clear out the character for the next iteration.
        for (iY = 0; iY < iCHeight; iY++)
        {
            for (iX = 0; iX < iCWidth; iX++)
            {
                SetPixel(hDC,iX,iY,RGB(255,255,255));
            }
        }

        int iTrueQuantity = iBWidth*iCHeight;
        unsigned char aucFormatted[5];
        FormatByte(aucBitmap[0],aucFormatted);
        kOStr << aucFormatted;
        for (int j = 1; j < iTrueQuantity; j++)
        {
            FormatByte(aucBitmap[j],aucFormatted);
            kOStr << "," << aucFormatted;
        }
        kOStr << "};" << endl;

        kOStr << "static const BitmapFontChar gs_kChar" << i
            << "(0,0," << iCWidth << "," << iCHeight << ","
            << "gs_aucChar" << i << ");" << endl << endl;
    }

    kOStr << "static const BitmapFontChar* const gs_kChars[] =" << endl;
    kOStr << "{" << endl;
    kOStr << "    &gs_kChar0";
    for (i = 1; i < 256; i++)
    {
        kOStr << "," << endl;
        kOStr << "    &gs_kChar" << i;
    }
    kOStr << endl << "};" << endl << endl;

    kOStr << "const BitmapFont g_k" << acName
        << "(\"" << acName << "\",256,gs_kChars);" << endl << endl;

    kOStr << "}" << endl;

    kOStr.close();

    delete[] aucBitmap;
}
//----------------------------------------------------------------------------
LRESULT CALLBACK WindowEventHandler (HWND hWnd, UINT uiMsg, WPARAM wParam,
    LPARAM lParam)
{
    switch (uiMsg) 
    {
        case WM_PAINT:
        {
            PAINTSTRUCT kPS;
            HDC hDC = BeginPaint(hWnd,&kPS);
            EndPaint(hWnd,&kPS);
            return 0;
        }
        case WM_CHAR:
        {
            unsigned char ucKey = (unsigned char)(char)wParam;

            // quit application if the KEY_TERMINATE key is pressed
            if (ucKey == 'q')
            {
                PostQuitMessage(0);
                return 0;
            }
            return 0;
        }
    }

    return DefWindowProc(hWnd,uiMsg,wParam,lParam);
}
//----------------------------------------------------------------------------
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // register the window class
    static char s_acWindowClass[] = "BitmapFontCreator";
    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = WindowEventHandler;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = 0;
    wc.hIcon         = LoadIcon(0,IDI_APPLICATION);
    wc.hCursor       = LoadCursor(0,IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = s_acWindowClass;
    wc.lpszMenuName  = 0;
    RegisterClass(&wc);

    // require the window to have the specified client area
    int iWidth = 512, iHeight = 512;
    RECT kRect = { 0, 0, iWidth-1, iHeight-1 };
    AdjustWindowRect(&kRect,WS_OVERLAPPEDWINDOW,false);

    // create the application window
    HWND hWnd = CreateWindow(s_acWindowClass,"Font Display",
        WS_OVERLAPPEDWINDOW,0,0,kRect.right-kRect.left+1,
        kRect.bottom-kRect.top+1,0,0,0,0);

    // display the window
    ShowWindow(hWnd,SW_SHOW);
    UpdateWindow(hWnd);

    // create the font
    const char* acFace = "Verdana";
    int iSize = 16;
    bool bBold = false;
    bool bItalic = false;
    HFONT hFont = CreateFont(iSize,0,0,0,(bBold ? FW_BOLD : FW_REGULAR),
        (DWORD)bItalic,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,VARIABLE_PITCH,acFace);
    HDC hDC = GetDC(hWnd);
    SelectObject(hDC,hFont);
    SetTextColor(hDC,RGB(0,0,0));

    const size_t uiSize = 256;
    char acName[uiSize];
#if _MSC_VER >= 1400
    sprintf_s(acName,uiSize,"%s_S%dB%dI%d",acFace,iSize,(bBold ? 1 : 0),
        (bItalic ? 1 : 0));
#else
    sprintf(acName,"%s_S%dB%dI%d",acFace,iSize,(bBold ? 1 : 0),
        (bItalic ? 1 : 0));
#endif
    CreateFontSource(acName,hDC);

    // start the message pump
    MSG kMsg;
    while (true)
    {
        if (PeekMessage(&kMsg,0,0,0,PM_REMOVE))
        {
            if (kMsg.message == WM_QUIT)
            {
                break;
            }

            HACCEL hAccel = 0;
            if (!TranslateAccelerator(hWnd,hAccel,&kMsg))
            {
                TranslateMessage(&kMsg);
                DispatchMessage(&kMsg);
            }
        }
    }

    DeleteObject(hFont);
    return 0;
}
//----------------------------------------------------------------------------
