///////////////////////////////////////////////////////////////////////  
//  Forest.h
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

//#define SPEEDTREE_HEAP_FRIENDLY_MODE

#pragma once
#include <speedtree/core/Core.h>
#include <speedtree/core/Map.h>
#include <speedtree/core/Set.h>
#include <speedtree/core/String.h>
#include <speedtree/core/Wind.h>
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
    #include <speedtree/core/FixedArray.h>
#endif
#include <speedtree/core/ExportBegin.h>

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  Constants

    const st_int32 c_nMaxNumShadowMaps = 4;


    ///////////////////////////////////////////////////////////////////////  
    //  Class CInstance

    class ST_STORAGE_CLASS CInstance
    {
    public:
            friend class CCullEngine;

                                        CInstance( );
                                        ~CInstance( );

            // geometry parameter settings
            //
            // these functions must be called on an instance before it is added
            // to the forest
            void                        SetPos(const Vec3& vPos);
            void                        SetScale(st_float32 fScale); 
            void                        SetRotation(st_float32 fRadians);

            // geometry parameter queries
            const Vec3&                 GetPos(void) const;
            st_float32                  GetScale(void) const;
            const st_float32*           GetPosAndScale(void) const;
            st_float32                  GetRotationAngle(void) const;   // in radians
            Vec3                        GetRotationVector(void) const;  // for optimized shader rotations

            // culling
            const Vec3&                 GetGeometricCenter(void) const;
            st_float32                  GetCullingRadius(void) const;
            st_bool                     IsCulled(void) const;

            // operators
            st_bool                     operator<(const CInstance& cIn) const;
            st_bool                     operator==(const CInstance& cIn) const;

            // internal use
            void                        ComputeCullParameters(const CTree* pBase);

    private:
            // note: scale must follow m_vPos for GetPosAndScale() to work
            Vec3                        m_vPos;                 // default to (0.0f, 0.0f, 0.0f)
            st_float32                  m_fScale;               // 1.0 = no scale

            Vec3                        m_vGeometricCenter;     // includes position offset
            st_float32                  m_fCullingRadius;
            st_int8                     m_anRotationVector[2];  // two floats of range [-1.0,1.0] are each compressed
                                                                // as two signed 1-byte values
            st_uint8                    m_nRotation;            // float of range [0.0,2pi] is compressed into a
                                                                // unsigned 1-byte value
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SCellKey

    struct SCellKey
    {
                            SCellKey(st_int32 nRow, st_int32 nCol) :
                                m_nRow(nRow),
                                m_nCol(nCol)
                            {
                            }

            st_bool         operator<(const SCellKey& sIn) const    { return (m_nRow == sIn.m_nRow) ? (m_nCol < sIn.m_nCol) : (m_nRow < sIn.m_nRow); }
            st_bool         operator!=(const SCellKey& sIn) const   { return (m_nRow != sIn.m_nRow || m_nCol != sIn.m_nCol); }

            void            Set(st_int32 nRow, st_int32 nCol)       { m_nRow = nRow; m_nCol = nCol; }

            st_int32        m_nRow;
            st_int32        m_nCol;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Type definitions

    typedef CArray<CInstance> TInstanceArray;
    typedef CMap<const CTree*, TInstanceArray> TBaseToInstanceMap;
    class CCellInstances;


    ///////////////////////////////////////////////////////////////////////  
    //  Class CCellBaseTreeItr
    //
    //  CCellBaseTreeItr abstracts accessing a series of base trees stored as either
    //  an array, if SPEEDTREE_HEAP_FRIENDLY_MODE is defined (less flexible, better
    //  use of heap), or in a CMap, if SPEEDTREE_HEAP_FRIENDLY_MODE is not defined
    //  (much more flexible, more heap use with frequent allocations).

    class ST_STORAGE_CLASS CCellBaseTreeItr
    {
    public:
                                                            CCellBaseTreeItr( );
                                                            CCellBaseTreeItr(const CCellInstances* pCell, const CTree* pBaseTree);
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
                                                            CCellBaseTreeItr(const CCellInstances* pCell, st_int32 nBaseTreeIndex);
#else
                                                            CCellBaseTreeItr(const CCellInstances* pCell, const TBaseToInstanceMap::const_iterator& iBaseTreeItr);
#endif
                                                            ~CCellBaseTreeItr( );

            const CTree*                                    TreePtr(void) const;
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            st_int32                                        TreeIndex(void) const;
#else
            const TBaseToInstanceMap::const_iterator&       TreeItr(void) const;
#endif

            CCellBaseTreeItr&                               operator++(void);

    private:
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            st_int32                                        m_nBaseTreeItr;
#else
            TBaseToInstanceMap::const_iterator              m_iBaseTreeItr;
#endif
            const CCellInstances*                           m_pCell;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CCellInstances

    class ST_STORAGE_CLASS CCellInstances
#ifndef SPEEDTREE_HEAP_FRIENDLY_MODE
        : public TBaseToInstanceMap
#endif
    {
    public:
            friend class CCellBaseTreeItr;

                                            CCellInstances( );
                                            ~CCellInstances( );

            CCellBaseTreeItr                FirstBaseTree(void) const;

            // base tree management
            st_int32                        NumBaseTrees(void) const;

            // after complete, don't use these
            const CTree*                    GetBaseTree(st_int32 nIndex) const;

            st_bool                         AddBaseTree(const CTree* pBaseTree);
            st_bool                         DeleteBaseTree(const CTree* pBaseTree);
            st_bool                         IsBaseTreeInCell(const CTree* pBaseTree) const;

            // instance management
            st_int32                        NumInstances(const CTree* pBaseTree) const;
            st_bool                         AddInstance(const CTree* pBaseTree, const CInstance& cInstance);
            const CInstance*                GetInstances(const CCellBaseTreeItr& iBaseTree, st_int32& nNumInstances) const;
            st_bool                         DeleteInstance(const CTree* pBaseTree, const CInstance& cInstance);
            st_bool                         ClearInstances(const CTree* pBaseTree);

#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            void                            UseExternalMemory(st_byte* pBlock, st_int32 nNumBaseTrees, st_int32 nNumInstances);

    private:
            st_int32                        GetBaseTreeIndex(const CTree* pBaseTree) const;

            // base trees
            st_int32                        m_nNumBaseTrees;
            CArray<const CTree*>            m_aBaseTrees;
            CArray<st_int32>                m_aInstanceIndices;

            // instances
            CArray<st_int32>                m_aNumInstances;
            CArray<CInstance>               m_aInstances;
            st_int32                        m_nTotalNumInstances;

            // billboard render block data
            CArray<st_int32>                m_aBillboardBlocks;

#else

            typedef CMap<const CTree*, st_int32> TBaseToRenderInfoMap;
            TBaseToRenderInfoMap            m_mBillboardBlocks;
#endif
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CCell

    class ST_STORAGE_CLASS CCell
    {
    public:
                                        CCell( );
            virtual                     ~CCell( );

            st_bool                     IsNew(void) const;

            // row/col attributes
            st_int32                    Col(void) const;
            st_int32                    Row(void) const;
            void                        SetRowCol(st_int32 nRow, st_int32 nCol);

            // extents
            const CExtents&             GetExtents(void) const;
            void                        InvalidateExtents(void);
    virtual void                        ComputeExtents(void) { }

            // culling
            const Vec3&                 GetCenter(void) const;
            st_float32                  GetCullingRadius(void) const;

            // update index
            st_int32                    GetUpdateIndex(void) const;
            void                        SetUpdateIndex(st_int32 nUpdateIndex);

    protected:
            st_int32                    m_nRow;
            st_int32                    m_nCol;

            st_int32                    m_nUpdateIndex;

            // extents & culling
            CExtents                    m_cExtents;
            Vec3                        m_vCenter;
            st_float32                  m_fCullRadius;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CTreeCell

    class ST_STORAGE_CLASS CTreeCell : public CCell
    {
    public:
                                        CTreeCell( );
            virtual                     ~CTreeCell( );

            // cell contents
            CCellInstances&             GetCellInstances(void);
            const CCellInstances&       GetCellInstances(void) const;

            // extents & culling
            void                        ComputeExtents(void);

            // have any instances been added or removed since the last call to Cull()?
            st_bool                     ContentsHaveChanged(void) const;
            void                        SetContentsChanged(st_bool bFlag);

            // billboard rendering support
            st_int32                    GetBillboardBlock(void) const;
            void                        SetBillboardBlock(st_int32 nBlock);
            st_bool                     HasBillboardBlock(void) const;
            void                        ClearBillboardBlock(void);

    private:
            CCellInstances              m_cCellInstances;
            st_bool                     m_bContentsChanged;
            st_int32                    m_nBillboardBlock;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SInstanceLod

    struct ST_STORAGE_CLASS SInstanceLod
    {
            st_bool                     operator<(const SInstanceLod& sRight) const
            {
                return m_fDistanceFromCameraSquared < sRight.m_fDistanceFromCameraSquared;
            }

            const CInstance*            m_pInstance;                    // the instance this object is storing LOD info for
            st_float32                  m_fDistanceFromCameraSquared;   // distance from camera (or LOD ref point)
            st_float32                  m_fLod;                         // [-1.0 to 1.0] value indicating LOD state
            st_float32                  m_afShaderLerps[GEOMETRY_TYPE_NUM_3D_TYPES]; // LOD hints to the shader system for smooth transitions
            SLodSnapshot                m_sLodSnapshot;                 // stores discrete LOD levels for each geometry type
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Type definitions, cont

    typedef CArray<CTree*>                                              TTreeArray;
    typedef CArray<const CTreeCell*>                                    TTreeCellArrayConst;
    typedef CArray<CTreeCell*>                                          TTreeCellArray;
    typedef CArray<SInstanceLod>                                        TInstanceLodArray;
    typedef CArray<const CTree*>                                        TTreeArrayConst;
    typedef CMap<const CTree*, TInstanceLodArray>                       TBaseToInstanceLodMap;


    ///////////////////////////////////////////////////////////////////////  
    //  Class CCellContainer

    template<class TCellType>
    class ST_STORAGE_CLASS CCellContainer : public CMap<SCellKey, TCellType>
    {
    public:
            // internal type definitions
            typedef typename CCellContainer::iterator TCellIterator;
            typedef typename CCellContainer::const_iterator TCellConstIterator;

                                        CCellContainer( );
            virtual                     ~CCellContainer( );

            TCellType*                  GetCellPtrByRowCol_Add(st_int32 nRow, st_int32 nCol);
            const TCellType*            GetCellPtrByRowCol(st_int32 nRow, st_int32 nCol) const;
            TCellType*                  GetCellPtrByPos_Add(const Vec3& vPos);
            const TCellType*            GetCellPtrByPos(const Vec3& vPos) const;

            // internal
            TCellIterator               GetCellItrByRowCol_Add(st_int32 nRow, st_int32 nCol);
            TCellConstIterator          GetCellItrByRowCol(st_int32 nRow, st_int32 nCol) const;
            TCellIterator               GetCellItrByPos_Add(const Vec3& vPos);
            TCellConstIterator          GetCellItrByPos(const Vec3& vPos) const;
            TCellIterator               Erase(TCellIterator iCell);

            // cell size
            st_float32                  GetCellSize(void) const;
            void                        SetCellSize(st_float32 fCellSize);

#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            void                        ManuallyCreateCell(st_int32 nRow,
                                                           st_int32 nCol,
                                                           st_byte* pBlock,
                                                           st_int32 nNumBaseTrees,
                                                           st_int32 nNumInstances);
            void                        SetMaxNumCells(st_int32 nMaxNumCells);
#endif

    private:
            st_float32                  m_fCellSize;
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            st_int32                    m_nMaxNumCells;
#endif
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Type definitions, cont

    typedef CCellContainer<CTreeCell>   TTreeCellMap;


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SForestCullResults
    //
    //  IMPORTANT NOTE: The contents of SForestCullResults, as retrieved from
    //  CForest::CullAndComputeLOD(), should not be referenced after a forest
    //  population function (AddInstances, DeleteInstances, ChangeInstances) 
    //  has been called.  Make another call to CullAndComputeLOD as some
    //  of the cells pointed to by members of SForestCullResults may be dangling.

    struct ST_STORAGE_CLASS SForestCullResults
    {
            friend class CForest;

                                            SForestCullResults( );
    virtual                                 ~SForestCullResults( );

            // general results
            const TTreeArray*               m_pBaseTrees;                       // pointer to array holding all base trees in CForest object
            TTreeCellArrayConst             m_aVisibleCells;                    // array holding every cell visible in the frustum used by the cull operation

            // billboard-related
            TTreeCellArray                  m_aNewVisibleCells;                 // contains newly created, newly visible, or modified cells; wrt last cull call
            TTreeCellArray                  m_aPreviousVisibleCells;            // contains cells that were in frustum last frame, but out on current frame
            st_bool                         m_bFrustumContainsModifiedCells;    // flag to update billboard contents even if cells weren't added/remove from frustum

            //  In SPEEDTREE_HEAP_FRIENDLY_MODE, it is advisable to use this function before rendering 
            //  begins.  This will prevent dynamic allocations from occurring during the render loop.
    virtual st_bool                         Reserve(const TTreeArray& aBaseTrees,
                                                    st_int32 nMaxNumBaseTrees, 
                                                    st_int32 nMaxNumVisibleCells,
                                                    st_int32 nMaxVisibleInstancesPerBase,
                                                    st_bool bHorzBillboards);

            // parameter nBaseTreeIndex indexes base trees as stored in m_pBaseTrees above
            const TInstanceLodArray*        VisibleInstances(st_int32 nBaseTreeIndex) const;

    private:
            // access m_a3dInstances/m_m3dInstances through VisibleInstances(), which abstracts
            // the different data structures
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            CArray<TInstanceLodArray>       m_a3dInstances;                     // essentially a 2D array holding base trees, then instances per base tree
#else
            TBaseToInstanceLodMap           m_m3dInstances;                     // a map of base trees mapped to arrays of instances
#endif
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Enumeration EFrustumPlanes

    enum EFrustumPlanes
    {
        NEAR_PLANE, FAR_PLANE, RIGHT_PLANE, LEFT_PLANE, BOTTOM_PLANE, TOP_PLANE, NUM_PLANES
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CView

    class ST_STORAGE_CLASS CView
    {
    public:

                                        CView( );

            // returns true if the values passed in our different from the internal values, false otherwise
            st_bool                     Set(const Vec3& vCameraPos,
                                            const Mat4x4& mProjection,
                                            const Mat4x4& mModelview,
                                            st_float32 fNearClip,
                                            st_float32 fFarClip);
            void                        SetLodRefPoint(const Vec3& vLodRefPoint);

            // get parameters set directly
            const Vec3&                 GetCameraPos(void) const;
            const Vec3&                 GetLodRefPoint(void) const;
            const Mat4x4&               GetProjection(void) const;
            const Mat4x4&               GetModelview(void) const;
            const Mat4x4&               GetModelviewNoTranslate(void) const;
            st_float32                  GetNearClip(void) const;
            st_float32                  GetFarClip(void) const;

            // get derived parameters
            const Vec3&                 GetCameraDir(void) const;
            const Mat4x4&               GetComposite(void) const;
            const Mat4x4&               GetCompositeNoTranslate(void) const;
            st_float32                  GetCameraAzimuth(void) const;
            st_float32                  GetCameraPitch(void) const;
            const Vec3*                 GetFrustumPoints(void) const;
            const Vec4*                 GetFrustumPlanes(void) const;
            const CExtents&             GetFrustumExtents(void) const;

            // get derived-by-request parameters
            const Mat4x4&               GetCameraFacingMatrix(void) const;

            // horizontal billboard support
            void                        SetHorzBillboardFadeAngles(st_float32 fStart, st_float32 fEnd); // in radians
            void                        GetHorzBillboardFadeAngles(st_float32& fStart, st_float32& fEnd) const; // in radians
            st_float32                  GetHorzBillboardFadeValue(void) const; // 0.0 = horz bbs are transparent, 1.0 = horz bb's opaque

    private:
            void                        ComputeCameraFacingMatrix(void);
            void                        ComputeFrustumValues(void);
            void                        ExtractFrustumPlanes(void);

            // parameters are set directly
            Vec3                        m_vCameraPos;
            Vec3                        m_vLodRefPoint;
            st_bool                     m_bLodRefPointSet;
            Mat4x4                      m_mProjection;
            Mat4x4                      m_mModelview;
            st_float32                  m_fNearClip;
            st_float32                  m_fFarClip;

            // derived
            Vec3                        m_vCameraDir;
            Mat4x4                      m_mComposite;
            Mat4x4                      m_mModelviewNoTranslate;
            Mat4x4                      m_mCompositeNoTranslate;
            st_float32                  m_fCameraAzimuth;
            st_float32                  m_fCameraPitch;
            Vec3                        m_avFrustumPoints[8];
            Vec4                        m_avFrustumPlanes[NUM_PLANES];
            CExtents                    m_cFrustumExtents;

            // derived on request
            Mat4x4                      m_mCameraFacingMatrix;

            // horizontal billboards
            st_float32                  m_fHorzFadeStartAngle;
            st_float32                  m_fHorzFadeEndAngle;
            st_float32                  m_fHorzFadeValue;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CForest

    class ST_STORAGE_CLASS CForest
    {
    public:
                                        CForest( );
    virtual                             ~CForest( );

            // base tree management
            st_bool                     RegisterTree(CTree* pTree);
            st_bool                     UnregisterTree(const CTree* pTree);
#ifndef SPEEDTREE_HEAP_FRIENDLY_MODE
    virtual st_bool                     ReplaceTree(const CTree* pOldTree, CTree* pNewTree);
#endif
    virtual CTree*                      AllocateTree(void) const;
            st_bool                     TreeIsRegistered(const CTree* pTree) const;
            const TTreeArray&           GetBaseTrees(void) const;

            // tree instance management
            st_bool                     ClearInstances(const CTree* pInstanceOf = NULL, st_bool bDeleteCells = true); // when null, all are cleared
            st_int32                    NumInstances(const CTree* pInstanceOf = NULL) const;
            st_bool                     GetInstances(const CTree* pInstanceOf, TInstanceArray& aInstances) const; // an expensive call
#ifndef SPEEDTREE_HEAP_FRIENDLY_MODE
            st_bool                     AddInstances(const CTree* pInstanceOf, const CInstance* pInstances, st_int32 nNumInstances);
            st_bool                     DeleteInstances(const CTree* pInstanceOf, const CInstance* pInstances, st_int32 nNumInstances, st_bool bDeleteCellIfEmpty = true);
            st_bool                     ChangeInstance(const CTree* pTree, const CInstance& cOrigInstance, const CInstance& cNewInstance);
#else
            // while SPEEDTREE_HEAP_FRIENDLY_MODE is much friendlier regarding heap fragmentation, the trade off is
            // flexibility; CForest populations must be specified upfront and cannot change throughout the CForest
            // object's lifetime; always use PopulateAtOnce() to populate a CForest while in this mode
    private:
            st_bool                     AddInstances(const CTree* pInstanceOf, const CInstance* pInstances, st_int32 nNumInstances);
#endif

    public:
            struct SCompletePopulation
            {
                                        SCompletePopulation(st_int32 nNumBaseTrees)
                                        {
                                            m_aBaseTrees.resize(nNumBaseTrees);
                                            m_aaInstances.resize(nNumBaseTrees);
                                        }

                TTreeArray              m_aBaseTrees;
                CArray<TInstanceArray>  m_aaInstances;
            };
            st_bool                     PopulateAtOnce(const SCompletePopulation& sPopulation);

            // billboards
            st_bool                     InitBillboardSystem(void);

            // extents & limits
            st_int32                    UpdateTreeCellExtents(void);
            void                        ComputeForestExtents(CExtents& cExtents) const;
            void                        EndInitialPopulation(void);

            // collision
            st_bool                     CollisionAdjust(Vec3& vPoint, const SForestCullResults& sResults);

            // culling
    virtual void                        CullAndComputeLOD(const CView& cView, SForestCullResults& sResults, st_bool bSort = false);
            void                        SetCullCellSize(st_float32 fCellSize);
            void                        FrameEnd(void);
            st_int32                    GetUpdateIndex(void) const;

            // wind management
            void                        EnableWind(st_bool bFlag);
            st_bool                     IsWindEnabled(void) const;
            void                        SetGlobalWindStrength(st_float32 fStrength);
            st_float32                  GetGlobalWindStrength(void) const;
            void                        SetGlobalWindDirection(const Vec3& vDir);
            const Vec3&                 GetGlobalWindDirection(void) const;
            void                        AdvanceGlobalWind(void);
            void                        SetWindLeader(const CWind* pLeader);
            const CWind&                GetWindLeader(void) const;
            void                        SetGlobalTime(st_float32 fSeconds);
            st_float32                  GetGlobalTime(void) const;

            // shadows & lighting support
            void                        SetLightDir(const Vec3& vLightDir);
            const Vec3&                 GetLightDir(void) const;
            st_bool                     LightDirChanged(void) const;
            st_bool                     ComputeLightView(const Vec3& vLightDir, const Vec3 avMainFrustum[8], st_int32 nMapIndex, CView& sLightView, st_float32 fRearExtension) const;
            void                        SetCascadedShadowMapDistances(const st_float32 afSplits[c_nMaxNumShadowMaps], st_float32 fFarClip); // each entry marks the end distance of its respective map
            const st_float32*           GetCascadedShadowMapDistances(void) const;
            void                        SetShadowFadePercentage(st_float32 fFade);
            st_float32                  GetShadowFadePercentage(void) const;

            // statistics
            struct SPopulationStats
            {
                                                SPopulationStats( );

                st_int32                        m_nNumCells;
                st_int32                        m_nNumBaseTrees;
                st_int32                        m_nNumInstances;
                st_float32                      m_fAverageNumInstancesPerBase;
                st_int32                        m_nMaxNumBillboardsPerCell; // same as m_nMaxNumInstancesPerCell
                st_int32                        m_nMaxNumInstancesPerCell;  // same as m_nMaxNumBillboardsPerCell
                CMap<const CTree*, st_int32>    m_mMaxNumInstancesPerCellPerBase;
                st_float32                      m_fAverageInstancesPerCell;
                st_int32                        m_nMaxNumBillboardImages;
            };
            void                        GetPopulationStats(SPopulationStats& sStats);
            
            // utility
            enum EHint
            {
                HINT_MAX_NUM_VISIBLE_CELLS,
                HINT_MAX_NUM_COLLISION_TREES
            };

            st_int32                    GetHint(EHint eHint) const;
            void                        SetHint(EHint eHint, st_int32 nHintValue);

            // internal
            const TTreeCellMap&         GetTreeCellMap(void) const;

    protected:
            void                        RemoveDeletedCells(SForestCullResults& sResults);
            void                        DetermineCellAges(SForestCullResults& sResults) const;
            TTreeCellMap::iterator      DeleteTreeCell(const TTreeCellMap::iterator& iCell);

            // base trees
            TTreeArray                  m_aBaseTrees;
            st_bool                     m_bBaseTreesChanged;

            // instance storage & culling
            TTreeCellMap                m_cTreeCellMap;
            st_float32                  m_fLargestInstanceCullRadius;
            st_int32                    m_nUpdateIndex;
            st_bool                     m_bCellDeletedSinceLastCull;

            // billboards
            CArray<st_int32>            m_aAvailableBillboardVboBlocks;

            // wind
            st_bool                     m_bWindEnabled;
            st_float32                  m_fGlobalWindStrength;
            Vec3                        m_vWindDir;
            st_float32                  m_fGlobalTime;
            CWind                       m_cWindLeader;

            // shadows & lighting support
            Vec3                        m_vLightDir;
            st_bool                     m_bLightDirChanged;
            st_float32                  m_afCascadedShadowMapSplits[c_nMaxNumShadowMaps + 1];
            st_float32                  m_fShadowFadePercentage;

            // user-specified hints
            st_int32                    m_nHintMaxNumVisibleCells;
            st_int32                    m_nHintMaxNumCollisionTrees;

#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            st_byte*                    m_pCellMemPool;
#endif
    };

    // include inline functions
    #include <speedtree/forest/Cells.inl>
    #include <speedtree/forest/Forest.inl>
    #include <speedtree/forest/CullResults.inl>
    #include <speedtree/forest/CellContainer.inl>
    #include <speedtree/forest/Instance.inl>

} // end namespace SpeedTree


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif

#include <speedtree/core/ExportEnd.h>