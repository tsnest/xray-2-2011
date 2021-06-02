///////////////////////////////////////////////////////////////////////  
//  Set.h
//
//  This file is part of IDV's lightweight STL-like container classes.
//  Like STL, these containers are templated and portable, but are thin
//  enough not to have some of the portability issues of STL nor will
//  it conflict with an application's similar libraries.  These containers
//  also contain some mechanisms for mitigating the inordinate number of
//  of heap allocations associated with the standard STL.
//
//
//  *** INTERACTIVE DATA VISUALIZATION (IDV) PROPRIETARY INFORMATION ***
//
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Interactive Data Visualization and may
//  not be copied or disclosed except in accordance with the terms of
//  that agreement
//
//      Copyright (c) 2003-2010 IDV, Inc.
//      All Rights Reserved.
//
//      IDV, Inc.
//      http://www.idvinc.com
//
//  *** Release version 5.2.0 ***


///////////////////////////////////////////////////////////////////////
// Preprocessor

#pragma once
#include <speedtree/core/ExportBegin.h>
#include <cstdlib>
#include <cassert>
#include <speedtree/core/Array.h>
#include <speedtree/core/BlockPool.h>

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////
    // class CSet

    template <class T, bool bUseCustomAllocator = true>
    class ST_STORAGE_CLASS CSet
    {
    protected:
        class CNode;
        typedef CBlockPool<bUseCustomAllocator> CPool;
        typedef typename CBlockPool<bUseCustomAllocator>::CReference CNodeReference;

    public:
                                    CSet(size_t uiStartingPoolSize = 10);
                                    CSet(const CSet& cRight);
    virtual                         ~CSet(void);

            // standard functions
            void                    clear(void);
            bool                    empty(void) const;
            size_t                  size(void) const;
            CSet&                   operator = (const CSet& cRight);

            // iterator access
            class iterator
            {
            friend class CSet;
            public:
                                    iterator(CNodeReference pNode = NULL, const CPool* pPool = NULL);
                bool                operator == (const iterator& cRight) const;
                bool                operator != (const iterator& cRight) const;
                iterator&           operator ++ (void);
                iterator&           operator -- (void);
                
                const T&            operator * (void) const;
                T*                  operator -> (void) const;
                
            protected:
                CNode*              Ptr(void) const;

                CNodeReference      m_pNode;
                const CPool*        m_pPool;
            };
            typedef iterator const_iterator;

            iterator                begin(void) const;
            iterator                rbegin(void) const;
            iterator                end(void) const;
            iterator                find(const T& tData) const;
            iterator                insert(const T& tData);
            iterator                erase(iterator iterWhere);  
            iterator                lower(const T& tData) const;
            iterator                higher(const T& tData) const;
            void                    lower_and_higher(const T& tData, iterator& iterLower, iterator& iterHigher) const;

            // memory pool access
            void                    ResizePool(size_t uiSize);

    protected:
            void                    Rebalance(CNodeReference pCurrent);
            CNodeReference          Allocate(const T& tData, CNodeReference pParent = NULL);
            void                    Deallocate(CNodeReference pData);
            CNode*                  Ptr(CNodeReference pNode) const;

            class CNode
            {
            friend class CSet;
            friend class iterator;
            public:
                                CNode(const T& tData, CNodeReference pParent = NULL);
                T               m_tData;

            private:
                void            DeleteChildren(CSet* pSet);
                CNode&          operator = (const CNode& cR) { } // disallow assignment for const correctness
                CNodeReference  m_pLeft;
                CNodeReference  m_pRight;
                CNodeReference  m_pParent;
                size_t          m_uiLevel;
            };

            CNodeReference      m_pRoot;
            size_t              m_uiSize;
            CPool               m_cPool;
    };

    // include inline functions
    #include <speedtree/core/Set.inl>

} // end namespace SpeedTree


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif

#include <speedtree/core/ExportEnd.h>


