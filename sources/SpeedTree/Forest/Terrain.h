///////////////////////////////////////////////////////////////////////  
//  Terrain.h
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
#include "Forest/Forest.h"
#include "Core/ExportBegin.h"

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  Class CTerrainCell

    class ST_STORAGE_CLASS CTerrainCell : public CCell
    {
    public:
            friend class CTerrain;


                                        CTerrainCell( );
            virtual                     ~CTerrainCell( );

            // geometry
            ST_ILEX void                GetIndices(st_uint32& uiOffset, st_uint32& uiNumIndices, st_uint32& uiMinIndex, st_uint32& uiNumVertices) const;
            st_int32                    GetLod(void) const;

            // extents & culling
            void                        SetExtents(const CExtents& cExtents);
            st_float32                  GetDistanceFromCamera(void) const;

            // graphics-related
            void*                       GetVbo(void) const;
            void                        SetVbo(void* pVbo);

    private:
            st_uint32                   m_uiIndicesOffset;
            st_uint32                   m_uiNumIndices;
            st_uint32                   m_uiMinIndex;
            st_uint32                   m_uiNumVertices;
            st_float32                  m_fDistanceFromCamera;
            st_int32                    m_nLod;
            void*                       m_pVbo;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Type definitions

    typedef CArray<CTerrainCell*>           TTerrainCellArray;
    typedef CCellContainer<CTerrainCell>    TTerrainCellMap;
    typedef CArray<void*>                   TTerrainVboArray;


    ///////////////////////////////////////////////////////////////////////  
    //  Structure STerrainCullResults

    struct ST_STORAGE_CLASS STerrainCullResults
    {
            TTerrainCellArray           m_aCellsToUpdate;
            TTerrainVboArray            m_aFreedVbos;
            TTerrainCellArray           m_aVisibleCells;

            st_bool                     Reserve(st_int32 nMaxNumCells);
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CTerrain

    class ST_STORAGE_CLASS CTerrain
    {
    public:
                                        CTerrain( );
    virtual                             ~CTerrain( );

    virtual st_bool                     Init(st_int32 nNumLods, st_int32 nMaxTileRes, st_float32 fCellSize);
            st_bool                     IsEnabled(void) const;
            st_int32                    GetNumLods(void) const;
            st_int32                    GetMaxTileRes(void) const;
            st_float32                  GetCellSize(void) const;
            void                        SetHeightHints(st_float32 fGlobalLowPoint, st_float32 fGlobalHighPoint);

            // LOD
            void                        SetLodRange(st_float32 fNear, st_float32 fFar);
            void                        GetLodRange(st_float32& fNear, st_float32& fFar) const;
            const CArray<st_uint32>&    GetCompositeIndices(void) const;

    virtual void                        CullAndComputeLOD(const CView& cView, STerrainCullResults& sCullResults);

            // utility
            enum EHint
            {
                HINT_MAX_NUM_VISIBLE_CELLS
            };

            void                        SetHint(EHint eHint, st_int32 nHintValue);

    protected:
            void                        ComputeCellLods(const CView& cView, STerrainCullResults& sResults) const;
            void                        RemoveInactiveCells(TTerrainVboArray& aFreedVbos);
            void                        InitLodIndexStrips(void);

            // LOD
            st_int32                    m_nNumLods;                 // the number of discrete LOD stages
            st_int32                    m_nMaxTileRes;              // the highest LOD terrain tile will be a grid mesh of m_nMaxTileRes X m_nMaxTileRes
            st_float32                  m_fNearLodDistance;         // at this distance, the highest LOD cell is used
            st_float32                  m_fFarLodDistance;          // at this distance, the lowest LOD cell is used

            // cell data
            st_int32                    m_nUpdateIndex;             // incremented each time CullAndComputeLOD() is called; used to remove cells that can be deleted
            TTerrainCellMap             m_cTerrainCellMap;          // maps the (row,col) search key to a terrain cell
            st_float32                  m_fGlobalLowPoint;          //
            st_float32                  m_fGlobalHighPoint;         //

            // indices used to control LOD of a given terrain tile
            CArray<st_uint32>           m_aMasterLodIndexStrip;     // contains all of the strips for all LOD/edge combinations

            struct SIndexData
            {
                st_uint32               m_uiOffset;
                st_uint32               m_uiStripLength;
                st_uint32               m_uiMinIndex;
                st_uint32               m_uiNumVertices;
            };
            CArray<CArray<SIndexData> > m_aStripIndexDataByLod;

            // user-specified hints
            st_int32                    m_nHintMaxNumVisibleCells;
    };

    // include inline functions
    #include "Terrain.inl"

} // end namespace SpeedTree


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif

#include "Core/ExportEnd.h"

