///////////////////////////////////////////////////////////////////////  
//  Timer.h
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
#ifdef WIN32
    #ifndef _WINDOWS_
        #include <windows.h>
    #endif
#endif
#ifdef _XBOX
    #include <Xtl.h>
#endif
#ifdef __CELLOS_LV2__
    #include <sys/sys_time.h>
#elif defined(__GNUC__)
    #include <sys/time.h>
#endif
#include "Core/ExportBegin.h"


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  class CTimer
    /// CTimer is used like a stopwatch timer and can return the time in various units.

    class ST_STORAGE_CLASS CTimer
    {
    public:
                                CTimer(st_bool bStart = false);

            st_float32          Stop(void) const;
            void                Start(void);

            st_float32          GetSec(void) const;
            st_float32          GetMilliSec(void) const;
            st_float32          GetMicroSec(void) const;

    private:
    #if defined(WIN32) || defined(_XBOX)
            LARGE_INTEGER       m_liFreq;
            LARGE_INTEGER       m_liStart;
    mutable LARGE_INTEGER       m_liStop;
    #endif
    #ifdef __CELLOS_LV2__
            sys_time_sec_t      m_tStartSeconds;
            sys_time_nsec_t     m_tStartNanoseconds;
    mutable sys_time_sec_t      m_tStopSeconds;
    mutable sys_time_nsec_t     m_tStopNanoseconds;
    #elif defined(__GNUC__)
            timeval             m_tStart;
    mutable timeval             m_tStop;
    #endif
    };


    ///////////////////////////////////////////////////////////////////////  
    //  CTimer::CTimer
    /// CTimer constructor. If \a bStart is true, the timer will start by calling Start() internally.

    inline CTimer::CTimer(st_bool bStart)
    {
        #if defined(WIN32) || defined(_XBOX)
            (void) QueryPerformanceFrequency(&m_liFreq);
            m_liStart.QuadPart = 0;
            m_liStop.QuadPart = 0;
        #endif

        if (bStart)
            Start( );
    }


    ///////////////////////////////////////////////////////////////////////  
    //  CTimer::Start
    /// Starts the timer

    inline void CTimer::Start(void)
    {
        #if defined(WIN32) || defined(_XBOX)
            m_liStart.QuadPart = 0;
            QueryPerformanceCounter(&m_liStart);
        #endif
        #ifdef __CELLOS_LV2__
            sys_time_get_current_time(&m_tStartSeconds, &m_tStartNanoseconds);
        #elif defined(__GNUC__)
            gettimeofday(&m_tStart, NULL);
        #endif
    }


    ///////////////////////////////////////////////////////////////////////  
    //  CTimer::Stop
    /// Stops the timer and returns the milliseconds elapsed since it was started.

    inline st_float32 CTimer::Stop(void) const
    {
        #if defined(WIN32) || defined(_XBOX)
            QueryPerformanceCounter(&m_liStop);
        #endif 
        #ifdef __CELLOS_LV2__
            sys_time_get_current_time(&m_tStopSeconds, &m_tStopNanoseconds);
        #elif defined(__GNUC__)
            gettimeofday(&m_tStop, NULL);
        #endif

        return GetMilliSec( );
    }


    ///////////////////////////////////////////////////////////////////////  
    //  CTimer::GetMicroSec
    /// Returns the elapsed microseconds between the last Start() and Stop()

    inline st_float32 CTimer::GetMicroSec(void) const
    {
        return GetSec( ) * 1.0e6f;
    }


    ///////////////////////////////////////////////////////////////////////  
    //  CTimer::GetMilliSec
    /// Returns the elapsed milliseconds between the last Start() and Stop()

    inline st_float32 CTimer::GetMilliSec(void) const
    {
        return GetSec( ) * 1.0e3f;
    }


    ///////////////////////////////////////////////////////////////////////  
    //  CTimer::GetSec
    /// Returns the elapsed seconds between the last Start() and Stop()

    inline st_float32 CTimer::GetSec(void) const
    {
        #if defined(WIN32) || defined(_XBOX)
            return (m_liStop.QuadPart - m_liStart.QuadPart) / st_float32(m_liFreq.QuadPart);
        #endif
        #ifdef __CELLOS_LV2__
            return (m_tStopSeconds - m_tStartSeconds) + (m_tStopNanoseconds - m_tStartNanoseconds) / 1.0e9f;
        #elif defined(__GNUC__)
            return (m_tStop.tv_sec - m_tStart.tv_sec) + (m_tStop.tv_usec - m_tStart.tv_usec) / 1.0e6f;
        #endif
    }

} // end namespace SpeedTree

