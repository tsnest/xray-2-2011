///////////////////////////////////////////////////////////////////////  
//  CellContainer.inl
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
//  Function ComputeCellCoords

inline void ComputeCellCoords(const Vec3& vPos, st_float32 fCellSize, st_int32& nRow, st_int32& nCol)
{
    assert(fCellSize > 0.0f);

    Vec3 vPos_cs = CCoordSys::ConvertToStd(vPos);

    nRow = (vPos_cs.y < 0.0f) ? st_int32((vPos_cs.y - fCellSize) / fCellSize) : st_int32(vPos_cs.y / fCellSize);
    nCol = (vPos_cs.x < 0.0f) ? st_int32((vPos_cs.x - fCellSize) / fCellSize) : st_int32(vPos_cs.x / fCellSize);
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::CCellContainer

template<class TCellType>
inline CCellContainer<TCellType>::CCellContainer( )
    : CMap<SCellKey, TCellType>(10)
    , m_fCellSize(1200.0f)
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
    , m_nMaxNumCells(100000) // default to practically unlimited
#endif
{
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::~CCellContainer

template<class TCellType>
inline CCellContainer<TCellType>::~CCellContainer( )
{
#ifndef NDEBUG
    m_fCellSize = -1.0f;
#endif
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::GetCellPtrByRowCol_Add

template<class TCellType>
inline TCellType* CCellContainer<TCellType>::GetCellPtrByRowCol_Add(st_int32 nRow, st_int32 nCol)
{
    typename CCellContainer<TCellType>::iterator iCell = GetCellItrByRowCol_Add(nRow, nCol);
    if (iCell == CMap<SCellKey, TCellType>::end( ))
        return NULL;
    else
        return &iCell->second;
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::GetCellPtrByRowCol

template<class TCellType>
inline const TCellType* CCellContainer<TCellType>::GetCellPtrByRowCol(st_int32 nRow, st_int32 nCol) const
{
    typename CCellContainer<TCellType>::const_iterator iCell = GetCellItrByRowCol(nRow, nCol);
    if (iCell == CMap<SCellKey, TCellType>::end( ))
        return NULL;
    else
        return &iCell->second;
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::GetCellPtrByPos_Add

template<class TCellType>
inline TCellType* CCellContainer<TCellType>::GetCellPtrByPos_Add(const Vec3& vPos)
{
    typename CCellContainer<TCellType>::iterator iCell = GetCellItrByPos(vPos);
    if (iCell == CMap<SCellKey, TCellType>::end( ))
        return NULL;
    else
        return &iCell->second;
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::GetCellPtrByPos

template<class TCellType>
inline const TCellType* CCellContainer<TCellType>::GetCellPtrByPos(const Vec3& vPos) const
{
    typename CCellContainer<TCellType>::const_iterator iCell = GetCellItrByPos(vPos);
    if (iCell == CMap<SCellKey, TCellType>::end( ))
        return NULL;
    else
        return &iCell->second;
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::GetCellItrByRowCol_Add

template<class TCellType>
inline typename CCellContainer<TCellType>::iterator CCellContainer<TCellType>::GetCellItrByRowCol_Add(st_int32 nRow, st_int32 nCol)
{
    // use a cell key to see if the map already contains this row/col cell
    SCellKey sKey(nRow, nCol);
    typename CCellContainer<TCellType>::iterator iCell = CMap<SCellKey, TCellType>::find(sKey);

    // cell wasn't found; did the caller specify that a new cell should be added?
    if (iCell == CMap<SCellKey, TCellType>::end( ))
    {
        #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            // return an empty cell if addition would exceed preset limit
            if (st_int32(CMap<SCellKey, TCellType>::size( )) >= m_nMaxNumCells)
                return CMap<SCellKey, TCellType>::end( );
        #endif

        // add it to the map and return the result
        TCellType& cCell = (*this)[sKey];
        assert(!cCell.GetExtents( ).Valid( ));
        cCell.SetRowCol(nRow, nCol);

        iCell = CMap<SCellKey, TCellType>::find(sKey);
        assert((iCell != CMap<SCellKey, TCellType>::end( )));

        // sanity test
        assert(cCell.IsNew( ));
    }

    return iCell;
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::GetCellItrByRowCol

template<class TCellType>
inline typename CCellContainer<TCellType>::const_iterator CCellContainer<TCellType>::GetCellItrByRowCol(st_int32 nRow, st_int32 nCol) const
{
    // use a cell key to see if the map already contains this row/col cell
    SCellKey sKey(nRow, nCol);
    typename CCellContainer::const_iterator iCell = CMap<SCellKey, TCellType>::find(sKey);

    return iCell;
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::GetCellItrByPos_Add

template<class TCellType>
inline typename CCellContainer<TCellType>::iterator CCellContainer<TCellType>::GetCellItrByPos_Add(const Vec3& vPos)
{
    // convert 3D position to a row/col pair
    st_int32 nRow, nCol;
    ComputeCellCoords(vPos, m_fCellSize, nRow, nCol);

    // use the row/col pair to get the cell
    return GetCellItrByRowCol_Add(nRow, nCol);
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::GetCellItrByPos

template<class TCellType>
inline typename CCellContainer<TCellType>::const_iterator CCellContainer<TCellType>::GetCellItrByPos(const Vec3& vPos) const
{
    // convert 3D position to a row/col pair
    st_int32 nRow, nCol;
    ComputeCellCoords(vPos, m_fCellSize, nRow, nCol);

    // use the row/col pair to get the cell
    return GetCellItrByRowCol(nRow, nCol);
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::Erase

template<class TCellType>
inline typename CCellContainer<TCellType>::iterator CCellContainer<TCellType>::Erase(typename CCellContainer<TCellType>::iterator iCell)
{
    return erase(iCell);
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::GetCellSize

template<class TCellType>
inline st_float32 CCellContainer<TCellType>::GetCellSize(void) const
{
    return m_fCellSize;
}


///////////////////////////////////////////////////////////////////////
//  CCellContainer::SetCellSize

template<class TCellType>
inline void CCellContainer<TCellType>::SetCellSize(st_float32 fCellSize)
{
    m_fCellSize = fCellSize;
}


#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE

    ///////////////////////////////////////////////////////////////////////
    //  CCellContainer::ManuallyCreateCell

    template<class TCellType>
    inline void CCellContainer<TCellType>::ManuallyCreateCell(st_int32 nRow,
                                                              st_int32 nCol,
                                                              st_byte* pBlock,
                                                              st_int32 nNumBaseTrees,
                                                              st_int32 nNumInstances)
    {
        #ifndef NDEBUG
            // when using ManuallyCreateCell, we have to assume that the cell doesn't already exist
            typename CCellContainer<TCellType>::iterator iCell = CMap<SCellKey, TCellType>::find(SCellKey(nRow, nCol));
            assert((iCell == CMap<SCellKey, TCellType>::end( )));
        #endif

        // get the cell using the standard interface
        TCellType* pCell = GetCellPtrByRowCol_Add(nRow, nCol);
        assert(pCell);

        // have the cell's data structures use the externally supplied block
        pCell->GetCellInstances( ).UseExternalMemory(pBlock, nNumBaseTrees, nNumInstances);
    }


    ///////////////////////////////////////////////////////////////////////
    //  CCellContainer::SetMaxNumCells

    template<class TCellType>
    inline void CCellContainer<TCellType>::SetMaxNumCells(st_int32 nMaxNumCells)
    {
        m_nMaxNumCells = nMaxNumCells;
    }

#endif


///////////////////////////////////////////////////////////////////////
//  CCellBaseTreeItr::CCellBaseTreeItr

inline CCellBaseTreeItr::CCellBaseTreeItr( ) :
    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        m_nBaseTreeItr(-1),
    #endif
    m_pCell(NULL)
{
}


///////////////////////////////////////////////////////////////////////
//  CCellBaseTreeItr::CCellBaseTreeItr

inline CCellBaseTreeItr::CCellBaseTreeItr(const CCellInstances* pCell, const CTree* pBaseTree) :
    m_pCell(pCell)
{
    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        m_nBaseTreeItr = pCell->GetBaseTreeIndex(pBaseTree);
    #else
        TBaseToInstanceMap::const_iterator iFind = pCell->find(pBaseTree);
        if (iFind != pCell->end( ))
            m_iBaseTreeItr = iFind;
    #endif
}


#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE

    ///////////////////////////////////////////////////////////////////////
    //  CCellBaseTreeItr::CCellBaseTreeItr

    inline CCellBaseTreeItr::CCellBaseTreeItr(const CCellInstances* pCell, st_int32 nBaseTreeIndex) :
        m_pCell(pCell)
    {
        m_nBaseTreeItr = nBaseTreeIndex;
    }

#else // SPEEDTREE_HEAP_FRIENDLY_MODE

    ///////////////////////////////////////////////////////////////////////
    //  CCellBaseTreeItr::CCellBaseTreeItr

    inline CCellBaseTreeItr::CCellBaseTreeItr(const CCellInstances* pCell, const TBaseToInstanceMap::const_iterator& iBaseTreeItr) :
        m_pCell(pCell)
    {
        m_iBaseTreeItr = iBaseTreeItr;
    }

#endif // SPEEDTREE_HEAP_FRIENDLY_MODE


///////////////////////////////////////////////////////////////////////
//  CCellBaseTreeItr::~CCellBaseTreeItr

inline CCellBaseTreeItr::~CCellBaseTreeItr( )
{
}


///////////////////////////////////////////////////////////////////////
//  CCellBaseTreeItr::TreePtr

inline const CTree* CCellBaseTreeItr::TreePtr(void) const
{
    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        return (m_nBaseTreeItr == -1 || m_nBaseTreeItr >= m_pCell->m_nNumBaseTrees) ? NULL : m_pCell->m_aBaseTrees[m_nBaseTreeItr];
    #else
        return (m_iBaseTreeItr == m_pCell->end( )) ? NULL : m_iBaseTreeItr->first;
    #endif
}


#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE

    ///////////////////////////////////////////////////////////////////////
    //  CCellBaseTreeItr::TreeIndex

    inline st_int32 CCellBaseTreeItr::TreeIndex(void) const
    {
        return (m_nBaseTreeItr > m_pCell->m_nNumBaseTrees) ? -1 : m_nBaseTreeItr;
    }

#else

    ///////////////////////////////////////////////////////////////////////
    //  CCellBaseTreeItr::TreeItr

    inline const TBaseToInstanceMap::const_iterator& CCellBaseTreeItr::TreeItr(void) const
    {
        return m_iBaseTreeItr;
    }

#endif // SPEEDTREE_HEAP_FRIENDLY_MODE


///////////////////////////////////////////////////////////////////////
//  CCellBaseTreeItr::operator++

inline CCellBaseTreeItr& CCellBaseTreeItr::operator++(void)
{
    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        ++m_nBaseTreeItr;
    #else
        ++m_iBaseTreeItr;
    #endif

    return *this;
}


///////////////////////////////////////////////////////////////////////
//  CCellInstances::CCellInstances

inline CCellInstances::CCellInstances( )
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        : m_nNumBaseTrees(0)
        , m_nTotalNumInstances(0)
    {
    }
#else
    {
    }
#endif


///////////////////////////////////////////////////////////////////////
//  CCellInstances::~CCellInstances

inline CCellInstances::~CCellInstances( )
{
}


///////////////////////////////////////////////////////////////////////
//  CCellInstances::FirstBaseTree

inline CCellBaseTreeItr CCellInstances::FirstBaseTree(void) const
{
    CCellBaseTreeItr iBaseTreeItr;

    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        iBaseTreeItr = CCellBaseTreeItr(this, st_int32(0));
    #else
        iBaseTreeItr = CCellBaseTreeItr(this, TBaseToInstanceMap::begin( ));
    #endif

    return iBaseTreeItr;
}


///////////////////////////////////////////////////////////////////////
//  CCellInstances::NumBaseTrees

inline st_int32 CCellInstances::NumBaseTrees(void) const
{
    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        return m_nNumBaseTrees;
    #else
        return st_int32(TBaseToInstanceMap::size( ));
    #endif
}


///////////////////////////////////////////////////////////////////////
//  CCellInstances::AddBaseTree
//
//  Returns true if base tree was added, false if it was already in the table.

inline st_bool CCellInstances::AddBaseTree(const CTree* pBaseTree)
{
    assert(pBaseTree);

    st_bool bTreeAdded = false;

    if (!IsBaseTreeInCell(pBaseTree))
    {
        // not added yet, so add it
        #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            // make sure we haven't reached the limit
            if (size_t(m_nNumBaseTrees) < m_aBaseTrees.capacity( ))
            {
                // safe to add the tree
                m_aBaseTrees[m_nNumBaseTrees] = pBaseTree;
                m_aNumInstances[m_nNumBaseTrees] = 0;

                // determine instance insert point
                m_aInstanceIndices[m_nNumBaseTrees] = m_nTotalNumInstances;
                m_nNumBaseTrees++;

                bTreeAdded = true;
            }
            else
                CCore::SetError("CCellInstances::AddBaseTree, max number of base trees (%d) reached, SPEEDTREE_HEAP_FRIENDLY_MODE is active", m_aBaseTrees.capacity( ));
        #else
            (void) TBaseToInstanceMap::operator[](pBaseTree);
            bTreeAdded = true;
        #endif
    }

    return bTreeAdded;
}


///////////////////////////////////////////////////////////////////////
//  CCellInstances::DeleteBaseTree

inline st_bool CCellInstances::DeleteBaseTree(const CTree* pBaseTree)
{
    assert(pBaseTree);

    st_bool bSuccess = false;

    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        st_int32 nBaseTreeIndex = GetBaseTreeIndex(pBaseTree);
        if (nBaseTreeIndex > -1)
        {
            assert(nBaseTreeIndex < m_nNumBaseTrees);

            // clear the delete base tree's values
            m_aInstanceIndices[nBaseTreeIndex] = 0;
            m_aNumInstances[nBaseTreeIndex] = 0;

            // need to shift three separate arrays
            for (st_int32 i = nBaseTreeIndex; i < m_nNumBaseTrees - 1; ++i)
            {
                // m_apBaseTrees
                m_aBaseTrees[i] = m_aBaseTrees[i + 1];

                // m_anNumInstances
                m_aNumInstances[i] = m_aNumInstances[i + 1];

                // m_anInstanceIndices
                // leave intact

                // m_acInstances
                assert(!m_aInstances.empty( ));
                memcpy(&m_aInstances[0] + m_aInstanceIndices[i], &m_aInstances[0] + m_aInstanceIndices[i + 1], m_aNumInstances[i + 1] * sizeof(CInstance));
            }

            --m_nNumBaseTrees;
            bSuccess = true;
        }
    #else
        TBaseToInstanceMap::iterator iFind = TBaseToInstanceMap::find(pBaseTree);
        if (iFind != TBaseToInstanceMap::end( ))
        {
            TBaseToInstanceMap::erase(iFind);
            bSuccess = true;
        }
    #endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CCellInstances::IsBaseTreeInCell

inline st_bool CCellInstances::IsBaseTreeInCell(const CTree* pBaseTree) const
{
    st_bool bFound = false;

    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        // use a simple linear search
        for (st_int32 nBase = 0; nBase < m_nNumBaseTrees; ++nBase)
        {
            if (m_aBaseTrees[nBase] == pBaseTree)
            {
                bFound = true;
                break;
            }
        }
    #else
        TBaseToInstanceMap::iterator iFind = TBaseToInstanceMap::find(pBaseTree);

        bFound = (iFind != TBaseToInstanceMap::end( ));
    #endif

    return bFound;
}


///////////////////////////////////////////////////////////////////////
//  CCellInstances::NumInstances
//
//  If nBaseTreeIndex is -1, then return the number of instances for all base trees

inline st_int32 CCellInstances::NumInstances(const CTree* pBaseTree) const
{
    st_int32 nNumInstances = 0;

    // if NULL, report the number of instances across all base trees
    if (pBaseTree == NULL)
    {
        #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            for (st_int32 nBase = 0; nBase < m_nNumBaseTrees; ++nBase)
                nNumInstances += m_aNumInstances[nBase];
        #else
            for (TBaseToInstanceMap::const_iterator iMap = TBaseToInstanceMap::begin( ); iMap != TBaseToInstanceMap::end( ); ++iMap)
                nNumInstances += st_int32(iMap->second.size( ));
        #endif
    }
    // if not NULL, report the number of instances for the specific base tree
    else
    {
        #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            st_int32 nBase = GetBaseTreeIndex(pBaseTree);
            if (nBase > -1)
            {
                assert(nBase < m_nNumBaseTrees);
                nNumInstances = m_aNumInstances[nBase];
            }
        #else
            TBaseToInstanceMap::const_iterator iMap = TBaseToInstanceMap::find(pBaseTree);

            // if present, retrieve the instance array size
            if (iMap != TBaseToInstanceMap::end( ))
                nNumInstances = st_int32(iMap->second.size( ));
        #endif
    }

    return nNumInstances;
}


#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE

    ///////////////////////////////////////////////////////////////////////
    //  Function: ArrayLowerBoundSearch
    //
    //  Assumes pArray is sorted in ascending order

    template<class T> inline st_int32 ArrayLowerBoundSearch(const T *pArray, st_int32 nArraySize, const T& cInstance)
    {
        st_int32 nInsertionIndex = 0;

        if (nArraySize > 0 && !(cInstance < pArray[0]))
        {
            st_int32 nWidth = nArraySize / 2;
            const CInstance* pStart = pArray;
            const CInstance* pEnd = pArray + nArraySize;

            while (nWidth > 0)
            {
                const CInstance* pMiddle = pStart + nWidth;
                if (cInstance < *pMiddle)
                    pEnd = pMiddle;
                else
                    pStart = pMiddle;

                nWidth = st_int32(pEnd - pStart) / 2;
            }

            nInsertionIndex = st_int32(pStart - pArray) + 1;
        }

        return nInsertionIndex;
    }


    ///////////////////////////////////////////////////////////////////////
    //  Function: ArrayInsert

    template<class T> inline void ArrayInsert(T *pArray, st_int32 nArraySize, st_int32 nInsertionIndex, const T& cInstance)
    {
        // move the memory forward
        if (nInsertionIndex < nArraySize)
        {
            st_int32 nElementsToMove = nArraySize - nInsertionIndex;
            if (nElementsToMove > 0)
                memmove(pArray + nInsertionIndex + 1, pArray + nInsertionIndex, nElementsToMove * sizeof(CInstance));
        }

        pArray[nInsertionIndex] = cInstance;
    }


    ///////////////////////////////////////////////////////////////////////
    //  Function: ArrayDeleteOrdered

    template<class T> inline st_bool ArrayDeleteOrdered(T *pArray, st_int32 nArraySize, st_int32 nDeletionIndex, const T& cInstance)
    {
        st_bool bFound = false;

        assert(nDeletionIndex > -1);

        if (nDeletionIndex < nArraySize && pArray[nDeletionIndex] == cInstance)
        {
            // delete it by copying the elements after it over it
            st_int32 nNumSuccessiveElements = nArraySize - nDeletionIndex - 1;
            if (nNumSuccessiveElements > 0)
                memmove(pArray + nDeletionIndex, pArray + nDeletionIndex + 1, nNumSuccessiveElements * sizeof(CInstance));

            bFound = true;
        }

        return bFound;
    }

#endif


///////////////////////////////////////////////////////////////////////
//  CCellInstances::AddInstance

inline st_bool CCellInstances::AddInstance(const CTree* pBaseTree, const CInstance& cInstance)
{
    assert(pBaseTree);

    st_bool bSuccess = false;

    // find base tree in map; add it if not already in
    (void) AddBaseTree(pBaseTree);

#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
    st_int32 nBaseIndex = GetBaseTreeIndex(pBaseTree);
    if (nBaseIndex > -1)
    {
        assert(size_t(nBaseIndex) < m_aBaseTrees.size( ));
        if (size_t(m_nTotalNumInstances) < m_aInstances.capacity( ))
        {
            // insert value into array
            assert(!m_aInstances.empty( ));
            CInstance* pArray = &m_aInstances[0] + m_aInstanceIndices[nBaseIndex];
            st_int32 nNumInstancesForBase = m_aNumInstances[nBaseIndex];

            st_int32 nInsertionIndex = ArrayLowerBoundSearch<CInstance>(pArray, nNumInstancesForBase, cInstance);
            ArrayInsert<CInstance>(pArray, m_nTotalNumInstances - m_aInstanceIndices[nBaseIndex], nInsertionIndex, cInstance);

            // increment global and base tree instance counts
            ++m_nTotalNumInstances;
            ++m_aNumInstances[nBaseIndex];

            // increment insertion indices for base trees occurring after this one
            for (st_int32 nSuccessiveBase = nBaseIndex + 1; nSuccessiveBase < m_nNumBaseTrees; ++nSuccessiveBase)
                ++m_aInstanceIndices[nSuccessiveBase];

            bSuccess = true;
        }
        else
            CCore::SetError("CCellInstances::AddInstance, additional instance exceeds instance limit of %d", m_aInstances.capacity( ));
    }
#else
    TBaseToInstanceMap::iterator iMap = TBaseToInstanceMap::find(pBaseTree);
    if (iMap != TBaseToInstanceMap::end( ))
    {
        iMap->second.insert_sorted(cInstance);
        bSuccess = true;
    }
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CCellInstances::GetInstances

inline const CInstance* CCellInstances::GetInstances(const CCellBaseTreeItr& iBaseTree, st_int32& nNumInstances) const
{
    const CTree* pTree = iBaseTree.TreePtr( );
    assert(pTree);

    const CInstance* pInstances = NULL;
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
    nNumInstances = NumInstances(pTree);
    if (nNumInstances > 0)
    {
        assert(!m_aInstances.empty( ));
        pInstances = &m_aInstances[0] + m_aInstanceIndices[iBaseTree.TreeIndex( )];
    }
#else
    TBaseToInstanceMap::iterator iMap = TBaseToInstanceMap::find(pTree);
    if (iMap != TBaseToInstanceMap::end( ))
    {
        nNumInstances = st_int32(iMap->second.size( ));
        if (nNumInstances > 0)
            pInstances = &iMap->second.at(0);
    }
#endif

    return pInstances;
}


///////////////////////////////////////////////////////////////////////
//  CCellInstances::DeleteInstance

inline st_bool CCellInstances::DeleteInstance(const CTree* pBaseTree, const CInstance& cInstance)
{
    assert(pBaseTree);

    st_bool bSuccess = false;

#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
    st_int32 nBaseIndex = GetBaseTreeIndex(pBaseTree);
    if (nBaseIndex > -1)
    {
        assert(nBaseIndex < m_nNumBaseTrees);

        // delete the value from the array
        assert(!m_aInstances.empty( ));
        CInstance* pArray = &m_aInstances[0] + m_aInstanceIndices[nBaseIndex];
        st_int32 nNumInstancesForBase = m_aNumInstances[nBaseIndex];

        if (nNumInstancesForBase > 0)
        {
            st_int32 nDeletionIndex = ArrayLowerBoundSearch<CInstance>(pArray, nNumInstancesForBase, cInstance) - 1;
            if (ArrayDeleteOrdered<CInstance>(pArray, m_nTotalNumInstances - m_aInstanceIndices[nBaseIndex], nDeletionIndex, cInstance))
            {
                // decrement global and base tree instance counts
                --m_nTotalNumInstances;
                --m_aNumInstances[nBaseIndex];

                // decrement insertion indices for base trees occurring after this one
                for (st_int32 nSuccessiveBase = nBaseIndex + 1; nSuccessiveBase < m_nNumBaseTrees; ++nSuccessiveBase)
                    --m_aInstanceIndices[nSuccessiveBase];

                bSuccess = true;
            }
        }
        else
            CCore::SetError("CCellInstances::DeleteInstance, no instances stored for requested base tree");
    }
    else
        CCore::SetError("CCellInstances::DeleteInstance, cannot find base tree");

#else // SPEEDTREE_HEAP_FRIENDLY_MODE

    TBaseToInstanceMap::iterator iMap = TBaseToInstanceMap::find(pBaseTree);
    if (iMap != TBaseToInstanceMap::end( ))
    {
        // this list should contain the instance we're looking to delete
        TInstanceArray& aInstanceArray = iMap->second;

        // perform deletion from an ordered array
        TInstanceArray::iterator iOrderedSearch = aInstanceArray.lower(cInstance);
        if (iOrderedSearch != aInstanceArray.end( ) &&
            *iOrderedSearch == cInstance)
        {
            // remove the instance from the list
            aInstanceArray.erase(iOrderedSearch);

            if (aInstanceArray.empty( ))
                TBaseToInstanceMap::erase(iMap);

            bSuccess = true;
        }
        else
            CCore::SetError("CCellInstances::DeleteInstance, failed to find requested instance\n");
    }
    else
        CCore::SetError("CCellInstances::DeleteInstance, internal error 1\n");
#endif // SPEEDTREE_HEAP_FRIENDLY_MODE

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CCellInstances::ClearInstances

inline st_bool CCellInstances::ClearInstances(const CTree* pBaseTree)
{
    st_bool bSuccess = false;

    if (pBaseTree)
        bSuccess = DeleteBaseTree(pBaseTree);
    else
    {
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        bSuccess = true;
        for (st_int32 nBaseTree = m_nNumBaseTrees - 1; nBaseTree > -1 ; --nBaseTree)
            bSuccess &= DeleteBaseTree(m_aBaseTrees[nBaseTree]);
#else
        // clear all instances for the entire cell
        TBaseToInstanceMap::clear( );
        bSuccess = true;
#endif
    }

    return bSuccess;
}


#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE

    ///////////////////////////////////////////////////////////////////////
    //  CCellInstances::UseExternalMemory

    inline void CCellInstances::UseExternalMemory(st_byte* pBlock, st_int32 nNumBaseTrees, st_int32 nNumInstances)
    {
        st_uchar* pBlockPtr = pBlock;

        m_aBaseTrees.SetExternalMemory(pBlockPtr, nNumBaseTrees * sizeof(CTree*));
        pBlockPtr += nNumBaseTrees * sizeof(const CTree*);
        m_aBaseTrees.resize(nNumBaseTrees);

        m_aInstanceIndices.SetExternalMemory(pBlockPtr, nNumBaseTrees * sizeof(st_int32));
        pBlockPtr += nNumBaseTrees * sizeof(st_int32);
        m_aInstanceIndices.resize(nNumBaseTrees);

        m_aNumInstances.SetExternalMemory(pBlockPtr, nNumBaseTrees * sizeof(st_int32));
        pBlockPtr += nNumBaseTrees * sizeof(st_int32);
        m_aNumInstances.resize(nNumBaseTrees);

        m_aInstances.SetExternalMemory(pBlockPtr, nNumInstances * sizeof(CInstance));
        pBlockPtr += nNumInstances * sizeof(CInstance);
        m_aInstances.resize(nNumInstances);

        m_aBillboardBlocks.SetExternalMemory(pBlockPtr, nNumBaseTrees * sizeof(st_int32));
        pBlockPtr += nNumBaseTrees * sizeof(st_int32);
        m_aBillboardBlocks.resize(nNumBaseTrees);
    }


    ///////////////////////////////////////////////////////////////////////
    //  CCellInstances::GetBaseTreeIndex

    inline st_int32 CCellInstances::GetBaseTreeIndex(const CTree* pBaseTree) const
    {
        st_int32 nIndex = -1;

        for (st_int32 nBase = 0; nBase < m_nNumBaseTrees; ++nBase)
        {
            if (m_aBaseTrees[nBase] == pBaseTree)
            {
                nIndex = nBase;
                break;
            }
        }

        return nIndex;
    }

#endif // SPEEDTREE_HEAP_FRIENDLY_MODE




