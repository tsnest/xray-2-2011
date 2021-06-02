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

#ifndef WM4LCPPOLYDIST_H
#define WM4LCPPOLYDIST_H

#include "Wm4PhysicsLIB.h"
#include "Wm4TTuple.h"
#include "Wm4Vector2.h"
#include "Wm4Vector3.h"
#include "Wm4LCPSolver.h"

namespace Wm4
{

template <class VectorIn, class TVector, class TTuple>
class WM4_PHYSICS_ITEM LCPPolyDist
{
public:
    // A class for computing the distance between two convex polygons or two
    // convex polyhedra.  The distance algorithm is formulated as a linear
    // complementarity problem (LCP) for three-dimensional data.  For convex
    // polygons in two dimensions, the third component of the vertices must
    // be zero.

    // Status codes returned by the polyhedra distance calculation.
    enum
    {
        SC_FOUND_SOLUTION
            = LCPSolver::SC_FOUND_SOLUTION, // solution

        SC_FOUND_TRIVIAL_SOLUTION
            = LCPSolver::SC_FOUND_TRIVIAL_SOLUTION, // solution (z = 0, w = q)

        SC_CANNOT_REMOVE_COMPLEMENTARY
            = LCPSolver::SC_CANNOT_REMOVE_COMPLEMENTARY, // no solution

        SC_EXCEEDED_MAX_RETRIES
            = LCPSolver::SC_EXCEEDED_MAX_RETRIES, 

        // no solution (round-off problems?)
        SC_VERIFY_FAILURE,  // VerifySolution failed
        SC_TEST_POINTS_TEST_FAILED, // VerifyWithTestPoints failed
    };

    // Polygons represented by vertices and edges.  The vertices are stored
    // as vectors in 3D with the last component always zero.
    //   iNumPoints1  = number of vertices of first polygon
    //   akPoint1     = array of vertices
    //   iNumFaces1   = number of edges (same as iNumPoints1)
    //   akFace1      = array of edges, each edge a pair of indices into the
    //                  vertex array akPoint1: <0,1>, <1,2>, ..., <n-2,n-1>,
    //                  <n-1,0>
    //   iNumPoints2  = number of vertices of second polygon
    //   akPoint2     = array of vertices
    //   iNumFaces2   = number of edges (same as iNumPoints2)
    //   akFace2      = array of edges, each edge a pair of indices into the
    //                  vertex array akPoint2: <0,1>, <1,2>, ..., <n-2,n-1>,
    //                  <n-1,0>
    //   riStatusCode = the status of the LCP numerical solver
    //   rfDistance   = distance between the polygons
    //   akRes        = array of two closest points, one on each polygon
    // 
    // Polyhedra represented by vertices and faces.
    //   iNumPoints1  = number of vertices of first polyhedron
    //   akPoint1     = array of vertices
    //   iNumFaces1   = number of triangular faces
    //   akFace1      = array of faces, each face a triple of indices into
    //                  the vertex array akPoint1
    //   iNumPoints2  = number of vertices of second polyhedron
    //   akPoint2     = array of vertices
    //   iNumFaces2   = number of triangular faces
    //   akFace2      = array of faces, each face a triple of indices into
    //                  the vertex array akPoint2
    //   riStatusCode = the status of the LCP numerical solver
    //   rfDistance   = distance between the polyhedra
    //   akClosest    = array of two closest points, one on each polyhedron

    LCPPolyDist (int iNumPoints1, VectorIn* akPoint1, int iNumFaces1,
        TTuple* akFace1, int iNumPoints2, VectorIn* akPoint2, int iNumFaces2,
        TTuple* akFace2, int& riStatusCode, float& rfDistance,
        VectorIn akClosest[/*2*/], double dVerifyMinDifference = 0.00001,
        double dRandomWidth = 0.0);

    // Polygons and polyhedra represented by halfspaces.  The halfspaces are
    // all the points p such that p.Dot(akAn[i]) <= afBn[i], n = 1 or 2.  The
    // arrays akAn[] are the outer pointing edge/face normals.  The arrays
    // afBn[] are the line/plane constants.
    //   iNum1 = number of halfspaces of first polygon/polyhedron
    //   afB1  = array of line/plane constants
    //   akA1  = array of outer pointing edge/face normals
    //   iNum2 = number of halfspaces of second polygon/polyhedron
    //   afB2  = array of line/plane constants
    //   akA2  = array of outer pointing edge/face normals
    //   riStatusCode = the status of the LCP numerical solver
    //   rfDistance   = distance between the polygons/polyhedra
    //   akClosest    = array of two closest points, one on each
    //                  polygon/polyhedron

    LCPPolyDist (int iNum1, float* afB1, VectorIn* akA1, int iNum2,
        float* afB2, VectorIn* akA2, int& riStatusCode, float& rfDistance,
        VectorIn akClosest[/*2*/], double dVerifyMinDifference = 0.00001,
        double dRandomWidth = 0.0);

    // VerifyMinDifference:  Solution vectors are tested to determine if they
    // meet constraints imposed by the halfspace statement of the problem:
    // V o Z <= B. VerifyMinDifference is the amount that this dot product can
    // exceed B without reporting an error.

    // RandomWidth:  Used in GenerateHalfSpaceDescription to produce a random
    // array of vertices whose means are the input values to FuzzArray. Each
    // vertex is selected from a uniform distribution of width RandomWidth. 

private:
    double ProcessLoop (bool bHS, int& riError, VectorIn* akRes);
    bool BuildMatrices (double** aadM, double* adQ);
    void ComputeHalfspaces (int iNumPoints, const TVector* akP,
        int iNumFaces, const TTuple* akF, TVector* akA, double* afB);
    void GenerateHalfSpaceDescription (int iNumPoints, TVector* akP1,
        int iNumFaces, TTuple* akF, double* afB, TVector* akA);
    void ChangeFaceOrder (int iNumFaces, TTuple* akF);
    void MoveHalfspaces (int iNumFaces, double* afB, TVector* akA);
    void MovePoints ();
    int VerifySolution (const TVector* akRes);

    int m_iDimension;
    int m_iNumEquations;
    int m_iNumPoints1;
    int m_iNumPoints2;
    int m_iNumFaces1;
    int m_iNumFaces2; 
    double* m_adB1;  // halfspace constants
    double* m_adB2;
    TVector* m_akA1;  // halfspace vectors
    TVector* m_akA2;
    TVector* m_akP1;  // points
    TVector* m_akP2;
    TTuple* m_akF1;  // faces
    TTuple* m_akF2;

    double m_dVerifyMinDifference;
    double m_dRandomWidth;

// For writing messages to a log file during testing and debugging.  If you
// enable this, the distance calculator significantly slows down.
//
//#define WM4_LCPPOLYDIST_LOG
#ifdef WM4_LOGFUNCTION
#undef WM4_LOGFUNCTION
#endif
#ifdef WM4_LCPPOLYDIST_LOG
    #define WM4_LOGFUNCTION(func) func;
#else
    #define WM4_LOGFUNCTION(func)
#endif

#ifdef WM4_LCPPOLYDIST_LOG
    void OpenLog ();
    void CloseLog ();
    void PrintMatrices (double** aadM, double* adQ);
    void LogRetries (int iTryNumber);
    void LCPSolverLoopLimit ();
    void LogVertices (const TVector* akP, int iNum);
    void LogVerticesAndFaces (int iNumPoints, const VectorIn* akP,
        int iNumFaces, const TTuple* akF);
    void LogHalfspaces (const double* afB, const TVector* akP, int iNum);
    void SetupTestPoints (int iI, int iNumFaces, int iNumPoints, 
        TVector kZRes, const double* afB, const TVector* akA,
        const TVector* akP, TVector* akTestPoints);
    void LogTestPoints (const TVector* akTestPoints);
    void LogSolutionWithTestPoints (int iI, TVector kZResult, 
        const TVector* akTestPoints);
    void VerifyWithTestPoints (const TVector* akRes, int& riError);
    void RandomizeArray (int iNumPoints, TVector* akP);
    void RandomizeHalfspaces ();
    void LogVerifyFailure (int iWhich, int i, double fDiff);
    std::ofstream m_kLog;
#endif
};

typedef LCPPolyDist<Vector2f,Vector2d,TTuple<2,int> > LCPPolyDist2;
typedef LCPPolyDist<Vector3f,Vector3d,TTuple<3,int> > LCPPolyDist3;

}

#endif
