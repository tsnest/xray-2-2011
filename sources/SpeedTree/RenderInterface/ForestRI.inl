///////////////////////////////////////////////////////////////////////  
//  ForestRI.inl
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
//  CForestRI::CForestRI

CForestRI_TemplateList
inline CForestRI_t::CForestRI( ) :
    m_eTextureAlphaMethod(TRANS_TEXTURE_ALPHA_TESTING),
    m_bTexturingEnabled(true),
    m_bQuadPrimitiveSupported(TGeometryBufferPolicy::IsPrimitiveTypeSupported(PRIMITIVE_QUADS))
{
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::~CForestRI

CForestRI_TemplateList
inline CForestRI_t::~CForestRI( )
{
    // these are hanging on to resources that are duplicated and freed elsewhere
    for (st_int32 i = 0; i < TL_NUM_TEX_LAYERS; ++i)
        m_atLastBoundTextures[i] = TTexturePolicy( );
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::InitTreeGraphics

CForestRI_TemplateList
inline st_bool CForestRI_t::InitTreeGraphics(CTreeRI_t* pTree, 
                                             st_int32 nMaxNumInstancesPerCell,
                                             st_bool bHorzBillboardsAllowed,
                                             const char* pTexturePath)

{
    st_bool bSuccess = false;

    // setup resource search paths
    CArray<CFixedString> aSearchPaths;
    if (pTree->GetFilename( ))
        aSearchPaths.push_back(CFixedString(pTree->GetFilename( )).Path( ));
    if (pTexturePath)
        aSearchPaths.push_back(pTexturePath);

    const TShaderTechniquePolicy* atTechniques[ ] = 
    { 
        &m_atBranchTechniques[SHADER_TYPE_STANDARD],
        &m_atFrondTechniques[SHADER_TYPE_STANDARD],
        &m_atLeafCardTechniques[SHADER_TYPE_STANDARD],
        &m_atLeafMeshTechniques[SHADER_TYPE_STANDARD],
        &m_atBillboardTechniques[SHADER_TYPE_STANDARD]
    };

    // load the tree's textures, build vertex and index buffers, etc.
    if (pTree->InitGraphics(aSearchPaths, m_bQuadPrimitiveSupported, atTechniques, m_sRenderInfo.m_nMaxAnisotropy))
    {
        if (pTree->InitBillboardSystem(nMaxNumInstancesPerCell, GetHint(HINT_MAX_NUM_VISIBLE_CELLS), bHorzBillboardsAllowed))
        {
            bSuccess = true;
        }
        else
            CCore::SetError("CForestRI::InitTreeGraphics, CTreeRI::InitBillboardSystem call failed");
    }
    else
        CCore::SetError("CForestRI::InitTreeGraphics, CTreeRI::InitGraphics call failed");

    return bSuccess;
}


#ifndef SPEEDTREE_HEAP_FRIENDLY_MODE

    ///////////////////////////////////////////////////////////////////////
    //  CForestRI::ReplaceTree

    CForestRI_TemplateList
    inline st_bool CForestRI_t::ReplaceTree(const CTreeRI_t* pOldTree, CTreeRI_t* pNewTree, const char* pTexturePath)
    {
        st_bool bSuccess = false;

        if (CForest::ReplaceTree(pOldTree, pNewTree))
        {
            if (!pNewTree->GraphicsAreInitialized( ))
            {
                // setup resource search paths
                CArray<CFixedString> aSearchPaths;
                if (pNewTree->GetFilename( ))
                    aSearchPaths.push_back(CFixedString(pNewTree->GetFilename( )).Path( ));
                if (pTexturePath)
                    aSearchPaths.push_back(pTexturePath);

                const TShaderTechniquePolicy* atTechniques[ ] = 
                { 
                    &m_atBranchTechniques[SHADER_TYPE_STANDARD],
                    &m_atFrondTechniques[SHADER_TYPE_STANDARD],
                    &m_atLeafCardTechniques[SHADER_TYPE_STANDARD],
                    &m_atLeafMeshTechniques[SHADER_TYPE_STANDARD] 
                };

                // load the tree's textures, build vertex and index buffers, etc.
                if (!pNewTree->InitGraphics(aSearchPaths, m_bQuadPrimitiveSupported, atTechniques, m_sRenderInfo.m_nMaxAnisotropy))
                    CCore::SetError("CForestRI::ReplaceTree, InitGraphics failed");
                else
                    bSuccess = true;
            }
            else
                bSuccess = true;
        }

        return bSuccess;
    }

#endif // !SPEEDTREE_HEAP_FRIENDLY_MODE


///////////////////////////////////////////////////////////////////////
//  CForestRI::AllocateTree

CForestRI_TemplateList
inline CTree* CForestRI_t::AllocateTree(void) const
{
    return st_new(CTreeRI_t, "CTree");
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::CullAndComputeLOD

CForestRI_TemplateList
inline void CForestRI_t::CullAndComputeLOD(const CView& cView, SForestCullResultsRI_t& sResults, st_bool bSort)
{
    CForest::CullAndComputeLOD(cView, sResults, bSort);
    m_sRenderStats.m_nNumCellsInFrustum = st_int32(sResults.m_aVisibleCells.size( ));

    UpdateBillboards(sResults);
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::SetRenderInfo

CForestRI_TemplateList
inline void CForestRI_t::SetRenderInfo(const SForestRenderInfo& sInfo)
{
    m_sRenderInfo = sInfo;

    EnableWind(m_sRenderInfo.m_bWindEnabled);
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::GetRenderInfo

CForestRI_TemplateList
inline const SForestRenderInfo& CForestRI_t::GetRenderInfo(void) const
{
    return m_sRenderInfo;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::InitGraphics

CForestRI_TemplateList
inline st_bool CForestRI_t::InitGraphics(st_bool bUsePrecompiledShaders)
{
    st_bool bSuccess = false;

    CForest::InitBillboardSystem( );

    // load all of the forest shaders & shader constants
    if (LoadShaders(bUsePrecompiledShaders))
    {
        bSuccess = true;

        // when textures fail to load, we use very small fallback textures like white and black
        InitFallbackTextures( );

        // cascaded shadow maps
        if (ShadowsAreEnabled( ))
        {
            for (st_int32 nShadowMap = 0; nShadowMap < m_sRenderInfo.m_nNumShadowMaps; ++nShadowMap)
            {
                if (!m_atShadowBuffers[nShadowMap].Create(m_sRenderInfo.m_nShadowMapResolution, m_sRenderInfo.m_nShadowMapResolution))
                {
                    CCore::SetError("Failed to create %dx%d shadow buffer #%d of %d", m_sRenderInfo.m_nShadowMapResolution, m_sRenderInfo.m_nShadowMapResolution, nShadowMap, m_sRenderInfo.m_nNumShadowMaps);
                    bSuccess = false;
                }
            }
        }

        // init overlays to show cascaded shadow maps; return value ignored since this is diagnostic only
        (void) InitOverlays( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::StartRender

CForestRI_TemplateList
inline st_bool CForestRI_t::StartRender(void)
{
    st_bool bSuccess = false;

    // upload global shader constants shared by all geometry, all passes; these values
    // are view-independent; view-dependent variables are uploaded with CForestRI::UploadViewShaderParameters
    {
        bSuccess = true;

        // shadow parameters
        bSuccess &= UploadShadowShaderConsts(m_tShadowSplitDistances, m_tShadowFadeParams, m_tFarClip);

        // fog
        bSuccess &= m_tFogParams.Set4f(m_sRenderInfo.m_fFogEndDistance, m_sRenderInfo.m_fFogEndDistance - m_sRenderInfo.m_fFogStartDistance, -m_sRenderInfo.m_fSkyFogMin, -1.0f / (m_sRenderInfo.m_fSkyFogMax - m_sRenderInfo.m_fSkyFogMin));
        bSuccess &= m_tFogColor.Set3fv(m_sRenderInfo.m_vFogColor);

        // sky
        bSuccess &= m_tSkyColor.Set3fv(m_sRenderInfo.m_vSkyColor);
        bSuccess &= m_tSunParams.Set3f(m_sRenderInfo.m_fSunSize, m_sRenderInfo.m_fSunSpreadExponent, m_sRenderInfo.m_fSunFogBloom);

        // user specifies light direction like DX/OpenGL, but the shader takes a negated value
        bSuccess &= m_tLightDir.Set3fv(-GetLightDir( ));

        bSuccess &= ClearBoundTextures( );
    }

    if (!bSuccess)
        CCore::SetError("CForestRI::StartRender() failed to set one or more shader constants");

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::EndRender

CForestRI_TemplateList
inline st_bool CForestRI_t::EndRender(void)
{
    CForest::FrameEnd( );

    return true;
}


////////////////////////////////////////////////////////////
//  CForestRI::UploadViewShaderParameters
//
//  These shader parameters are all dependent on the current view, so expect
//  this function to be called multiple times in a multipass render.

CForestRI_TemplateList
inline st_bool CForestRI_t::UploadViewShaderParameters(const CView& cView) const
{
    st_bool bSuccess = true;

    // update camera angles
    bSuccess &= m_tCameraAngles.Set2f(cView.GetCameraAzimuth( ), cView.GetCameraPitch( ));

    // upload other global shader variables
#ifdef SPEEDTREE_WORLD_TRANSLATE
    bSuccess &= m_tProjModelviewMatrix.SetMatrix(cView.GetCompositeNoTranslate( ));
#else
    bSuccess &= m_tProjModelviewMatrix.SetMatrix(cView.GetComposite( ));
#endif
    bSuccess &= m_tCameraPosition.Set3fv(cView.GetCameraPos( ));
    bSuccess &= m_tCameraDirection.Set3fv(cView.GetCameraDir( ));

    if (ShadowsAreEnabled( ))
        bSuccess &= m_tFarClip.Set1f(cView.GetFarClip( ));

    if (!bSuccess)
        CCore::SetError("CForestRI::UploadViewShaderParameters() failed to upload one or more shader constants");

    return bSuccess;
}


////////////////////////////////////////////////////////////
//  CForestRI::ClearBoundTextures

CForestRI_TemplateList
inline st_bool CForestRI_t::ClearBoundTextures(void)
{
    for (st_int32 i = 0; i < TL_NUM_TEX_LAYERS; ++i)
        m_atLastBoundTextures[i] = TTexturePolicy( );

    return true;
}



///////////////////////////////////////////////////////////////////////
//  CForestRI::BeginShadowMap

CForestRI_TemplateList
inline st_bool CForestRI_t::BeginShadowMap(st_int32 nShadowMap, const CView& cLightView)
{
    st_bool bSuccess = false;

    if (ShadowsAreEnabled( ))
    {
        assert(nShadowMap >= 0 && nShadowMap < m_sRenderInfo.m_nNumShadowMaps);

        if (m_atShadowBuffers[nShadowMap].SetAsTarget( ))
        {
            bSuccess = true;

            // shader constants
            bSuccess &= m_tLightViewMatricesArray.SetMatrixIndex(cLightView.GetComposite( ), nShadowMap);

            // render state changes for rendering into shadow map
            m_tRenderStatePolicy.SetColorMask(false, false, false, false);

            // only need to clear the depth buffer when rendering into the shadow map
            m_tRenderStatePolicy.ClearScreen(false, true);

            if (!bSuccess)
                CCore::SetError("CForestRI::BeginShadowMap failed to upload one or more shader constants");
        }
        else
            CCore::SetError("CForestRI::BeginShadowMap, SetAsTarget() failed");
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::EndShadowMap

CForestRI_TemplateList
inline st_bool CForestRI_t::EndShadowMap(st_int32 nShadowMap)
{
    st_bool bSuccess = false;

    if (ShadowsAreEnabled( ))
    {
        m_tRenderStatePolicy.SetPolygonOffset(0.0f, 0.0f);

        assert(nShadowMap >= 0 && nShadowMap < m_sRenderInfo.m_nNumShadowMaps);
        if (m_atShadowBuffers[nShadowMap].ReleaseAsTarget( ))
        {
            m_tRenderStatePolicy.SetColorMask(true, true, true, true);

            if (m_atShadowBuffers[nShadowMap].BindAsTexture( ) &&
                m_atShadowMaps[nShadowMap].SetTexture(m_atShadowBuffers[nShadowMap]))
            {
                bSuccess = true;
            }
            else
                CCore::SetError("CForestRI::EndShadowMap, Failed to bind shadow map as texture");
        }
        else
            CCore::SetError("CForestRI::EndShadowMap, ReleaseAsTarget() failed");
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::UploadShadowData

CForestRI_TemplateList
inline st_bool CForestRI_t::UploadShadowData(st_int32 nShadowMap)
{
    st_bool bSuccess = false;

    if (ShadowsAreEnabled( ))
    {
        assert(nShadowMap >= 0 && nShadowMap < m_sRenderInfo.m_nNumShadowMaps);
        bSuccess = m_atShadowBuffers[nShadowMap].BindAsTexture( ) && 
                   m_atShadowMaps[nShadowMap].SetTexture(m_atShadowBuffers[nShadowMap]);

        if (!bSuccess)
            CCore::SetError("CForestRI::UploadShadowData failed to bind one or more textures");
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::ShadowsAreEnabled

CForestRI_TemplateList
inline st_bool CForestRI_t::ShadowsAreEnabled(void) const
{
    return (m_sRenderInfo.m_nShadowMapResolution > 0);
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::GetShadowDepthTexture

CForestRI_TemplateList
inline TDepthTexturePolicy* CForestRI_t::GetShadowDepthTexture(st_int32 nShadowMap)
{
    return m_atShadowBuffers + nShadowMap;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::UploadShadowShaderConsts

CForestRI_TemplateList
inline st_bool CForestRI_t::UploadShadowShaderConsts(TShaderConstantPolicy tShadowSplitDistances, 
                                                         TShaderConstantPolicy tShadowFadeParams,
                                                         TShaderConstantPolicy tFarClip) const
{
    st_bool bSuccess = false;

    if (ShadowsAreEnabled( ))
    {
        bSuccess = true;

        // upload far clip value
        bSuccess &= tFarClip.Set1f(m_sRenderInfo.m_fVisibility);

        // when uploading the split distances, make sure the unused slots are ones, else the
        // shader technique we're using to efficiently pick the shadow map index will not work
        st_float32 afUploadedSplits[c_nMaxNumShadowMaps] = { 1.0f, 1.0f, 1.0f, 1.0f };
        const st_float32* c_fShadowSplits = GetCascadedShadowMapDistances( );
        assert(m_sRenderInfo.m_nNumShadowMaps <= c_nMaxNumShadowMaps);
        for (st_int32 i = 0; i < m_sRenderInfo.m_nNumShadowMaps; ++i)
            afUploadedSplits[i] = c_fShadowSplits[i + 1];

        if (m_sRenderInfo.m_nNumShadowMaps > 1) // m_tShadowSplitDistances not used unless more than one shadow map is used
            bSuccess &= tShadowSplitDistances.Set4fv(afUploadedSplits);

        // upload shadow fade parameters
        assert(m_sRenderInfo.m_nNumShadowMaps > 0);
        const st_float32 c_fShadowEndDist = afUploadedSplits[m_sRenderInfo.m_nNumShadowMaps - 1];
        st_float32 fSimpleFadePercent = m_fShadowFadePercentage * c_fShadowEndDist;
        st_float32 fTerrainAmbientOcclusionParam = 1.0f / (fSimpleFadePercent - afUploadedSplits[m_sRenderInfo.m_nNumShadowMaps - 1]); 
        st_float32 fShadowMapParam = 1.0f / (afUploadedSplits[m_sRenderInfo.m_nNumShadowMaps - 1] - fSimpleFadePercent); 
        bSuccess &= tShadowFadeParams.Set3f(fSimpleFadePercent, fTerrainAmbientOcclusionParam, fShadowMapParam);
    }
    else
        bSuccess = true; // if shadows are disabled, not uploading isn't a failure

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::RenderBranches

CForestRI_TemplateList
inline st_bool CForestRI_t::RenderBranches(const SForestCullResults& sVisibleTrees, ERenderPassType eRenderPass)
{
    st_bool bSuccess = false;

    // check early exit conditions
    if (!sVisibleTrees.m_pBaseTrees ||
        sVisibleTrees.m_aVisibleCells.empty( ))
        return true;

    // use the right shader, based on the type of rendering pass we're making (main, depth, shadow, etc.)
    TShaderTechniquePolicy& tRenderTechnique = PickTechnique(m_atBranchTechniques, eRenderPass);

    // bind the branch technique one time for rendering all of the 3D branches in the scene
    if (tRenderTechnique.Bind( ))
    {
        bSuccess = true;

        // it's possible that this particular view has 3D instances in it but they have no branch geometry due to
        // their type or LOD state; in the interest of efficiency, no geometry buffer state change will occur until
        // this bool is true
        st_bool bGeometryFormatSet = false;

        // run through the 3D trees, in order of type (minimize state changes)
        for (st_int32 nBaseTreeIndex = 0; nBaseTreeIndex < st_int32(sVisibleTrees.m_pBaseTrees->size( )); ++nBaseTreeIndex)
        {
            const CTreeRI_t* pBaseTree = (CTreeRI_t*) sVisibleTrees.m_pBaseTrees->at(nBaseTreeIndex);

            if (pBaseTree->HasGeometryType(GEOMETRY_TYPE_BRANCHES))
            {
                // get the list of 3D instances of this base type
                const TInstanceLodArray* pInstanceLods = sVisibleTrees.VisibleInstances(nBaseTreeIndex);
                if (pInstanceLods && !pInstanceLods->empty( ))
                {
                    st_int32 nNumInstances = st_int32(pInstanceLods->size( ));
                    SRenderStats::SGeometryStats& sStats = m_sRenderStats.m_aGeometries[STATS_CATEGORY_BRANCHES][eRenderPass];
        
                    // get the geometry buffer associated with this species (SRT file)
                    const TGeometryBufferPolicy* pGeometryBuffer = pBaseTree->GetGeometryBuffer(GEOMETRY_TYPE_BRANCHES);
                    assert(pGeometryBuffer);

                    // at this point, we know we have to draw some branch geometry, so let's set the geometry buffer
                    // format, but only do it once (bGeometryVisible prevents it from happening again)
                    if (!bGeometryFormatSet)
                    {
                        bSuccess &= pGeometryBuffer->EnableFormat( );
                        bGeometryFormatSet = true;
                    }

                    // bind the vertex & index buffers for this base tree
                    bSuccess &= pGeometryBuffer->BindVertexBuffer(EGeometryProcessingType(GEOMETRY_PROCESSING_BRANCHES + eRenderPass));
                    bSuccess &= pGeometryBuffer->BindIndexBuffer( );
                    ++sStats.m_uiNumVboBinds;

                    // if wind is enabled, upload the wind parameters
                    UploadBaseTreeParameters(pBaseTree);

                    const CLodRenderInfo* pLodRenderInfo = pBaseTree->GetLodRenderInfo(GEOMETRY_TYPE_BRANCHES);
                    assert(pLodRenderInfo);

                    const CArray<st_int32>* pMaterialsUsed = pLodRenderInfo->MaterialRefs( );
                    assert(pMaterialsUsed);

                    // record instance data for rendering stats
                    for (st_int32 i = 0; i < nNumInstances; ++i)
                    {
                        const SInstanceLod* pInstanceLod = &pInstanceLods->at(i);
                        if (pLodRenderInfo->LodHasGeometry(pInstanceLod->m_sLodSnapshot.m_nBranchLodIndex))
                            ++sStats.m_uiNumObjects;
                    }

                    for (CArray<st_int32>::const_iterator iMaterial = pMaterialsUsed->begin( ); iMaterial != pMaterialsUsed->end( ); ++iMaterial)
                    {
                        const st_int32 nMaterialId = *iMaterial;

                        // upload the material associated with the current material id
                        const SMaterialRI_t* pActiveMaterial = &pBaseTree->GetMaterials( )->at(nMaterialId);
                        st_int32 nNumTexturesBoundDuringUpload = UploadMaterial(pActiveMaterial);
                        if (nNumTexturesBoundDuringUpload > 0)
                        {
                            bSuccess &= tRenderTechnique.CommitTextures( );
                            sStats.m_uiNumTextureBinds += st_uint32(nNumTexturesBoundDuringUpload);
                        }

                        SetBackfaceStateByMaterial(pActiveMaterial, eRenderPass);

                        for (st_int32 nInstance = 0; nInstance < nNumInstances; ++nInstance)
                        {
                            const SInstanceLod* pInstanceLod = &pInstanceLods->at(nInstance);
                            const SLodSnapshot& sLod = pInstanceLod->m_sLodSnapshot;
                            if (sLod.m_nBranchLodIndex > -1)
                            {
                                const CInstance* pInstance = pInstanceLod->m_pInstance;
                                assert(pInstance);

                                // does this instance, at its current LOD, use the currently bound material?
                                const CLodRenderInfo::SRef* pRef = pLodRenderInfo->IsActive(sLod.m_nBranchLodIndex, nMaterialId);
                                if (pRef)
                                {
                                    // set instance's position & scale
                                    bSuccess &= m_tTreePosAndScale.Set4fv(pInstance->GetPosAndScale( ));

                                    // set instance's rotation & LOD info
                                    const st_float32* pRotationVector = pInstance->GetRotationVector( );
                                    bSuccess &= m_tTreeRotation.Set4f(pRotationVector[0], pRotationVector[1], pInstanceLod->m_afShaderLerps[GEOMETRY_TYPE_BRANCHES], pInstanceLod->m_fLod);

                                    // make sure all of the variables are updated
                                    bSuccess &= tRenderTechnique.CommitConstants( );

                                    // render the indexed triangle list
                                    bSuccess &= pGeometryBuffer->RenderIndexed(PRIMITIVE_TRIANGLES, 
                                                                               pRef->m_uiOffset, 
                                                                               pRef->m_uiLength, 
                                                                               pRef->m_uiMinIndex,
                                                                               pRef->m_uiNumVertices,
                                                                               EGeometryProcessingType(GEOMETRY_PROCESSING_BRANCHES + eRenderPass));
                                    ++sStats.m_uiNumDrawCalls;
                                    sStats.m_uiNumTriangles += pRef->m_uiLength / 3;
                                }
                            }
                        }
                    }

                    bSuccess &= pGeometryBuffer->UnBindVertexBuffer( );
                    bSuccess &= pGeometryBuffer->UnBindIndexBuffer( );
                }
            }
        }

        if (bGeometryFormatSet)
            bSuccess &= TGeometryBufferPolicy::DisableFormat( );

        bSuccess &= tRenderTechnique.UnBind( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::RenderFronds

CForestRI_TemplateList
inline st_bool CForestRI_t::RenderFronds(const SForestCullResults& sVisibleTrees, ERenderPassType eRenderPass)
{
    st_bool bSuccess = false;

    // check early exit conditions
    if (!sVisibleTrees.m_pBaseTrees ||
        sVisibleTrees.m_aVisibleCells.empty( ))
        return true;

    // use the right shader, based on the type of rendering pass we're making (main, depth, shadow, etc.)
    TShaderTechniquePolicy& tRenderTechnique = PickTechnique(m_atFrondTechniques, eRenderPass);

    // bind the frond technique one time for rendering all of the 3D fronds in the scene
    if (tRenderTechnique.Bind( ))
    {
        bSuccess = true;

        // it's possible that this particular view has 3D instances in it but they have no frond geometry due to
        // their type or LOD state; in the interest of efficiency, no geometry buffer state change will occur until
        // this bool is true
        st_bool bGeometryFormatSet = false;

        // run through the 3D trees, in order of type (minimize state changes)
        for (st_int32 nBaseTreeIndex = 0; nBaseTreeIndex < st_int32(sVisibleTrees.m_pBaseTrees->size( )); ++nBaseTreeIndex)
        {
            const CTreeRI_t* pBaseTree = (CTreeRI_t*) sVisibleTrees.m_pBaseTrees->at(nBaseTreeIndex);

            if (pBaseTree->HasGeometryType(GEOMETRY_TYPE_FRONDS))
            {
                // get the list of 3D instances of this base type
                const TInstanceLodArray* pInstanceLods = sVisibleTrees.VisibleInstances(nBaseTreeIndex);
                if (pInstanceLods && !pInstanceLods->empty( ))
                {
                    st_int32 nNumInstances = st_int32(pInstanceLods->size( ));
                    SRenderStats::SGeometryStats& sStats = m_sRenderStats.m_aGeometries[STATS_CATEGORY_FRONDS][eRenderPass];

                    // get the geometry buffer associate with this species (SRT file)
                    const TGeometryBufferPolicy* pGeometryBuffer = pBaseTree->GetGeometryBuffer(GEOMETRY_TYPE_FRONDS);
                    assert(pGeometryBuffer);

                    // at this point, we know we have to draw some frond geometry, so let's set the geometry buffer
                    // format, but only do it once (bGeometryVisible prevents it from happening again)
                    if (!bGeometryFormatSet)
                    {
                        bSuccess &= pGeometryBuffer->EnableFormat( );
                        bGeometryFormatSet = true;
                    }

                    // bind the vertex & index buffers for this base tree
                    bSuccess &= pGeometryBuffer->BindVertexBuffer(EGeometryProcessingType(GEOMETRY_PROCESSING_FRONDS + eRenderPass));
                    bSuccess &= pGeometryBuffer->BindIndexBuffer( );
                    ++sStats.m_uiNumVboBinds;

                    // if wind is enabled, upload the wind parameters
                    UploadBaseTreeParameters(pBaseTree);

                    const CLodRenderInfo* pLodRenderInfo = pBaseTree->GetLodRenderInfo(GEOMETRY_TYPE_FRONDS);
                    assert(pLodRenderInfo);

                    const CArray<st_int32>* pMaterialsUsed = pLodRenderInfo->MaterialRefs( );
                    assert(pMaterialsUsed);

                    // record instance data for rendering stats
                    for (st_int32 i = 0; i < nNumInstances; ++i)
                    {
                        const SInstanceLod* pInstanceLod = &pInstanceLods->at(i);
                        if (pLodRenderInfo->LodHasGeometry(pInstanceLod->m_sLodSnapshot.m_nFrondLodIndex))
                            ++sStats.m_uiNumObjects;
                    }

                    for (CArray<st_int32>::const_iterator iMaterial = pMaterialsUsed->begin( ); iMaterial != pMaterialsUsed->end( ); ++iMaterial)
                    {
                        const st_int32 nMaterialId = *iMaterial;

                        // upload the material associated with the current material id
                        const SMaterialRI_t* pActiveMaterial = &pBaseTree->GetMaterials( )->at(nMaterialId);
                        st_int32 nNumTexturesBoundDuringUpload = UploadMaterial(pActiveMaterial);
                        if (nNumTexturesBoundDuringUpload > 0)
                        {
                            bSuccess &= tRenderTechnique.CommitTextures( );
                            sStats.m_uiNumTextureBinds += st_uint32(nNumTexturesBoundDuringUpload);
                        }

                        SetBackfaceStateByMaterial(pActiveMaterial, eRenderPass);

                        for (st_int32 nInstance = 0; nInstance < nNumInstances; ++nInstance)
                        {
                            const SInstanceLod* pInstanceLod = &pInstanceLods->at(nInstance);
                            const SLodSnapshot& sLod = pInstanceLod->m_sLodSnapshot;
                            if (sLod.m_nFrondLodIndex > -1)
                            {
                                const CInstance* pInstance = pInstanceLod->m_pInstance;
                                assert(pInstance);

                                // does this instance, at its current LOD, use the currently bound material?
                                const CLodRenderInfo::SRef* pRef = pLodRenderInfo->IsActive(sLod.m_nFrondLodIndex, nMaterialId);
                                if (pRef)
                                {
                                    // set instance's position & scale
                                    bSuccess &= m_tTreePosAndScale.Set4fv(pInstance->GetPosAndScale( ));

                                    // set instance's rotation & LOD info
                                    const st_float32* pRotationVector = pInstance->GetRotationVector( );
                                    bSuccess &= m_tTreeRotation.Set4f(pRotationVector[0], pRotationVector[1], pInstanceLod->m_afShaderLerps[GEOMETRY_TYPE_FRONDS], pInstanceLod->m_fLod);

                                    // make sure all of the variables are updated
                                    bSuccess &= tRenderTechnique.CommitConstants( );

                                    // render the indexed triangle list
                                    bSuccess &= pGeometryBuffer->RenderIndexed(PRIMITIVE_TRIANGLES, 
                                                                               pRef->m_uiOffset, 
                                                                               pRef->m_uiLength, 
                                                                               pRef->m_uiMinIndex,
                                                                               pRef->m_uiNumVertices,
                                                                               EGeometryProcessingType(GEOMETRY_PROCESSING_FRONDS + eRenderPass));

                                    ++sStats.m_uiNumDrawCalls;
                                    sStats.m_uiNumTriangles += pRef->m_uiLength / 3;
                                }
                            }
                        }
                    }

                    bSuccess &= pGeometryBuffer->UnBindVertexBuffer( );
                    bSuccess &= pGeometryBuffer->UnBindIndexBuffer( );
                }
            }
        }

        if (bGeometryFormatSet)
            bSuccess &= TGeometryBufferPolicy::DisableFormat( );

        bSuccess &= tRenderTechnique.UnBind( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::RenderLeafMeshes

CForestRI_TemplateList
inline st_bool CForestRI_t::RenderLeafMeshes(const SForestCullResults& sVisibleTrees, ERenderPassType eRenderPass)
{
    st_bool bSuccess = false;

    // check early exit conditions
    if (!sVisibleTrees.m_pBaseTrees ||
        sVisibleTrees.m_aVisibleCells.empty( ))
        return true;

    // use the right shader, based on the type of rendering pass we're making (main, depth, shadow, etc.)
    TShaderTechniquePolicy& tRenderTechnique = PickTechnique(m_atLeafMeshTechniques, eRenderPass);

    // bind the leaf mesh technique one time for rendering all of the 3D leaf meshes in the scene
    if (tRenderTechnique.Bind( ))
    {
        bSuccess = true;

        // it's possible that this particular view has 3D instances in it but they have no leaf mesh geometry due to
        // their type or LOD state; in the interest of efficiency, no geometry buffer state change will occur until
        // this bool is true
        st_bool bGeometryFormatSet = false;

        // run through the 3D trees, in order of type (minimize state changes)
        for (st_int32 nBaseTreeIndex = 0; nBaseTreeIndex < st_int32(sVisibleTrees.m_pBaseTrees->size( )); ++nBaseTreeIndex)
        {
            const CTreeRI_t* pBaseTree = (CTreeRI_t*) sVisibleTrees.m_pBaseTrees->at(nBaseTreeIndex);

            if (pBaseTree->HasGeometryType(GEOMETRY_TYPE_LEAF_MESHES))
            {
                // get the list of 3D instances of this base type
                const TInstanceLodArray* pInstanceLods = sVisibleTrees.VisibleInstances(nBaseTreeIndex);
                if (pInstanceLods && !pInstanceLods->empty( ))
                {
                    st_int32 nNumInstances = st_int32(pInstanceLods->size( ));
                    SRenderStats::SGeometryStats& sStats = m_sRenderStats.m_aGeometries[STATS_CATEGORY_LEAF_MESHES][eRenderPass];

                    // get the geometry buffer associate with this species (SRT file)
                    const TGeometryBufferPolicy* pGeometryBuffer = pBaseTree->GetGeometryBuffer(GEOMETRY_TYPE_LEAF_MESHES);
                    assert(pGeometryBuffer);

                    // at this point, we know we have to draw some leaf mesh geometry, so let's set the geometry buffer
                    // format, but only do it once (bGeometryVisible prevents it from happening again)
                    if (!bGeometryFormatSet)
                    {
                        bSuccess &= pGeometryBuffer->EnableFormat( );
                        bGeometryFormatSet = true;
                    }

                    // bind the vertex & index buffers for this base tree
                    bSuccess &= pGeometryBuffer->BindVertexBuffer(EGeometryProcessingType(GEOMETRY_PROCESSING_LEAF_MESHES + eRenderPass));
                    bSuccess &= pGeometryBuffer->BindIndexBuffer( );
                    ++sStats.m_uiNumVboBinds;

                    // if wind is enabled, upload the wind parameters
                    UploadBaseTreeParameters(pBaseTree);

                    const CLodRenderInfo* pLodRenderInfo = pBaseTree->GetLodRenderInfo(GEOMETRY_TYPE_LEAF_MESHES);
                    assert(pLodRenderInfo);

                    const CArray<st_int32>* pMaterialsUsed = pLodRenderInfo->MaterialRefs( );
                    assert(pMaterialsUsed);

                    // record instance data for rendering stats
                    for (st_int32 i = 0; i < nNumInstances; ++i)
                    {
                        const SInstanceLod* pInstanceLod = &pInstanceLods->at(i);
                        if (pLodRenderInfo->LodHasGeometry(pInstanceLod->m_sLodSnapshot.m_nLeafMeshLodIndex))
                            ++sStats.m_uiNumObjects;
                    }

                    for (CArray<st_int32>::const_iterator iMaterial = pMaterialsUsed->begin( ); iMaterial != pMaterialsUsed->end( ); ++iMaterial)
                    {
                        const st_int32 nMaterialId = *iMaterial;

                        // upload the material associated with the current material id
                        const SMaterialRI_t* pActiveMaterial = &pBaseTree->GetMaterials( )->at(nMaterialId);
                        st_int32 nNumTexturesBoundDuringUpload = UploadMaterial(pActiveMaterial);
                        if (nNumTexturesBoundDuringUpload > 0)
                        {
                            bSuccess &= tRenderTechnique.CommitTextures( );
                            sStats.m_uiNumTextureBinds += st_uint32(nNumTexturesBoundDuringUpload);
                        }

                        SetBackfaceStateByMaterial(pActiveMaterial, eRenderPass);

                        for (st_int32 nInstance = 0; nInstance < nNumInstances; ++nInstance)
                        {
                            const SInstanceLod* pInstanceLod = &pInstanceLods->at(nInstance);

                            const SLodSnapshot& sLod = pInstanceLod->m_sLodSnapshot;
                            if (sLod.m_nLeafMeshLodIndex > -1)
                            {
                                const CInstance* pInstance = pInstanceLod->m_pInstance;
                                assert(pInstance);

                                // does this instance, at its current LOD, use the currently bound material?
                                const CLodRenderInfo::SRef* pRef = pLodRenderInfo->IsActive(sLod.m_nLeafMeshLodIndex, nMaterialId);
                                if (pRef)
                                {
                                    // set instance's position & scale
                                    bSuccess &= m_tTreePosAndScale.Set4fv(pInstance->GetPosAndScale( ));

                                    // set instance's rotation & LOD info
                                    const st_float32* pRotationVector = pInstance->GetRotationVector( );
                                    bSuccess &= m_tTreeRotation.Set4f(pRotationVector[0], pRotationVector[1], pInstanceLod->m_afShaderLerps[GEOMETRY_TYPE_LEAF_MESHES], pInstanceLod->m_fLod);

                                    // make sure all of the variables are updated
                                    bSuccess &= tRenderTechnique.CommitConstants( );

                                    // render the indexed triangle list
                                    bSuccess &= pGeometryBuffer->RenderIndexed(PRIMITIVE_TRIANGLES, 
                                                                               pRef->m_uiOffset, 
                                                                               pRef->m_uiLength, 
                                                                               pRef->m_uiMinIndex,
                                                                               pRef->m_uiNumVertices,
                                                                               EGeometryProcessingType(GEOMETRY_PROCESSING_LEAF_MESHES + eRenderPass));

                                    ++sStats.m_uiNumDrawCalls;
                                    sStats.m_uiNumTriangles += pRef->m_uiLength / 3;
                                }
                            }
                        }
                    }

                    bSuccess &= pGeometryBuffer->UnBindVertexBuffer( );
                    bSuccess &= pGeometryBuffer->UnBindIndexBuffer( );
                }
            }
        }

        if (bGeometryFormatSet)
            bSuccess &= TGeometryBufferPolicy::DisableFormat( );

        bSuccess &= tRenderTechnique.UnBind( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::RenderLeafCards

CForestRI_TemplateList
inline st_bool CForestRI_t::RenderLeafCards(const SForestCullResults& sVisibleTrees, ERenderPassType eRenderPass, const CView& cView)
{
    st_bool bSuccess = false;

    // check early exit conditions
    if (!sVisibleTrees.m_pBaseTrees ||
        sVisibleTrees.m_aVisibleCells.empty( ))
        return true;

    // use the right shader, based on the type of rendering pass we're making (main, depth, shadow, etc.)
    TShaderTechniquePolicy& tRenderTechnique = PickTechnique(m_atLeafCardTechniques, eRenderPass);

    // bind the leaf card technique one time for rendering all of the 3D leaf cards in the scene
    if (tRenderTechnique.Bind( ))
    {
        bSuccess = true;

        // since the cards face the camera, checking for backfacing triangles is a waste
        m_tRenderStatePolicy.SetFaceCulling(CULLTYPE_NONE);

        // upload a single matrix used by the leaf card shader to billboard every leaf card in the forest
        bSuccess &= m_tCameraFacingMatrix.SetMatrix(cView.GetCameraFacingMatrix( ));

        // it's possible that this particular view has 3D instances in it but they have no leaf card geometry due to
        // their type or LOD state; in the interest of efficiency, no geometry buffer state change will occur until
        // this bool is true
        st_bool bGeometryFormatSet = false;

        // run through the 3D trees, in order of type (minimize state changes)
        for (st_int32 nBaseTreeIndex = 0; nBaseTreeIndex < st_int32(sVisibleTrees.m_pBaseTrees->size( )); ++nBaseTreeIndex)
        {
            const CTreeRI_t* pBaseTree = (CTreeRI_t*) sVisibleTrees.m_pBaseTrees->at(nBaseTreeIndex);

            if (pBaseTree->HasGeometryType(GEOMETRY_TYPE_LEAF_CARDS))
            {
                // get the list of 3D instances of this base type
                const TInstanceLodArray* pInstanceLods = sVisibleTrees.VisibleInstances(nBaseTreeIndex);
                if (pInstanceLods && !pInstanceLods->empty( ))
                {
                    st_int32 nNumInstances = st_int32(pInstanceLods->size( ));
                    SRenderStats::SGeometryStats& sStats = m_sRenderStats.m_aGeometries[STATS_CATEGORY_LEAF_CARDS][eRenderPass];

                    // get the geometry buffer associate with this species (SRT file)
                    const TGeometryBufferPolicy* pGeometryBuffer = pBaseTree->GetGeometryBuffer(GEOMETRY_TYPE_LEAF_CARDS);
                    assert(pGeometryBuffer);

                    // at this point, we know we have to draw some leaf card geometry, so let's set the geometry buffer
                    // format, but only do it once (bGeometryVisible prevents it from happening again)
                    if (!bGeometryFormatSet)
                    {
                        bSuccess &= pGeometryBuffer->EnableFormat( );
                        bGeometryFormatSet = true;
                    }

                    // bind the vertex & index buffers for this base tree
                    bSuccess &= pGeometryBuffer->BindVertexBuffer(EGeometryProcessingType(GEOMETRY_PROCESSING_LEAF_CARDS + eRenderPass));
                    if (!m_bQuadPrimitiveSupported)
                        bSuccess &= pGeometryBuffer->BindIndexBuffer( );

                    ++sStats.m_uiNumVboBinds;

                    // if wind is enabled, upload the wind parameters
                    UploadBaseTreeParameters(pBaseTree);

                    const CLodRenderInfo* pLodRenderInfo = pBaseTree->GetLodRenderInfo(GEOMETRY_TYPE_LEAF_CARDS);
                    assert(pLodRenderInfo);

                    const CArray<st_int32>* pMaterialsUsed = pLodRenderInfo->MaterialRefs( );
                    assert(pMaterialsUsed);

                    // record instance data for rendering stats
                    for (st_int32 i = 0; i < nNumInstances; ++i)
                    {
                        const SInstanceLod* pInstanceLod = &pInstanceLods->at(i);
                        if (pLodRenderInfo->LodHasGeometry(pInstanceLod->m_sLodSnapshot.m_nLeafCardLodIndex))
                            ++sStats.m_uiNumObjects;
                    }

                    for (CArray<st_int32>::const_iterator iMaterial = pMaterialsUsed->begin( ); iMaterial != pMaterialsUsed->end( ); ++iMaterial)
                    {
                        const st_int32 nMaterialId = *iMaterial;

                        // upload the material associated with the current material id
                        st_int32 nNumTexturesBoundDuringUpload = UploadMaterial(&pBaseTree->GetMaterials( )->at(nMaterialId));
                        if (nNumTexturesBoundDuringUpload > 0)
                        {
                            bSuccess &= tRenderTechnique.CommitTextures( );
                            sStats.m_uiNumTextureBinds += st_uint32(nNumTexturesBoundDuringUpload);
                        }

                        for (st_int32 nInstance = 0; nInstance < nNumInstances; ++nInstance)
                        {
                            const SInstanceLod* pInstanceLod = &pInstanceLods->at(nInstance);
                            const SLodSnapshot& sLod = pInstanceLod->m_sLodSnapshot;
                            if (sLod.m_nLeafCardLodIndex > -1)
                            {
                                const CInstance* pInstance = pInstanceLod->m_pInstance;
                                assert(pInstance);

                                // does this instance, at its current LOD, use the currently bound material?
                                const CLodRenderInfo::SRef* pRef = pLodRenderInfo->IsActive(sLod.m_nLeafCardLodIndex, nMaterialId);
                                if (pRef)
                                {
                                    // set instance's position & scale
                                    bSuccess &= m_tTreePosAndScale.Set4fv(pInstance->GetPosAndScale( ));

                                    // set instance's rotation & LOD info
                                    const st_float32* pRotationVector = pInstance->GetRotationVector( );
                                    bSuccess &= m_tTreeRotation.Set4f(pRotationVector[0], pRotationVector[1], pInstanceLod->m_afShaderLerps[GEOMETRY_TYPE_LEAF_CARDS], pInstanceLod->m_fLod);

                                    // make sure all of the variables are updated
                                    bSuccess &= tRenderTechnique.CommitConstants( );

                                    // render the indexed triangle list
                                    if (m_bQuadPrimitiveSupported)
                                        bSuccess &= pGeometryBuffer->RenderArrays(PRIMITIVE_QUADS, pRef->m_uiOffset, pRef->m_uiLength, EGeometryProcessingType(GEOMETRY_PROCESSING_LEAF_CARDS + eRenderPass));
                                    else
                                        bSuccess &= pGeometryBuffer->RenderIndexed(PRIMITIVE_TRIANGLES, 
                                                                                   pRef->m_uiOffset, 
                                                                                   pRef->m_uiLength, 
                                                                                   pRef->m_uiMinIndex,
                                                                                   pRef->m_uiNumVertices,
                                                                                   EGeometryProcessingType(GEOMETRY_PROCESSING_LEAF_CARDS + eRenderPass));

                                    ++sStats.m_uiNumDrawCalls;
                                    sStats.m_uiNumTriangles += pRef->m_uiLength / 3;
                                }
                            }
                        }
                    }

                    bSuccess &= pGeometryBuffer->UnBindVertexBuffer( );
                    if (!m_bQuadPrimitiveSupported)
                        bSuccess &= pGeometryBuffer->UnBindIndexBuffer( );
                }
            }
        }

        if (bGeometryFormatSet)
            bSuccess &= TGeometryBufferPolicy::DisableFormat( );

        bSuccess &= tRenderTechnique.UnBind( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::EnableTexturing

CForestRI_TemplateList
inline void CForestRI_t::EnableTexturing(st_bool bFlag)
{
    m_bTexturingEnabled = bFlag;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::TransparentTextureModeNotify

CForestRI_TemplateList
inline void CForestRI_t::TransparentTextureModeNotify(ETextureAlphaRenderMode eMode)
{
    m_eTextureAlphaMethod = eMode;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::GetVertexBufferStats

CForestRI_TemplateList
inline st_bool CForestRI_t::GetVertexBufferStats(SForestVertexBufferStats& sStats) const
{
    // reset stats
    sStats = SForestVertexBufferStats( );

    // gather tree stats
    const TTreeArray& aBaseTrees = GetBaseTrees( );
    for (st_int32 nBaseTree = 0; nBaseTree < st_int32(aBaseTrees.size( )); ++nBaseTree)
    {
        const CTreeRI_t* pBaseTree = (const CTreeRI_t*) aBaseTrees[nBaseTree];

        for (st_int32 nType = GEOMETRY_TYPE_BRANCHES; nType < GEOMETRY_TYPE_NUM_3D_TYPES; ++nType)
        {
            const TGeometryBufferPolicy* pGeometryBuffer = pBaseTree->GetGeometryBuffer(EGeometryType(nType));
            sStats.m_asiTreeVBs[nType] += pGeometryBuffer->VertexSize( ) * pGeometryBuffer->NumVertices( );
            sStats.m_asiTreeIBs[nType] += pGeometryBuffer->IndexSize( ) * pGeometryBuffer->NumIndices( );

            sStats.m_siTotalTreeVBs += pGeometryBuffer->VertexSize( ) * pGeometryBuffer->NumVertices( );
            sStats.m_siTotalTreeIBs += pGeometryBuffer->IndexSize( ) * pGeometryBuffer->NumIndices( );
        }

        // billboard system
        {
            // vertex buffer contribution
            sStats.m_siBillboardVBs += pBaseTree->GetBillboardVboNumVertices( ) * sizeof(SBillboardVertex);

            // index buffer contribution
            sStats.m_siBillboardIBs += pBaseTree->GetBillboardIboNumIndices( ) * 2; // 16-bit indices
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::GetRenderStats

CForestRI_TemplateList
inline SRenderStats& CForestRI_t::GetRenderStats(void)
{
    return m_sRenderStats;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::GetShaderLoader

CForestRI_TemplateList
inline TShaderLoaderPolicy* CForestRI_t::GetShaderLoader(void)
{
    return &m_tShaderLoader;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::LoadShaders
//
//  Shader load failure is considered fatal.  LoadShaders() will only return
//  true upon complete success of loading all shaders & shader constants.

CForestRI_TemplateList
inline st_bool CForestRI_t::LoadShaders(st_bool bUsePrecompiledShaders)
{
    st_bool bSuccess = false;

    // attempt to keep heap allocations at a minimum
    m_aDefines.reserve(30);
    m_aDefines.resize(0);

    // upload hints about the graphics API
    #if defined(SPEEDTREE_OPENGL) || defined(__CELLOS_LV2__)
        m_aDefines.push_back("SPEEDTREE_OPENGL");
    #endif
    #if defined(SPEEDTREE_DIRECTX9) || defined(_XBOX)
        m_aDefines.push_back("SPEEDTREE_DIRECTX9");
    #endif
    #ifdef SPEEDTREE_DIRECTX10
        m_aDefines.push_back("SPEEDTREE_DIRECTX10");
    #endif
    m_aDefines.push_back("");

    // upload hints about the platform
    #ifdef _XBOX
        m_aDefines.push_back("_XBOX");
        m_aDefines.push_back("");
        m_aDefines.push_back("SPEEDTREE_XBOX_360");
        m_aDefines.push_back("");
    #endif
    #ifdef __CELLOS_LV2__
        m_aDefines.push_back("SPEEDTREE_PS3");
        m_aDefines.push_back("");
    #endif

    // upload information about the user's coordinate system
    if (CCoordSys::IsLeftHanded( ))
        m_aDefines.push_back("SPEEDTREE_LHCS");
    else
        m_aDefines.push_back("SPEEDTREE_RHCS");
    m_aDefines.push_back("");
    if (fabs(CCoordSys::UpAxis( ).y) > 0.0f)
        m_aDefines.push_back("SPEEDTREE_Y_UP");
    else
        m_aDefines.push_back("SPEEDTREE_Z_UP");
    m_aDefines.push_back("");

    #ifdef SPEEDTREE_WORLD_TRANSLATE
        m_aDefines.push_back("SPEEDTREE_WORLD_TRANSLATE");
        m_aDefines.push_back("");
    #endif

    #ifdef SPEEDTREE_ADVANCED_WIND
        m_aDefines.push_back("SPEEDTREE_ADVANCED_WIND");
        m_aDefines.push_back("");
    #endif

    // shadow map resolution
    if (ShadowsAreEnabled( ))
    {
        m_aDefines.push_back("SPEEDTREE_SHADOW_MAP_RES");
        m_aDefines.push_back(CFixedString::Format("%d", m_sRenderInfo.m_nShadowMapResolution));

        m_aDefines.push_back("SPEEDTREE_NUM_SHADOW_MAPS");
        m_aDefines.push_back(CFixedString::Format("%d", m_sRenderInfo.m_nNumShadowMaps));
    }

    m_aDefines.push_back("SPEEDTREE_SMOOTH_SHADOWS");
    m_aDefines.push_back(m_sRenderInfo.m_bSmoothShadows ? "true" : "false");

    // max billboard images
    m_aDefines.push_back("SPEEDTREE_MAX_BILLBOARD_IMAGES");
    m_aDefines.push_back(CFixedString::Format("%d", m_sRenderInfo.m_nMaxBillboardImagesByBase));

    if (m_sRenderInfo.m_bZPrePass)
    {
        m_aDefines.push_back("SPEEDTREE_Z_PREPASS");
        m_aDefines.push_back("");
    }

    if (IsWindEnabled( ))
    {
        m_aDefines.push_back("SPEEDTREE_DIRECTIONAL_WIND");
        m_aDefines.push_back("");
    }

    if (m_sRenderInfo.m_bFrondRippling)
    {
        m_aDefines.push_back("SPEEDTREE_FROND_RIPPLING");
        m_aDefines.push_back("");
    }

    if (m_sRenderInfo.m_bShowShadowSplitsOnTerrain)
    {
        m_aDefines.push_back("SPEEDTREE_TERRAIN_SHOW_SPLITS");
        m_aDefines.push_back("");
    }

    if (m_sRenderInfo.m_nMaxAnisotropy > 0)
    {
        m_aDefines.push_back("SPEEDTREE_MAX_ANISOTROPY");
        m_aDefines.push_back(CFixedString::Format("%d", m_sRenderInfo.m_nMaxAnisotropy));
    }

    if (m_eTextureAlphaMethod == TRANS_TEXTURE_ALPHA_TESTING)
    {
        m_aDefines.push_back("SPEEDTREE_ALPHA_TESTING");
        m_aDefines.push_back("");
    }

    if (m_sRenderInfo.m_bHorizontalBillboards)
    {
        m_aDefines.push_back("SPEEDTREE_HORZ_BBS_ACTIVE");
        m_aDefines.push_back("");
    }

    // lighting
    {
        m_aDefines.push_back("SPEEDTREE_SPECULAR_LIGHTING");
        m_aDefines.push_back(m_sRenderInfo.m_bSpecularLighting ? "true" : "false");

        m_aDefines.push_back("SPEEDTREE_TRANSMISSION_LIGHTING");
        m_aDefines.push_back(m_sRenderInfo.m_bTransmissionLighting ? "true" : "false");

        m_aDefines.push_back("SPEEDTREE_AMBIENT_CONTRAST");
        m_aDefines.push_back(m_sRenderInfo.m_bAmbientContrast ? "true" : "false");

        m_aDefines.push_back("SPEEDTREE_DETAIL_TEXTURE");
        m_aDefines.push_back(m_sRenderInfo.m_bDetailLayer ? "true" : "false");

        m_aDefines.push_back("SPEEDTREE_DETAIL_NORMAL_MAPPING");
        m_aDefines.push_back(m_sRenderInfo.m_bDetailNormalMapping ? "true" : "false");
    }

    if (m_tShaderLoader.Init(m_aDefines, (m_sRenderInfo.m_strShaderPath + "Effects.fx").c_str( ), bUsePrecompiledShaders))
    {
        bSuccess = true;

        // determine shader filenames
        CFixedString strBranchShaderFilename = m_sRenderInfo.m_strShaderPath + "Branch.hlsl";
        CFixedString strFrondShaderFilename = m_sRenderInfo.m_strShaderPath + "Frond.hlsl";
        CFixedString strLeafMeshShaderFilename = m_sRenderInfo.m_strShaderPath + "LeafMesh.hlsl";
        CFixedString strLeafCardShaderFilename = m_sRenderInfo.m_strShaderPath + "LeafCard.hlsl";
        CFixedString strBillboardShaderFilename = m_sRenderInfo.m_strShaderPath + "Billboard.hlsl";

        // standard shaders
        bSuccess &= m_tShaderLoader.GetTechnique(strBranchShaderFilename.c_str( ), "BranchVertex", "BranchPixel", "Branch", m_atBranchTechniques[SHADER_TYPE_STANDARD]);
        bSuccess &= m_tShaderLoader.GetTechnique(strFrondShaderFilename.c_str( ), "FrondVertex", "FrondPixel", "Frond", m_atFrondTechniques[SHADER_TYPE_STANDARD]);
        bSuccess &= m_tShaderLoader.GetTechnique(strLeafMeshShaderFilename.c_str( ), "LeafMeshVertex", "LeafMeshPixel", "LeafMesh", m_atLeafMeshTechniques[SHADER_TYPE_STANDARD]);
        bSuccess &= m_tShaderLoader.GetTechnique(strLeafCardShaderFilename.c_str( ), "LeafCardVertex", "LeafCardPixel", "LeafCard", m_atLeafCardTechniques[SHADER_TYPE_STANDARD]);
        bSuccess &= m_tShaderLoader.GetTechnique(strBillboardShaderFilename.c_str( ), "BillboardVertex", "BillboardPixel", "Billboard", m_atBillboardTechniques[SHADER_TYPE_STANDARD]);

        // depth-only shaders
        if (m_sRenderInfo.m_bZPrePass)
        {
            bSuccess &= m_tShaderLoader.GetTechnique(strBranchShaderFilename.c_str( ), "BranchVertex_Depth", "BranchPixel_Depth", "Branch_Depth", m_atBranchTechniques[SHADER_TYPE_DEPTH_ONLY]);
            bSuccess &= m_tShaderLoader.GetTechnique(strFrondShaderFilename.c_str( ), "FrondVertex_Depth", "FrondPixel_Depth", "Frond_Depth", m_atFrondTechniques[SHADER_TYPE_DEPTH_ONLY]);

            #ifdef SPEEDTREE_XBOX_360
                // 360 platform: having some trouble getting the output of LeafMesh_Depth to match LeafMesh when wind is on; use less efficient full shader for depth pass as workaround
                bSuccess &= m_tShaderLoader.GetTechnique(strLeafMeshShaderFilename.c_str( ), "LeafMeshVertex", "LeafMeshPixel", "LeafMesh_Depth", m_atLeafMeshTechniques[SHADER_TYPE_DEPTH_ONLY]);
            #else
                bSuccess &= m_tShaderLoader.GetTechnique(strLeafMeshShaderFilename.c_str( ), "LeafMeshVertex_Depth", "LeafMeshPixel_Depth", "LeafMesh_Depth", m_atLeafMeshTechniques[SHADER_TYPE_DEPTH_ONLY]);
            #endif

            bSuccess &= m_tShaderLoader.GetTechnique(strLeafCardShaderFilename.c_str( ), "LeafCardVertex_Depth", "LeafCardPixel_Depth", "LeafCard_Depth", m_atLeafCardTechniques[SHADER_TYPE_DEPTH_ONLY]);
            bSuccess &= m_tShaderLoader.GetTechnique(strBillboardShaderFilename.c_str( ), "BillboardVertex_Depth", "BillboardPixel_Depth", "Billboard_Depth", m_atBillboardTechniques[SHADER_TYPE_DEPTH_ONLY]);
        }

        // shadow shaders
        if (ShadowsAreEnabled( ))
        {
            bSuccess &= m_tShaderLoader.GetTechnique(strBranchShaderFilename.c_str( ), "BranchVertex_Shadow", "BranchPixel_Shadow", "Branch_Shadow", m_atBranchTechniques[SHADER_TYPE_SHADOW]);
            bSuccess &= m_tShaderLoader.GetTechnique(strFrondShaderFilename.c_str( ), "FrondVertex_Shadow", "FrondPixel_Shadow", "Frond_Shadow", m_atFrondTechniques[SHADER_TYPE_SHADOW]);
            bSuccess &= m_tShaderLoader.GetTechnique(strLeafMeshShaderFilename.c_str( ), "LeafMeshVertex_Shadow", "LeafMeshPixel_Shadow", "LeafMesh_Shadow", m_atLeafMeshTechniques[SHADER_TYPE_SHADOW]);
            bSuccess &= m_tShaderLoader.GetTechnique(strLeafCardShaderFilename.c_str( ), "LeafCardVertex_Shadow", "LeafCardPixel_Shadow", "LeafCard_Shadow", m_atLeafCardTechniques[SHADER_TYPE_SHADOW]);
        }

        // overlay
        bSuccess &= SetupOverlayShader( );

        // load the shared global variables
        bSuccess &= QueryUniformVariables( );
    }
    else
        CCore::SetError("CForestRI::LoadShaders, shader system failed to initialize");

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::QueryUniformVariables

CForestRI_TemplateList
inline st_bool CForestRI_t::QueryUniformVariables(void)
{
    st_bool bSuccess = false;

    const TShaderTechniquePolicy& tLoadConstsFromBranches = m_atBranchTechniques[SHADER_TYPE_STANDARD];
    const TShaderTechniquePolicy& tLoadConstsFromFronds = m_atFrondTechniques[SHADER_TYPE_STANDARD];
    const TShaderTechniquePolicy& tLoadConstsFromLeafCards = m_atLeafCardTechniques[SHADER_TYPE_STANDARD];
    const TShaderTechniquePolicy& tLoadConstsFromBillboards = m_atBillboardTechniques[SHADER_TYPE_STANDARD];

    // main global shader constants, shared by more than one shader
    bSuccess  = m_tShaderLoader.GetConstant("g_mModelViewProj", tLoadConstsFromBranches, m_tProjModelviewMatrix, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_mCameraFacingMatrix", tLoadConstsFromLeafCards, m_tCameraFacingMatrix, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vTreePosAndScale", tLoadConstsFromBranches, m_tTreePosAndScale, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vTreeRotation", tLoadConstsFromBranches, m_tTreeRotation, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vLodProfile", tLoadConstsFromBranches, m_tLodProfile, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vMaterialAmbient", tLoadConstsFromFronds, m_tMaterialAmbient, CONST_LOCATION_PIXEL);
    bSuccess &= m_tShaderLoader.GetConstant("g_vMaterialDiffuse", tLoadConstsFromFronds, m_tMaterialDiffuse, CONST_LOCATION_PIXEL);
    if (m_sRenderInfo.m_bSpecularLighting)
        bSuccess &= m_tShaderLoader.GetConstant("g_vMaterialSpecular", tLoadConstsFromFronds, m_tMaterialSpecular, CONST_LOCATION_PIXEL);
    if (m_sRenderInfo.m_bTransmissionLighting)
    {
        bSuccess &= m_tShaderLoader.GetConstant("g_vMaterialEmissive", tLoadConstsFromFronds, m_tMaterialEmissive, CONST_LOCATION_PIXEL);
        bSuccess &= m_tShaderLoader.GetConstant("g_vMaterialTransmission", tLoadConstsFromLeafCards, m_tMaterialTransmission, CONST_LOCATION_BOTH);
    }
    bSuccess &= m_tShaderLoader.GetConstant("g_vMaterialExtras", tLoadConstsFromBranches, m_tMaterialExtras, CONST_LOCATION_BOTH);
    bSuccess &= m_tShaderLoader.GetConstant("g_vMaterialExtras", tLoadConstsFromBillboards, m_tMaterialExtrasBBs, CONST_LOCATION_BOTH);
    bSuccess &= m_tShaderLoader.GetConstant("g_vLightDir", tLoadConstsFromBranches, m_tLightDir, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vFogParams", tLoadConstsFromBranches, m_tFogParams, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vFogColor", tLoadConstsFromBranches, m_tFogColor, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vCameraAngles", tLoadConstsFromBillboards, m_tCameraAngles, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vCameraPosition", tLoadConstsFromBranches, m_tCameraPosition, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vCameraDirection", tLoadConstsFromBranches, m_tCameraDirection, CONST_LOCATION_VERTEX);

    // wind
    if (IsWindEnabled( ))
    {
        bSuccess &= m_tShaderLoader.GetConstant("g_vWindDir", tLoadConstsFromFronds, m_tWindDir, CONST_LOCATION_VERTEX);
        bSuccess &= m_tShaderLoader.GetConstant("g_vWindTimes", tLoadConstsFromFronds, m_tWindTimes, CONST_LOCATION_VERTEX);
        bSuccess &= m_tShaderLoader.GetConstant("g_vWindDistances", tLoadConstsFromFronds, m_tWindDistances, CONST_LOCATION_VERTEX);
        bSuccess &= m_tShaderLoader.GetConstant("g_vWindLeaves", tLoadConstsFromLeafCards, m_tWindLeaves, CONST_LOCATION_VERTEX);
        bSuccess &= m_tShaderLoader.GetConstant("g_vWindGust", tLoadConstsFromFronds, m_tWindGust, CONST_LOCATION_VERTEX);
        bSuccess &= m_tShaderLoader.GetConstant("g_vWindGustHints", tLoadConstsFromFronds, m_tWindGustHints, CONST_LOCATION_VERTEX);
        if (m_sRenderInfo.m_bFrondRippling)
            bSuccess &= m_tShaderLoader.GetConstant("g_vWindFrondRipple", tLoadConstsFromFronds, m_tWindFrondRipple, CONST_LOCATION_VERTEX);

        #ifdef SPEEDTREE_ADVANCED_WIND
            const TShaderTechniquePolicy& tLoadConstsFromLeafMeshes = m_atLeafMeshTechniques[SHADER_TYPE_STANDARD];
            bSuccess &= m_tShaderLoader.GetConstant("g_vWindRollingBranches", tLoadConstsFromLeafMeshes, m_tWindRollingBranches, CONST_LOCATION_VERTEX);
            bSuccess &= m_tShaderLoader.GetConstant("g_vWindRollingLeaves", tLoadConstsFromLeafMeshes, m_tWindRollingLeaves, CONST_LOCATION_VERTEX);
            bSuccess &= m_tShaderLoader.GetConstant("g_vWindTwitchingLeaves", tLoadConstsFromLeafMeshes, m_tWindTwitchingLeaves, CONST_LOCATION_VERTEX);
            bSuccess &= m_tShaderLoader.GetConstant("g_vWindTumblingLeaves", tLoadConstsFromLeafMeshes, m_tWindTumblingLeaves, CONST_LOCATION_VERTEX);
        #endif
    }

    // billboards
    bSuccess &= m_tShaderLoader.GetConstant("g_fNum360Images", tLoadConstsFromBillboards, m_tNum360Images, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vCameraAzimuthTrig", tLoadConstsFromBillboards, m_tCameraAzimuthTrig, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_av360TexCoords", tLoadConstsFromBillboards, m_t360BillboardTexCoords, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vVertBillboardDimensions", tLoadConstsFromBillboards, m_tVertBillboardDimensions, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_avBillboardTangents", tLoadConstsFromBillboards, m_tBillboardTangents, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_fHorzBillboardFade", tLoadConstsFromBillboards, m_tHorzBillboardFade, CONST_LOCATION_VERTEX);
    if (m_sRenderInfo.m_bHorizontalBillboards)
    {
        bSuccess &= m_tShaderLoader.GetConstant("g_vHorzBillboardTexCoords", tLoadConstsFromBillboards, m_tHorzBillboardTexCoords, CONST_LOCATION_VERTEX);
        bSuccess &= m_tShaderLoader.GetConstant("g_vHorzBillboardDimensions", tLoadConstsFromBillboards, m_tHorzBillboardDimensions, CONST_LOCATION_VERTEX);
    }

    // sky
    bSuccess &= m_tShaderLoader.GetConstant("g_vSkyColor", tLoadConstsFromBranches, m_tSkyColor, CONST_LOCATION_VERTEX);
    bSuccess &= m_tShaderLoader.GetConstant("g_vSunParams", tLoadConstsFromBranches, m_tSunParams, CONST_LOCATION_VERTEX);

    // shadows
    if (ShadowsAreEnabled( ))
    {
        bSuccess &= m_tShaderLoader.GetConstant("g_fFarClip", tLoadConstsFromBranches, m_tFarClip, CONST_LOCATION_VERTEX);
        bSuccess &= m_tShaderLoader.GetConstant("g_amLightViewProjs", tLoadConstsFromBranches, m_tLightViewMatricesArray, CONST_LOCATION_VERTEX);
        bSuccess &= m_tShaderLoader.GetConstant("g_vShadowFadeParams", tLoadConstsFromBranches, m_tShadowFadeParams, CONST_LOCATION_PIXEL);

        // not used unless more than one shadow map is used
        if (m_sRenderInfo.m_nNumShadowMaps > 1)
            bSuccess &= m_tShaderLoader.GetConstant("g_vSplitDistances", tLoadConstsFromBranches, m_tShadowSplitDistances, CONST_LOCATION_PIXEL);

        // shadow maps
        bSuccess &= m_tShaderLoader.GetConstant(m_tShaderLoader.GetTextureConstantName("ShadowMap0").c_str( ), tLoadConstsFromBranches, m_atShadowMaps[0], CONST_LOCATION_PIXEL);
        if (m_sRenderInfo.m_nNumShadowMaps > 1)
            bSuccess &= m_tShaderLoader.GetConstant(m_tShaderLoader.GetTextureConstantName("ShadowMap1").c_str( ), tLoadConstsFromBranches, m_atShadowMaps[1], CONST_LOCATION_PIXEL);
        if (m_sRenderInfo.m_nNumShadowMaps > 2)
            bSuccess &= m_tShaderLoader.GetConstant(m_tShaderLoader.GetTextureConstantName("ShadowMap2").c_str( ), tLoadConstsFromBranches, m_atShadowMaps[2], CONST_LOCATION_PIXEL);
        if (m_sRenderInfo.m_nNumShadowMaps > 3)
            bSuccess &= m_tShaderLoader.GetConstant(m_tShaderLoader.GetTextureConstantName("ShadowMap3").c_str( ), tLoadConstsFromBranches, m_atShadowMaps[3], CONST_LOCATION_PIXEL);
    }

    // textures
    bSuccess &= m_tShaderLoader.GetConstant(m_tShaderLoader.GetTextureConstantName("Diffuse").c_str( ), tLoadConstsFromBranches, m_atTextures[TL_DIFFUSE], CONST_LOCATION_PIXEL);
    bSuccess &= m_tShaderLoader.GetConstant(m_tShaderLoader.GetTextureConstantName("Normal").c_str( ), tLoadConstsFromBranches, m_atTextures[TL_NORMAL], CONST_LOCATION_PIXEL);
    if (m_sRenderInfo.m_bSpecularLighting)
        bSuccess &= m_tShaderLoader.GetConstant(m_tShaderLoader.GetTextureConstantName("Specular").c_str( ), tLoadConstsFromBranches, m_atTextures[TL_SPECULAR], CONST_LOCATION_PIXEL);
    if (m_sRenderInfo.m_bDetailLayer)
        bSuccess &= m_tShaderLoader.GetConstant(m_tShaderLoader.GetTextureConstantName("DetailDiffuse").c_str( ), tLoadConstsFromBranches, m_atTextures[TL_DETAIL_DIFFUSE], CONST_LOCATION_PIXEL);
    if (m_sRenderInfo.m_bDetailNormalMapping)
        bSuccess &= m_tShaderLoader.GetConstant(m_tShaderLoader.GetTextureConstantName("DetailNormal").c_str( ), tLoadConstsFromBranches, m_atTextures[TL_DETAIL_NORMAL], CONST_LOCATION_PIXEL);

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::PickTechnique

CForestRI_TemplateList
inline TShaderTechniquePolicy& CForestRI_t::PickTechnique(TShaderTechniquePolicy atAvailableTechniques[SHADER_TYPE_NUMTYPES], ERenderPassType eRenderPass)
{
    EShaderType eType = SHADER_TYPE_STANDARD;

    switch (eRenderPass)
    {
    case RENDER_PASS_DEPTH_ONLY:
        eType = SHADER_TYPE_DEPTH_ONLY;
        break;
    case RENDER_PASS_SHADOW:
        eType = SHADER_TYPE_SHADOW;
        break;
    default:
        // do nothing since eType defaults to SHADER_TYPE_STANDARD
        break;
    }

    // some of these different shader types might not exist or have loaded successfully, so fall back to
    // the standard shader
    if (!atAvailableTechniques[eType].IsValid( ))
        eType = SHADER_TYPE_STANDARD;

    return atAvailableTechniques[eType];
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::UploadMaterial

CForestRI_TemplateList
inline st_int32 CForestRI_t::UploadMaterial(const SMaterialRI_t* pMaterial, st_bool bTextureOnly)
{
    assert(pMaterial);

    st_int32 nNumTexturesBound = 0;

    // upload color components of material
    if (!bTextureOnly)
    {
        (void) m_tMaterialAmbient.Set4fv(pMaterial->m_vAmbient * m_sRenderInfo.m_sLightMaterial.m_vAmbient);
        (void) m_tMaterialDiffuse.Set4fv(pMaterial->m_vDiffuse * m_sRenderInfo.m_sLightMaterial.m_vDiffuse * pMaterial->m_fLightScalar * m_sRenderInfo.m_fGlobalLightScalar);
        if (m_sRenderInfo.m_bSpecularLighting)
            (void) m_tMaterialSpecular.Set4fv(pMaterial->m_vSpecular * m_sRenderInfo.m_sLightMaterial.m_vSpecular);
        if (m_sRenderInfo.m_bTransmissionLighting)
        {
            // the transmission color is setup unconventionally; it is always multiplied by 3.0 and a value of 1/3 is used to represent "no transmission"
            const st_float32 c_fTransmissionScalar = 3.0f;
            st_float32 fScale = m_sRenderInfo.m_fGlobalLightScalar * 3.0f * CForestRI_Interpolate<st_float32>(1.0f / c_fTransmissionScalar, 1.0f, m_sRenderInfo.m_fTransmissionScalar);
            (void) m_tMaterialEmissive.Set4fv(pMaterial->m_vEmissive * (m_sRenderInfo.m_sLightMaterial.m_vDiffuse * pMaterial->m_fLightScalar) * fScale);

            (void) m_tMaterialTransmission.Set3f(pMaterial->m_fTransmissionShadow, 
                                                 pMaterial->m_fTransmissionViewDependence,
                                                 pMaterial->m_eCullType == CULLTYPE_BACK ? -1.0f : 1.0f);
        }

        // alpha scalar must be adjusted depending on the transparent render mode:
        //  - if alpha-to-coverage, no adjustment made since that's the mode the value was tuned with in the modeler
        //  - if alpha testing, lower the scalar to allow fewer pixels through, mimicking the look of a2c.
        st_float32 fAlphaScalar = (m_eTextureAlphaMethod == TRANS_TEXTURE_ALPHA_TESTING) ? pMaterial->m_fAlphaScalar * m_sRenderInfo.m_fAlphaTestScalar : pMaterial->m_fAlphaScalar;
        (void) m_tMaterialExtras.Set4f(pMaterial->m_fLightScalar * m_sRenderInfo.m_fGlobalLightScalar, fAlphaScalar, pMaterial->m_fShininess, 1.0f - pMaterial->m_fAmbientContrast);
    }

    // upload textures
    if (m_bTexturingEnabled)
    {
        for (st_int32 nLayer = 0; nLayer < TL_NUM_TEX_LAYERS; ++nLayer)
        {
            if (m_atTextures[nLayer].IsValid( ))
            {
                const TTexturePolicy& cTextureToBind = pMaterial->m_acTextureObjects[nLayer].IsValid( ) ? pMaterial->m_acTextureObjects[nLayer] : m_atFallbackTextures[nLayer];

                if (cTextureToBind.IsValid( ) && cTextureToBind != m_atLastBoundTextures[nLayer])
                {
                    (void) m_atTextures[nLayer].SetTexture(cTextureToBind);
                    m_atLastBoundTextures[nLayer] = cTextureToBind;
                    ++nNumTexturesBound;
                }
            }
        }
    }
    else
    {
        // texturing disabled, so load fallback textures to get a correct no-texture render with
        // the current shaders still bound
        for (st_int32 nLayer = 0; nLayer < TL_NUM_TEX_LAYERS; ++nLayer)
        {
            if (m_atTextures[nLayer].IsValid( ))
            {
                const TTexturePolicy& cTextureToBind = m_atFallbackTextures[nLayer];

                if (cTextureToBind.IsValid( ) && cTextureToBind != m_atLastBoundTextures[nLayer])
                {
                    (void) m_atTextures[nLayer].SetTexture(cTextureToBind);
                    m_atLastBoundTextures[nLayer] = cTextureToBind;
                    ++nNumTexturesBound;
                }
            }
        }
    }

    return nNumTexturesBound;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::UploadBaseTreeParameters

CForestRI_TemplateList
inline st_bool CForestRI_t::UploadBaseTreeParameters(const CTreeRI_t* pBaseTree) const
{
    st_bool bSuccess = true;

    // upload the LOD range so the shaders can adjust LOD
    bSuccess &= UpdateLodProfile(pBaseTree->GetLodProfile( ));

    // wind parameters
    bSuccess &= UploadWindParams(pBaseTree->GetWind( ));

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::UpdateLodProfile

CForestRI_TemplateList
inline st_bool CForestRI_t::UpdateLodProfile(const SLodProfile& sLodProfile) const
{
    st_bool bSuccess = false;

    if (sLodProfile.m_bLodIsPresent)
        bSuccess = m_tLodProfile.Set4fv(reinterpret_cast<const st_float32*>(&sLodProfile)); // cast for quicker upload
    else
    {
        const st_float32 c_fVeryFarAway = 999999.0f;
        static st_float32 afNoLodProfile[4] = { 0.0f, c_fVeryFarAway, c_fVeryFarAway * 2.0f, c_fVeryFarAway * 3.0f };
        bSuccess = m_tLodProfile.Set4fv(afNoLodProfile);
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::UploadWindParams

CForestRI_TemplateList
inline st_bool CForestRI_t::UploadWindParams(const CWind& cWind) const
{
    st_bool bSuccess = false;

    if (IsWindEnabled( ))
    {
        // get updated shader values for this wind object
        const st_float32* pShaderValues = cWind.GetShaderValues( );
        assert(pShaderValues);

        bSuccess = true;
        bSuccess &= m_tWindDir.Set3fv(pShaderValues + CWind::SH_WIND_DIR_X);
        bSuccess &= m_tWindTimes.Set4fv(pShaderValues + CWind::SH_TIME_PRIMARY);
        bSuccess &= m_tWindDistances.Set4fv(pShaderValues + CWind::SH_DIST_PRIMARY);
        bSuccess &= m_tWindLeaves.Set3fv(pShaderValues + CWind::SH_DIST_LEAVES);
        bSuccess &= m_tWindGust.Set3fv(pShaderValues + CWind::SH_STRENGTH_COMBINED);
        bSuccess &= m_tWindGustHints.Set3fv(pShaderValues + CWind::SH_HEIGHT_OFFSET);
        if (m_sRenderInfo.m_bFrondRippling)
            bSuccess &= m_tWindFrondRipple.Set3fv(pShaderValues + CWind::SH_DIST_FROND_RIPPLE);

        #ifdef SPEEDTREE_ADVANCED_WIND
            bSuccess &= m_tWindRollingBranches.Set4fv(pShaderValues + CWind::SH_ROLLING_BRANCHES_MAX_SCALE);
            bSuccess &= m_tWindRollingLeaves.Set4fv(pShaderValues + CWind::SH_ROLLING_LEAVES_MAX_SCALE);
            bSuccess &= m_tWindTwitchingLeaves.Set4fv(pShaderValues + CWind::SH_LEAVES_TWITCH_AMOUNT);
            bSuccess &= m_tWindTumblingLeaves.Set4fv(pShaderValues + CWind::SH_LEAVES_TUMBLE_X);
        #endif
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::SetBackfaceStateByMaterial

CForestRI_TemplateList
inline void CForestRI_t::SetBackfaceStateByMaterial(const SMaterialRI_t* pMaterial, ERenderPassType eRenderPass) const
{
    assert(pMaterial);

    // set face culling mode based on the coordinate system and render pass type
    ECullType eCullType = pMaterial->m_eCullType;

    // flip table
    static ECullType aeCullFlipStates[ ] = { CULLTYPE_NONE, CULLTYPE_FRONT, CULLTYPE_BACK };

    // flip if shadow
    if (eRenderPass == RENDER_PASS_SHADOW)
        eCullType = aeCullFlipStates[eCullType];

    // flip if left-handed coord sys
    if (CCoordSys::IsLeftHanded( ))
        eCullType = aeCullFlipStates[eCullType];

    // set final state
    m_tRenderStatePolicy.SetFaceCulling(eCullType);
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::DisableTransparentTextures

CForestRI_TemplateList
inline void CForestRI_t::DisableTransparentTextures(void)
{
    m_tRenderStatePolicy.SetBlending(false);
    m_tRenderStatePolicy.SetAlphaTesting(false);
    m_tRenderStatePolicy.SetAlphaToCoverage(false);
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::InitFallbackTextures

CForestRI_TemplateList
inline void CForestRI_t::InitFallbackTextures(void)
{
    for (st_int32 i = 0; i < TL_NUM_TEX_LAYERS; ++i)
    {
        TTexturePolicy& tTex = m_atFallbackTextures[i];

        switch (i)
        {
        case TL_DIFFUSE:
        case TL_SPECULAR:
            tTex.LoadColor(0xffffffff); 
            break;
        case TL_NORMAL:
        case TL_DETAIL_NORMAL:
            tTex.LoadColor(0x7f7fffff);
            break;
        case TL_DETAIL_DIFFUSE:
            tTex.LoadColor(0x00000000);
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::OnResetDevice

CForestRI_TemplateList
inline void CForestRI_t::OnResetDevice(void)
{
    m_tShaderLoader.OnResetDevice( );
    m_atShadowBuffers[0].OnResetDevice( );
    m_atShadowBuffers[1].OnResetDevice( );
    m_atShadowBuffers[2].OnResetDevice( );
    m_atShadowBuffers[3].OnResetDevice( );
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::OnLostDevice

CForestRI_TemplateList
inline void CForestRI_t::OnLostDevice(void)
{
    m_tShaderLoader.OnLostDevice( );
    m_atShadowBuffers[0].OnLostDevice( );
    m_atShadowBuffers[1].OnLostDevice( );
    m_atShadowBuffers[2].OnLostDevice( );
    m_atShadowBuffers[3].OnLostDevice( );
}


