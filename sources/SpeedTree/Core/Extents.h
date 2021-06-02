///////////////////////////////////////////////////////////////////////  
//  Extents.h
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


/////////////////////////////////////////////////////////////////////
// Preprocessor

#pragma once
#include <speedtree/core/ExportBegin.h>
#include <speedtree/core/CoordSys.h>
#include <speedtree/core/Vector.h>
#include <cfloat>
#include <cassert>

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    /////////////////////////////////////////////////////////////////////
    // class CExtents
    //
    // Represents an axis-aligned bounding box

    class ST_STORAGE_CLASS CExtents
    {
    public:
                            CExtents( );
                            CExtents(const st_float32 afExtents[6]); // [0-2] = min(x,y,z), [3-5] = max(x,y,z)
                            CExtents(const Vec3& cMin, const Vec3& cMax);
                            ~CExtents( );                                           

            void            Reset(void);
            void            SetToZeros(void);
            void            Order(void);
            bool            Valid(void) const;

            void            ExpandAround(const st_float32 afPoint[3]);
            void            ExpandAround(const Vec3& vPoint);
            void            ExpandAround(const Vec3& vPoint, st_float32 fRadius);
            void            ExpandAround(const CExtents& cOther);
            void            Scale(st_float32 fScale);
            void            Translate(const Vec3& vTranslation);
            void            Rotate(st_float32 fRadians);    // around 'up' axis

            st_float32      ComputeRadiusFromCenter3D(void);
            st_float32      ComputeRadiusFromCenter2D(void);

            const Vec3&     Min(void) const;
            const Vec3&     Max(void) const;
            st_float32      Midpoint(st_uint32 uiAxis) const;
            Vec3            GetCenter(void) const;
            Vec3            GetDiagonal(void) const;

                            operator st_float32*(void);
                            operator const st_float32*(void) const;

    private:
            Vec3            m_cMin;
            Vec3            m_cMax;
    };

    // include inline functions
    #include <speedtree/core/Extents.inl>

} // end namespace SpeedTree


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif

#include <speedtree/core/ExportEnd.h>



