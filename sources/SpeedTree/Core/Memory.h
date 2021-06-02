///////////////////////////////////////////////////////////////////////  
//  Memory.h
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
//  Preprocessor

#pragma once

#include "Allocator.h"
#include <new>

#ifdef _XBOX
    #include <malloc.h>
#endif
#ifdef __GNUC__
    #include <stdlib.h>
#endif

#if defined(_WIN32) || defined(_XBOX)
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////
//  namespace SpeedTree

namespace SpeedTree
{
    #ifdef SPEEDTREE_ALLOCATORS_DECLARED
        ST_STORAGE_CLASS SpeedTree::CAllocator *g_pAllocator = NULL;
        ST_STORAGE_CLASS size_t g_siHeapMemoryUsed = 0;
        ST_STORAGE_CLASS size_t g_siNumHeapAllocs = 0;
    #else // SPEEDTREE_ALLOCATORS_DECLARED
        #ifdef SPEEDTREE_USE_SDK_AS_DLLS
            __declspec(dllimport) SpeedTree::CAllocator* g_pAllocator;
            __declspec(dllimport) size_t g_siHeapMemoryUsed;
            __declspec(dllimport) size_t g_siNumHeapAllocs;
        #else
            extern SpeedTree::CAllocator* g_pAllocator;
            extern size_t g_siHeapMemoryUsed;
            extern size_t g_siNumHeapAllocs;
        #endif
    #endif // SPEEDTREE_ALLOCATORS_DECLARED


    ///////////////////////////////////////////////////////////////////////
    //  struct SHeapHandle
    //
    //  Necessary overhead for overriding array allocation.

    struct SHeapHandle
    {
        size_t  m_siNumElements;
    };


    ///////////////////////////////////////////////////////////////////////
    //  st_new
    //
    //  Alternate path for C++ operator::new; uses placement new to request
    //  allocation through SpeedTree's allocator interface.
    //
    //  Example usage:
    //    CObject* pObj = st_new(CObject, "my object")(constr_param1, constr_param2, etc)

    #define st_new(TYPE, pDescription) new (st_allocate<TYPE>(pDescription)) TYPE


    ///////////////////////////////////////////////////////////////////////
    //  st_allocate
    //
    //  Example usage:
    //    CObject* pObj = new (st_allocate<CObject>("my object")) CObject(constr_param1, constr_param2, constr_param3);

    template <typename TYPE> inline TYPE* st_allocate(const char* pDescription)
    {
        size_t siTotalSize = sizeof(TYPE);

        char* pRawBlock = (char*) (SpeedTree::g_pAllocator ? SpeedTree::g_pAllocator->Alloc(siTotalSize) : 0);//malloc(siTotalSize));
        if (pRawBlock)
        {
            TYPE* pBlock = (TYPE*) pRawBlock;

            // track all SpeedTree memory allocation
            SpeedTree::g_siHeapMemoryUsed += siTotalSize;
            SpeedTree::g_siNumHeapAllocs++;
    #ifdef SPEEDTREE_MEMORY_STATS
            SpeedTree::CAllocator::TrackAlloc(pDescription ? pDescription : "Unknown", pRawBlock, siTotalSize);
    #else
            pDescription = pDescription; // avoid unreferenced parameter warning
    #endif

            return pBlock;
        }

        return NULL;
    }


    ///////////////////////////////////////////////////////////////////////
    //  st_new_array (equivalent of C++ operator::new[])

    template <typename TYPE> inline TYPE* st_new_array(size_t siNumElements, const char* pDescription)
    {
        size_t siTotalSize = sizeof(SpeedTree::SHeapHandle) + siNumElements * sizeof(TYPE);

        char* pRawBlock = (char*) (SpeedTree::g_pAllocator ? SpeedTree::g_pAllocator->Alloc(siTotalSize) : 0);//malloc(siTotalSize));
        if (pRawBlock)
        {
            // get the main part of the data, after the heap handle
            TYPE* pBlock = (TYPE*) (pRawBlock + sizeof(SpeedTree::SHeapHandle));

            // record the number of elements
            *((size_t*) pRawBlock) = siNumElements;

            // manually call the constructors using placement new
            TYPE* pElements = pBlock;
            for (size_t i = 0; i < siNumElements; ++i)
                (void) new (static_cast<void*>(pElements + i)) TYPE;

            // track all SpeedTree memory allocation
            SpeedTree::g_siHeapMemoryUsed += siTotalSize;
            SpeedTree::g_siNumHeapAllocs++;

            #ifdef SPEEDTREE_MEMORY_STATS
                SpeedTree::CAllocator::TrackAlloc(pDescription ? pDescription : "Unknown", pRawBlock, siTotalSize);
            #else
                pDescription = pDescription; // avoid unreferenced parameter warning
            #endif

            return pBlock;
        }
        
        return NULL;
    }


    ///////////////////////////////////////////////////////////////////////
    //  st_delete (equivalent of C++ delete)

    template <typename TYPE> inline void st_delete(TYPE*& pBlock)
    {
        #ifdef SPEEDTREE_MEMORY_STATS
            if (pBlock)
                SpeedTree::CAllocator::TrackFree(pBlock, pBlock ? sizeof(TYPE) : 0);
        #endif

        if (pBlock)
        {
            pBlock->~TYPE( );
            if (SpeedTree::g_pAllocator)
                SpeedTree::g_pAllocator->Free(pBlock);
//            else
//                free(pBlock);
            pBlock = NULL;

            // track all SpeedTree memory allocation
            SpeedTree::g_siHeapMemoryUsed -= sizeof(TYPE);
        }
    }


    ///////////////////////////////////////////////////////////////////////
    //  st_delete_array (equivalent of C++ delete[])

    template <typename TYPE> inline void st_delete_array(TYPE*& pRawBlock)
    {
        if (pRawBlock)
        {
            // extract the array size
            SpeedTree::SHeapHandle* pHeapHandle = (SpeedTree::SHeapHandle*) ( ((char*) pRawBlock) - sizeof(SpeedTree::SHeapHandle) );

            if (pHeapHandle)
            {
                // point to the elements
                TYPE* pElements = pRawBlock; 

                if (pElements)
                {
                    // track all SpeedTree memory allocation
                    SpeedTree::g_siHeapMemoryUsed -= sizeof(SpeedTree::SHeapHandle) + pHeapHandle->m_siNumElements * sizeof(TYPE);

                    for (size_t i = 0; i < pHeapHandle->m_siNumElements; ++i)
                        (pElements + i)->~TYPE( );

                    #ifdef SPEEDTREE_MEMORY_STATS
                        SpeedTree::CAllocator::TrackFree(pHeapHandle, pHeapHandle->m_siNumElements * sizeof(TYPE) + sizeof(SpeedTree::SHeapHandle));
                    #endif

                    if (SpeedTree::g_pAllocator)
                        SpeedTree::g_pAllocator->Free(pHeapHandle);
//                    else
//                        free(pHeapHandle);
                    pRawBlock = NULL;
                }
            }
        }
    }

} // end namespace SpeedTree


#if defined(_WIN32) || defined(_XBOX)
    #pragma pack(pop)
#endif

