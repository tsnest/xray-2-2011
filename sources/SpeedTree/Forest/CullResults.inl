///////////////////////////////////////////////////////////////////////  
//  CullResults.inl
//
//  *** INTERACTIVE DATA VISUALIZATION (IDV) CONFIDENTIAL AND PROPRIETARY INFORMATION ***
//
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Interactive Data Visualization, Inc. and
//  may not be copied, disclosed, or exploited except in accordance with 
//  the terms of that agreement.
//
//      Copyright (c) 2003-2010 IDV, Inc.
//      All rights reserved in all media.
//
//      IDV, Inc.
//      http://www.idvinc.com
//
//  *** Release version 5.2.0 ***


///////////////////////////////////////////////////////////////////////
//  SForestCullResults::SForestCullResults

inline SForestCullResults::SForestCullResults( ) :
    m_pBaseTrees(NULL),
    m_bFrustumContainsModifiedCells(false)
{
}


///////////////////////////////////////////////////////////////////////
//  SForestCullResults::~SForestCullResults

inline SForestCullResults::~SForestCullResults( )
{
}


///////////////////////////////////////////////////////////////////////
//  SForestCullResults::Reserve

inline st_bool SForestCullResults::Reserve(const TTreeArray& /*aBaseTrees*/,
                                           st_int32 nMaxNumBaseTrees, 
                                           st_int32 nMaxNumVisibleCells,
                                           st_int32 nMaxVisibleInstancesPerBase,
                                           st_bool /*bHorzBillboards*/)
{
    st_bool bSuccess = true;

    bSuccess &= m_aVisibleCells.reserve(nMaxNumVisibleCells);
    bSuccess &= m_aNewVisibleCells.reserve(nMaxNumVisibleCells);
    bSuccess &= m_aPreviousVisibleCells.reserve(nMaxNumVisibleCells);

    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        bSuccess &= m_a3dInstances.resize(nMaxNumBaseTrees);
        for (st_int32 i = 0; i < nMaxNumBaseTrees; ++i)
        {
            m_a3dInstances[i].reserve(nMaxVisibleInstancesPerBase);
            m_a3dInstances[i].resize(0);
        }
    #else
        // unreferenced parameters
        nMaxNumBaseTrees = nMaxNumBaseTrees;
        nMaxVisibleInstancesPerBase = nMaxVisibleInstancesPerBase;
    #endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  SForestCullResults::VisibleInstances

inline const TInstanceLodArray* SForestCullResults::VisibleInstances(st_int32 nBaseTreeIndex) const
{
    assert(nBaseTreeIndex > -1 && nBaseTreeIndex < st_int32(m_pBaseTrees->size( )));

    const TInstanceLodArray* pVisibleInstances = NULL;

    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        pVisibleInstances = &(m_a3dInstances[nBaseTreeIndex]);
    #else
        const CTree* pBaseTree = m_pBaseTrees->at(nBaseTreeIndex);
        TBaseToInstanceLodMap::iterator iFind = m_m3dInstances.find(pBaseTree);
        if (iFind != m_m3dInstances.end( ))
            pVisibleInstances = &(iFind->second);
    #endif

    return pVisibleInstances;
}
