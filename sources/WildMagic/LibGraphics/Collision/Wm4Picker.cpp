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
// Version: 4.0.1 (2006/08/13)

#include "Wm4GraphicsPCH.h"
#include "Wm4Picker.h"
#include "Wm4SwitchNode.h"
#include "Wm4TriMesh.h"
#include "Wm4IntrLine3Triangle3.h"
using namespace Wm4;

const PickRecord Picker::ms_kInvalid;

//----------------------------------------------------------------------------
Picker::Picker ()
    :
    m_kOrigin(Vector3f::ZERO),
    m_kDirection(Vector3f::ZERO)
{
    m_fTMin = 0.0f;
    m_fTMax = 0.0f;
}
//----------------------------------------------------------------------------
void Picker::Execute (Spatial* pkScene, const Vector3f& rkOrigin,
    const Vector3f& rkDirection, float fTMin, float fTMax)
{
#ifdef _DEBUG
    if (fTMin == -Mathf::MAX_REAL)
    {
        assert(fTMax == Mathf::MAX_REAL);
    }
    else
    {
        assert(fTMin == 0.0f && fTMax > 0.0f);
    }
#endif

    m_kOrigin = rkOrigin;
    m_kDirection = rkDirection;
    m_fTMin = fTMin;
    m_fTMax = fTMax;
    Records.clear();
    ExecuteRecursive(pkScene);
}
//----------------------------------------------------------------------------
const PickRecord& Picker::GetClosestToZero () const
{
    if (Records.size() == 0)
    {
        return ms_kInvalid;
    }

    float fClosest = Mathf::FAbs(Records[0].T);
    int iClosest = 0;
    for (int i = 1; i < (int)Records.size(); i++)
    {
        float fTmp = Mathf::FAbs(Records[i].T);
        if (fTmp < fClosest)
        {
            fClosest = fTmp;
            iClosest = i;
        }
    }
    return Records[iClosest];
}
//----------------------------------------------------------------------------
const PickRecord& Picker::GetClosestNonnegative () const
{
    if (Records.size() == 0)
    {
        return ms_kInvalid;
    }

    // Get first nonnegative value.
    float fClosest = Mathf::MAX_REAL;
    int iClosest;
    for (iClosest = 0; iClosest < (int)Records.size(); iClosest++)
    {
        if (Records[iClosest].T >= 0.0f)
        {
            fClosest = Records[iClosest].T;
            break;
        }
    }
    if (iClosest == (int)Records.size())
    {
        // All values are negative.
        return ms_kInvalid;
    }

    for (int i = iClosest+1; i < (int)Records.size(); i++)
    {
        if (0.0f <= Records[i].T && Records[i].T < fClosest)
        {
            fClosest = Records[i].T;
            iClosest = i;
        }
    }
    return Records[iClosest];
}
//----------------------------------------------------------------------------
const PickRecord& Picker::GetClosestNonpositive () const
{
    if (Records.size() == 0)
    {
        return ms_kInvalid;
    }

    // Get first nonpositive value.
    float fClosest = -Mathf::MAX_REAL;
    int iClosest;
    for (iClosest = 0; iClosest < (int)Records.size(); iClosest++)
    {
        if (Records[iClosest].T <= 0.0f)
        {
            fClosest = Records[iClosest].T;
            break;
        }
    }
    if (iClosest == (int)Records.size())
    {
        // All values are positive.
        return ms_kInvalid;
    }

    for (int i = iClosest+1; i < (int)Records.size(); i++)
    {
        if (fClosest < Records[i].T && Records[i].T <= 0.0f)
        {
            fClosest = Records[i].T;
            iClosest = i;
        }
    }
    return Records[iClosest];
}
//----------------------------------------------------------------------------
void Picker::ExecuteRecursive (Spatial* pkObject)
{
    Triangles* pkMesh = DynamicCast<Triangles>(pkObject);
    if (pkMesh)
    {
        if (pkMesh->WorldBound->TestIntersection(m_kOrigin,m_kDirection,
            m_fTMin,m_fTMax))
        {
            // Convert the linear component to model-space coordinates.
            Line3f kLine(pkMesh->World.ApplyInverse(m_kOrigin),
                pkMesh->World.InvertVector(m_kDirection));

            // Compute intersections with the model-space triangles.
            int iTQuantity = pkMesh->GetTriangleQuantity();
            for (int i = 0; i < iTQuantity; i++)
            {
                int iV0, iV1, iV2;
                if (!pkMesh->GetTriangle(i,iV0,iV1,iV2))
                {
                    continue;
                }

                Triangle3f kTriangle(
                    pkMesh->VBuffer->Position3(iV0),
                    pkMesh->VBuffer->Position3(iV1),
                    pkMesh->VBuffer->Position3(iV2));

                IntrLine3Triangle3f kIntr(kLine,kTriangle);
                if (kIntr.Find() && m_fTMin <= kIntr.GetLineT()
                &&  kIntr.GetLineT() <= m_fTMax)
                {
                    PickRecord kRecord;
                    kRecord.Intersected = pkMesh;
                    kRecord.T = kIntr.GetLineT();
                    kRecord.Triangle = i;
                    kRecord.B0 = kIntr.GetTriB0();
                    kRecord.B1 = kIntr.GetTriB1();
                    kRecord.B2 = kIntr.GetTriB2();
                    Records.push_back(kRecord);
                }
            }
        }
        return;
    }

    SwitchNode* pkSwitchNode = DynamicCast<SwitchNode>(pkObject);
    if (pkSwitchNode)
    {
        int iActiveChild = pkSwitchNode->GetActiveChild();
        if (iActiveChild != SwitchNode::SN_INVALID_CHILD)
        {
            if (pkSwitchNode->WorldBound->TestIntersection(m_kOrigin,
                m_kDirection,m_fTMin,m_fTMax))
            {
                Spatial* pkChild = pkSwitchNode->GetChild(iActiveChild);
                if (pkChild)
                {
                    ExecuteRecursive(pkChild);
                }
            }
        }
        return;
    }

    Node* pkNode = DynamicCast<Node>(pkObject);
    if (pkNode)
    {
        if (pkNode->WorldBound->TestIntersection(m_kOrigin,m_kDirection,
            m_fTMin,m_fTMax))
        {
            for (int i = 0; i < pkNode->GetQuantity(); i++)
            {
                Spatial* pkChild = pkNode->GetChild(i);
                if (pkChild)
                {
                    ExecuteRecursive(pkChild);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
