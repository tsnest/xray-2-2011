///////////////////////////////////////////////////////////////////////  
//  Random.h
//
//  This class uses the Mersenne Twister algorithm, developed in 1997 
//  by Makoto Matsumoto and Takuji Nishimura. It provides for fast 
//  generation of very high quality random numbers.
//
//  reference:
//  M. Matsumoto and T. Nishimura, "Mersenne twister: A 623-dimensionally
//      equidistributed uniform pseudorandom number generator," ACM Trans. 
//      on Modeling and Computer Simulations, 1998.
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


/////////////////////////////////////////////////////////////////////
// Preprocessor/Includes

#pragma once
#include <speedtree/core/ExportBegin.h>
#include <speedtree/core/Types.h>
#include <cmath>
#ifdef __GNUC__
    #include <stdlib.h>
#else
    #include <memory.h>
#endif

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    /////////////////////////////////////////////////////////////////////
    // class CRandom

    class ST_STORAGE_CLASS CRandom
    {
    public:
                            CRandom( );
                            CRandom(st_uint32 uiSeed);
                            CRandom(const CRandom& cCopy);
                            ~CRandom( );
            
            // seeding
            void            Seed(st_uint32 uiSeed);

            // uniform random numbers
            st_int32        GetInteger(st_int32 nLow, st_int32 nHigh);      // returns [nLow, nHigh-1]
            st_float32      GetFloat(st_float32 fLow, st_float32 fHigh);
            st_float64      GetDouble(st_float64 fLow, st_float64 fHigh);

            // gaussian (normal) random numbers with mean = 0 and stddev = 1
            st_float32      GetGaussianFloat(void);
            st_float64      GetGaussianDouble(void);

    protected:
            void            Reload(void);
            st_uint32       GetRawInteger(void);
            st_uint32       Twist(st_uint32 uiPrime, st_uint32 uiInput0, st_uint32 uiInput1) const;

    protected:
            enum { SIZE = 624, PERIOD = 397 };
            st_uint32       m_uiSeed;
            st_uint32       m_auiTable[SIZE + 1];
            st_uint32*      m_pNext; 
            st_uint32       m_nCount;
    };

    // include inlined functions
    #include <speedtree/core/Random.inl>

} // end namespace SpeedTree


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif

#include <speedtree/core/ExportEnd.h>


