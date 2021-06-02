///////////////////////////////////////////////////////////////////////  
//  GrassRI.inl
//
//  *** INTERACTIVE DATA VISUALIZATION (IDV) CONFIDENTIAL AND PROPRIETARY INFORMATION ***
//
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Interactive Data Visualization, Inc. and
//  may not be copied, disclosed, or exploited except in accordance with 
//  the terms of thats agreement.
//
//      Copyright (c) 2003-2010 IDV, Inc.
//      All rights reserved in all media.
//
//      IDV, Inc.
//      http://www.idvinc.com


///////////////////////////////////////////////////////////////////////  
//  SGrassRenderInfo::SGrassRenderInfo

inline SGrassRenderInfo::SGrassRenderInfo( ) :
    m_fTerrainMatchFactorTop(0.7f),
    m_fTerrainMatchFactorBottom(1.0f),
    m_fNormalMapBlueScale(1.0f)
{
    st_int32 nSplatLayer = 0;
    for (nSplatLayer = 0; nSplatLayer < c_nNumTerrainSplatLayers; ++nSplatLayer)
        m_avTerrainSplatColors[nSplatLayer].Set(1.0f, 1.0f, 1.0f, 1.0f);
    for (nSplatLayer = 0; nSplatLayer < c_nNumTerrainSplatLayers; ++nSplatLayer)
        m_afSplatTileValues[nSplatLayer] = 1.0f;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::CGrassRI

CGrassRI_TemplateList
inline CGrassRI_t::CGrassRI( ) :
      m_pShaderLoader(NULL)
    , m_pVertexFormat(NULL)
    , m_bTexturingEnabled(true)
#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
    , m_pMemPool(NULL)
#endif
{
    m_tIndexBuffer.SetIndexFormat(INDEX_FORMAT_UNSIGNED_32BIT);
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::~CGrassRI
//
//  If SPEEDTREE_HEAP_FRIENDLY_MODE was #defined, each of the VBOs associated
//  with each cell were allocated using placement new, where new was fed a pointer
//  from m_pMemPool block shared by all of them.  This destructor makes accommodations
//  for this special case by explicitly calling these objects' destructors and then
//  deleted their shared memory block, m_pMemPool.

CGrassRI_TemplateList
inline CGrassRI_t::~CGrassRI( )
{
    // destroy VBOs assigned to the Grass cells
    for (TGrassCellMap::iterator iMap = m_cGrassCellMap.begin( ); iMap != m_cGrassCellMap.end( ); ++iMap)
    {
        CGrassCell* pCell = &iMap->second;
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
//  CGrassRI::SetShaderLoader

CGrassRI_TemplateList
inline void CGrassRI_t::SetShaderLoader(TShaderLoaderPolicy* pShaderLoader)
{
    m_pShaderLoader = pShaderLoader;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::SetRenderInfo

CGrassRI_TemplateList
inline void CGrassRI_t::SetRenderInfo(const SGrassRenderInfo& sInfo)
{
    m_sRenderInfo = sInfo;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::GetRenderInfo

CGrassRI_TemplateList
inline const SGrassRenderInfo& CGrassRI_t::GetRenderInfo(void) const
{
    return m_sRenderInfo;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::Init
//
//  Init() may be called more than once in an application's lifetime; it may
//  be used to set new render parameters for level levels, forests, etc.
//
//  Init() does assume that the vertex format doesn't change in subsequent calls.

CGrassRI_TemplateList
inline st_bool CGrassRI_t::Init(const char* pTextureFilename, st_int32 nImageRows, st_int32 nImageCols, st_float32 fCellSize, const SVertexAttribDesc* pVertexFormat)
{
    st_bool bSuccess = false;

    assert(pVertexFormat);

    // load the grass texture
    bSuccess = m_tCompositeTexture.Load(pTextureFilename);

    // don't load if it was loaded previously
    if (!m_tWhiteTexture.IsValid( ))
        (void) m_tWhiteTexture.LoadColor(0xffffffff);

    // shader
    if (!ShaderIsLoaded( ))
        bSuccess &= InitShader( );

    // vbos, allocated up front
    if (m_aVboPool.empty( ) && m_cGrassCellMap.empty( ))
    {
        m_pVertexFormat = pVertexFormat;
        bSuccess &= InitVboPool( );
        if (!TGeometryBufferPolicy::IsPrimitiveTypeSupported(PRIMITIVE_QUADS))
            bSuccess &= ResizeIndexBuffer(m_nHintMaxGrassBladesPerCell);
    }

    // if cells exist, then this is a subsequent Init() call and we need to destroy the grass cells, 
    // saving off their respective VBOs in the process
    if (!m_cGrassCellMap.empty( ))
    {
        for (TGrassCellMap::iterator iMap = m_cGrassCellMap.begin( ); iMap != m_cGrassCellMap.end( ); ++iMap)
        {
            CGrassCell* pCell = &iMap->second;
            assert(pCell);
            TGeometryBufferPolicy* pVbo = (TGeometryBufferPolicy*) pCell->GetVbo( );
            assert(pVbo);

            m_aVboPool.push_back(pVbo);
        }
    }

    bSuccess &= CGrass::Init(pTextureFilename, nImageRows, nImageCols, fCellSize);

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::Render

CGrassRI_TemplateList
inline st_bool CGrassRI_t::Render(const SGrassCullResults& sCullResults, 
                                      ERenderPassType eRenderPass,
                                      const Vec3& vLightDir,
                                      const SMaterial& sLighting, 
                                      SRenderStats* pMainStats)
{
    st_bool bSuccess = true;

    // if stats are passed in, pick the appropriate geometry group to fill out
    SRenderStats::SGeometryStats* pStats = pMainStats ? &pMainStats->m_aGeometries[STATS_CATEGORY_GRASS][eRenderPass] : NULL;

    // first and foremost, see if any of the grass is visible
    if (!sCullResults.m_aVisibleCells.empty( ))
    {
        TShaderTechniquePolicy& tTechnique = m_tTechnique;
        if (BeginRender(tTechnique, vLightDir, sLighting))
        {
            bSuccess &= tTechnique.CommitConstants( );

            if (pStats)
                pStats->m_uiNumTextureBinds++;

            // enable the vertex format by enabling with the first valid grass VBO
            TGeometryBufferPolicy* pFirstVbo = NULL;
            for (st_int32 nCell = 0; nCell < st_int32(sCullResults.m_aVisibleCells.size( )); ++nCell)
            {
                pFirstVbo = (TGeometryBufferPolicy*) sCullResults.m_aVisibleCells[nCell]->GetVbo( );
                if (pFirstVbo)
                    break;
            }
            if (pFirstVbo)
                bSuccess &= pFirstVbo->EnableFormat( );

            // determine how the grass will be rendered
            const st_bool c_bCanRenderQuads = TGeometryBufferPolicy::IsPrimitiveTypeSupported(PRIMITIVE_QUADS);
            if (!c_bCanRenderQuads)
                bSuccess &= m_tIndexBuffer.BindIndexBuffer( );

            // render the individual grass cells
            for (st_int32 nCell = 0; nCell < st_int32(sCullResults.m_aVisibleCells.size( )); ++nCell)
            {
                const CGrassCell* pCell = sCullResults.m_aVisibleCells[nCell];
                assert(pCell);
                TGeometryBufferPolicy* pVbo = (TGeometryBufferPolicy*) pCell->GetVbo( );

                if (pVbo)
                {
                    if (pVbo->BindVertexBuffer( ))
                    {
                        if (c_bCanRenderQuads)
                        {
                            bSuccess &= pVbo->RenderArrays(PRIMITIVE_QUADS, 0, pCell->GetNumBlades( ) * 4);
                        }
                        else
                        {
                            const st_uint32 c_uiNumVerts = pCell->GetNumBlades( ) * 6;
                            if (c_uiNumVerts > m_tIndexBuffer.NumIndices( ))
                            {
                                CCore::SetError("CGrassRI::Render(), exceeded preset limit of %d grass blades per cell, now %d; use CGrass::SetHint() to adjust",
                                    m_tIndexBuffer.NumIndices( ) / 6, pCell->GetNumBlades( ));

                                bSuccess &= m_tIndexBuffer.UnBindIndexBuffer( );
                                bSuccess &= ResizeIndexBuffer(pCell->GetNumBlades( ));
                                bSuccess &= m_tIndexBuffer.BindIndexBuffer( );
                            }
                            bSuccess &= m_tIndexBuffer.RenderIndexed(PRIMITIVE_TRIANGLES, 0, c_uiNumVerts, 0, c_uiNumVerts);
                        }

                        if (pStats)
                        {
                            pStats->m_uiNumVboBinds++;
                            pStats->m_uiNumDrawCalls++;
                            pStats->m_uiNumTriangles += pCell->GetNumBlades( ) * 2;
                        }

                        bSuccess &= pVbo->UnBindVertexBuffer( );
                    }
                    else
                    {
                        CCore::SetError("CGrassRI::Render, BindVertexBuffer() failed");
                        bSuccess = false;
                    }
                }
            }

            bSuccess &= pFirstVbo->DisableFormat( );
            bSuccess &= EndRender(tTechnique);
            if (!c_bCanRenderQuads)
                bSuccess &= m_tIndexBuffer.UnBindIndexBuffer( );

            if (bSuccess && pStats)
            {
                pStats->m_uiNumObjects++;
            }
        }
        else
        {
            CCore::SetError("CGrassRI::Render, BeginRender() failed");
            bSuccess = false;
        }
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::Cull

CGrassRI_TemplateList
inline void CGrassRI_t::Cull(const CView& cView, SGrassCullResults& sCullResults)
{
    // determine which cells are visible and which need to be populated
    CGrass::Cull(cView, sCullResults);

    // add the newly-available VBOs to the list of available VBOs
    st_int32 i = 0;
    for (i = 0; i < st_int32(sCullResults.m_aFreedVbos.size( )); ++i)
        m_aVboPool.push_back((TGeometryBufferPolicy*) sCullResults.m_aFreedVbos[i]);

    // assign VBOs to the newly-created cells
    for (i = 0; i < st_int32(sCullResults.m_aCellsToUpdate.size( )); ++i)
        sCullResults.m_aCellsToUpdate[i]->SetVbo((void*) NextVbo( ));

    // compute new grass unit billboard since the view has presumably changed
    ComputeUnitBillboard(cView);
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::EnableTexturing

CGrassRI_TemplateList
inline void CGrassRI_t::EnableTexturing(st_bool bFlag)
{
    m_bTexturingEnabled = bFlag;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::BeginRender

CGrassRI_TemplateList
inline st_bool CGrassRI_t::BeginRender(TShaderTechniquePolicy& tTechnique, const Vec3& vLightDir, const SMaterial& sLighting)
{
    st_bool bSuccess = false;

    // set render states
    m_tRenderStatePolicy.SetFaceCulling(CULLTYPE_NONE);

    if (tTechnique.Bind( ))
    {
        bSuccess = true;

        // user specifies light direction like DX/OpenGL, but the shader takes a negated value
        bSuccess &= m_tLightDir.Set3fv(-vLightDir);

        // upload the texture
        if (m_bTexturingEnabled && m_tCompositeTexture.IsValid( ))
            bSuccess &= m_tCompositeTextureVar.SetTexture(m_tCompositeTexture);
        else
            (void) m_tCompositeTextureVar.SetTexture(m_tWhiteTexture);

        // set shader variables
        bSuccess &= m_tUnitBillboard.SetMatrix(m_mCameraFacingUnitBillboard);
        bSuccess &= m_tRenderInfo.Set4f(m_fStartFade, m_fEndFade - m_fStartFade, m_sRenderInfo.m_fTerrainMatchFactorTop, m_sRenderInfo.m_fTerrainMatchFactorBottom);
        bSuccess &= m_tTerrainSplatColors.SetArray4f(m_sRenderInfo.m_avTerrainSplatColors[0], 3);
        bSuccess &= m_tLightAndMaterialAmbient.Set4fv(m_sRenderInfo.m_sMaterial.m_vAmbient * sLighting.m_vAmbient);
        bSuccess &= m_tLightAndMaterialDiffuse.Set4fv(m_sRenderInfo.m_sMaterial.m_vDiffuse * sLighting.m_vDiffuse * m_sRenderInfo.m_sMaterial.m_fLightScalar);
        
        // upload extra terrain data (usually tiling parameters)
        bSuccess &= m_tSplatMapTiles.Set4fv(reinterpret_cast<st_float32*>(&m_sRenderInfo.m_afSplatTileValues)); 
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::EndRender

CGrassRI_TemplateList
inline st_bool CGrassRI_t::EndRender(TShaderTechniquePolicy& tTechnique)
{
    bool bSuccess = false;

    bSuccess = tTechnique.UnBind( );

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::ComputeUnitBillboard

CGrassRI_TemplateList
inline void CGrassRI_t::ComputeUnitBillboard(const CView& cView)
{
    // setup initial billboard coords for default coordinate system
    Mat4x4 mUnitBillboard(0.0f, 0.5f, 1.0f, 0.0f,
                          0.0f, -0.5f, 1.0f, 0.0f,
                          0.0f, -0.5f, 0.0f, 0.0f,
                          0.0f, 0.5f, 0.0f, 0.0f);

    // convert unit billboard to current coordinate system
    if (!CCoordSys::IsDefaultCoordSys( ))
    {
        for (st_int32 i = 0; i < 4; ++i)
        {
            Vec3* pCoord = reinterpret_cast<Vec3*>(mUnitBillboard.m_afSingle + i * 4);
            *pCoord = CCoordSys::ConvertFromStd(*pCoord);
        }
    }

    // compute pitch and azimuth
    float fAzimuth = -cView.GetCameraAzimuth( );
    float fPitch = cView.GetCameraPitch( );

    // alternate coordinate system adjustment
    if (!CCoordSys::IsLeftHanded( ) && CCoordSys::IsYAxisUp( ))
        fPitch = -fPitch;

    m_mCameraFacingUnitBillboard.SetIdentity( );
    CCoordSys::RotateOutAxis(m_mCameraFacingUnitBillboard, 0.75f * fPitch);
    CCoordSys::RotateUpAxis(m_mCameraFacingUnitBillboard, fAzimuth);
    mUnitBillboard.Transpose(mUnitBillboard);
    m_mCameraFacingUnitBillboard = m_mCameraFacingUnitBillboard * mUnitBillboard;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::InitVboPool

CGrassRI_TemplateList
inline st_bool CGrassRI_t::InitVboPool(void)
{
    st_bool bSuccess = false;

    if (m_nHintMaxNumVisibleCells > 0)
    {
        #ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            assert(!m_pMemPool);
            size_t siPoolSize = m_nHintMaxNumVisibleCells * sizeof(TGeometryBufferPolicy);
            m_pMemPool = st_new_array<st_byte>(siPoolSize, "GrassMemPool");
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
//  CGrassRI::NextVbo

CGrassRI_TemplateList
inline TGeometryBufferPolicy* CGrassRI_t::NextVbo(void)
{
    TGeometryBufferPolicy* pVbo = NULL;

    if (m_aVboPool.empty( ))
    {
        #ifndef SPEEDTREE_HEAP_FRIENDLY_MODE
            pVbo = NewVbo( );
        #else
            CCore::SetError("Grass cell limit reached: %d, raise using CGrass::SetHint(HINT_MAX_NUM_VISIBLE_CELLS)", m_nHintMaxNumVisibleCells);
        #endif
    }
    else
    {
        pVbo = m_aVboPool.back( );
        m_aVboPool.pop_back( );
    }

    return pVbo;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::ResizeIndexBuffer

CGrassRI_TemplateList
inline st_bool CGrassRI_t::ResizeIndexBuffer(st_int32 nNumGrassBlades)
{
    st_bool bSuccess = false;

    // index buffer is only necessary if quads can't be rendered
    if (nNumGrassBlades > 0 &&
        m_tIndexBuffer.NumIndices( ) < st_uint32(nNumGrassBlades * 6))
    {
        CArray<st_uint32> aIndices(nNumGrassBlades * 6);

        st_uint32* pBuffer = &aIndices[0];
        for (st_uint32 i = 0; i < st_uint32(nNumGrassBlades); ++i)
        {
            st_uint32 uiOffset = i * 4;
            *pBuffer++ = uiOffset + 0;
            *pBuffer++ = uiOffset + 1;
            *pBuffer++ = uiOffset + 2;
            *pBuffer++ = uiOffset + 0;
            *pBuffer++ = uiOffset + 2;
            *pBuffer++ = uiOffset + 3;
        }
        assert(st_uint32(pBuffer - &aIndices[0]) == st_uint32(nNumGrassBlades * 6));

        // copy buffer contents into index buffer
        bSuccess = m_tIndexBuffer.ClearIndices( );
        bSuccess &= m_tIndexBuffer.AppendIndices((const st_byte*) &aIndices[0], st_uint32(aIndices.size( )));
        bSuccess &= m_tIndexBuffer.EndIndices( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::NewVbo

CGrassRI_TemplateList
inline TGeometryBufferPolicy* CGrassRI_t::NewVbo(TGeometryBufferPolicy* pVboObjectBuffer)
{
    TGeometryBufferPolicy* pVbo = NULL;
    if (pVboObjectBuffer)
    {
        pVbo = new (pVboObjectBuffer) TGeometryBufferPolicy(true, false);
    }
    else
    {
        pVbo = st_new(TGeometryBufferPolicy, "Grass VBO")(true, false);
    }

    assert(m_pVertexFormat);
    pVbo->SetVertexFormat(m_pVertexFormat, &m_tTechnique);

    // go ahead and allocate the VBO so it doesn't happen after app's init time
    assert(m_nHintMaxGrassBladesPerCell > 0);

    // size VB
    const st_int32 c_nVertsPerBlade = 4;
    CLocalArray<st_byte> aEmpty(m_nHintMaxGrassBladesPerCell * c_nVertsPerBlade * pVbo->VertexSize( ), "CGrassRI::NewVbo");
    pVbo->AppendVertices(&aEmpty[0], m_nHintMaxGrassBladesPerCell * c_nVertsPerBlade);
    pVbo->EndVertices( );

    return pVbo;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::InitShader

CGrassRI_TemplateList
inline st_bool CGrassRI_t::InitShader(void)
{
    bool bSuccess = false;

    if (m_pShaderLoader)
    {
        // standard shader
        bSuccess = m_pShaderLoader->GetTechnique((m_sRenderInfo.m_strShaderPath + "Grass.hlsl").c_str( ), 
                                                "GrassVertex", "GrassPixel", "Grass", m_tTechnique);

        // query grass-specific uniform variables
        if (bSuccess)
        {
            const TShaderTechniquePolicy& tLoadConstsFromHere = m_tTechnique;
            bSuccess &= m_pShaderLoader->GetConstant(m_pShaderLoader->GetTextureConstantName("GrassBlades").c_str( ), tLoadConstsFromHere, m_tCompositeTextureVar);
            bSuccess &= m_pShaderLoader->GetConstant("g_mGrassUnitBillboard", tLoadConstsFromHere, m_tUnitBillboard);
            bSuccess &= m_pShaderLoader->GetConstant("g_avGrassSplatColors", tLoadConstsFromHere, m_tTerrainSplatColors);
            bSuccess &= m_pShaderLoader->GetConstant("g_vGrassInfo", tLoadConstsFromHere, m_tRenderInfo);
            bSuccess &= m_pShaderLoader->GetConstant("g_vSplatMapTiles", tLoadConstsFromHere, m_tSplatMapTiles);
            bSuccess &= m_pShaderLoader->GetConstant("g_vLightDir", tLoadConstsFromHere, m_tLightDir);
            bSuccess &= m_pShaderLoader->GetConstant("g_vMaterialAmbient", tLoadConstsFromHere, m_tLightAndMaterialAmbient);
            bSuccess &= m_pShaderLoader->GetConstant("g_vMaterialDiffuse", tLoadConstsFromHere, m_tLightAndMaterialDiffuse);
        }
        else
            CCore::SetError("CGrassRI::InitShader, technique 'Grass' failed to load");
    }
    else
        CCore::SetError("CGrassRI::InitShader, no shader loader was passed in from CForestRI");

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGrassRI::ShaderIsLoaded

CGrassRI_TemplateList
inline st_bool CGrassRI_t::ShaderIsLoaded(void) const
{
    // assume that if a shader constant is loaded that the shader has already been loaded
    return m_tUnitBillboard.IsValid( );
}


