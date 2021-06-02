///////////////////////////////////////////////////////////////////////  
//  MiscRI.inl
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


////////////////////////////////////////////////////////////
// CForestRI_Interpolate

template <class T> inline T CForestRI_Interpolate(const T& tStart, const T& tEnd, float fPercent)
{
    return static_cast<T>((tStart + (tEnd - tStart) * fPercent));
}


////////////////////////////////////////////////////////////
// SMaterialRI::Copy

template<class TTexturePolicy>
inline void SMaterialRI<TTexturePolicy>::Copy(const SMaterial& sSource)
{
    *((SMaterial*) this) = sSource;
}


////////////////////////////////////////////////////////////
// CLodRenderInfo::SRef::SRef

inline CLodRenderInfo::SRef::SRef( ) :
    m_nMaterialId(-1),
    m_uiOffset(0),
    m_uiLength(0),
    m_uiMinIndex(0),
    m_uiNumVertices(0)
{
}


////////////////////////////////////////////////////////////
// CLodRenderInfo::CLodRenderInfo

inline CLodRenderInfo::CLodRenderInfo( ) :
    m_nNumLods(0),
    m_nMaxMaterialId(0),
    m_pMemPool(NULL)
{
}


////////////////////////////////////////////////////////////
// CLodRenderInfo::~CLodRenderInfo

inline CLodRenderInfo::~CLodRenderInfo( )
{
    st_delete_array<st_byte>(m_pMemPool);
}


////////////////////////////////////////////////////////////
// CLodRenderInfo::InitIndexed

inline void CLodRenderInfo::InitIndexed(const SIndexedTriangles* pLods, st_int32 nNumLods)
{
    // determine the highest material id used by all LODs
    m_nMaxMaterialId = 0;
    for (st_int32 nLod = 0; nLod < nNumLods; ++nLod)
    {
        const SIndexedTriangles* pLod = pLods + nLod;
        for (st_int32 nMaterialGroup = 0; nMaterialGroup < pLod->m_nNumMaterialGroups; ++nMaterialGroup)
        {
            const SDrawCallInfo* pDrawCallInfo = pLod->m_pDrawCallInfo + nMaterialGroup;
            m_nMaxMaterialId = st_max(m_nMaxMaterialId, pDrawCallInfo->m_nMaterialIndex);
        }
    }

    m_nMaxMaterialId++;
    m_nNumLods = nNumLods;

    AllocMemPool( );
}


////////////////////////////////////////////////////////////
// CLodRenderInfo::InitLeafCards

inline void CLodRenderInfo::InitLeafCards(const SLeafCards* pLods, st_int32 nNumLods)
{
    // determine the highest material id used by all LODs
    m_nMaxMaterialId = 0;
    for (st_int32 nLod = 0; nLod < nNumLods; ++nLod)
    {
        const SLeafCards* pLod = pLods + nLod;
        for (st_int32 nMaterialGroup = 0; nMaterialGroup < pLod->m_nNumMaterialGroups; ++nMaterialGroup)
        {
            const SDrawCallInfo* pDrawCallInfo = pLod->m_pDrawCallInfo + nMaterialGroup;
            m_nMaxMaterialId = st_max(m_nMaxMaterialId, pDrawCallInfo->m_nMaterialIndex);
        }
    }

    m_nMaxMaterialId++;
    m_nNumLods = nNumLods;

    AllocMemPool( );
}


////////////////////////////////////////////////////////////
// CLodRenderInfo::AddRef

inline void CLodRenderInfo::AddRef(st_int32 nLod, const SRef& sRef)
{
    assert(nLod < m_nNumLods);
    assert(sRef.m_nMaterialId < m_nMaxMaterialId);

    m_aRefs[sRef.m_nMaterialId * m_nNumLods + nLod] = sRef;

    m_aLodsWithGeometry[nLod + 1] = true; // +1 to include -1 as a valid LOD value

    m_aMaterialsUsed.insert_sorted_unique(sRef.m_nMaterialId);
    assert(m_aMaterialsUsed.size( ) <= size_t(m_nMaxMaterialId));
}


////////////////////////////////////////////////////////////
// CLodRenderInfo::IsActive

inline const CLodRenderInfo::SRef* CLodRenderInfo::IsActive(st_int32 nLod, st_int32 nMaterialId) const
{
    assert(nLod < m_nNumLods);
    assert(nMaterialId < m_nMaxMaterialId);

    const CLodRenderInfo::SRef* pRef = &m_aRefs[nMaterialId * m_nNumLods + nLod];

    if (pRef->m_nMaterialId == -1)
        return NULL;

    return pRef;
}


////////////////////////////////////////////////////////////
// CLodRenderInfo::LodHasGeometry

inline st_bool CLodRenderInfo::LodHasGeometry(st_int32 nLod) const
{
    return m_aLodsWithGeometry[nLod + 1]; // +1 to include -1 as a valid LOD value
}


////////////////////////////////////////////////////////////
// CLodRenderInfo::MaterialRefs

inline const CArray<st_int32>* CLodRenderInfo::MaterialRefs(void) const
{
    return &m_aMaterialsUsed;
}


////////////////////////////////////////////////////////////
// CLodRenderInfo::AllocMemPool

inline void CLodRenderInfo::AllocMemPool(void)
{
    size_t siBlockSize = 0;;

    // size of m_aRefs
    const size_t c_siRefsBlockSize = m_nNumLods * m_nMaxMaterialId * sizeof(SRef);
    siBlockSize += c_siRefsBlockSize;

    // size of m_aLodsWithGeometry
    const size_t c_siLodsWithGoemetryBlockSize = (m_nNumLods + 1) * sizeof(st_bool);
    siBlockSize += c_siLodsWithGoemetryBlockSize;

    // size of m_aMaterialsUsed
    const size_t c_siMaterialsUsedBlockSize = m_nMaxMaterialId * sizeof(st_int32);
    siBlockSize += c_siMaterialsUsedBlockSize;

    // allocate block
    assert(!m_pMemPool);
    m_pMemPool = st_new_array<st_byte>(siBlockSize, "CLodRenderInfo::m_pMemPool");
    st_byte* pMemPoolPtr = m_pMemPool;

    // assign part of memory pool to m_aRefs
    SRef sDefault;

    m_aRefs.SetExternalMemory(pMemPoolPtr, c_siRefsBlockSize);
    m_aRefs.resize(m_nNumLods * m_nMaxMaterialId, sDefault);
    pMemPoolPtr += c_siRefsBlockSize;

    // assign part of memory pool to m_aLodsWithGeometry
    m_aLodsWithGeometry.SetExternalMemory(pMemPoolPtr, c_siLodsWithGoemetryBlockSize);
    m_aLodsWithGeometry.resize(m_nNumLods + 1, false); // +1 to include -1 as a valid LOD value
    pMemPoolPtr += c_siLodsWithGoemetryBlockSize;

    // assign part of memory pool to m_aMaterialsUsed
    m_aMaterialsUsed.SetExternalMemory(pMemPoolPtr, c_siMaterialsUsedBlockSize);
    pMemPoolPtr += c_siMaterialsUsedBlockSize;

    assert(size_t(pMemPoolPtr - m_pMemPool) == siBlockSize);
}


///////////////////////////////////////////////////////////////////////
//  SRenderStats::SGeometryStats::SGeometryStats

inline SRenderStats::SGeometryStats::SGeometryStats( )
{
    Reset( );
}


///////////////////////////////////////////////////////////////////////
//  SRenderStats::SGeometryStats::Reset

inline void SRenderStats::SGeometryStats::Reset(void)
{
    m_uiNumObjects = 0;
    m_uiNumTextureBinds = 0;
    m_uiNumDrawCalls = 0;
    m_uiNumVboBinds = 0;
    m_uiNumTriangles = 0;
}


///////////////////////////////////////////////////////////////////////
//  SRenderStats::SRenderStats

inline SRenderStats::SRenderStats( ) :
    m_nNumCellsInFrustum(0)
{
    Reset( );
}


///////////////////////////////////////////////////////////////////////
//  SRenderStats::Reset

inline void SRenderStats::Reset(void)
{
    for (st_int32 i = 0; i < STATS_CATEGORY_COUNT; ++i)
        for (st_int32 j = 0; j < RENDER_PASS_NUM_TYPES; ++j)
            m_aGeometries[i][j].Reset( );

    m_fCullAndPopulateTime = 0.0f;
}


///////////////////////////////////////////////////////////////////////
//  SRenderStats::GetTotals

inline void SRenderStats::GetTotals(SRenderStats::SGeometryStats& sTotals) const
{
    sTotals.Reset( );

    for (st_int32 i = 0; i < STATS_CATEGORY_COUNT; ++i)
    {
        for (st_int32 j = 0; j < RENDER_PASS_NUM_TYPES; ++j)
        {
            sTotals.m_uiNumObjects += m_aGeometries[i][j].m_uiNumObjects;
            sTotals.m_uiNumTextureBinds += m_aGeometries[i][j].m_uiNumTextureBinds;
            sTotals.m_uiNumDrawCalls += m_aGeometries[i][j].m_uiNumDrawCalls;
            sTotals.m_uiNumVboBinds += m_aGeometries[i][j].m_uiNumVboBinds;
            sTotals.m_uiNumTriangles += m_aGeometries[i][j].m_uiNumTriangles;
        }
    }
}


///////////////////////////////////////////////////////////////////////
//  SForestVertexBufferStats::SForestVertexBufferStats

inline SForestVertexBufferStats::SForestVertexBufferStats( )
{
    memset(this, 0, sizeof(SForestVertexBufferStats));
}


///////////////////////////////////////////////////////////////////////
//  SForestRenderInfo::SForestRenderInfo

inline SForestRenderInfo::SForestRenderInfo( ) :
    // general rendering
    m_strShaderPath(""),
    m_nMaxAnisotropy(0),
    m_bHorizontalBillboards(true),
    m_fAlphaTestScalar(0.57f),
    m_bZPrePass(false),
    m_nMaxBillboardImagesByBase(50),
    m_fVisibility(1000.0f),
    // lighting
    m_fGlobalLightScalar(1.0f),
    m_bSpecularLighting(false),
    m_bTransmissionLighting(false),
    m_bDetailLayer(false),
    m_bDetailNormalMapping(false),
    m_bAmbientContrast(false),
    m_fTransmissionScalar(1.0f),
    // fog
    m_fFogStartDistance(2500.0f),
    m_fFogEndDistance(5000.0f),
    m_vFogColor(1.0f, 1.0f, 1.0),
    // sky
    m_vSkyColor(0.2f, 0.3f, 0.5f),
    m_fSkyFogMin(-0.5f),
    m_fSkyFogMax(1.0f),
    // sun
    m_vSunColor(1.0f, 1.0f, 0.85f),
    m_fSunSize(0.001f),
    m_fSunSpreadExponent(200.0f),
    m_fSunFogBloom(0.0f),
    // shadows
    m_nNumShadowMaps(3),
    m_nShadowMapResolution(1024),
    m_bSmoothShadows(false),
    m_bShowShadowSplitsOnTerrain(false),
    // wind
    m_bWindEnabled(true),
    m_bFrondRippling(true)
{
}


///////////////////////////////////////////////////////////////////////
//  SForestCullResultsRI_t::SBillboardIboData::SBillboardIboData

SForestCullResultsRI_TemplateList
inline SForestCullResultsRI_t::SBillboardIboData::SBillboardIboData( ) :
    m_tIbo(false, true),
    m_uiNumIndices(0),
    m_uiMinIndex(0),
    m_uiNumVertices(0)
{
}


///////////////////////////////////////////////////////////////////////
//  SForestCullResultsRI_t::Reserve

SForestCullResultsRI_TemplateList
inline st_bool SForestCullResultsRI_t::Reserve(const TTreeArray& aBaseTrees, 
                                               st_int32 nMaxNumBaseTrees, 
                                               st_int32 nMaxNumVisibleCells,
                                               st_int32 nMaxVisibleInstancesPerBase,
                                               st_bool bHorzBillboards)
{
    m_mBillboardIbos.clear( );

    // for each base tree, make an entry in m_mBillboardIbos
    for (st_int32 nBaseTree = 0; nBaseTree < st_int32(aBaseTrees.size( )); ++nBaseTree)
    {
        // get base tree pointer
        const CTreeRI_t* pBaseTree = (const CTreeRI_t*) aBaseTrees[nBaseTree];
        assert(pBaseTree);

        // determine billboard parameters
        const st_bool c_bHasVertBillboards = pBaseTree->HasGeometryType(GEOMETRY_TYPE_VERTICAL_BILLBOARDS);
        const st_bool c_bHasHorzBillboards = pBaseTree->HasGeometryType(GEOMETRY_TYPE_HORIZONTAL_BILLBOARDS) && bHorzBillboards;

        if (c_bHasVertBillboards || c_bHasHorzBillboards)
        {
            // create a new IBO entry
            typename SForestCullResultsRI_t::SBillboardIboData* pBillboardIboData = &(m_mBillboardIbos[pBaseTree]);
            assert(pBillboardIboData);

            // setup empty IBO if tree has any billboards, vertical or horizontal
            {
                pBillboardIboData->m_tIbo.SetIndexFormat(INDEX_FORMAT_UNSIGNED_32BIT);
                const st_int32 c_nNumIndices = pBaseTree->GetBillboardIboNumIndices( );
                CLocalArray<st_uint32> aEmptyIndices(c_nNumIndices, "UpdateBillboardIbos, indices");
                (void) pBillboardIboData->m_tIbo.AppendIndices(&aEmptyIndices[0], c_nNumIndices);
                (void) pBillboardIboData->m_tIbo.EndIndices( );
                assert(pBillboardIboData->m_tIbo.NumIndices( ) == st_uint32(c_nNumIndices));
            }
        }
    }

    // reserve the rest of the main cull results structure
    return SForestCullResults::Reserve(aBaseTrees, nMaxNumBaseTrees, nMaxNumVisibleCells, nMaxVisibleInstancesPerBase, bHorzBillboards);
}
