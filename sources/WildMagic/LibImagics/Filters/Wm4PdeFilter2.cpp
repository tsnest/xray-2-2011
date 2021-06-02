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
// Version: 4.0.0 (2006/09/21)

#include "Wm4ImagicsPCH.h"
#include "Wm4PdeFilter2.h"
#include "Wm4Math.h"
using namespace Wm4;

//----------------------------------------------------------------------------
PdeFilter2::PdeFilter2 (int iXBound, int iYBound, float fXSpacing,
    float fYSpacing, const float* afData, const bool* abMask,
    float fBorderValue, ScaleType eScaleType)
    :
    PdeFilter(iXBound*iYBound,afData,fBorderValue,eScaleType)
{
    m_iXBound = iXBound;
    m_iYBound = iYBound;
    m_fXSpacing = fXSpacing;
    m_fYSpacing = fYSpacing;
    m_fInvDx = 1.0f/fXSpacing;
    m_fInvDy = 1.0f/fYSpacing;
    m_fHalfInvDx = 0.5f*m_fInvDx;
    m_fHalfInvDy = 0.5f*m_fInvDy;
    m_fInvDxDx = m_fInvDx*m_fInvDx;
    m_fFourthInvDxDy = m_fHalfInvDx*m_fHalfInvDy;
    m_fInvDyDy = m_fInvDy*m_fInvDy;

    // Create two buffers for filtering, one a source buffer and one a
    // destination buffer.  Their roles are reversed on each iteration of
    // the filter.
    int iXBp2 = m_iXBound + 2, iYBp2 = m_iYBound + 2;
    Allocate<float>(iXBp2,iYBp2,m_aafSrc);
    Allocate<float>(iXBp2,iYBp2,m_aafDst);
    if (abMask)
    {
        Allocate<bool>(iXBp2,iYBp2,m_aabMask);

        // This sets all mask values to 'false' initially.  The interior mask
        // values will be filled in later using the input abMask.  The
        // boundary mask values will remain 'false'.
        size_t uiNumBytes = iXBp2*iYBp2*sizeof(bool);
        memset(m_aabMask[0],0,uiNumBytes);
    }
    else
    {
        m_aabMask = 0;
    }

    // Copy the inputs afData and abMask to the interior elements of m_aafSrc
    // and m_aabMask.
    int iX, iY, iXp, iYp, i;
    for (iY = 0, iYp = 1, i = 0; iY < m_iYBound; iY++, iYp++)
    {
        for (iX = 0, iXp = 1; iX < m_iXBound; iX++, iXp++, i++)
        {
            m_aafSrc[iYp][iXp] = m_fOffset + (afData[i] - m_fMin)*m_fScale;
            if (m_aabMask)
            {
                m_aabMask[iYp][iXp] = abMask[i];
            }
        }
    }

    // Assign values to the 1-pixel image border.
    if (m_fBorderValue != Mathf::MAX_REAL)
    {
        AssignDirichletImageBorder();
    }
    else
    {
        AssignNeumannImageBorder();
    }

    // To handle masks that do not cover the entire image, assign values to
    // those pixels that are 8-neighbors of the mask pixels.
    if (m_aabMask)
    {
        if (m_fBorderValue != Mathf::MAX_REAL)
        {
            AssignDirichletMaskBorder();
        }
        else
        {
            AssignNeumannMaskBorder();
        }
    }
}
//----------------------------------------------------------------------------
PdeFilter2::~PdeFilter2 ()
{
    Deallocate<float>(m_aafSrc);
    Deallocate<float>(m_aafDst);
    if (m_aabMask)
    {
        Deallocate<bool>(m_aabMask);
    }
}
//----------------------------------------------------------------------------
void PdeFilter2::AssignDirichletImageBorder ()
{
    int iXBp1 = m_iXBound + 1, iYBp1 = m_iYBound + 1;
    int iX, iY;

    // vertex (0,0)
    m_aafSrc[0][0] = m_fBorderValue;
    m_aafDst[0][0] = m_fBorderValue;
    if (m_aabMask)
    {
        m_aabMask[0][0] = false;
    }

    // vertex (xmax,0)
    m_aafSrc[0][iXBp1] = m_fBorderValue;
    m_aafDst[0][iXBp1] = m_fBorderValue;
    if (m_aabMask)
    {
        m_aabMask[0][iXBp1] = false;
    }

    // vertex (0,ymax)
    m_aafSrc[iYBp1][0] = m_fBorderValue;
    m_aafDst[iYBp1][0] = m_fBorderValue;
    if (m_aabMask)
    {
        m_aabMask[iYBp1][0] = false;
    }

    // vertex (xmax,ymax)
    m_aafSrc[iYBp1][iXBp1] = m_fBorderValue;
    m_aafDst[iYBp1][iXBp1] = m_fBorderValue;
    if (m_aabMask)
    {
        m_aabMask[iYBp1][iXBp1] = false;
    }

    // edges (x,0) and (x,ymax)
    for (iX = 1; iX <= m_iXBound; iX++)
    {
        m_aafSrc[0][iX] = m_fBorderValue;
        m_aafDst[0][iX] = m_fBorderValue;
        if (m_aabMask)
        {
            m_aabMask[0][iX] = false;
        }

        m_aafSrc[iYBp1][iX] = m_fBorderValue;
        m_aafDst[iYBp1][iX] = m_fBorderValue;
        if (m_aabMask)
        {
            m_aabMask[iYBp1][iX] = false;
        }
    }

    // edges (0,y) and (xmax,y)
    for (iY = 1; iY <= m_iYBound; iY++)
    {
        m_aafSrc[iY][0] = m_fBorderValue;
        m_aafDst[iY][0] = m_fBorderValue;
        if (m_aabMask)
        {
            m_aabMask[iY][0] = false;
        }

        m_aafSrc[iY][iXBp1] = m_fBorderValue;
        m_aafDst[iY][iXBp1] = m_fBorderValue;
        if (m_aabMask)
        {
            m_aabMask[iY][iXBp1] = false;
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter2::AssignNeumannImageBorder ()
{
    int iXBp1 = m_iXBound + 1, iYBp1 = m_iYBound + 1;
    int iX, iY;
    float fDuplicate;

    // vertex (0,0)
    fDuplicate = m_aafSrc[1][1];
    m_aafSrc[0][0] = fDuplicate;
    m_aafDst[0][0] = fDuplicate;
    if (m_aabMask)
    {
        m_aabMask[0][0] = false;
    }

    // vertex (xmax,0)
    fDuplicate = m_aafSrc[1][m_iXBound];
    m_aafSrc[0][iXBp1] = fDuplicate;
    m_aafDst[0][iXBp1] = fDuplicate;
    if (m_aabMask)
    {
        m_aabMask[0][iXBp1] = false;
    }

    // vertex (0,ymax)
    fDuplicate = m_aafSrc[m_iYBound][1];
    m_aafSrc[iYBp1][0] = fDuplicate;
    m_aafDst[iYBp1][0] = fDuplicate;
    if (m_aabMask)
    {
        m_aabMask[iYBp1][0] = false;
    }

    // vertex (xmax,ymax)
    fDuplicate = m_aafSrc[m_iYBound][m_iXBound];
    m_aafSrc[iYBp1][iXBp1] = fDuplicate;
    m_aafDst[iYBp1][iXBp1] = fDuplicate;
    if (m_aabMask)
    {
        m_aabMask[iYBp1][iXBp1] = false;
    }

    // edges (x,0) and (x,ymax)
    for (iX = 1; iX <= m_iXBound; iX++)
    {
        fDuplicate = m_aafSrc[1][iX];
        m_aafSrc[0][iX] = fDuplicate;
        m_aafDst[0][iX] = fDuplicate;
        if (m_aabMask)
        {
            m_aabMask[0][iX] = false;
        }

        fDuplicate = m_aafSrc[m_iYBound][iX];
        m_aafSrc[iYBp1][iX] = fDuplicate;
        m_aafDst[iYBp1][iX] = fDuplicate;
        if (m_aabMask)
        {
            m_aabMask[iYBp1][iX] = false;
        }
    }

    // edges (0,y) and (xmax,y)
    for (iY = 1; iY <= m_iYBound; iY++)
    {
        fDuplicate = m_aafSrc[iY][1];
        m_aafSrc[iY][0] = fDuplicate;
        m_aafDst[iY][0] = fDuplicate;
        if (m_aabMask)
        {
            m_aabMask[iY][0] = false;
        }

        fDuplicate = m_aafSrc[iY][m_iXBound];
        m_aafSrc[iY][iXBp1] = fDuplicate;
        m_aafDst[iY][iXBp1] = fDuplicate;
        if (m_aabMask)
        {
            m_aabMask[iY][iXBp1] = false;
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter2::AssignDirichletMaskBorder ()
{
    assert(m_aabMask);

    for (int iY = 1; iY <= m_iYBound; iY++)
    {
        for (int iX = 1; iX <= m_iXBound; iX++)
        {
            if (m_aabMask[iY][iX])
            {
                continue;
            }

            bool bFound = false;
            int i0, i1, iLX, iLY;
            for (i1 = 0, iLY = iY-1; i1 < 3 && !bFound; i1++, iLY++)
            {
                for (i0 = 0, iLX = iX-1; i0 < 3; i0++, iLX++)
                {
                    if (m_aabMask[iLY][iLX])
                    {
                        m_aafSrc[iY][iX] = m_fBorderValue;
                        m_aafDst[iY][iX] = m_fBorderValue;
                        bFound = true;
                        break;
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter2::AssignNeumannMaskBorder ()
{
    // Recompute the values just outside the masked region.  This guarantees
    // that derivative estimations use the current values around the boundary.
    for (int iY = 1; iY <= m_iYBound; iY++)
    {
        for (int iX = 1; iX <= m_iXBound; iX++)
        {
            if (m_aabMask[iY][iX])
            {
                continue;
            }

            int iCount = 0;
            float fAverage = 0.0f;
            int i0, i1, iLX, iLY;
            for (i1 = 0, iLY = iY-1; i1 < 3; i1++, iLY++)
            {
                for (i0 = 0, iLX = iX-1; i0 < 3; i0++, iLX++)
                {
                    if (m_aabMask[iLY][iLX])
                    {
                        fAverage += m_aafSrc[iLY][iLX];
                        iCount++;
                    }
                }
            }

            if (iCount > 0)
            {
                fAverage /= (float)iCount;
                m_aafSrc[iY][iX] = fAverage;
                m_aafDst[iY][iX] = fAverage;
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter2::OnPreUpdate ()
{
    if (m_aabMask && m_fBorderValue == Mathf::MAX_REAL)
    {
        // Neumann boundary conditions are in use, so recompute the mask
        // border.
        AssignNeumannMaskBorder();
    }
    // else: No mask has been specified or Dirichlet boundary conditions are
    // in use.  Nothing to do.
}
//----------------------------------------------------------------------------
void PdeFilter2::OnUpdate ()
{
    for (int iY = 1; iY <= m_iYBound; iY++)
    {
        for (int iX = 1; iX <= m_iXBound; iX++)
        {
            if (!m_aabMask || m_aabMask[iY][iX])
            {
                OnUpdate(iX,iY);
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter2::OnPostUpdate ()
{
    // Swap the buffers for the next pass.
    float** aafSave = m_aafSrc;
    m_aafSrc = m_aafDst;
    m_aafDst = aafSave;
}
//----------------------------------------------------------------------------
void PdeFilter2::LookUp5 (int iX, int iY)
{
    int iXm = iX - 1, iXp = iX + 1;
    int iYm = iY - 1, iYp = iY + 1;
    m_fUzm = m_aafSrc[iYm][iX ];
    m_fUmz = m_aafSrc[iY ][iXm];
    m_fUzz = m_aafSrc[iY ][iX ];
    m_fUpz = m_aafSrc[iY ][iXp];
    m_fUzp = m_aafSrc[iYp][iX ];
}
//----------------------------------------------------------------------------
void PdeFilter2::LookUp9 (int iX, int iY)
{
    int iXm = iX - 1, iXp = iX + 1;
    int iYm = iY - 1, iYp = iY + 1;
    m_fUmm = m_aafSrc[iYm][iXm];
    m_fUzm = m_aafSrc[iYm][iX ];
    m_fUpm = m_aafSrc[iYm][iXp];
    m_fUmz = m_aafSrc[iY ][iXm];
    m_fUzz = m_aafSrc[iY ][iX ];
    m_fUpz = m_aafSrc[iY ][iXp];
    m_fUmp = m_aafSrc[iYp][iXm];
    m_fUzp = m_aafSrc[iYp][iX ];
    m_fUpp = m_aafSrc[iYp][iXp];
}
//----------------------------------------------------------------------------
