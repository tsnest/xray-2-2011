///////////////////////////////////////////////////////////////////////  
//  Grass.h
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
#include <speedtree/Forest/Forest.h>
#include <speedtree/core/Wind.h>
#include <speedtree/core/ExportBegin.h>

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  Class CGrassCell

    class ST_STORAGE_CLASS CGrassCell : public CCell
    {
    public:
            friend class CGrass;


                                        CGrassCell( );
            virtual                     ~CGrassCell( );

            // extents & culling
            void                        SetExtents(const CExtents& cExtents);
            st_float32                  GetDistanceSquaredFromCamera(void) const;

            // graphics-related
            void*                       GetVbo(void) const;
            void                        SetVbo(void* pVbo);
            st_int32                    GetNumBlades(void) const;
            void                        SetNumBlades(st_int32 nNumBlades);

    private:
            void*                       m_pVbo;
            st_int32                    m_nNumBlades;
            st_float32                  m_fDistanceSquaredFromCamera;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Type definitions

    typedef CArray<CGrassCell*>         TGrassCellArray;
    typedef CCellContainer<CGrassCell>  TGrassCellMap;
    typedef CArray<void*>               TGrassVboArray;


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SGrassCullResults

    struct ST_STORAGE_CLASS SGrassCullResults
    {
    public:
            TGrassCellArray             m_aCellsToUpdate;
            TGrassVboArray              m_aFreedVbos;
            TGrassCellArray             m_aVisibleCells;

            st_bool                     Reserve(st_int32 nMaxNumCells);
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CTerrain

    class ST_STORAGE_CLASS CGrass
    {
    public:
                                        CGrass( );
    virtual                             ~CGrass( );

    virtual st_bool                     Init(const char* pTextureFilename, st_int32 nImageRows, st_int32 nImageCols, st_float32 fCellSize);
            st_bool                     IsEnabled(void) const;
            st_float32                  GetCellSize(void) const;
            void                        SetHeightHints(st_float32 fGlobalLowPoint, st_float32 fGlobalHighPoint);
            CWind&                      GetWind(void);
            const CWind&                GetWind(void) const;

            // LOD range
            void                        SetLodRange(st_float32 fStartFade, st_float32 fEndFade);
            void                        GetLodRange(st_float32& fStartFade, st_float32& fEndFade);

            // used during render loop
            void                        AdvanceWind(st_float32 fCurrentTime);
    virtual void                        Cull(const CView& cView, SGrassCullResults& sResults);

            // texture-related
            const char*                 GetTexture(void) const;
            st_int32                    GetNumSubImages(void) const;
            const st_float32*           GetBladeTexCoords(st_int32 nImageIndex, bool bMirror) const; // four values per blade (min_s, min_t, max_s, max_t)
            const st_uchar*             GetBladeTexCoordsUChar(st_int32 nImageIndex, bool bMirror) const; // four values per blade (min_s, min_t, max_s, max_t)

            // utility
            enum EHint
            {
                HINT_MAX_GRASS_BLADES_PER_CELL,
                HINT_MAX_NUM_VISIBLE_CELLS
            };

            void                        SetHint(EHint eHint, st_int32 nHintValue);
            st_int32                    GetHint(EHint eHint) const;

    protected:
            void                        RemoveInactiveCells(TGrassVboArray& aFreedVbos);
            void                        BuildTexCoordTable(void);

            // LOD
            st_float32                  m_fStartFade;           // distance from camera where grass blades begin to fade
            st_float32                  m_fEndFade;             // distance from camera where grass blades are completely faded

            // wind
            CWind                       m_cWind;

            // cell data
            st_int32                    m_nUpdateIndex;         // incremented each time CullAndComputeLOD() is called; used to remove cells that can be deleted
            TGrassCellMap               m_cGrassCellMap;        // maps the (row,col) search key to a grass cell
            st_float32                  m_fGlobalLowPoint;      // used to initialize a cull cell's extents before its populated; should be lowest point grass will populate
            st_float32                  m_fGlobalHighPoint;     // used to initialize a cull cell's extents before its populated; should be highest point grass will populate

            // texture data
            CFixedString                m_strTexture;           // texture filename holding the composite grass map
            st_int32                    m_nNumImageCols;        // number of columns in the composite grass map (assumes evenly sized sub-images)
            st_int32                    m_nNumImageRows;        // number of rows in the composite grass map (assumes evenly sized sub-images)
            CArray<st_float32>          m_aBladeTexCoords;      // the texture coordinates for each sub-image based on the cols & rows
            CArray<st_uchar>            m_aBladeTexCoordsUChar; // the texture coordinates for each sub-image based on the cols & rows

            // user-specified hints
            st_int32                    m_nHintMaxGrassBladesPerCell;
            st_int32                    m_nHintMaxNumVisibleCells;
    };

    // include inline functions
    #include <speedtree/forest/Grass.inl>

} // end namespace SpeedTree


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif

#include <speedtree/core/ExportEnd.h>

