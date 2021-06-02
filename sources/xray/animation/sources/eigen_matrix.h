////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EIGEN_MATRIX_H_INCLUDED
#define EIGEN_MATRIX_H_INCLUDED

namespace xray {
namespace math {


class Eigen
{
public:

    //Eigen (int iSize);
    //Eigen (const Matrix2<float>& rkM);
    //Eigen (const Matrix3<float>& rkM);

    Eigen (const float4x4& rkM);
    //~Eigen ();

    // set the matrix for eigensolving
    //float& operator() (int iRow, int iCol);
    //Eigen& operator= (const Matrix2<float>& rkM);
    //Eigen& operator= (const Matrix3<float>& rkM);
    //Eigen& operator= (const GMatrix<float>& rkM);

    // Get the eigenresults (eigenvectors are columns of eigenmatrix).  The
    // GetEigenvector calls involving Vector2 and Vector3 should only be
    // called if you know that the eigenmatrix is of the appropriate size.

    //float GetEigenvalue (int i) const;

    //const float* GetEigenvalues () const;
    //void GetEigenvector (int i, Vector2<float>& rkV) const;
    //void GetEigenvector (int i, Vector3<float>& rkV) const;

   float4 GetEigenvector (int i) const;

   // const GMatrix<float>& GetEigenvectors () const;

    // solve eigensystem
    //void EigenStuff2 ();
    //void EigenStuff3 ();
    //void EigenStuffN ();
    //void EigenStuff  ();

    // solve eigensystem, use decreasing sort on eigenvalues
    //void DecrSortEigenStuff2 ();
    //void DecrSortEigenStuff3 ();
    void DecrSortEigenStuffN ();
   // void DecrSortEigenStuff  ();

    // solve eigensystem, use increasing sort on eigenvalues
    //void IncrSortEigenStuff2 ();
    //void IncrSortEigenStuff3 ();
    //void IncrSortEigenStuffN ();
    //void IncrSortEigenStuff  ();

private:
    int m_iSize;
    float4x4 m_kMat;
    float4 m_afDiag;
    float4 m_afSubd;

    // For odd size matrices, the Householder reduction involves an odd
    // number of reflections.  The product of these is a reflection.  The
    // QL algorithm uses rotations for further reductions.  The final
    // orthogonal matrix whose columns are the eigenvectors is a reflection,
    // so its determinant is -1.  For even size matrices, the Householder
    // reduction involves an even number of reflections whose product is a
    // rotation.  The final orthogonal matrix has determinant +1.  Many
    // algorithms that need an eigendecomposition want a rotation matrix.
    // We want to guarantee this is the case, so m_bRotation keeps track of
    // this.  The DecrSort and IncrSort further complicate the issue since
    // they swap columns of the orthogonal matrix, causing the matrix to
    // toggle between rotation and reflection.  The value m_bRotation must
    // be toggled accordingly.
    bool m_bIsRotation;
    void GuaranteeRotation ();

    // Householder reduction to tridiagonal form
    //void Tridiagonal2 ();
    //void Tridiagonal3 ();

    void TridiagonalN ();

    // QL algorithm with implicit shifting, applies to tridiagonal matrices
    bool QLAlgorithm ();

    // sort eigenvalues from largest to smallest
    void DecreasingSort ();

    // sort eigenvalues from smallest to largest
    void IncreasingSort ();
};



} // namespace math
} // namespace xray

#endif // #ifndef EIGEN_MATRIX_H_INCLUDED