///////////////////////////////////////////////////////////////////////  
//  TerrainRI.inl
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
//  STerrainRenderInfo::STerrainRenderInfo

inline STerrainRenderInfo::STerrainRenderInfo( ) :
    m_fNormalMapBlueScale(1.0f),
    m_bShadowsEnabled(false),
    m_bZPrePass(false)
{
    for (st_int32 nSplatLayer = 0; nSplatLayer < c_nNumTerrainSplatLayers; ++nSplatLayer)
        m_afSplatTileValues[nSplatLayer] = 1.0f;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::CTerrainRI

CTerrainRI_TemplateList
inline CTerrainRI_t::CTerrainRI( ) :
      m_nMaxAnisotropy(0)
    , m_pShaderLoader(NULL)
    , m_pVertexFormat(NULL)
    , m_bTexturingEnabled(true)
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
    , m_pMemPool(NULL)
#endif
{
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::~CTerrainRI
//
//  If SPEEDTREE_HEAP_FRIENDLY_MODE was #defined, each of the VBOs associated
//  with each cell were allocated using placement new, where new was fed a pointer
//  from m_pMemPool block shared by all of them.  This destructor makes accommodations
//  for this special case by explicitly calling these objects' destructors and then
//  deleted their shared memory block, m_pMemPool.

CTerrainRI_TemplateList
inline CTerrainRI_t::~CTerrainRI( )
{
    // destroy VBOs assigned to the terrain cells
    for (TTerrainCellMap::iterator iMap = m_cTerrainCellMap.begin( ); iMap != m_cTerrainCellMap.end( ); ++iMap)
    {
        CTerrainCell* pCell = &iMap->second;
        assert(pCell);
        TGeometryBufferPolicy* pVbo = (TGeometryBufferPolicy*) pCell->GetVbo( );

        if (pVbo)
        {
            #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
                pVbo->~TGeometryBufferPolicy( );
            #else
                st_delete<TGeometryBufferPolicy>(pVbo);
            #endif
        }
    }

    // destroy VBO pool
    for (st_int32 i = 0; i < st_int32(m_aVboPool.size( )); ++i)
    {
        if (m_aVboPool[i])
        {
            #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
                m_aVboPool[i]->~TGeometryBufferPolicy( );
            #else
                st_delete<TGeometryBufferPolicy>(m_aVboPool[i]);
            #endif
        }
    }

    #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
        st_delete_array<st_byte>(m_pMemPool);
    #endif
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::SetShaderLoader

CTerrainRI_TemplateList
inline void CTerrainRI_t::SetShaderLoader(TShaderLoaderPolicy* pShaderLoader)
{
    m_pShaderLoader = pShaderLoader;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::SetRenderInfo

CTerrainRI_TemplateList
inline void CTerrainRI_t::SetRenderInfo(const STerrainRenderInfo& sInfo)
{
    m_sRenderInfo = sInfo;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::GetRenderInfo

CTerrainRI_TemplateList
inline const STerrainRenderInfo& CTerrainRI_t::GetRenderInfo(void) const
{
    return m_sRenderInfo;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::SetMaxAnisotropy

CTerrainRI_TemplateList
inline void CTerrainRI_t::SetMaxAnisotropy(st_int32 nMaxAnisotropy)
{
    m_nMaxAnisotropy = nMaxAnisotropy;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::Init

CTerrainRI_TemplateList
inline st_bool CTerrainRI_t::Init(st_int32 nNumLods, st_int32 nMaxTileRes, st_float32 fCellSize, const SVertexAttribDesc* pVertexFormat)
{
    st_bool bSuccess = false;

    // load textures
    bSuccess = m_tNormalMapTexture.Load(m_sRenderInfo.m_strNormalMap.c_str( ), m_nMaxAnisotropy);
    bSuccess &= m_tSplatMapTexture.Load(m_sRenderInfo.m_strSplatMap.c_str( ), m_nMaxAnisotropy);
    for (st_int32 nSplatLayer = 0; nSplatLayer < c_nNumTerrainSplatLayers; ++nSplatLayer)
        bSuccess &= m_atSplatTextureLayers[nSplatLayer].Load(m_sRenderInfo.m_astrSplatLayers[nSplatLayer].c_str( ), m_nMaxAnisotropy);

    // load white texture (for use when texturing is 'disabled')
    if (!m_tWhiteTexture.IsValid( ))
        (void) m_tWhiteTexture.LoadColor(0xffffffff);

    // shader
    if (!ShaderIsLoaded( ))
        bSuccess &= InitShader( );

    // if cells exist, then this is a subsequent Init() call and we need to destroy the terrain cells,
    // saving off their respective VBOs in the process
    if (!m_cTerrainCellMap.empty( ))
    {
        for (TTerrainCellMap::iterator iMap = m_cTerrainCellMap.begin( ); iMap != m_cTerrainCellMap.end( ); ++iMap)
        {
            CTerrainCell* pCell = &iMap->second;
            assert(pCell);
            TGeometryBufferPolicy* pVbo = (TGeometryBufferPolicy*) pCell->GetVbo( );
            assert(pVbo);

            m_aVboPool.push_back(pVbo);
        }
    }

    // init graphics-independent portion from Forest library
    bSuccess &= CTerrain::Init(nNumLods, nMaxTileRes, fCellSize);

    // vbos, allocated ahead of time
    if (m_aVboPool.empty( ) && m_cTerrainCellMap.empty( ))
    {
        m_pVertexFormat = pVertexFormat;
        bSuccess &= InitVboPool( );
    }

    // initialize the single index buffer that all terrain tiles will share
    bSuccess &= InitIndexBuffer( );

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::Render

CTerrainRI_TemplateList
inline st_bool CTerrainRI_t::Render(const CForestRI_t* pForest, 
                                        const STerrainCullResults& sCullResults, 
                                        ERenderPassType eRenderPass,
                                        const Vec3& vLightDir,
                                        const SMaterial& sLighting, 
                                        SRenderStats* pMainStats)
{
    st_bool bSuccess = true;

    // if stats are passed in, pick the appropriate geometry group to fill out
    SRenderStats::SGeometryStats* pStats = pMainStats ? &pMainStats->m_aGeometries[STATS_CATEGORY_TERRAIN][eRenderPass] : NULL;

    // first and foremost, see if any of the terrain is visible
    if (!sCullResults.m_aVisibleCells.empty( ))
    {
        TShaderTechniquePolicy& tTechnique = CForestRI_t::PickTechnique(m_atTechniques, eRenderPass);
        if (BeginRender(pForest, tTechnique, vLightDir, sLighting))
        {
            bSuccess &= tTechnique.CommitConstants( );

            // enable the master index buffer
            bSuccess &= m_tIndexBuffer.BindIndexBuffer( );

            // enable the vertex format
            TGeometryBufferPolicy* pFirstVbo = NULL;
            for (st_int32 nCell = 0; nCell < st_int32(sCullResults.m_aVisibleCells.size( )); ++nCell)
            {
                pFirstVbo = (TGeometryBufferPolicy*) sCullResults.m_aVisibleCells[nCell]->GetVbo( );
                if (pFirstVbo)
                    break;
            }
            if (pFirstVbo)
                bSuccess &= pFirstVbo->EnableFormat( );

            for (st_int32 nCell = 0; nCell < st_int32(sCullResults.m_aVisibleCells.size( )); ++nCell)
            {
                const CTerrainCell* pCell = sCullResults.m_aVisibleCells[nCell];
                assert(pCell);
                TGeometryBufferPolicy* pVbo = (TGeometryBufferPolicy*) pCell->GetVbo( );

                if (pVbo)
                {
                    if (pVbo->BindVertexBuffer( ))
                    {
                        // query offset into master index buffer
                        st_uint32 uiOffset = 0, uiNumIndices = 0, uiMinIndex = 0, uiNumVertices = 0;
                        pCell->GetIndices(uiOffset, uiNumIndices, uiMinIndex, uiNumVertices);

                        // render the tile at the correct LOD and transitional borders
                        m_tIndexBuffer.RenderIndexed(PRIMITIVE_TRIANGLE_STRIP, uiOffset, uiNumIndices, uiMinIndex, uiNumVertices);

                        if (pStats)
                        {
                            pStats->m_uiNumVboBinds++;
                            pStats->m_uiNumDrawCalls++;
                            pStats->m_uiNumTriangles += uiNumIndices - 2;
                        }

                        bSuccess &= pVbo->UnBindVertexBuffer( );
                    }
                    else
                    {
                        CCore::SetError("CTerrainRI::Render, BindVertexBuffer() failed");
                        bSuccess = false;
                    }
                }
            }

            // with the render loop complete, unbind the index buffer & disable the vbo vertex format
            bSuccess &= m_tIndexBuffer.UnBindIndexBuffer( );
            bSuccess &= TGeometryBufferPolicy::DisableFormat( );

            if (bSuccess && pStats)
            {
                pStats->m_uiNumObjects++;
                pStats->m_uiNumTextureBinds += c_nNumTerrainSplatLayers;
            }
        }
        else
        {
            CCore::SetError("CTerrainRI::Render, BeginRender() failed");
            bSuccess = false;
        }

        // final state changes, if any
        bSuccess &= EndRender(tTechnique);
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::UploadShaderConstants

CTerrainRI_TemplateList
inline st_bool CTerrainRI_t::UploadShaderConstants(const CForestRI_t* pForest, const Vec3& vLightDir, const SMaterial& sLighting)
{
    st_bool bSuccess = false;

    // upload the textures
    bSuccess = SetTexture(m_tNormalMapTextureVar, m_tNormalMapTexture);
    bSuccess &= SetTexture(m_tSplatMapTextureVar, m_tSplatMapTexture);
    for (st_int32 nSplatLayer = 0; nSplatLayer < c_nNumTerrainSplatLayers; ++nSplatLayer)
        bSuccess &= SetTexture(m_atSplatTextureLayersVars[nSplatLayer], m_atSplatTextureLayers[nSplatLayer]);

    // set shader variables
    {
        // user specifies light direction like DX/OpenGL, but the shader takes a negated value
        bSuccess &= m_tLightDir.Set3fv(-vLightDir);
        bSuccess &= m_tLightAndMaterialAmbient.Set4fv(m_sRenderInfo.m_sMaterial.m_vAmbient * sLighting.m_vAmbient);
        bSuccess &= m_tLightAndMaterialDiffuse.Set4fv(m_sRenderInfo.m_sMaterial.m_vDiffuse * sLighting.m_vDiffuse * m_sRenderInfo.m_sMaterial.m_fLightScalar);

        // upload extra terrain data (usually tiling parameters)
        bSuccess &= m_tSplatMapTiles.Set4fv(reinterpret_cast<st_float32*>(&m_sRenderInfo.m_afSplatTileValues));

        pForest->UploadShadowShaderConsts(m_tShadowSplitDistances, m_tShadowFadeParams, m_tFarClip);
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::CullAndComputeLOD

CTerrainRI_TemplateList
inline void CTerrainRI_t::CullAndComputeLOD(const CView& cView, STerrainCullResults& sCullResults)
{
    // determine which cells are visible and which need to be populated
    CTerrain::CullAndComputeLOD(cView, sCullResults);

    // add the newly-available VBOs to the list of available VBOs
    st_int32 i = 0;
    for (i = 0; i < st_int32(sCullResults.m_aFreedVbos.size( )); ++i)
        m_aVboPool.push_back((TGeometryBufferPolicy*) sCullResults.m_aFreedVbos[i]);

    // assign VBOs to the newly-created cells
    for (i = 0; i < st_int32(sCullResults.m_aCellsToUpdate.size( )); ++i)
        sCullResults.m_aCellsToUpdate[i]->SetVbo((void*) NextVbo( ));
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::EnableTexturing

CTerrainRI_TemplateList
inline void CTerrainRI_t::EnableTexturing(st_bool bFlag)
{
    m_bTexturingEnabled = bFlag;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::InitIndexBuffer

CTerrainRI_TemplateList
inline st_bool CTerrainRI_t::InitIndexBuffer(void)
{
    bool bSuccess = false;

    const CArray<st_uint32>& aCompositeIndices = GetCompositeIndices( );
    if (!aCompositeIndices.empty( ))
    {
        m_tIndexBuffer.SetIndexFormat(INDEX_FORMAT_UNSIGNED_32BIT);
        bSuccess = m_tIndexBuffer.AppendIndices((const st_byte*) &aCompositeIndices[0], st_uint32(aCompositeIndices.size( )));
        bSuccess &= m_tIndexBuffer.EndIndices( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::InitShader

CTerrainRI_TemplateList
inline st_bool CTerrainRI_t::InitShader(void)
{
    bool bSuccess = false;

    if (m_pShaderLoader)
    {
        // shader location
        CFixedString strShaderFilename = (m_sRenderInfo.m_strShaderPath + "Terrain.hlsl").c_str( );

        // standard shader
        bSuccess = m_pShaderLoader->GetTechnique(strShaderFilename.c_str( ), "TerrainVertex", "TerrainPixel", "Terrain", m_atTechniques[CForestRI_t::SHADER_TYPE_STANDARD]);
        if (m_sRenderInfo.m_bZPrePass)
            bSuccess &= m_pShaderLoader->GetTechnique(strShaderFilename.c_str( ), "TerrainVertex_Depth", "TerrainPixel_Depth", "Terrain", m_atTechniques[CForestRI_t::SHADER_TYPE_DEPTH_ONLY]);

        // query terrain-specific uniform variables
        if (bSuccess)
        {
            const TShaderTechniquePolicy& tLoadConstsFromHere = m_atTechniques[CForestRI_t::SHADER_TYPE_STANDARD];
            bSuccess &= m_pShaderLoader->GetConstant(m_pShaderLoader->GetTextureConstantName("TerrainNormalMap").c_str( ), tLoadConstsFromHere, m_tNormalMapTextureVar);
            bSuccess &= m_pShaderLoader->GetConstant(m_pShaderLoader->GetTextureConstantName("TerrainSplatMap").c_str( ), tLoadConstsFromHere, m_tSplatMapTextureVar);
            bSuccess &= m_pShaderLoader->GetConstant(m_pShaderLoader->GetTextureConstantName("TerrainSplatLayer0").c_str( ), tLoadConstsFromHere, m_atSplatTextureLayersVars[0]);
            bSuccess &= m_pShaderLoader->GetConstant(m_pShaderLoader->GetTextureConstantName("TerrainSplatLayer1").c_str( ), tLoadConstsFromHere, m_atSplatTextureLayersVars[1]);
            bSuccess &= m_pShaderLoader->GetConstant(m_pShaderLoader->GetTextureConstantName("TerrainSplatLayer2").c_str( ), tLoadConstsFromHere, m_atSplatTextureLayersVars[2]);
            bSuccess &= m_pShaderLoader->GetConstant("g_vSplatMapTiles", tLoadConstsFromHere, m_tSplatMapTiles);
            bSuccess &= m_pShaderLoader->GetConstant("g_vLightDir", tLoadConstsFromHere, m_tLightDir);
            bSuccess &= m_pShaderLoader->GetConstant("g_vMaterialAmbient", tLoadConstsFromHere, m_tLightAndMaterialAmbient);
            bSuccess &= m_pShaderLoader->GetConstant("g_vMaterialDiffuse", tLoadConstsFromHere, m_tLightAndMaterialDiffuse);
            if (m_sRenderInfo.m_bShadowsEnabled)
            {
                bSuccess &= m_pShaderLoader->GetConstant("g_vShadowFadeParams", tLoadConstsFromHere, m_tShadowFadeParams);
                bSuccess &= m_pShaderLoader->GetConstant("g_vSplitDistances", tLoadConstsFromHere, m_tShadowSplitDistances);
                bSuccess &= m_pShaderLoader->GetConstant("g_fFarClip", tLoadConstsFromHere, m_tFarClip);
            }
        }
        else
            CCore::SetError("CTerrainRI::InitShader, technique 'Terrain' failed to load");
    }
    else
        CCore::SetError("CTerrainRI::InitShader, no shader loader was passed in from CForestRI");

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::ShaderIsLoaded

CTerrainRI_TemplateList
inline st_bool CTerrainRI_t::ShaderIsLoaded(void) const
{
    // assume that if a shader constant is loaded that the shader has already been loaded
    return m_tSplatMapTiles.IsValid( );
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::BeginRender

CTerrainRI_TemplateList
inline st_bool CTerrainRI_t::BeginRender(const CForestRI_t* pForest, TShaderTechniquePolicy& tTechnique, const Vec3& vLightDir, const SMaterial& sLighting)
{
    st_bool bSuccess = false;

    // set render states
    m_tRenderStatePolicy.SetAlphaTesting(false);
    m_tRenderStatePolicy.SetFaceCulling(CCoordSys::IsLeftHanded( ) ? CULLTYPE_BACK : CULLTYPE_FRONT);

    // bind the technique
    if (tTechnique.Bind( ))
    {
        bSuccess = UploadShaderConstants(pForest, vLightDir, sLighting);
    }
    else
        CCore::SetError("CTerrainRI::BeginRender, tTechnique.Bind() failed");

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::EndRender

CTerrainRI_TemplateList
inline st_bool CTerrainRI_t::EndRender(TShaderTechniquePolicy& tTechnique)
{
    bool bSuccess = false;

    bSuccess = tTechnique.UnBind( );

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::SetTexture

CTerrainRI_TemplateList
inline st_bool CTerrainRI_t::SetTexture(const TShaderConstantPolicy& tTextureVar, const TTexturePolicy& tTextureMap) const
{
    bool bSuccess = false;

    if (m_bTexturingEnabled && tTextureVar.IsValid( ) && tTextureMap.IsValid( ))
        bSuccess = tTextureVar.SetTexture(tTextureMap);
    else
        bSuccess = tTextureVar.SetTexture(m_tWhiteTexture);

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::InitVboPool

CTerrainRI_TemplateList
inline st_bool CTerrainRI_t::InitVboPool(void)
{
    st_bool bSuccess = false;

    if (m_nHintMaxNumVisibleCells > 0)
    {
        #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            assert(!m_pMemPool);
            size_t siPoolSize = m_nHintMaxNumVisibleCells * sizeof(TGeometryBufferPolicy);
            m_pMemPool = st_new_array<st_byte>(siPoolSize, "TerrainMemPool");
            TGeometryBufferPolicy* pPoolPtr = (TGeometryBufferPolicy*) m_pMemPool;
        #endif

        m_aVboPool.resize(m_nHintMaxNumVisibleCells);
        for (st_int32 i = 0; i < m_nHintMaxNumVisibleCells; ++i)
        {
            #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
                m_aVboPool[i] = NewVbo(pPoolPtr++);
            #else
                m_aVboPool[i] = NewVbo( );
            #endif
        }

        bSuccess = true;
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::NextVbo

CTerrainRI_TemplateList
inline TGeometryBufferPolicy* CTerrainRI_t::NextVbo(void)
{
    TGeometryBufferPolicy* pVbo = NULL;

    if (m_aVboPool.empty( ))
        #ifndef SPEEDTREE_HEAP_FRIENDLY_MODE
            pVbo = NewVbo( );
        #else
            CCore::SetError("Terrain cell limit reached: %d, raise using CTerrain::SetHint(HINT_MAX_NUM_VISIBLE_CELLS)", m_nHintMaxNumVisibleCells);
        #endif
    else
    {
        pVbo = m_aVboPool.back( );
        m_aVboPool.pop_back( );
    }

    return pVbo;
}


///////////////////////////////////////////////////////////////////////  
//  CTerrainRI::NewVbo

CTerrainRI_TemplateList
inline TGeometryBufferPolicy* CTerrainRI_t::NewVbo(TGeometryBufferPolicy* pVboObjectBuffer) const
{
    TGeometryBufferPolicy* pVbo = NULL;
    if (pVboObjectBuffer)
    {
        pVbo = new (pVboObjectBuffer) TGeometryBufferPolicy(true, false);
    }
    else
        pVbo = st_new(TGeometryBufferPolicy, "Terrain VBO")(true, false);

    assert(m_pVertexFormat);
    pVbo->SetVertexFormat(m_pVertexFormat, &m_atTechniques[CForestRI_t::SHADER_TYPE_STANDARD]);

    // size VB
    st_uint32 uiNumVertices = st_uint32(GetMaxTileRes( ) * GetMaxTileRes( ));
    CLocalArray<st_byte> aEmpty(uiNumVertices * pVbo->VertexSize( ), "CTerrainRI::NewVbo");
    pVbo->AppendVertices(&aEmpty[0], uiNumVertices);
    pVbo->EndVertices( );

    return pVbo;
}


