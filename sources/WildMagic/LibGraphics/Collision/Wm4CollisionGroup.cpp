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

#include "Wm4GraphicsPCH.h"
#include "Wm4CollisionGroup.h"
#include "Wm4CollisionRecord.h"
using namespace Wm4;

//----------------------------------------------------------------------------
CollisionGroup::CollisionGroup ()
{
}
//----------------------------------------------------------------------------
CollisionGroup::~CollisionGroup ()
{
    for (int i = 0; i < (int)m_kRecord.size(); i++)
    {
        WM4_DELETE m_kRecord[i];
    }
}
//----------------------------------------------------------------------------
bool CollisionGroup::Add (CollisionRecord* pkRecord)
{
    for (int i = 0; i < (int)m_kRecord.size(); i++)
    {
        if (pkRecord == m_kRecord[i])
        {
            return false;
        }
    }

    m_kRecord.push_back(pkRecord);
    return true;
}
//----------------------------------------------------------------------------
bool CollisionGroup::Remove (CollisionRecord* pkRecord)
{
    std::vector<CollisionRecord*>::iterator pkIter = m_kRecord.begin();
    for (/**/; pkIter != m_kRecord.end(); pkIter++)
    {
        if (pkRecord == *pkIter)
        {
            m_kRecord.erase(pkIter);
            WM4_DELETE pkRecord;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
void CollisionGroup::TestIntersection ()
{
    // objects are assumed to be stationary, compare all pairs
    for (int i0 = 0; i0 < (int)m_kRecord.size(); i0++)
    {
        CollisionRecord* pkRecord0 = m_kRecord[i0];
        for (int i1 = i0+1; i1 < (int)m_kRecord.size(); i1++)
        {
            CollisionRecord* pkRecord1 = m_kRecord[i1];
            pkRecord0->TestIntersection(*pkRecord1);
        }
    }
}
//----------------------------------------------------------------------------
void CollisionGroup::FindIntersection ()
{
    // objects are assumed to be stationary, compare all pairs
    for (int i0 = 0; i0 < (int)m_kRecord.size(); i0++)
    {
        CollisionRecord* pkRecord0 = m_kRecord[i0];
        for (int i1 = i0+1; i1 < (int)m_kRecord.size(); i1++)
        {
            CollisionRecord* pkRecord1 = m_kRecord[i1];
            pkRecord0->FindIntersection(*pkRecord1);
        }
    }
}
//----------------------------------------------------------------------------
void CollisionGroup::TestIntersection (float fTMax)
{
    // objects are assumed to be moving, compare all pairs
    for (int i0 = 0; i0 < (int)m_kRecord.size(); i0++)
    {
        CollisionRecord* pkRecord0 = m_kRecord[i0];
        for (int i1 = i0+1; i1 < (int)m_kRecord.size(); i1++)
        {
            CollisionRecord* pkRecord1 = m_kRecord[i1];
            if ( pkRecord0->GetVelocity() || pkRecord1->GetVelocity() )
                pkRecord0->TestIntersection(fTMax,*pkRecord1);
        }
    }
}
//----------------------------------------------------------------------------
void CollisionGroup::FindIntersection (float fTMax)
{
    // objects are assumed to be moving, compare all pairs
    for (int i0 = 0; i0 < (int)m_kRecord.size(); i0++)
    {
        CollisionRecord* pkRecord0 = m_kRecord[i0];
        for (int i1 = i0+1; i1 < (int)m_kRecord.size(); i1++)
        {
            CollisionRecord* pkRecord1 = m_kRecord[i1];
            if ( pkRecord0->GetVelocity() || pkRecord1->GetVelocity() )
                pkRecord0->FindIntersection(fTMax,*pkRecord1);
        }
    }
}
//----------------------------------------------------------------------------
