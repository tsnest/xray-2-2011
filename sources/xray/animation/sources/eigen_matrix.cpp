////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "eigen_matrix.h"

namespace xray {
namespace math {


void Eigen::DecrSortEigenStuffN ()
{
    TridiagonalN();
    QLAlgorithm();
    DecreasingSort();
    GuaranteeRotation();
}


Eigen::Eigen (const float4x4& rkM)
    :
    m_kMat(rkM)
{
    m_iSize = 4;//rkM.GetRows();
    ASSERT( m_iSize >= 2 ); //&& (rkM.GetColumns() == m_iSize));
    //m_afDiag = WM4_NEW float[m_iSize];
    //m_afSubd = WM4_NEW float[m_iSize];
    m_bIsRotation = false;
}



float4 Eigen::GetEigenvector (int i) const
{
	return float4 ( m_kMat.elements[0][i], m_kMat.elements[1][i], m_kMat.elements[2][i], m_kMat.elements[3][i] );
}



void Eigen::TridiagonalN ()
{
    int i0, i1, i2, i3;

    for (i0 = m_iSize-1, i3 = m_iSize-2; i0 >= 1; i0--, i3--)
    {
        float fH = (float)0.0, fScale = (float)0.0;

        if (i3 > 0)
        {
            for (i2 = 0; i2 <= i3; i2++)
            {
                fScale += math::abs( m_kMat.elements[i0][i2] );
            }
            if (fScale == (float)0.0)
            {
                m_afSubd[i0] = m_kMat.elements[i0][i3];
            }
            else
            {
                float fInvScale = ((float)1.0)/fScale;
                for (i2 = 0; i2 <= i3; i2++)
                {
                    m_kMat.elements[i0][i2] *= fInvScale;
                    fH += m_kMat.elements[i0][i2]*m_kMat.elements[i0][i2];
                }
                float fF = m_kMat.elements[i0][i3];
                float fG = math::sqrt(fH);
                if (fF > (float)0.0)
                {
                    fG = -fG;
                }
                m_afSubd[i0] = fScale*fG;
                fH -= fF*fG;
                m_kMat.elements[i0][i3] = fF-fG;
                fF = (float)0.0;
                float fInvH = ((float)1.0)/fH;
                for (i1 = 0; i1 <= i3; i1++)
                {
                    m_kMat.elements[i1][i0] = m_kMat.elements[i0][i1]*fInvH;
                    fG = (float)0.0;
                    for (i2 = 0; i2 <= i1; i2++)
                    {
                        fG += m_kMat.elements[i1][i2]*m_kMat.elements[i0][i2];
                    }
                    for (i2 = i1+1; i2 <= i3; i2++)
                    {
                        fG += m_kMat.elements[i2][i1]*m_kMat.elements[i0][i2];
                    }
                    m_afSubd[i1] = fG*fInvH;
                    fF += m_afSubd[i1]*m_kMat.elements[i0][i1];
                }
                float fHalfFdivH = ((float)0.5)*fF*fInvH;
                for (i1 = 0; i1 <= i3; i1++)
                {
                    fF = m_kMat.elements[i0][i1];
                    fG = m_afSubd[i1] - fHalfFdivH*fF;
                    m_afSubd[i1] = fG;
                    for (i2 = 0; i2 <= i1; i2++)
                    {
                        m_kMat.elements[i1][i2] -= fF*m_afSubd[i2] +
                            fG*m_kMat.elements[i0][i2];
                    }
                }
            }
        }
        else
        {
            m_afSubd[i0] = m_kMat.elements[i0][i3];
        }

        m_afDiag[i0] = fH;
    }

    m_afDiag[0] = (float)0.0;
    m_afSubd[0] = (float)0.0;
    for (i0 = 0, i3 = -1; i0 <= m_iSize-1; i0++, i3++)
    {
        if (m_afDiag[i0] != (float)0.0)
        {
            for (i1 = 0; i1 <= i3; i1++)
            {
                float fSum = (float)0.0;
                for (i2 = 0; i2 <= i3; i2++)
                {
                    fSum += m_kMat.elements[i0][i2]*m_kMat.elements[i2][i1];
                }
                for (i2 = 0; i2 <= i3; i2++)
                {
                    m_kMat.elements[i2][i1] -= fSum*m_kMat.elements[i2][i0];
                }
            }
        }
        m_afDiag[i0] = m_kMat.elements[i0][i0];
        m_kMat.elements[i0][i0] = (float)1.0;
        for (i1 = 0; i1 <= i3; i1++)
        {
            m_kMat.elements[i1][i0] = (float)0.0;
            m_kMat.elements[i0][i1] = (float)0.0;
        }
    }

    // re-ordering if Eigen::QLAlgorithm is used subsequently
    for (i0 = 1, i3 = 0; i0 < m_iSize; i0++, i3++)
    {
        m_afSubd[i3] = m_afSubd[i0];
    }
    m_afSubd[m_iSize-1] = (float)0.0;

    m_bIsRotation = ((m_iSize % 2) == 0);
}


bool Eigen::QLAlgorithm ()
{
    const int iMaxIter = 32;

    for (int i0 = 0; i0 < m_iSize; i0++)
    {
        int i1;
        for (i1 = 0; i1 < iMaxIter; i1++)
        {
            int i2;
            for (i2 = i0; i2 <= m_iSize-2; i2++)
            {
                float fTmp = math::abs(m_afDiag[i2]) +
                    math::abs(m_afDiag[i2+1]);

                if ( math::abs(m_afSubd[i2]) + fTmp == fTmp)
                {
                    break;
                }
            }
            if (i2 == i0)
            {
                break;
            }

            float fG = (m_afDiag[i0+1] - m_afDiag[i0])/(((float)2.0) *
                m_afSubd[i0]);
            float fR = math::sqrt(fG*fG+(float)1.0);
            if ( fG < (float)0.0 )
            {
                fG = m_afDiag[i2]-m_afDiag[i0]+m_afSubd[i0]/(fG-fR);
            }
            else
            {
                fG = m_afDiag[i2]-m_afDiag[i0]+m_afSubd[i0]/(fG+fR);
            }
            float fSin = (float)1.0, fCos = (float)1.0, fP = (float)0.0;
            for (int i3 = i2-1; i3 >= i0; i3--)
            {
                float fF = fSin*m_afSubd[i3];
                float fB = fCos*m_afSubd[i3];
				float afF = math::abs(fF);
                if ( afF >= math::abs(fG))
                {
					fCos = afF > 0.f ?  fG/fF : 0 ;

                    fR = math::sqrt(fCos*fCos+(float)1.0);
                    m_afSubd[i3+1] = fF*fR;
                    fSin = ((float)1.0)/fR;
                    fCos *= fSin;
                }
                else
                {
                    fSin = fF/fG;
                    fR = math::sqrt(fSin*fSin+(float)1.0);
                    m_afSubd[i3+1] = fG*fR;
                    fCos = ((float)1.0)/fR;
                    fSin *= fCos;
                }
                fG = m_afDiag[i3+1]-fP;
                fR = (m_afDiag[i3]-fG)*fSin+((float)2.0)*fB*fCos;
                fP = fSin*fR;
                m_afDiag[i3+1] = fG+fP;
                fG = fCos*fR-fB;

                for (int i4 = 0; i4 < m_iSize; i4++)
                {
                    fF = m_kMat.elements[i4][i3+1];
					m_kMat.elements[i4][i3+1] = fSin*m_kMat.elements[i4][i3]+fCos*fF;
                    m_kMat.elements[i4][i3] = fCos*m_kMat.elements[i4][i3]-fSin*fF;
                }
            }
            m_afDiag[i0] -= fP;
            m_afSubd[i0] = fG;
            m_afSubd[i2] = (float)0.0;
        }
        if (i1 == iMaxIter)
        {
            return false;
        }
    }

    return true;
}


void Eigen::DecreasingSort ()
{
    // sort eigenvalues in decreasing order, e[0] >= ... >= e[iSize-1]
    for (int i0 = 0, i1; i0 <= m_iSize-2; i0++)
    {
        // locate maximum eigenvalue
        i1 = i0;
        float fMax = m_afDiag[i1];
        int i2;
        for (i2 = i0+1; i2 < m_iSize; i2++)
        {
            if (m_afDiag[i2] > fMax)
            {
                i1 = i2;
                fMax = m_afDiag[i1];
            }
        }

        if (i1 != i0)
        {
            // swap eigenvalues
            m_afDiag[i1] = m_afDiag[i0];
            m_afDiag[i0] = fMax;

            // swap eigenvectors
            for (i2 = 0; i2 < m_iSize; i2++)
            {
                float fTmp = m_kMat.elements[i2][i0];
                m_kMat.elements[i2][i0] = m_kMat.elements[i2][i1];
                m_kMat.elements[i2][i1] = fTmp;
                m_bIsRotation = !m_bIsRotation;
            }
        }
    }
}



void Eigen::GuaranteeRotation ()
{
    if (!m_bIsRotation)
    {
        // change sign on the first column
        for (int iRow = 0; iRow < m_iSize; iRow++)
        {
            m_kMat.elements[iRow][0] = -m_kMat.elements[iRow][0];
        }
    }
}


} // namespace math
} // namespace xray