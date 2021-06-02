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
#include "Wm4PdeFilter3.h"
#include "Wm4Math.h"
using namespace Wm4;

//----------------------------------------------------------------------------
PdeFilter3::PdeFilter3 (int iXBound, int iYBound, int iZBound,
    float fXSpacing, float fYSpacing, float fZSpacing, const float* afData,
    const bool* abMask, float fBorderValue, ScaleType eScaleType)
    :
    PdeFilter(iXBound*iYBound*iZBound,afData,fBorderValue,eScaleType)
{
    m_iXBound = iXBound;
    m_iYBound = iYBound;
    m_iZBound = iZBound;
    m_fXSpacing = fXSpacing;
    m_fYSpacing = fYSpacing;
    m_fZSpacing = fZSpacing;
    m_fInvDx = 1.0f/fXSpacing;
    m_fInvDy = 1.0f/fYSpacing;
    m_fInvDz = 1.0f/fZSpacing;
    m_fHalfInvDx = 0.5f*m_fInvDx;
    m_fHalfInvDy = 0.5f*m_fInvDy;
    m_fHalfInvDz = 0.5f*m_fInvDz;
    m_fInvDxDx = m_fInvDx*m_fInvDx;
    m_fFourthInvDxDy = m_fHalfInvDx*m_fHalfInvDy;
    m_fFourthInvDxDz = m_fHalfInvDx*m_fHalfInvDz;
    m_fInvDyDy = m_fInvDy*m_fInvDy;
    m_fFourthInvDyDz = m_fHalfInvDy*m_fHalfInvDz;
    m_fInvDzDz = m_fInvDz*m_fInvDz;

    // Create two buffers for filtering, one a source buffer and one a
    // destination buffer.  Their roles are reversed on each iteration of
    // the filter.
    int iXBp2 = m_iXBound + 2, iYBp2 = m_iYBound + 2, iZBp2 = m_iZBound + 2;
    Allocate<float>(iXBp2,iYBp2,iZBp2,m_aaafSrc);
    Allocate<float>(iXBp2,iYBp2,iZBp2,m_aaafDst);
    if (abMask)
    {
        Allocate<bool>(iXBp2,iYBp2,iZBp2,m_aaabMask);

        // This sets all mask values to 'false' initially.  The interior mask
        // values will be filled in later using the input abMask.  The
        // boundary mask values will remain 'false'.
        size_t uiNumBytes = iXBp2*iYBp2*iZBp2*sizeof(bool);
        memset(m_aaabMask[0][0],0,uiNumBytes);
    }
    else
    {
        m_aaabMask = 0;
    }

    // Copy the inputs afData and abMask to the interior elements of
    // m_aaafSrc and m_aaabMask.
    int iX, iY, iZ, iXp, iYp, iZp, i;
    for (iZ = 0, iZp = 1, i = 0; iZ < m_iZBound; iZ++, iZp++)
    {
        for (iY = 0, iYp = 1; iY < m_iYBound; iY++, iYp++)
        {
            for (iX = 0, iXp = 1; iX < m_iXBound; iX++, iXp++, i++)
            {
                m_aaafSrc[iZp][iYp][iXp] = m_fOffset + (afData[i] - m_fMin) *
                    m_fScale;

                if (m_aaabMask)
                {
                    m_aaabMask[iZp][iYp][iXp] = abMask[i];
                }
            }
        }
    }

    // Assign values to the 1-voxel thick border.
    if (m_fBorderValue != Mathf::MAX_REAL)
    {
        AssignDirichletImageBorder();
    }
    else
    {
        AssignNeumannImageBorder();
    }

    // To handle masks that do not cover the entire image, assign values to
    // those voxels that are 26-neighbors of the mask voxels.
    if (m_aaabMask)
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
PdeFilter3::~PdeFilter3 ()
{
    Deallocate<float>(m_aaafSrc);
    Deallocate<float>(m_aaafDst);
    if (m_aaabMask)
    {
        Deallocate<bool>(m_aaabMask);
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::AssignDirichletImageBorder ()
{
    int iXBp1 = m_iXBound + 1, iYBp1 = m_iYBound + 1, iZBp1 = m_iZBound + 1;
    int iX, iY, iZ;

    // vertex (0,0,0)
    m_aaafSrc[0][0][0] = m_fBorderValue;
    m_aaafDst[0][0][0] = m_fBorderValue;
    if (m_aaabMask)
    {
        m_aaabMask[0][0][0] = false;
    }

    // vertex (xmax,0,0)
    m_aaafSrc[0][0][iXBp1] = m_fBorderValue;
    m_aaafDst[0][0][iXBp1] = m_fBorderValue;
    if (m_aaabMask)
    {
        m_aaabMask[0][0][iXBp1] = false;
    }

    // vertex (0,ymax,0)
    m_aaafSrc[0][iYBp1][0] = m_fBorderValue;
    m_aaafDst[0][iYBp1][0] = m_fBorderValue;
    if (m_aaabMask)
    {
        m_aaabMask[0][iYBp1][0] = false;
    }

    // vertex (xmax,ymax,0)
    m_aaafSrc[0][iYBp1][iXBp1] = m_fBorderValue;
    m_aaafDst[0][iYBp1][iXBp1] = m_fBorderValue;
    if (m_aaabMask)
    {
        m_aaabMask[0][iYBp1][iXBp1] = false;
    }

    // vertex (0,0,zmax)
    m_aaafSrc[iZBp1][0][0] = m_fBorderValue;
    m_aaafDst[iZBp1][0][0] = m_fBorderValue;
    if (m_aaabMask)
    {
        m_aaabMask[iZBp1][0][0] = false;
    }

    // vertex (xmax,0,zmax)
    m_aaafSrc[iZBp1][0][iXBp1] = m_fBorderValue;
    m_aaafDst[iZBp1][0][iXBp1] = m_fBorderValue;
    if (m_aaabMask)
    {
        m_aaabMask[iZBp1][0][iXBp1] = false;
    }

    // vertex (0,ymax,zmax)
    m_aaafSrc[iZBp1][iYBp1][0] = m_fBorderValue;
    m_aaafDst[iZBp1][iYBp1][0] = m_fBorderValue;
    if (m_aaabMask)
    {
        m_aaabMask[iZBp1][iYBp1][0] = false;
    }

    // vertex (xmax,ymax,zmax)
    m_aaafSrc[iZBp1][iYBp1][iXBp1] = m_fBorderValue;
    m_aaafDst[iZBp1][iYBp1][iXBp1] = m_fBorderValue;
    if (m_aaabMask)
    {
        m_aaabMask[iZBp1][iYBp1][iXBp1] = false;
    }

    // edges (x,0,0) and (x,ymax,0)
    for (iX = 1; iX <= m_iXBound; iX++)
    {
        m_aaafSrc[0][0][iX] = m_fBorderValue;
        m_aaafDst[0][0][iX] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[0][0][iX] = false;
        }

        m_aaafSrc[0][iYBp1][iX] = m_fBorderValue;
        m_aaafDst[0][iYBp1][iX] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[0][iYBp1][iX] = false;
        }
    }

    // edges (0,y,0) and (xmax,y,0)
    for (iY = 1; iY <= m_iYBound; iY++)
    {
        m_aaafSrc[0][iY][0] = m_fBorderValue;
        m_aaafDst[0][iY][0] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[0][iY][0] = false;
        }

        m_aaafSrc[0][iY][iXBp1] = m_fBorderValue;
        m_aaafDst[0][iY][iXBp1] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[0][iY][iXBp1] = false;
        }
    }

    // edges (x,0,zmax) and (x,ymax,zmax)
    for (iX = 1; iX <= m_iXBound; iX++)
    {
        m_aaafSrc[iZBp1][0][iX] = m_fBorderValue;
        m_aaafDst[iZBp1][0][iX] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[iZBp1][0][iX] = false;
        }

        m_aaafSrc[iZBp1][iYBp1][iX] = m_fBorderValue;
        m_aaafDst[iZBp1][iYBp1][iX] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[iZBp1][iYBp1][iX] = false;
        }
    }

    // edges (0,y,zmax) and (xmax,y,zmax)
    for (iY = 1; iY <= m_iYBound; iY++)
    {
        m_aaafSrc[iZBp1][iY][0] = m_fBorderValue;
        m_aaafDst[iZBp1][iY][0] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[iZBp1][iY][0] = false;
        }

        m_aaafSrc[iZBp1][iY][iXBp1] = m_fBorderValue;
        m_aaafDst[iZBp1][iY][iXBp1] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[iZBp1][iY][iXBp1] = false;
        }
    }

    // edges (0,0,z) and (xmax,0,z)
    for (iZ = 1; iZ <= m_iZBound; iZ++)
    {
        m_aaafSrc[iZ][0][0] = m_fBorderValue;
        m_aaafDst[iZ][0][0] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[iZ][0][0] = false;
        }

        m_aaafSrc[iZ][0][iXBp1] = m_fBorderValue;
        m_aaafDst[iZ][0][iXBp1] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[iZ][0][iXBp1] = false;
        }
    }

    // edges (0,ymax,z) and (xmax,ymax,z)
    for (iZ = 1; iZ <= m_iZBound; iZ++)
    {
        m_aaafSrc[iZ][iYBp1][0] = m_fBorderValue;
        m_aaafDst[iZ][iYBp1][0] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[iZ][iYBp1][0] = false;
        }

        m_aaafSrc[iZ][iYBp1][iXBp1] = m_fBorderValue;
        m_aaafDst[iZ][iYBp1][iXBp1] = m_fBorderValue;
        if (m_aaabMask)
        {
            m_aaabMask[iZ][iYBp1][iXBp1] = false;
        }
    }

    // faces (x,y,0) and (x,y,zmax)
    for (iY = 1; iY <= m_iYBound; iY++)
    {
        for (iX = 1; iX <= m_iXBound; iX++)
        {
            m_aaafSrc[0][iY][iX] = m_fBorderValue;
            m_aaafDst[0][iY][iX] = m_fBorderValue;
            if (m_aaabMask)
            {
                m_aaabMask[0][iY][iX] = false;
            }

            m_aaafSrc[iZBp1][iY][iX] = m_fBorderValue;
            m_aaafDst[iZBp1][iY][iX] = m_fBorderValue;
            if (m_aaabMask)
            {
                m_aaabMask[iZBp1][iY][iX] = false;
            }
        }
    }

    // faces (x,0,z) and (x,ymax,z)
    for (iZ = 1; iZ <= m_iZBound; iZ++)
    {
        for (iX = 1; iX <= m_iXBound; iX++)
        {
            m_aaafSrc[iZ][0][iX] = m_fBorderValue;
            m_aaafDst[iZ][0][iX] = m_fBorderValue;
            if (m_aaabMask)
            {
                m_aaabMask[iZ][0][iX] = false;
            }

            m_aaafSrc[iZ][iYBp1][iX] = m_fBorderValue;
            m_aaafDst[iZ][iYBp1][iX] = m_fBorderValue;
            if (m_aaabMask)
            {
                m_aaabMask[iZ][iYBp1][iX] = false;
            }
        }
    }

    // faces (0,y,z) and (xmax,y,z)
    for (iZ = 1; iZ <= m_iZBound; iZ++)
    {
        for (iY = 1; iY <= m_iYBound; iY++)
        {
            m_aaafSrc[iZ][iY][0] = m_fBorderValue;
            m_aaafDst[iZ][iY][0] = m_fBorderValue;
            if (m_aaabMask)
            {
                m_aaabMask[iZ][iY][0] = false;
            }

            m_aaafSrc[iZ][iY][iXBp1] = m_fBorderValue;
            m_aaafDst[iZ][iY][iXBp1] = m_fBorderValue;
            if (m_aaabMask)
            {
                m_aaabMask[iZ][iY][iXBp1] = false;
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::AssignNeumannImageBorder ()
{
    int iXBp1 = m_iXBound + 1, iYBp1 = m_iYBound + 1, iZBp1 = m_iZBound + 1;
    int iX, iY, iZ;
    float fDuplicate;

    // vertex (0,0,0)
    fDuplicate = m_aaafSrc[1][1][1];
    m_aaafSrc[0][0][0] = fDuplicate;
    m_aaafDst[0][0][0] = fDuplicate;
    if (m_aaabMask)
    {
        m_aaabMask[0][0][0] = false;
    }

    // vertex (xmax,0,0)
    fDuplicate = m_aaafSrc[1][1][m_iXBound];
    m_aaafSrc[0][0][iXBp1] = fDuplicate;
    m_aaafDst[0][0][iXBp1] = fDuplicate;
    if (m_aaabMask)
    {
        m_aaabMask[0][0][iXBp1] = false;
    }

    // vertex (0,ymax,0)
    fDuplicate = m_aaafSrc[1][m_iYBound][1];
    m_aaafSrc[0][iYBp1][0] = fDuplicate;
    m_aaafDst[0][iYBp1][0] = fDuplicate;
    if (m_aaabMask)
    {
        m_aaabMask[0][iYBp1][0] = false;
    }

    // vertex (xmax,ymax,0)
    fDuplicate = m_aaafSrc[1][m_iYBound][m_iXBound];
    m_aaafSrc[0][iYBp1][iXBp1] = fDuplicate;
    m_aaafDst[0][iYBp1][iXBp1] = fDuplicate;
    if (m_aaabMask)
    {
        m_aaabMask[0][iYBp1][iXBp1] = false;
    }

    // vertex (0,0,zmax)
    fDuplicate = m_aaafSrc[m_iZBound][1][1];
    m_aaafSrc[iZBp1][0][0] = fDuplicate;
    m_aaafDst[iZBp1][0][0] = fDuplicate;
    if (m_aaabMask)
    {
        m_aaabMask[iZBp1][0][0] = false;
    }

    // vertex (xmax,0,zmax)
    fDuplicate = m_aaafSrc[m_iZBound][1][m_iXBound];
    m_aaafSrc[iZBp1][0][iXBp1] = fDuplicate;
    m_aaafDst[iZBp1][0][iXBp1] = fDuplicate;
    if (m_aaabMask)
    {
        m_aaabMask[iZBp1][0][iXBp1] = false;
    }

    // vertex (0,ymax,zmax)
    fDuplicate = m_aaafSrc[m_iZBound][m_iYBound][1];
    m_aaafSrc[iZBp1][iYBp1][0] = fDuplicate;
    m_aaafDst[iZBp1][iYBp1][0] = fDuplicate;
    if (m_aaabMask)
    {
        m_aaabMask[iZBp1][iYBp1][0] = false;
    }

    // vertex (xmax,ymax,zmax)
    fDuplicate = m_aaafSrc[m_iZBound][m_iYBound][m_iXBound];
    m_aaafSrc[iZBp1][iYBp1][iXBp1] = fDuplicate;
    m_aaafDst[iZBp1][iYBp1][iXBp1] = fDuplicate;
    if (m_aaabMask)
    {
        m_aaabMask[iZBp1][iYBp1][iXBp1] = false;
    }

    // edges (x,0,0) and (x,ymax,0)
    for (iX = 1; iX <= m_iXBound; iX++)
    {
        fDuplicate = m_aaafSrc[1][1][iX];
        m_aaafSrc[0][0][iX] = fDuplicate;
        m_aaafDst[0][0][iX] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[0][0][iX] = false;
        }

        fDuplicate = m_aaafSrc[1][m_iYBound][iX];
        m_aaafSrc[0][iYBp1][iX] = fDuplicate;
        m_aaafDst[0][iYBp1][iX] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[0][iYBp1][iX] = false;
        }
    }

    // edges (0,y,0) and (xmax,y,0)
    for (iY = 1; iY <= m_iYBound; iY++)
    {
        fDuplicate = m_aaafSrc[1][iY][1];
        m_aaafSrc[0][iY][0] = fDuplicate;
        m_aaafDst[0][iY][0] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[0][iY][0] = false;
        }

        fDuplicate = m_aaafSrc[1][iY][m_iXBound];
        m_aaafSrc[0][iY][iXBp1] = fDuplicate;
        m_aaafDst[0][iY][iXBp1] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[0][iY][iXBp1] = false;
        }
    }

    // edges (x,0,zmax) and (x,ymax,zmax)
    for (iX = 1; iX <= m_iXBound; iX++)
    {
        fDuplicate = m_aaafSrc[m_iZBound][1][iX];
        m_aaafSrc[iZBp1][0][iX] = fDuplicate;
        m_aaafDst[iZBp1][0][iX] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[iZBp1][0][iX] = false;
        }

        fDuplicate = m_aaafSrc[m_iZBound][m_iYBound][iX];
        m_aaafSrc[iZBp1][iYBp1][iX] = fDuplicate;
        m_aaafDst[iZBp1][iYBp1][iX] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[iZBp1][iYBp1][iX] = false;
        }
    }

    // edges (0,y,zmax) and (xmax,y,zmax)
    for (iY = 1; iY <= m_iYBound; iY++)
    {
        fDuplicate = m_aaafSrc[m_iZBound][iY][1];
        m_aaafSrc[iZBp1][iY][0] = fDuplicate;
        m_aaafDst[iZBp1][iY][0] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[iZBp1][iY][0] = false;
        }

        fDuplicate = m_aaafSrc[m_iZBound][iY][m_iXBound];
        m_aaafSrc[iZBp1][iY][iXBp1] = fDuplicate;
        m_aaafDst[iZBp1][iY][iXBp1] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[iZBp1][iY][iXBp1] = false;
        }
    }

    // edges (0,0,z) and (xmax,0,z)
    for (iZ = 1; iZ <= m_iZBound; iZ++)
    {
        fDuplicate = m_aaafSrc[iZ][1][1];
        m_aaafSrc[iZ][0][0] = fDuplicate;
        m_aaafDst[iZ][0][0] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[iZ][0][0] = false;
        }

        fDuplicate = m_aaafSrc[iZ][1][m_iXBound];
        m_aaafSrc[iZ][0][iXBp1] = fDuplicate;
        m_aaafDst[iZ][0][iXBp1] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[iZ][0][iXBp1] = false;
        }
    }

    // edges (0,ymax,z) and (xmax,ymax,z)
    for (iZ = 1; iZ <= m_iZBound; iZ++)
    {
        fDuplicate = m_aaafSrc[iZ][m_iYBound][1];
        m_aaafSrc[iZ][iYBp1][0] = fDuplicate;
        m_aaafDst[iZ][iYBp1][0] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[iZ][iYBp1][0] = false;
        }

        fDuplicate = m_aaafSrc[iZ][m_iYBound][m_iXBound];
        m_aaafSrc[iZ][iYBp1][iXBp1] = fDuplicate;
        m_aaafDst[iZ][iYBp1][iXBp1] = fDuplicate;
        if (m_aaabMask)
        {
            m_aaabMask[iZ][iYBp1][iXBp1] = false;
        }
    }

    // faces (x,y,0) and (x,y,zmax)
    for (iY = 1; iY <= m_iYBound; iY++)
    {
        for (iX = 1; iX <= m_iXBound; iX++)
        {
            fDuplicate = m_aaafSrc[1][iY][iX];
            m_aaafSrc[0][iY][iX] = fDuplicate;
            m_aaafDst[0][iY][iX] = fDuplicate;
            if (m_aaabMask)
            {
                m_aaabMask[0][iY][iX] = false;
            }

            fDuplicate = m_aaafSrc[m_iZBound][iY][iX];
            m_aaafSrc[iZBp1][iY][iX] = fDuplicate;
            m_aaafDst[iZBp1][iY][iX] = fDuplicate;
            if (m_aaabMask)
            {
                m_aaabMask[iZBp1][iY][iX] = false;
            }
        }
    }

    // faces (x,0,z) and (x,ymax,z)
    for (iZ = 1; iZ <= m_iZBound; iZ++)
    {
        for (iX = 1; iX <= m_iXBound; iX++)
        {
            fDuplicate = m_aaafSrc[iZ][1][iX];
            m_aaafSrc[iZ][0][iX] = fDuplicate;
            m_aaafDst[iZ][0][iX] = fDuplicate;
            if (m_aaabMask)
            {
                m_aaabMask[iZ][0][iX] = false;
            }

            fDuplicate = m_aaafSrc[iZ][m_iYBound][iX];
            m_aaafSrc[iZ][iYBp1][iX] = fDuplicate;
            m_aaafDst[iZ][iYBp1][iX] = fDuplicate;
            if (m_aaabMask)
            {
                m_aaabMask[iZ][iYBp1][iX] = false;
            }
        }
    }

    // faces (0,y,z) and (xmax,y,z)
    for (iZ = 1; iZ <= m_iZBound; iZ++)
    {
        for (iY = 1; iY <= m_iYBound; iY++)
        {
            fDuplicate = m_aaafSrc[iZ][iY][1];
            m_aaafSrc[iZ][iY][0] = fDuplicate;
            m_aaafDst[iZ][iY][0] = fDuplicate;
            if (m_aaabMask)
            {
                m_aaabMask[iZ][iY][0] = false;
            }

            fDuplicate = m_aaafSrc[iZ][iY][m_iXBound];
            m_aaafSrc[iZ][iY][iXBp1] = fDuplicate;
            m_aaafDst[iZ][iY][iXBp1] = fDuplicate;
            if (m_aaabMask)
            {
                m_aaabMask[iZ][iY][iXBp1] = false;
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::AssignDirichletMaskBorder ()
{
    assert(m_aaabMask);

    for (int iZ = 1; iZ <= m_iZBound; iZ++)
    {
        for (int iY = 1; iY <= m_iYBound; iY++)
        {
            for (int iX = 1; iX <= m_iXBound; iX++)
            {
                if (m_aaabMask[iZ][iY][iX])
                {
                    continue;
                }

                bool bFound = false;
                int i0, i1, i2, iLX, iLY, iLZ;
                for (i2 = 0, iLZ = iZ-1; i2 < 3 && !bFound; i2++, iLZ++)
                {
                    for (i1 = 0, iLY = iY-1; i1 < 3 && !bFound; i1++, iLY++)
                    {
                        for (i0 = 0, iLX = iX-1; i0 < 3; i0++, iLX++)
                        {
                            if (m_aaabMask[iLZ][iLY][iLX])
                            {
                                m_aaafSrc[iZ][iY][iX] = m_fBorderValue;
                                m_aaafDst[iZ][iY][iX] = m_fBorderValue;
                                bFound = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::AssignNeumannMaskBorder ()
{
    // Recompute the values just outside the masked region.  This guarantees
    // that derivative estimations use the current values around the boundary.
    for (int iZ = 1; iZ <= m_iZBound; iZ++)
    {
        for (int iY = 1; iY <= m_iYBound; iY++)
        {
            for (int iX = 1; iX <= m_iXBound; iX++)
            {
                if (m_aaabMask[iZ][iY][iX])
                {
                    continue;
                }

                int iCount = 0;
                float fAverage = 0.0f;
                int i0, i1, i2, iLX, iLY, iLZ;
                for (i2 = 0, iLZ = iZ-1; i2 < 3; i2++, iLZ++)
                {
                    for (i1 = 0, iLY = iY-1; i1 < 3; i1++, iLY++)
                    {
                        for (i0 = 0, iLX = iX-1; i0 < 3; i0++, iLX++)
                        {
                            if (m_aaabMask[iLZ][iLY][iLX])
                            {
                                fAverage += m_aaafSrc[iLZ][iLY][iLX];
                                iCount++;
                            }
                        }
                    }
                }

                if (iCount > 0)
                {
                    fAverage /= (float)iCount;
                    m_aaafSrc[iZ][iY][iX] = fAverage;
                    m_aaafDst[iZ][iY][iX] = fAverage;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::OnPreUpdate ()
{
    if (m_aaabMask && m_fBorderValue == Mathf::MAX_REAL)
    {
        // Neumann boundary conditions are in use, so recompute the mask
        // border.
        AssignNeumannMaskBorder();
    }
    // else: No mask has been specified or Dirichlet boundary conditions are
    // in use.  Nothing to do.
}
//----------------------------------------------------------------------------
void PdeFilter3::OnUpdate ()
{
    for (int iZ = 1; iZ <= m_iZBound; iZ++)
    {
        for (int iY = 1; iY <= m_iYBound; iY++)
        {
            for (int iX = 1; iX <= m_iXBound; iX++)
            {
                if (!m_aaabMask || m_aaabMask[iZ][iY][iX])
                {
                    OnUpdate(iX,iY,iZ);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void PdeFilter3::OnPostUpdate ()
{
    // Swap the buffers for the next pass.
    float*** aaafSave = m_aaafSrc;
    m_aaafSrc = m_aaafDst;
    m_aaafDst = aaafSave;
}
//----------------------------------------------------------------------------
void PdeFilter3::LookUp7 (int iX, int iY, int iZ)
{
    int iXm = iX - 1, iXp = iX + 1;
    int iYm = iY - 1, iYp = iY + 1;
    int iZm = iZ - 1, iZp = iZ + 1;
    m_fUzzm = m_aaafSrc[iZm][iY ][iX ];
    m_fUzmz = m_aaafSrc[iZ ][iYm][iX ];
    m_fUmzz = m_aaafSrc[iZ ][iY ][iXm];
    m_fUzzz = m_aaafSrc[iZ ][iY ][iX ];
    m_fUpzz = m_aaafSrc[iZ ][iY ][iXp];
    m_fUzpz = m_aaafSrc[iZ ][iYp][iX ];
    m_fUzzp = m_aaafSrc[iZp][iY ][iX ];
}
//----------------------------------------------------------------------------
void PdeFilter3::LookUp27 (int iX, int iY, int iZ)
{
    int iXm = iX - 1, iXp = iX + 1;
    int iYm = iY - 1, iYp = iY + 1;
    int iZm = iZ - 1, iZp = iZ + 1;
    m_fUmmm = m_aaafSrc[iZm][iYm][iXm];
    m_fUzmm = m_aaafSrc[iZm][iYm][iX ];
    m_fUpmm = m_aaafSrc[iZm][iYm][iXp];
    m_fUmzm = m_aaafSrc[iZm][iY ][iXm];
    m_fUzzm = m_aaafSrc[iZm][iY ][iX ];
    m_fUpzm = m_aaafSrc[iZm][iY ][iXp];
    m_fUmpm = m_aaafSrc[iZm][iYp][iXm];
    m_fUzpm = m_aaafSrc[iZm][iYp][iX ];
    m_fUppm = m_aaafSrc[iZm][iYp][iXp];
    m_fUmmz = m_aaafSrc[iZ ][iYm][iXm];
    m_fUzmz = m_aaafSrc[iZ ][iYm][iX ];
    m_fUpmz = m_aaafSrc[iZ ][iYm][iXp];
    m_fUmzz = m_aaafSrc[iZ ][iY ][iXm];
    m_fUzzz = m_aaafSrc[iZ ][iY ][iX ];
    m_fUpzz = m_aaafSrc[iZ ][iY ][iXp];
    m_fUmpz = m_aaafSrc[iZ ][iYp][iXm];
    m_fUzpz = m_aaafSrc[iZ ][iYp][iX ];
    m_fUppz = m_aaafSrc[iZ ][iYp][iXp];
    m_fUmmp = m_aaafSrc[iZp][iYm][iXm];
    m_fUzmp = m_aaafSrc[iZp][iYm][iX ];
    m_fUpmp = m_aaafSrc[iZp][iYm][iXp];
    m_fUmzp = m_aaafSrc[iZp][iY ][iXm];
    m_fUzzp = m_aaafSrc[iZp][iY ][iX ];
    m_fUpzp = m_aaafSrc[iZp][iY ][iXp];
    m_fUmpp = m_aaafSrc[iZp][iYp][iXm];
    m_fUzpp = m_aaafSrc[iZp][iYp][iX ];
    m_fUppp = m_aaafSrc[iZp][iYp][iXp];
}
//----------------------------------------------------------------------------
