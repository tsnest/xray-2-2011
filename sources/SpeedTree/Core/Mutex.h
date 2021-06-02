///////////////////////////////////////////////////////////////////////  
//  Mutex.h
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


///////////////////////////////////////////////////////////////////////  
//  Preprocessor

#pragma once
#include "Core/Core.h"

#ifdef _XBOX
    #include <xtl.h>
#elif defined(_WIN32)
    #include <windows.h>
#elif defined(__CELLOS_LV2__)
    #include <sys/synchronization.h>
#elif defined(_OPENMP)
    #include <omp.h>
#else
    #error SpeedTree CMutex not yet defined for this platform
#endif

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  Class CMutex

    class ST_STORAGE_CLASS CMutex
    {
    public:
                                CMutex( );
                                ~CMutex( );

            void                Lock(void);
            void                Unlock(void);
    private:
#ifdef _WIN32 // Windows or Xbox 360
            CRITICAL_SECTION    m_sLock;
#elif defined(__CELLOS_LV2__)
            sys_lwmutex_t       m_sLock;
#elif defined(_OPENMP)
            omp_lock_t          m_sLock;
#endif
    };


    ///////////////////////////////////////////////////////////////////////  
    //  CMutex::CMutex
    
    inline CMutex::CMutex( )
    {
    #ifdef _WIN32 // Windows or Xbox 360
        InitializeCriticalSection(&m_sLock);
    #elif defined(__CELLOS_LV2__)
        sys_lwmutex_attribute_t sAttribute;
        sys_lwmutex_attribute_initialize(sAttribute);

        if (sys_lwmutex_create(&m_sLock, &sAttribute) != CELL_OK)
            CCore::SetError("sys_lwmutex_create() failed in CMutex constructor");
    #elif defined(_OPENMP)
        omp_init_lock(&m_sLock);
    #endif
    }


    ///////////////////////////////////////////////////////////////////////  
    //  CMutex::~CMutex
    
    inline CMutex::~CMutex( )
    {
    #ifdef _WIN32 // Windows or Xbox 360
        DeleteCriticalSection(&m_sLock);
    #elif defined(__CELLOS_LV2__)
        if (sys_lwmutex_destroy(&m_sLock) != CELL_OK)
            CCore::SetError("sys_lwmutex_destroy() failed in CMutex destructor");
    #elif defined(_OPENMP)
        omp_destroy_lock(&m_sLock);
    #endif
    }


    ///////////////////////////////////////////////////////////////////////  
    //  CMutex::Lock

    inline void CMutex::Lock(void)
    {
    #ifdef _WIN32 // Windows or Xbox 360
        EnterCriticalSection(&m_sLock);
    #elif defined(__CELLOS_LV2__)
        if (sys_lwmutex_lock(&m_sLock, SYS_NO_TIMEOUT) != CELL_OK)
            CCore::SetError("sys_lwmutex_lock() failed in CMutex::Lock()");
    #elif defined(_OPENMP)
        omp_set_lock(&m_sLock);
    #endif
    }


    ///////////////////////////////////////////////////////////////////////  
    //  CMutex::Unlock

    inline void CMutex::Unlock(void)
    {
    #ifdef _WIN32 // Windows or Xbox 360
        LeaveCriticalSection(&m_sLock);
    #elif defined(__CELLOS_LV2__)
        if (sys_lwmutex_unlock(&m_sLock) != CELL_OK)
            CCore::SetError("sys_lwmutex_lock() failed in CMutex::Lock()");
    #elif defined(_OPENMP)
        omp_unset_lock(&m_sLock);
    #endif
    }

} // end namespace SpeedTree


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif
