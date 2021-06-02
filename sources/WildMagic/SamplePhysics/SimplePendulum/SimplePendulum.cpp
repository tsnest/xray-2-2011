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

#include "SimplePendulum.h"
using namespace Wm4;

WM4_CONSOLE_APPLICATION(SimplePendulum);

ImageRGB82D* SimplePendulum::ms_pkImage = 0;
unsigned int SimplePendulum::ms_uiColor = 0;
float SimplePendulum::ms_fK = 1.0f;

//----------------------------------------------------------------------------
void SimplePendulum::DrawPixel (int iX, int iY)
{
    if (0 <= iX && iX < SIZE && 0 <= iY && iY < SIZE)
    {
        (*ms_pkImage)(iX,iY) = ms_uiColor;
    }
}
//----------------------------------------------------------------------------
float* SimplePendulum::ExplicitEuler (float fX0, float fY0, float fH)
{
    float* afExplicit = WM4_NEW float[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        float fX1 = fX0 + fH*fY0;
        float fY1 = fY0 - fH*ms_fK*Mathf::Sin(fX0);

        afExplicit[i] = fX1;
        fX0 = fX1;
        fY0 = fY1;
    }
    return afExplicit;
}
//----------------------------------------------------------------------------
float* SimplePendulum::ImplicitEuler (float fX0, float fY0, float fH)
{
    const float fK0 = ms_fK*fH*fH;

    float* afImplicit = WM4_NEW float[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        float fK1 = fX0 + fH*fY0;
        float fX1 = fX0;
        int jMaxIteration = 32;
        for (int j = 0; j < jMaxIteration; j++)
        {
            float fG = fX1 + fK0*Mathf::Sin(fX1) - fK1;
            float fGDer = 1.0f + fK0*Mathf::Cos(fX1);
            fX1 -= fG/fGDer;
        }
        float fY1 = fY0 - fH*ms_fK*Mathf::Sin(fX1);

        afImplicit[i] = fX1;
        fX0 = fX1;
        fY0 = fY1;
    }
    return afImplicit;
}
//----------------------------------------------------------------------------
float* SimplePendulum::RungeKutta (float fX0, float fY0, float fH)
{
    float* afRunge = WM4_NEW float[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        float fK1X = fH*fY0;
        float fK1Y = -fH*ms_fK*Mathf::Sin(fX0);
        float fX1 = fX0 + 0.5f*fK1X;
        float fY1 = fY0 + 0.5f*fK1Y;
        float fK2X = fH*fY1;
        float fK2Y = -fH*ms_fK*Mathf::Sin(fX1);
        fX1 = fX0 + 0.5f*fK2X;
        fY1 = fY0 + 0.5f*fK2Y;
        float fK3X = fH*fY1;
        float fK3Y = -fH*ms_fK*Mathf::Sin(fX1);
        fX1 = fX0 + fK3X;
        fY1 = fY0 + fK3Y;
        float fK4X = fH*fY1;
        float fK4Y = -fH*ms_fK*Mathf::Sin(fX1);
        fX1 = fX0 + (fK1X + 2.0f*fK2X + 2.0f*fK3X + fK4X)/6.0f;
        fY1 = fY0 + (fK1Y + 2.0f*fK2Y + 2.0f*fK3Y + fK4Y)/6.0f;

        afRunge[i] = fX1;
        fX0 = fX1;
        fY0 = fY1;
    }
    return afRunge;
}
//----------------------------------------------------------------------------
float* SimplePendulum::LeapFrog (float fX0, float fY0, float fH)
{
    float* afLeapFrog = WM4_NEW float[SIZE];

    // generate first iterate with Euler's to start up the process
    float fX1 = fX0 + fH*fY0;
    float fY1 = fY0 - fH*ms_fK*Mathf::Sin(fX0);
    afLeapFrog[0] = fX1;

    for (int i = 1; i < SIZE; i++)
    {
        float fX2 = fX0 + 2.0f*fH*fY1;
        float fY2 = fY0 - 2.0f*fH*ms_fK*Mathf::Sin(fX1);

        afLeapFrog[i] = fX2;
        fX0 = fX1;
        fY0 = fY1;
        fX1 = fX2;
        fY1 = fY2;
    }
    return afLeapFrog;
}
//----------------------------------------------------------------------------
void SimplePendulum::SolveMethod (float* (*oMethod)(float,float,float),
    const char* acOutIm, const char* acOutTxt)
{
    float fX0 = 0.1f, fY0 = 1.0f;
    float fH = 0.1f;

    float* afApprox = oMethod(fX0,fY0,fH);
    std::ofstream kOutTxt(System::GetPath("",acOutTxt));
    int i;
    for (i = 0; i < SIZE; i++)
    {
        kOutTxt << "i = " << i << ", " << afApprox[i] << std::endl;
    }

    // set image to white
    for (i = 0; i < ms_pkImage->GetQuantity(); i++)
    {
        (*ms_pkImage)[i] = GetColor24(255,255,255);
    }

    // draw the approximate solution
    float fY = 256.0f*(afApprox[0]+3.0f)/6.0f;
    int iY0 = SIZE-1-(int)fY;
    for (i = 1; i < SIZE; i++)
    {
        fY = 256.0f*(afApprox[i]+3.0f)/6.0f;
        int iY1 = SIZE-1-(int)fY;
        Line2D(i-1,iY0,i,iY1,DrawPixel);
        iY0 = iY1;
    }

    ms_pkImage->Save(System::GetPath("",acOutIm));

    WM4_DELETE[] afApprox;
}
//----------------------------------------------------------------------------
void SimplePendulum::Stiff1 ()
{
    const int iMaxIter = 1024+256;
    const float fCSqr = 2.0f, fC = Mathf::Sqrt(2.0f);

    float fH = 0.01f;
    float fX0 = 1.0f, fX0Save = fX0;
    float fY0 = -fC*fX0;

    float* afApprox = WM4_NEW float[iMaxIter];
    int i;
    for (i = 0; i < iMaxIter; i++)
    {
        float fK1X = fH*fY0;
        float fK1Y = fH*fCSqr*fX0;
        float fX1 = fX0 + 0.5f*fK1X;
        float fY1 = fY0 + 0.5f*fK1Y;
        float fK2X = fH*fY1;
        float fK2Y = fH*fCSqr*fX1;
        fX1 = fX0 + 0.5f*fK2X;
        fY1 = fY0 + 0.5f*fK2Y;
        float fK3X = fH*fY1;
        float fK3Y = fH*fCSqr*fX1;
        fX1 = fX0 + fK3X;
        fY1 = fY0 + fK3Y;
        float fK4X = fH*fY1;
        float fK4Y = fH*fCSqr*fX1;
        fX1 = fX0 + (fK1X + 2.0f*fK2X + 2.0f*fK3X + fK4X)/6.0f;
        fY1 = fY0 + (fK1Y + 2.0f*fK2Y + 2.0f*fK3Y + fK4Y)/6.0f;

        afApprox[i] = fX1;
        fX0 = fX1;
        fY0 = fY1;
    }

    std::ofstream kOutTxt(System::GetPath("","stiff1.txt"));
    for (i = 0; i < iMaxIter; i++)
    {
        kOutTxt << "i = " << i << ", " << afApprox[i] << std::endl;
    }

    // set image to white
    for (i = 0; i < ms_pkImage->GetQuantity(); i++)
    {
        (*ms_pkImage)[i] = GetColor24(255,255,255);
    }

    // draw the true solution
    float fY = 256.0f*(fX0Save+3.0f)/6.0f;
    int iY0 = SIZE-1-(int)fY;
    for (i = 1; i < SIZE; i++)
    {
        int j = (iMaxIter-1)*i/(SIZE-1);
        fY = 256.0f*(fX0Save*Mathf::Exp(-fC*j*fH)+3.0f)/6.0f;
        int iY1 = SIZE-1-(int)fY;
        Line2D(i-1,iY0,i,iY1,DrawPixel);
        iY0 = iY1;
    }

    ms_pkImage->Save(System::GetPath("","stiff1_true.im"));

    // set image to white
    for (i = 0; i < ms_pkImage->GetQuantity(); i++)
    {
        (*ms_pkImage)[i] = GetColor24(255,255,255);
    }

    // draw the approximate solution
    fY = 256.0f*(afApprox[0]+3.0f)/6.0f;
    iY0 = SIZE-1-(int)fY;
    for (i = 1; i < SIZE; i++)
    {
        int j = (iMaxIter-1)*i/(SIZE-1);
        fY = 256.0f*(afApprox[j]+3.0f)/6.0f;
        int iY1 = SIZE-1-(int)fY;
        Line2D(i-1,iY0,i,iY1,DrawPixel);
        iY0 = iY1;
    }
    ms_pkImage->Save(System::GetPath("","stiff1_appr.im"));

    WM4_DELETE[] afApprox;
}
//----------------------------------------------------------------------------
float SimplePendulum::F0 (float fT, float fX, float fY)
{
    return 9.0f*fX + 24.0f*fY + 5.0f*Mathf::Cos(fT) - Mathf::Sin(fT)/3.0f;
}
//----------------------------------------------------------------------------
float SimplePendulum::F1 (float fT, float fX, float fY)
{
    return -24.0f*fX - 51.0f*fY - 9.0f*Mathf::Cos(fT) + Mathf::Sin(fT)/3.0f;
}
//----------------------------------------------------------------------------
void SimplePendulum::Stiff2True ()
{
    float fH = 0.05f;
    float fX0 = 4.0f/3.0f;
    float fY0 = 2.0f/3.0f;
    float fT0 = 0.0f;

    // true solution
    std::ofstream kOutTxt(System::GetPath("","stiff2_true.txt"));
    const size_t uiSize = 512;
    char acMsg[uiSize];
    const int iMaxIter = 20;
    for (int i = 0; i <= iMaxIter; i++, fT0 += fH)
    {
        float fE0 = Mathf::Exp(-3.0f*fT0);
        float fE1 = Mathf::Exp(-39.0f*fT0);
        float fCDiv3 = Mathf::Cos(fT0)/3.0f;
        fX0 = 2.0f*fE0 - fE1 + fCDiv3;
        fY0 = -fE0 + 2.0f*fE1 - fCDiv3;
        if (i >= 2 && ((i%2) == 0))
        {
            System::Sprintf(acMsg,uiSize,"i = %d, %+8.6f, %+8.6f",i,fX0,fY0);
            kOutTxt << acMsg << std::endl;
        }
    }
}
//----------------------------------------------------------------------------
void SimplePendulum::Stiff2Approximate ()
{
    // approximation with step size 0.05
    float fH = 0.05f;
    float fX0 = 4.0f/3.0f;
    float fY0 = 2.0f/3.0f;
    float fT0 = 0.0f;

    const int iMaxIter = 20;
    float* afApprox0 = WM4_NEW float[iMaxIter+1];
    float* afApprox1 = WM4_NEW float[iMaxIter+1];
    afApprox0[0] = fX0;
    afApprox1[0] = fY0;
    int i;
    for (i = 1; i <= iMaxIter; i++)
    {
        float fK1X = fH*F0(fT0,fX0,fY0);
        float fK1Y = fH*F1(fT0,fX0,fY0);
        float fX1 = fX0 + 0.5f*fK1X;
        float fY1 = fY0 + 0.5f*fK1Y;
        float fK2X = fH*F0(fT0+0.5f*fH,fX1,fY1);
        float fK2Y = fH*F1(fT0+0.5f*fH,fX1,fY1);
        fX1 = fX0 + 0.5f*fK2X;
        fY1 = fY0 + 0.5f*fK2Y;
        float fK3X = fH*F0(fT0+0.5f*fH,fX1,fY1);
        float fK3Y = fH*F1(fT0+0.5f*fH,fX1,fY1);
        fX1 = fX0 + fK3X;
        fY1 = fY0 + fK3Y;
        float fK4X = fH*F0(fT0+fH,fX1,fY1);
        float fK4Y = fH*F1(fT0+fH,fX1,fY1);
        fX1 = fX0 + (fK1X + 2.0f*fK2X + 2.0f*fK3X + fK4X)/6.0f;
        fY1 = fY0 + (fK1Y + 2.0f*fK2Y + 2.0f*fK3Y + fK4Y)/6.0f;

        afApprox0[i] = fX1;
        afApprox1[i] = fY1;
        fX0 = fX1;
        fY0 = fY1;
        fT0 += fH;
    }

    std::ofstream kOutTxt(System::GetPath("","stiff2_appr_h0.05.txt"));
    const size_t uiSize = 512;
    char acMsg[uiSize];
    for (i = 0; i <= iMaxIter; i++)
    {
        if ((i % 2) == 0)
        {
            System::Sprintf(acMsg,uiSize,"i = %d, %+8.6f, %+8.6f",i,
                afApprox0[i],afApprox1[i]);
            kOutTxt << acMsg << std::endl;
        }
    }
    kOutTxt.close();

    // approximation with step size 0.1
    fH = 0.1f;
    fX0 = 4.0f/3.0f;
    fY0 = 2.0f/3.0f;
    fT0 = 0.0f;

    afApprox0[0] = fX0;
    afApprox1[0] = fY0;
    for (i = 1; i <= iMaxIter/2; i++)
    {
        float fK1X = fH*F0(fT0,fX0,fY0);
        float fK1Y = fH*F1(fT0,fX0,fY0);
        float fX1 = fX0 + 0.5f*fK1X;
        float fY1 = fY0 + 0.5f*fK1Y;
        float fK2X = fH*F0(fT0+0.5f*fH,fX1,fY1);
        float fK2Y = fH*F1(fT0+0.5f*fH,fX1,fY1);
        fX1 = fX0 + 0.5f*fK2X;
        fY1 = fY0 + 0.5f*fK2Y;
        float fK3X = fH*F0(fT0+0.5f*fH,fX1,fY1);
        float fK3Y = fH*F1(fT0+0.5f*fH,fX1,fY1);
        fX1 = fX0 + fK3X;
        fY1 = fY0 + fK3Y;
        float fK4X = fH*F0(fT0+fH,fX1,fY1);
        float fK4Y = fH*F1(fT0+fH,fX1,fY1);
        fX1 = fX0 + (fK1X + 2.0f*fK2X + 2.0f*fK3X + fK4X)/6.0f;
        fY1 = fY0 + (fK1Y + 2.0f*fK2Y + 2.0f*fK3Y + fK4Y)/6.0f;

        afApprox0[i] = fX1;
        afApprox1[i] = fY1;
        fX0 = fX1;
        fY0 = fY1;
        fT0 += fH;
    }

    kOutTxt.open("stiff2_appr_h0.10.txt");
    for (i = 0; i <= iMaxIter/2; i++)
    {
        System::Sprintf(acMsg,uiSize,"i = %d, %+8.6f, %+8.6f",i,afApprox0[i],
            afApprox1[i]);
        kOutTxt << acMsg << std::endl;
    }
    kOutTxt.close();

    WM4_DELETE[] afApprox0;
    WM4_DELETE[] afApprox1;
}
//----------------------------------------------------------------------------
int SimplePendulum::Main (int, char**)
{
    ms_pkImage = WM4_NEW ImageRGB82D(SIZE,SIZE);

    SolveMethod(ExplicitEuler,"explicit.im","explicit.txt");
    SolveMethod(ImplicitEuler,"implicit.im","implicit.txt");
    SolveMethod(RungeKutta,"runge.im","runge.txt");
    SolveMethod(LeapFrog,"leapfrog.im","leapfrog.txt");
    Stiff1();
    Stiff2True();
    Stiff2Approximate();

    WM4_DELETE ms_pkImage;
    return 0;
}
//----------------------------------------------------------------------------
