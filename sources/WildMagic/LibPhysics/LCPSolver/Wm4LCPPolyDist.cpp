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

#include "Wm4PhysicsPCH.h"
#include "Wm4LCPPolyDist.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
LCPPolyDist<VectorIn,TVector,TTuple>::LCPPolyDist (int iNumPoints1, 
    VectorIn* akP1, int iNumFaces1, TTuple* akF1, int iNumPoints2, 
    VectorIn* akP2, int iNumFaces2, TTuple* akF2, int& riStatusCode,
    float& rfDist, VectorIn akRes[/*2*/], double dVerifyMinDifference,
    double dRandomWidth)
{
    m_dVerifyMinDifference = dVerifyMinDifference;
    m_dRandomWidth = dRandomWidth;

    // Vertices and faces entry point
    m_iDimension = sizeof(TVector)/sizeof(double);

    WM4_LOGFUNCTION(OpenLog());
    WM4_LOGFUNCTION(LogVerticesAndFaces(iNumPoints1,akP1,iNumFaces1,akF1));
    WM4_LOGFUNCTION(LogVerticesAndFaces(iNumPoints2,akP2,iNumFaces2,akF2));

    m_iNumPoints1 = iNumPoints1;
    m_akP1 = WM4_NEW TVector[iNumPoints1];
    int i, j;
    for (i = 0; i < iNumPoints1; i++)
    {
        for (j = 0; j < m_iDimension; j++)
        {
            m_akP1[i][j] = (double) akP1[i][j];
        }
    }
    m_iNumFaces1 = iNumFaces1;
    m_akF1 = akF1;
    m_iNumPoints2 = iNumPoints2;
    m_akP2 = WM4_NEW TVector[iNumPoints2];
    for (i = 0; i < iNumPoints2; i++)
    {
        for (j = 0; j < m_iDimension; j++)
        {
            m_akP2[i][j] = (double) akP2[i][j];
        }
    }
    m_iNumFaces2 = iNumFaces2;
    m_akF2 = akF2;

    m_iNumEquations = iNumFaces1+iNumFaces2+2*m_iDimension;

    m_adB1 = WM4_NEW double[m_iNumEquations];
    m_akA1 = WM4_NEW TVector[iNumFaces1];
    GenerateHalfSpaceDescription(m_iNumPoints1,m_akP1,m_iNumFaces1,m_akF1,
        m_adB1,m_akA1);

    m_adB2 = WM4_NEW double[m_iNumEquations];
    m_akA2 = WM4_NEW TVector[iNumFaces2];
    GenerateHalfSpaceDescription(m_iNumPoints2,m_akP2,m_iNumFaces2,m_akF2,
        m_adB2,m_akA2);

    rfDist = (float) ProcessLoop(false,riStatusCode,akRes);

    WM4_DELETE[] m_akA1;
    WM4_DELETE[] m_adB1;
    WM4_DELETE[] m_akA2;
    WM4_DELETE[] m_adB2;
    WM4_DELETE[] m_akP1;
    WM4_DELETE[] m_akP2;

    WM4_LOGFUNCTION(CloseLog ());
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
LCPPolyDist<VectorIn,TVector,TTuple>::LCPPolyDist (int iNum1, float* afB1,
    VectorIn* akA1, int iNum2, float* afB2, VectorIn* akA2, int& riStatusCode,
    float& rfDist, VectorIn akRes[/*2*/], double dVerifyMinDifference,
    double dRandomWidth)
{
    m_dVerifyMinDifference = dVerifyMinDifference;
    m_dRandomWidth = dRandomWidth;

    // Halfspaces entry point
    m_iDimension = sizeof(TVector)/sizeof(double);

    m_iNumEquations = iNum1+iNum2+2*m_iDimension;
    m_iNumFaces1 = iNum1;
    m_iNumFaces2 = iNum2;

    m_adB1 = WM4_NEW double[iNum1];
    m_akA1 = WM4_NEW TVector[iNum1];
    int i, j;
    for (i = 0; i < iNum1; i++)
    {
        m_adB1[i] = (double) afB1[i];
        for (j = 0; j < m_iDimension; j++)
        {
            m_akA1[i][j] = (double) akA1[i][j];
        }
    }
    m_adB2 = WM4_NEW double[iNum1];
    m_akA2 = WM4_NEW TVector[iNum1];
    for (i = 0; i < iNum2; i++)
    {
        m_adB2[i] = (double) afB2[i];
        for (j = 0; j < m_iDimension; j++)
        {
            m_akA2[i][j] = (double) akA2[i][j];
        }
    }

    WM4_LOGFUNCTION(OpenLog());
    WM4_LOGFUNCTION(LogHalfspaces(m_adB1,m_akA1,iNum1));
    WM4_LOGFUNCTION(LogHalfspaces(m_adB2,m_akA2,iNum2));

    WM4_LOGFUNCTION(RandomizeHalfspaces());
    
    rfDist = (float) ProcessLoop(true,riStatusCode,akRes);

    WM4_LOGFUNCTION(CloseLog());

    WM4_DELETE[] m_adB1;
    WM4_DELETE[] m_akA1;
    WM4_DELETE[] m_adB2;
    WM4_DELETE[] m_akA2;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
double LCPPolyDist<VectorIn,TVector,TTuple>::ProcessLoop (bool bHS,
    int& riStatusCode, VectorIn* akRes)
{
    int i;
    double* adQ = WM4_NEW double[m_iNumEquations];;
    double** aadM = WM4_NEW double*[m_iNumEquations];
    for (i = 0; i < m_iNumEquations; i++)
    {
        aadM[i] = WM4_NEW double[m_iNumEquations];
    }

    riStatusCode = 0;

    bool bContinue = true;
    if (!BuildMatrices(aadM,adQ))
    {
        WM4_LOGFUNCTION(CloseLog());
        akRes[0] = VectorIn::ZERO;
        akRes[1] = VectorIn::ZERO;
        bContinue = false;
    }

    WM4_LOGFUNCTION(PrintMatrices(aadM,adQ));

    int iTryNumber = 0;
    float fDistance = -10.0f;
    while (bContinue)
    {
        double* adZResult = WM4_NEW double[m_iNumEquations];
        double* adWResult = WM4_NEW double[m_iNumEquations];
        int iReturn;

        LCPSolver::LCPSolver(m_iNumEquations,aadM,adQ,adZResult,adWResult,
            iReturn);

        switch (iReturn)
        {
        case SC_FOUND_TRIVIAL_SOLUTION:
        {
            riStatusCode = SC_FOUND_TRIVIAL_SOLUTION;
            break;
        }
        case SC_FOUND_SOLUTION:
        {
            // solution found
            TVector* akResD = WM4_NEW TVector[2];
            for (i = 0; i < m_iDimension; i++)
            {
                akResD[0][i] = (float) adZResult[i];
                akResD[1][i] = (float) adZResult[i+m_iDimension];
            }

            TVector kDiff = akResD[0]-akResD[1];
            fDistance = (float) kDiff.Length();

            riStatusCode = VerifySolution(akResD);

            if (!bHS)
            {
                WM4_LOGFUNCTION(VerifyWithTestPoints(akResD,riStatusCode));
            }

            for (i = 0; i < m_iDimension; i++)
            {
                akRes[0][i] = (float) akResD[0][i];
                akRes[1][i] = (float) akResD[1][i];
            }
            WM4_DELETE[] akResD;
            bContinue = false;
            break;
        }
        case SC_CANNOT_REMOVE_COMPLEMENTARY:
        {
            WM4_LOGFUNCTION(LogRetries(iTryNumber))
            if (iTryNumber == 3)
            {
                riStatusCode = SC_CANNOT_REMOVE_COMPLEMENTARY;
                bContinue = false;
                break;
            }
            MoveHalfspaces(m_iNumFaces1,m_adB1,m_akA1);
            WM4_LOGFUNCTION(LogHalfspaces(m_adB1,m_akA1,m_iNumFaces1));
            MoveHalfspaces(m_iNumFaces2,m_adB2,m_akA2);
            WM4_LOGFUNCTION(LogHalfspaces(m_adB2,m_akA2,m_iNumFaces2));
            BuildMatrices(aadM,adQ);
            iTryNumber++;
            break;
        }
        case SC_EXCEEDED_MAX_RETRIES:
        {
            WM4_LOGFUNCTION(LCPSolverLoopLimit());
            riStatusCode = SC_EXCEEDED_MAX_RETRIES;
            bContinue = false;
            break;
        }
        }
        WM4_DELETE[] adWResult;
        WM4_DELETE[] adZResult;
    }
    for (i = 0; i < m_iNumEquations; i++)
    {
        WM4_DELETE[] aadM[i];
    }
    WM4_DELETE[] aadM;
    WM4_DELETE[] adQ;

    return fDistance;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
bool LCPPolyDist<VectorIn,TVector,TTuple>::BuildMatrices (double** aadM,
    double* adQ)
{
    // zero aadM and adQ
    memset(adQ,0,m_iNumEquations*sizeof(double));
    int i;
    for (i = 0; i < m_iNumEquations; i++)
    {
        memset(aadM[i],0,m_iNumEquations*sizeof(double));
    }

    // enter pdZCoef terms
    // first matrix S
    int iTwoDimension = 2*m_iDimension;
    for (i = 0; i < iTwoDimension; i++)
    {
        aadM[i][i] = 2.0;
        if (i < m_iDimension)
        {
            aadM[i][i+m_iDimension] = -2.0;
            aadM[i+m_iDimension][i] = -2.0;
        }
    }

    double dQMin = 1.0;
    int j, k;
    for (i = 0, j = iTwoDimension; i < m_iNumFaces1; i++, j++)
    {
        for (k = 0; k < m_iDimension; k++)
        {
            aadM[j][k] = -m_akA1[i][k];       // -A1
            aadM[k][j] = m_akA1[i][k];        // A1 transpose
        }

        adQ[j] = m_adB1[i];
        if (adQ[j] < dQMin)
        {
            dQMin = adQ[j];
        }
    }

    for (i = 0, j = iTwoDimension+m_iNumFaces1; i < m_iNumFaces2; i++, j++)
    {
        int n;
        for (k = 0, n = m_iDimension; k < m_iDimension; k++, n++)
        {
            aadM[j][n] = -m_akA2[i][k];     // -A2
            aadM[n][j] = m_akA2[i][k];      // A2 transpose
        }

        adQ[j] = m_adB2[i];
        if (adQ[j] < dQMin)
        {
            dQMin = adQ[j];
        }
    }
    if (dQMin >= 0)
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::ComputeHalfspaces (int iNumPoints,
    const TVector* akP, int iNumFaces, const TTuple* akF, TVector* akA,
    double* afB)
{
    int j, k;

    TVector kAvgPt = TVector::ZERO;
    for (j = 0; j < iNumPoints; j++)
    {
        for (k = 0; k < m_iDimension; k++)
        {
            kAvgPt[k] += akP[j][k];
        }
    }
    kAvgPt /= (double)iNumPoints;

    TVector kD1, kD2, kN;

    for (j = 0; j < iNumFaces; j++)
    {
        // In 2D we need the perp of a single vector.  In 3D we need the
        // cross product of two vectors.  This template needs to have code
        // that supports both 2D and 3D.  The use of operator[] allows us to
        // do this without having to add a "Cross" function to Vector2.
        kD1 = akP[akF[j][1]]-akP[akF[j][0]];
        if (m_iDimension == 3)
        {
            kD2 = akP[akF[j][2]] - akP[akF[j][0]];
            kN[0] = kD1[1]*kD2[2] - kD1[2]*kD2[1],
            kN[1] = kD1[2]*kD2[0] - kD1[0]*kD2[2],
            kN[2] = kD1[0]*kD2[1] - kD1[1]*kD2[0];
        }
        else
        {
            kN[0] = kD1[1];
            kN[1] = -kD1[0];
        }

        double fRHS = kN.Dot(akP[akF[j][1]]);

        double fMult = (kN.Dot(kAvgPt) <= fRHS ? 1.0f : -1.0f);
        afB[j] = fMult*fRHS;
        for (k = 0; k < m_iDimension; k++)
        {
            akA[j][k] = fMult*kN[k];
        }
    }
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::GenerateHalfSpaceDescription
    (int iNumPoints, TVector* akP, int iNumFaces, TTuple* akF, double* afB,
    TVector* akA)
{
    WM4_LOGFUNCTION(RandomizeArray(iNumPoints,akP));
    WM4_LOGFUNCTION(LogVertices(akP,iNumPoints));

    ComputeHalfspaces(iNumPoints,akP,iNumFaces,akF,akA,afB);

    WM4_LOGFUNCTION(LogHalfspaces(afB,akA,iNumFaces));
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::ChangeFaceOrder (int iNumFaces, 
    TTuple* akF)
{
    TTuple kTemp = akF[0];
    for (int j = 1; j < iNumFaces; j++)
    {
        akF[j-1] = akF[j];
    }
    akF[iNumFaces-1] = kTemp;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::MoveHalfspaces (int iNumFaces,
    double* afB, TVector* akA)
{
    TVector kTemp = akA[0];
    double fTemp = afB[0];
    for (int j = 1; j < iNumFaces; j++)
    {
        akA[j-1] = akA[j];
        afB[j-1] = afB[j];
    }
    akA[iNumFaces-1] = kTemp;
    afB[iNumFaces-1] = fTemp;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::MovePoints ()
{ 
    ChangeFaceOrder(m_iNumFaces1,m_akF1);
    ChangeFaceOrder(m_iNumFaces2,m_akF2);
    GenerateHalfSpaceDescription(m_iNumPoints1,m_akP1,m_iNumFaces1,m_akF1,
        m_adB1,m_akA1);
    GenerateHalfSpaceDescription(m_iNumPoints2,m_akP2,m_iNumFaces2,m_akF2,
        m_adB2,m_akA2);
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
int LCPPolyDist<VectorIn,TVector,TTuple>::VerifySolution (
    const TVector* akRes)
{
    // check to see that the first solution point is in the first polyhedron
    // and the second in the second
    double fDiff;
    int i;
    int iReturnValue = SC_FOUND_SOLUTION;

    for (i = 0; i < m_iNumFaces1; i++)
    {
        fDiff = m_akA1[i].Dot(akRes[0])-m_adB1[i];
        if (fDiff > m_dVerifyMinDifference)
        {
            WM4_LOGFUNCTION(LogVerifyFailure(1,i,fDiff));
            iReturnValue = SC_VERIFY_FAILURE;
        }
    }

    for (i = 0; i < m_iNumFaces2; i++)
    {
        fDiff = m_akA2[i].Dot(akRes[1])-m_adB2[i];
        if (fDiff > m_dVerifyMinDifference)
        {
            WM4_LOGFUNCTION(LogVerifyFailure(2,i,fDiff));
            iReturnValue = SC_VERIFY_FAILURE;
        }
    }
    return iReturnValue;
}
//----------------------------------------------------------------------------

#ifdef WM4_LCPPOLYDIST_LOG
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::OpenLog ()
{
    if (!m_kLog.is_open())
    {
        // open the log file
        m_kLog.open("LCPPolyDist.log");
        assert( m_kLog );
        if (!m_kLog)
        {
            return;
        }
    }
    
    m_kLog << "LCP Polyhedron Distance Log" << std::endl;

    // print the current date and time
    time_t kClock;
    time(&kClock);
    m_kLog << "Time: " << asctime(localtime(&kClock)) << std::endl
           << std::endl;

    // use scientific notation for floating point output
    m_kLog.setf(std::ios::scientific,std::ios::floatfield);
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::CloseLog ()
{
    m_kLog.flush();
    m_kLog.close();
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::PrintMatrices(double** aadM,
    double* adQ)
{
    // print the input to the solver
    m_kLog << "There are " << m_iNumEquations
        << " rows and columns in this problem." << std::endl << std::endl;

    m_kLog << "The matrix M." << std::endl;
    int i;
    for (i = 0; i < m_iNumEquations; i++)
    {
        for (int j = 0; j < m_iNumEquations; j++)
        {
            m_kLog << std::showpos << aadM[i][j] << ' ';
        }
        m_kLog << std::endl;
    }

    m_kLog << "The vector Q." << std::endl;
    for (i = 0; i < m_iNumEquations; i++)
    {
        m_kLog << std::showpos << adQ[i] << ' ';
    }
    m_kLog << std::endl;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::LogRetries (int iTryNumber)
{
    m_kLog << "LCPPolyDist cannot remove complementary variable. ";
    m_kLog << "No solution. iTryNumber = " << iTryNumber << std::endl;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::LCPSolverLoopLimit ()
{
    m_kLog << "No solution found. LCPSolver used the maximum";
    m_kLog << " parameterized number of loops." << std::endl;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::LogVertices (const TVector* akP,
    int iNum)
{
    // write triangle vertices to log
    m_kLog << std::endl << "Triangle vertices." << std::endl;
    for (int i = 0; i < iNum; i++)
    {
        m_kLog << "(" << akP[i][0];
        for (int j = 1; j < m_iDimension; j++)
        {
            m_kLog << ", " << akP[i][j];
        }
        m_kLog << ")" << std::endl;
    }
    m_kLog << std::endl;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::LogVerticesAndFaces (
    int iNumPoints, const VectorIn* akP, int iNumFaces, const TTuple* akF)
{
    m_kLog << std::endl << "This polyhedron has " << iNumPoints; 
    m_kLog << " vertices." << std::endl << "They are:" << std::endl;
    int i, j;
    for (i = 0; i < iNumPoints; i++)
    {
        m_kLog << "(" << akP[i][0];
        for (j = 1; j < m_iDimension; j++)
        {
            m_kLog << ", " << akP[i][j];
        }
        m_kLog << ")" << std::endl;
    }
    m_kLog << " The number of faces is " << iNumFaces << "." << std::endl; 
    m_kLog << " And they are (counting the vertices above from 0 to ";
    m_kLog << iNumPoints-1 << ")" << std::endl;
    for (i = 0; i < iNumFaces; i++)
    {
        m_kLog << "(" << akF[i][0];
        for (j = 1; j < m_iDimension; j++)
        {
            m_kLog << ", " << akF[i][j];
        }
        m_kLog << ")" << std::endl;
    }
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::LogHalfspaces (const double* afB,
    const TVector* akP, int iNum)
{
    // write halfspaces generated by triangle vertices to log
    m_kLog << std::endl << "Halfspaces:" << std::endl;
    std::string sYZ[] = { "*y ", "*z " };
    for (int i = 0; i < iNum; i++)
    {
        m_kLog << akP[i][0] << "*x ";
        for (int j = 1; j < m_iDimension; j++)
        {
            m_kLog << akP[i][j] << sYZ[j-1];
        }
        m_kLog << "<= " << afB[i] << std::endl;
    }
    m_kLog << std::endl;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::SetupTestPoints (int iIndex,
    int iNumFaces, int iNumPoints, TVector kZRes, const double* afB,
    const TVector* akA, const TVector* akP, TVector* akTestPoints)
{
    int i;
    TVector kSum = TVector::ZERO;

    for (i = 0; i < iNumPoints; i++)
    {
        kSum += akP[i];
    }

    akTestPoints[0] = kSum/((double)iNumPoints);

    int j = (iNumPoints < 4 ? iNumPoints : 4);
    for (i = 0; i < 4; i++)
    {
        if (i < j)
        {
            akTestPoints[i+1] = kZRes*0.5f+akP[i]*0.5f;
            akTestPoints[i+5] = kZRes*0.8f+akP[i]*0.2f;
        }
        else
        {
            akTestPoints[i+1] = kZRes;
            akTestPoints[i+5] = kZRes;
        }
    }

    double fAmtOut = 0.2f;
    akTestPoints[9] = (akTestPoints[1]+akTestPoints[5])*0.5f;
    akTestPoints[10] = kZRes*0.5f+akTestPoints[0]*0.5f;
    akTestPoints[11] = kZRes*0.8f+akTestPoints[0]*0.2f;
    akTestPoints[12] = kZRes*(1.0f+fAmtOut)-
        akTestPoints[0]*fAmtOut;        // should be outside polyhedron

    LogSolutionWithTestPoints(iIndex,kZRes,akTestPoints);

    // eliminate generated points outside poly
    for (i = 0; i < 13; i++)
    {
        for (j = 0; j < iNumFaces; j++)
        {
            // For each point and each constraint, calculate the constraint
            // value.
            double fDiff = akA[j].Dot(akTestPoints[i]) - afB[j];
            if (fDiff > m_dVerifyMinDifference)
            {
                // replace with solution point
                akTestPoints[i] = kZRes;
            }
        }
    }
    m_kLog << "Test points reduced for containment in polyhedron:"
           << std::endl;
    LogTestPoints(akTestPoints);
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::LogTestPoints (
    const TVector* akTestPoints)
{
    for (int i = 0; i < 13; i++) 
    {
        m_kLog << "(" << akTestPoints[i][0];
        for (int j = 1; j < m_iDimension; j++ )
        {
            m_kLog << ", " << akTestPoints[i][j];
        }
        m_kLog << ")" << std::endl;
    }
    m_kLog << std::endl;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::LogSolutionWithTestPoints (
    int iIndex, TVector kZRes, const TVector* akTestPoints)
{
    // write generated test points to log
    if (iIndex == 1)
    {
        m_kLog << std::endl << std::endl;
    }
    m_kLog << "Solution point from polyhedron " << iIndex;
    m_kLog << " = (" << kZRes[0];
    for (int i = 1; i < m_iDimension; i++)
    {
        m_kLog << ", " << kZRes[i];
    }
    m_kLog << ")" << std::endl;
    m_kLog << "The generated test points are :" << std::endl;
    LogTestPoints(akTestPoints);
}       
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::VerifyWithTestPoints (
    const TVector* akRes, int& riStatusCode)
{
    TVector* akTestPoints1 = WM4_NEW TVector[13];
    TVector* akTestPoints2 = WM4_NEW TVector[13];

    SetupTestPoints(1,m_iNumFaces1,m_iNumPoints1,akRes[0],m_adB1,m_akA1,
        m_akP1,akTestPoints1);
    SetupTestPoints(2,m_iNumFaces2,m_iNumPoints2,akRes[1],m_adB2,m_akA2,
        m_akP2,akTestPoints2);

    double fDiffSq;
    double fMin;
    TVector kDiff;
    int iLine = 0;
    int jLine = 0;

    // min distance between generated points note that one of these points
    // is the "solution"
    int i;
    for (i = 0; i < 13; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            kDiff = akTestPoints1[i]-akTestPoints2[j];
            fDiffSq = kDiff.Dot(kDiff);
            if (i == 0 && j == 0)
            {
                fMin = fDiffSq;
            }
            else
            {
                if (fDiffSq < fMin)
                {
                    fMin = fDiffSq;
                    iLine = i;
                    jLine = j;
                }
            }
        }
    }

    kDiff = akRes[0]-akRes[1];
    float fDistance = (float)kDiff.Dot(kDiff);

    riStatusCode = SC_FOUND_SOLUTION;
    if (fDistance > fDiffSq)
    {
        m_kLog << "Test points closer than solution points by ";
        m_kLog << fDistance-fDiffSq << " squared units.\n";
        if ((fDistance-fDiffSq > m_dVerifyMinDifference)
        &&  (iLine != 12 || jLine != 12))
        {
            riStatusCode = SC_TEST_POINTS_TEST_FAILED;
        }
    }
    m_kLog << std::endl << " Solution points are separated by "
        << Mathf::Sqrt(fDistance);
    m_kLog << " units." << std::endl;
    m_kLog << "The smallest distance between test points is "
        << Mathf::Sqrt((float)fMin);
    m_kLog << std::endl << "and occurs for (" << akTestPoints1[iLine][0];
    for (i = 1; i < m_iDimension; i++)
    {
        m_kLog << ", " << akTestPoints1[iLine][i];
    }
    m_kLog << ") and (" << akTestPoints2[jLine][0];
    for (i = 1; i < m_iDimension; i++)
    {
        m_kLog << ", " << akTestPoints2[jLine][i];
    }
    m_kLog << ")" << std::endl;

    WM4_DELETE[] akTestPoints1;
    WM4_DELETE[] akTestPoints2;
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::RandomizeArray (int iNumPoints,
    TVector* akP)
{
    // large values (0.9) of RANDOM_WIDTH tolerated with sample data files 
    for (int j = 0; j < iNumPoints; j++)
    {
        for (int k = 0; k < m_iDimension; k++)
        {
            akP[j][k] *= 1.0 + m_dRandomWidth*Mathd::SymmetricRandom();
        }
    }
}     
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::RandomizeHalfspaces ()
{
    // small values (0.05) of RANDOM_WIDTH tolerated with sample data files
    int j, k;
    for (j = 0; j < m_iNumFaces1; j++)
    {
        m_adB1[j] *= 1.0 + m_dRandomWidth*Mathd::SymmetricRandom();
        for (k = 0; k < m_iDimension; k++)
        {
            m_akA1[j][k] *= 1.0 + m_dRandomWidth*Mathd::SymmetricRandom();
        }
    }
    for (j = 0; j < m_iNumFaces2; j++)
    {
        m_adB2[j] *= 1.0 + m_dRandomWidth*Mathd::SymmetricRandom();
        for (k = 0; k < m_iDimension; k++)
        {
            m_akA2[j][k] *= 1.0 + m_dRandomWidth*Mathd::SymmetricRandom();
        }
    }
}
//----------------------------------------------------------------------------
template <class VectorIn, class TVector, class TTuple>
void LCPPolyDist<VectorIn,TVector,TTuple>::LogVerifyFailure (
    int iWhichPolyhedron, int i, double fDiff)
{
    m_kLog << "Solution fails to meet constraint " << i;
    if (iWhichPolyhedron == 1)
    {
        m_kLog << " for the first polyhedron." << std::endl;
    }
    else
    {
        m_kLog << " for the second polyhedron." << std::endl;           
    }
    m_kLog << "The error is " << fDiff << std::endl;
}
//----------------------------------------------------------------------------
#endif

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_PHYSICS_ITEM
class LCPPolyDist<Vector2f,Vector2d,TTuple<2,int> >;

template WM4_PHYSICS_ITEM
class LCPPolyDist<Vector3f,Vector3d,TTuple<3,int> >;
//----------------------------------------------------------------------------
}
