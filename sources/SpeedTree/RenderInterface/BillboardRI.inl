///////////////////////////////////////////////////////////////////////  
//  BillboardRI.inl
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
//  CForestRI::RenderBillboards

CForestRI_TemplateList
inline st_bool CForestRI_t::RenderBillboards(const SForestCullResultsRI_t& sVisibleTrees, ERenderPassType eRenderPass, const CView& cView)
{
    st_bool bSuccess = true;

    // an empty frustum shouldn't result in a rendering error
    if (sVisibleTrees.m_aVisibleCells.empty( ))
        return true;

    // setup vertical billboard technique
    TShaderTechniquePolicy& tBillboardTechnique = PickTechnique(m_atBillboardTechniques, eRenderPass);
    if (tBillboardTechnique.Bind( ))
    {
        // setup render & shader state, shared by all base trees
        {
            m_tRenderStatePolicy.SetFaceCulling(CULLTYPE_NONE);

            // upload rotation parameters, common to all billboards
            st_float32 fAzimuth = cView.GetCameraAzimuth( );
            st_float32 fSine = sinf(-fAzimuth);

            bSuccess &= m_tCameraAzimuthTrig.Set4f(fSine, cosf(fAzimuth), -fSine, 0.0f);

            if (eRenderPass == RENDER_PASS_STANDARD)
            {
                bSuccess &= UploadBillboardTangents(fAzimuth);

                // the material shader matrix normally holds a geometry's material or the material multiplied
                // against the light materials; for the billboards, it will only contain the light material and
                // only diffuse and ambient are used
                bSuccess &= m_tMaterialAmbient.Set4fv(m_sRenderInfo.m_sLightMaterial.m_vAmbient);
                bSuccess &= m_tMaterialDiffuse.Set4fv(m_sRenderInfo.m_sLightMaterial.m_vDiffuse);
            }

            bSuccess &= m_tMaterialExtrasBBs.Set4f(m_sRenderInfo.m_fGlobalLightScalar, 1.0f, 0.0f, 0.55f);

            // upload horizontal fade value
            bSuccess &= m_tHorzBillboardFade.Set1f(m_sRenderInfo.m_bHorizontalBillboards ? cView.GetHorzBillboardFadeValue( ) : 0.0f);
        }

        // track rendering statistics
        SRenderStats::SGeometryStats& sStats = m_sRenderStats.m_aGeometries[STATS_CATEGORY_BILLBOARDS][eRenderPass];

        // flag to keep format from being redundantly enabled
        st_bool bVboFormatEnabled = false;

        // run through the base trees, rendering each of their VBOs; should result in one draw call per base tree
        // to render the entire population of billboards
        for (st_int32 nBaseTreeIndex = 0; nBaseTreeIndex < st_int32(sVisibleTrees.m_pBaseTrees->size( )); ++nBaseTreeIndex)
        {
            // get the base tree pointer
            CTreeRI_t* pBaseTree = (CTreeRI_t*) sVisibleTrees.m_pBaseTrees->at(nBaseTreeIndex);
            assert(pBaseTree);

            // get the base tree's vertical billboard attributes
            const SGeometry* pGeometry = pBaseTree->GetGeometry( );
            assert(pGeometry);
            const st_bool c_bHasVertBillboards = pBaseTree->HasGeometryType(GEOMETRY_TYPE_VERTICAL_BILLBOARDS);
            const SVerticalBillboards& sVertBBs = pGeometry->m_sVertBBs;
            const st_bool c_bHasHorzBillboards = pBaseTree->HasGeometryType(GEOMETRY_TYPE_HORIZONTAL_BILLBOARDS) && m_sRenderInfo.m_bHorizontalBillboards;
            const SHorizontalBillboard& sHorzBB = pGeometry->m_sHorzBB;

            // don't continue if no billboards exist for this base tree
            if (!c_bHasVertBillboards && !c_bHasHorzBillboards)
                continue;
            
            // lookup billboard index buffer rendering data; move on if not found
            typename SForestCullResultsRI_t::TTreeIboMap::const_iterator iFind = sVisibleTrees.m_mBillboardIbos.find(pBaseTree);
            if (iFind == sVisibleTrees.m_mBillboardIbos.end( ))
                continue;

            const typename SForestCullResultsRI_t::SBillboardIboData* pBillboardIboData = &(iFind->second);
            assert(pBillboardIboData);

            // upload shader constants specific to this base tree
            {
                // LOD range
                bSuccess &= UpdateLodProfile(pBaseTree->GetLodProfile( ));

                // upload data shared by both vertical and horizontal billboards
                st_int32 nNumTexturesNewlyBound = UploadMaterial(&pBaseTree->GetMaterials( )->at(sVertBBs.m_nMaterialIndex), true);
                if (nNumTexturesNewlyBound > 0)
                {
                    bSuccess &= tBillboardTechnique.CommitTextures( );
                    sStats.m_uiNumTextureBinds += st_uint32(nNumTexturesNewlyBound);
                }

                // upload vertical billboard-specific data
                st_int32 nNumBillboardsToUpload = sVertBBs.m_nNumBillboards;
                if (nNumBillboardsToUpload > 0)
                {
                    bSuccess &= m_tNum360Images.Set1f(float(nNumBillboardsToUpload));

                    // upload billboard dimensions
                    bSuccess &= m_tVertBillboardDimensions.Set4f(0.5f * sVertBBs.m_fWidth, -0.5f * sVertBBs.m_fWidth, sVertBBs.m_fTopCoord, sVertBBs.m_fBottomCoord);

                    // upload the vertical billboard texcoords
                    if (nNumBillboardsToUpload > m_sRenderInfo.m_nMaxBillboardImagesByBase)
                    {
                        nNumBillboardsToUpload = m_sRenderInfo.m_nMaxBillboardImagesByBase;
                        CCore::SetError("CForestRI::RenderBillboards, tried to upload %d billboards for a base tree, but %d was the max set in CForestRI:SetRenderInfo(); truncating",
                            sVertBBs.m_nNumBillboards, nNumBillboardsToUpload);
                    }
                    bSuccess &= m_t360BillboardTexCoords.SetArray4f(sVertBBs.m_pTexCoords, nNumBillboardsToUpload);

                    // upload wind data
                    bSuccess &= UploadWindParams(pBaseTree->GetWind( ));
                }

                // upload horizontal billboard-specific data
                if (c_bHasHorzBillboards)
                {
                    if (CCoordSys::IsYAxisUp( ))
                        bSuccess &= m_tHorzBillboardDimensions.Set4f(sHorzBB.m_avCoords[0].x, sHorzBB.m_avCoords[1].x, sHorzBB.m_avCoords[0].z, sHorzBB.m_avCoords[2].z);
                    else
                        bSuccess &= m_tHorzBillboardDimensions.Set4f(sHorzBB.m_avCoords[0].x, sHorzBB.m_avCoords[1].x, sHorzBB.m_avCoords[0].y, sHorzBB.m_avCoords[2].y);

                    // upload the horizontal billboard texcoords if present
                    bSuccess &= m_tHorzBillboardTexCoords.Set4fv(pGeometry->m_sHorzBB.m_vTexCoordsShader);
                }

                tBillboardTechnique.CommitConstants( );
            }

            // find the geometry buffer associated with this base tree
            const TGeometryBufferPolicy* pBillboardVB = pBaseTree->GetBillboardVB( );
            assert(pBillboardVB);
            const TGeometryBufferPolicy* pBillboardIB = &pBillboardIboData->m_tIbo;
            assert(pBillboardIB);
            
            // ensure this base tree has billboards to render
            if (pBillboardIB->NumIndices( ) > 0 && pBillboardIboData->m_uiNumIndices > 0)
            {
                // render the index buffer associated with this base tree
                if (!bVboFormatEnabled)
                {
                    bSuccess &= pBillboardVB->EnableFormat( );
                    bVboFormatEnabled = true;
                }

                if (pBillboardVB->BindVertexBuffer( ) &&
                    pBillboardIB->BindIndexBuffer( ))
                {
                    ++sStats.m_uiNumVboBinds;

                    if (m_bQuadPrimitiveSupported)
                    {
                        bSuccess &= pBillboardIB->RenderIndexed(PRIMITIVE_QUADS, 0, pBillboardIboData->m_uiNumIndices, pBillboardIboData->m_uiMinIndex, pBillboardIboData->m_uiNumVertices);
                        sStats.m_uiNumTriangles += pBillboardIboData->m_uiNumIndices / 2;
                        sStats.m_uiNumObjects += pBillboardIboData->m_uiNumIndices / 4;
                    }
                    else
                    {
                        bSuccess &= pBillboardIB->RenderIndexed(PRIMITIVE_TRIANGLES, 0, pBillboardIboData->m_uiNumIndices, pBillboardIboData->m_uiMinIndex, pBillboardIboData->m_uiNumVertices);
                        sStats.m_uiNumTriangles += pBillboardIboData->m_uiNumIndices / 3;
                        sStats.m_uiNumObjects += pBillboardIboData->m_uiNumIndices / 6;
                    }
                    bSuccess &= pBillboardIB->UnBindIndexBuffer( );
                    bSuccess &= pBillboardVB->UnBindVertexBuffer( );

                    sStats.m_uiNumDrawCalls++;
                }
                else
                    bSuccess = false;
            }
        }

        if (bVboFormatEnabled)
            bSuccess &= TGeometryBufferPolicy::DisableFormat( );

        bSuccess &= tBillboardTechnique.UnBind( );
    }
    else
        bSuccess = false;


    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::UpdateBillboards

CForestRI_TemplateList
inline st_bool CForestRI_t::UpdateBillboards(SForestCullResultsRI_t& sVisibleTrees)
{
    #ifdef SPEEDTREE_OPENMP
        // need to keep parallel threads from adjusting m_aAvailableBillboardVboBlocks simultaneously
        #pragma omp section
    #endif
    {
        // recycle the billboard blocks from the previously visible cells (those that have just
        // left the frustum on this frame) to the list of available blocks
        for (st_int32 nCell = 0; nCell < st_int32(sVisibleTrees.m_aPreviousVisibleCells.size( )); ++nCell)
        {
            // get cell
            CTreeCell* pCell = sVisibleTrees.m_aPreviousVisibleCells[nCell];
            assert(pCell);

            // free billboard block only if it has a block and the cell hasn't been tagged as visible (which
            // could have happened from a separate Cull call)
            if (pCell->HasBillboardBlock( ) && pCell->GetUpdateIndex( ) != GetUpdateIndex( ))
            {
                m_aAvailableBillboardVboBlocks.push_back(pCell->GetBillboardBlock( ));
                pCell->ClearBillboardBlock( );
            }
        }

        // assign new billboard blocks to the cells that have just entered the frustum from the list
        // of available blocks
        st_bool bNoBlocksErrorIssued = false; // keep an excessive # of errors from being issued
        for (st_int32 nCell = 0; nCell < st_int32(sVisibleTrees.m_aNewVisibleCells.size( )); ++nCell)
        {
            // get cell
            CTreeCell* pCell = sVisibleTrees.m_aNewVisibleCells[nCell];
            assert(pCell);

            // cells are stored in m_aNewVisibleCells for one of three reasons:
            //
            //   1. The cells have just been created.
            //   2. The cells are newly visible (just entered the frustum on this frame)
            //   3. The instances in the cell have changed

            // if a block is needed, assign one
            if (!pCell->HasBillboardBlock( ))
            {
                if (!m_aAvailableBillboardVboBlocks.empty( ))
                {
                    pCell->SetBillboardBlock(m_aAvailableBillboardVboBlocks.back( ));
                    m_aAvailableBillboardVboBlocks.pop_back( );
                }
                else if (!bNoBlocksErrorIssued)
                {
                    CCore::SetError("CForestRI::UpdateBillboards(), exceeded preset limit of %d billboard cells; use CForest::SetHint(HINT_MAX_NUM_VISIBLE_CELLS) to adjust", m_nHintMaxNumVisibleCells);
                    bNoBlocksErrorIssued = true; // keeps an excessive # of errors from being issued
                }
            }
        }
    }

    // adjust VBs based on new and old cells
    for (st_int32 nCell = 0; nCell < st_int32(sVisibleTrees.m_aNewVisibleCells.size( )); ++nCell)
    {
        // get cell
        CTreeCell* pCell = sVisibleTrees.m_aNewVisibleCells[nCell];
        assert(pCell);

        // a cell may not have a billboard block associated with it, if blocks were available
        if (pCell->HasBillboardBlock( ))
            UpdateBillboardVbos(pCell);
    }

    if (sVisibleTrees.m_bFrustumContainsModifiedCells)
        sVisibleTrees.m_mBillboardIbos.clear( );

    // adjust index buffers for any base tree that's been affected
    if (!sVisibleTrees.m_aNewVisibleCells.empty( ) ||
        !sVisibleTrees.m_aPreviousVisibleCells.empty( ))
    {
        UpdateBillboardIbos(sVisibleTrees);
    }

    return true;
}


///////////////////////////////////////////////////////////////////////
//  Function: EncodeCornerIndexAndRotation

inline st_float32 EncodeCornerIndexAndRotation(st_int32 nCornerIndex, st_float32 fRotation)
{
    assert(fRotation >= 0.0 && fRotation <= c_fTwoPi);

    const st_float32 c_fFracRange = 0.95f;

    return st_float32(nCornerIndex) + c_fFracRange * fRotation / c_fTwoPi;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::UpdateBillboardVbos

CForestRI_TemplateList
inline void CForestRI_t::UpdateBillboardVbos(CTreeCell* pCell)
{
    assert(pCell);
    assert(pCell->HasBillboardBlock( ));
    CCellBaseTreeItr iBaseTree = pCell->GetCellInstances( ).FirstBaseTree( );
    CTreeRI_t* pBaseTree = (CTreeRI_t*) iBaseTree.TreePtr( );

    while (pBaseTree)
    {
        // gather some data together needed for VBO population
        const st_bool c_bVerticalBillboardsPresent = pBaseTree->HasGeometryType(GEOMETRY_TYPE_VERTICAL_BILLBOARDS);
        const st_bool c_bHorizontalBillboardsPresent = pBaseTree->HasGeometryType(GEOMETRY_TYPE_HORIZONTAL_BILLBOARDS) && m_sRenderInfo.m_bHorizontalBillboards;
        st_int32 nNumBillboardTypes = (c_bVerticalBillboardsPresent ? 1 : 0) + (c_bHorizontalBillboardsPresent ? 1 : 0);

        if (nNumBillboardTypes > 0)
        {
            // each base tree in the cell contains a list of instances
            st_int32 nNumInstances = 0;
            const CInstance* pInstances = pCell->GetCellInstances( ).GetInstances(iBaseTree, nNumInstances);

            // truncate the # of instances if it exceeds the available buffer
            const st_int32 c_nMaxInstances = pBaseTree->GetBillboardMaxInstancesPerCell( );
            if (nNumInstances > c_nMaxInstances)
            {
                CCore::SetError("CForestRI::UpdateBillboardVbos, instance count, %d, for base tree [%s] in cell [%d, %d] exceeded limit of [%d]",
                    nNumInstances, pBaseTree->GetFilename( ), pCell->Row( ), pCell->Col( ), c_nMaxInstances);
                nNumInstances = c_nMaxInstances;
            }

            if (nNumInstances > 0)
            {
                // get SPerBaseBillboardData associated with this base tree
                TGeometryBufferPolicy* pBillboardVB = pBaseTree->GetBillboardVB( );

                // gather stats on the base tree's horizontal billboard
                const SHorizontalBillboard& sHorzBB = pBaseTree->GetGeometry( )->m_sHorzBB;

                // prepare local buffer for copying into VBO
                st_int32 nNumVertices = nNumInstances * 4 * nNumBillboardTypes;
                assert(nNumInstances <= st_int32(pBillboardVB->NumVertices( )));
                CLocalArray<SBillboardVertex> aPopulateBuffer(nNumVertices, "UpdateBillboardVbos");
                SBillboardVertex* pVertex = &aPopulateBuffer[0];

                // populate the VBO with vertices representing the instance list
                for (st_int32 i = 0; i < nNumInstances; ++i)
                {
                    const CInstance& cInstance = pInstances[i];

                    // parameters common to all vertices in this billboard
                    const Vec3& c_vPos = cInstance.GetPos( );
                    const st_float32 c_fScale = cInstance.GetScale( );
                    const st_float32 c_fRotationAngle = cInstance.GetRotationAngle( );

                    if (c_bVerticalBillboardsPresent)
                    {
                        // vertex 0
                        pVertex->m_vPosition = c_vPos;
                        pVertex->m_fCornerAndRotation = EncodeCornerIndexAndRotation(0, c_fRotationAngle);
                        pVertex->m_fScale = c_fScale;
                        ++pVertex;

                        // vertex 1
                        pVertex->m_vPosition = c_vPos;
                        pVertex->m_fCornerAndRotation = EncodeCornerIndexAndRotation(1, c_fRotationAngle);
                        pVertex->m_fScale = c_fScale;
                        ++pVertex;

                        // vertex 2
                        pVertex->m_vPosition = c_vPos;
                        pVertex->m_fCornerAndRotation = EncodeCornerIndexAndRotation(2, c_fRotationAngle);
                        pVertex->m_fScale = c_fScale;
                        ++pVertex;

                        // vertex 3
                        pVertex->m_vPosition = c_vPos;
                        pVertex->m_fCornerAndRotation = EncodeCornerIndexAndRotation(3, c_fRotationAngle);
                        pVertex->m_fScale = c_fScale;
                        ++pVertex;
                    }

                    if (c_bHorizontalBillboardsPresent)
                    {
                        const Vec3 c_vHorzPos(c_vPos + CCoordSys::UpAxis( ) * CCoordSys::UpComponent(sHorzBB.m_avCoords[0]) * c_fScale);

                        // vertex 0
                        pVertex->m_vPosition = c_vHorzPos;
                        pVertex->m_fCornerAndRotation = EncodeCornerIndexAndRotation(4, c_fRotationAngle);
                        pVertex->m_fScale = c_fScale;
                        ++pVertex;

                        // vertex 1
                        pVertex->m_vPosition = c_vHorzPos;
                        pVertex->m_fCornerAndRotation = EncodeCornerIndexAndRotation(5, c_fRotationAngle);
                        pVertex->m_fScale = c_fScale;
                        ++pVertex;

                        // vertex 2
                        pVertex->m_vPosition = c_vHorzPos;
                        pVertex->m_fCornerAndRotation = EncodeCornerIndexAndRotation(6, c_fRotationAngle);
                        pVertex->m_fScale = c_fScale;
                        ++pVertex;

                        // vertex 3
                        pVertex->m_vPosition = c_vHorzPos;
                        pVertex->m_fCornerAndRotation = EncodeCornerIndexAndRotation(7, c_fRotationAngle);
                        pVertex->m_fScale = c_fScale;
                        ++pVertex;
                    }
                }

                assert(st_int32(pVertex - &aPopulateBuffer[0]) == nNumVertices);

                // update VBO
                st_uint32 uiVboOffset = pBaseTree->GetBillboardVboBlockNumVertices( ) * st_uint32(pCell->GetBillboardBlock( ));
                assert(uiVboOffset + nNumVertices <= pBillboardVB->NumVertices( ));
                pBillboardVB->OverwriteVertices(&aPopulateBuffer[0], nNumVertices, uiVboOffset);
            }
        }

        ++iBaseTree;
        pBaseTree = (CTreeRI_t*) iBaseTree.TreePtr( );
    }
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::UpdateBillboardIbos

CForestRI_TemplateList
inline st_bool CForestRI_t::UpdateBillboardIbos(SForestCullResultsRI_t& sVisibleTrees) const
{
    st_bool bSuccess = true;

    // populate each base tree's index buffer
    const TTreeArray& aBaseTrees = GetBaseTrees( );
    for (st_int32 nBaseTree = 0; nBaseTree < st_int32(aBaseTrees.size( )); ++nBaseTree)
    {
        // get base tree pointer
        CTreeRI_t* pBaseTree = (CTreeRI_t*) aBaseTrees[nBaseTree];
        assert(pBaseTree);

        // determine billboard parameters
        const st_bool c_bHasVertBillboards = pBaseTree->HasGeometryType(GEOMETRY_TYPE_VERTICAL_BILLBOARDS);
        const st_bool c_bHasHorzBillboards = pBaseTree->HasGeometryType(GEOMETRY_TYPE_HORIZONTAL_BILLBOARDS) && m_sRenderInfo.m_bHorizontalBillboards;

        if (!c_bHasVertBillboards && !c_bHasHorzBillboards)
            continue;

        // find the IB associated with this base tree; create one for this view if not available
        typename SForestCullResultsRI_t::TTreeIboMap::iterator iFind = sVisibleTrees.m_mBillboardIbos.find(pBaseTree);

        // if not found, create it
        typename SForestCullResultsRI_t::SBillboardIboData* pBillboardIboData = NULL;
        if (iFind == sVisibleTrees.m_mBillboardIbos.end( ))
        {
            // create a new IBO
            pBillboardIboData = &(sVisibleTrees.m_mBillboardIbos[pBaseTree]);

            // setup empty IBO
            {
                bSuccess &= pBillboardIboData->m_tIbo.SetIndexFormat(INDEX_FORMAT_UNSIGNED_32BIT);
                const st_int32 c_nNumIndices = pBaseTree->GetBillboardIboNumIndices( );
                CLocalArray<st_uint32> aEmptyIndices(c_nNumIndices, "UpdateBillboardIbos, indices");
                bSuccess &= pBillboardIboData->m_tIbo.AppendIndices(&aEmptyIndices[0], c_nNumIndices);
                bSuccess &= pBillboardIboData->m_tIbo.EndIndices( );
                assert(pBillboardIboData->m_tIbo.NumIndices( ) == st_uint32(c_nNumIndices));
            }
        }
        else 
            pBillboardIboData = &(iFind->second);
        assert(pBillboardIboData);

        // find limits associated with base tree's buffer
        const st_int32 c_nMaxInstances = pBaseTree->GetBillboardMaxInstancesPerCell( );

        // create population buffer
        const st_int32 c_nMaxNumIndices = pBaseTree->GetBillboardIboNumIndices( );
        CLocalArray<st_uint32> aIndices(c_nMaxNumIndices, "UpdateBillboardIbos");

        // set starting write location
        st_uint32* pIndicesPtr = &aIndices[0];

        // for each base tree, scan through all of the visible cells
        st_uint32 uiMinIndex = INT_MAX;
        st_uint32 uiMaxIndex = 0;
        for (st_int32 nCell = 0; nCell < st_int32(sVisibleTrees.m_aVisibleCells.size( )); ++nCell)
        {
            // get the cell instances
            const CTreeCell* pCell = sVisibleTrees.m_aVisibleCells[nCell];
            assert(pCell);

            if (pCell->HasBillboardBlock( ))
            {
                const CCellInstances& cCellInstances = pCell->GetCellInstances( );

                // get the instances for the current base tree from this cell
                CCellBaseTreeItr iBaseTreeItr(&cCellInstances, pBaseTree);
                if (iBaseTreeItr.TreePtr( )) // this cell may not hold instances of pBaseTree
                {
                    st_int32 nNumInstances = 0;
                    const CInstance* pInstances = cCellInstances.GetInstances(iBaseTreeItr, nNumInstances);
                    if (pInstances && nNumInstances > 0)
                    {
                        st_int32 nBillboardBlock = pCell->GetBillboardBlock( );
                        assert(pCell->HasBillboardBlock( ));

                        // adjust instance count based on vertical or horz bb presence
                        nNumInstances = ((c_bHasHorzBillboards ? 1 : 0) + (c_bHasVertBillboards ? 1 : 0)) * nNumInstances;

                        // truncate nNumInstances if necessary
                        if (nNumInstances > c_nMaxInstances)
                        {
                            CCore::SetError("Billboard count of [%d] exceeds max of [%d]", nNumInstances, c_nMaxInstances);
                            nNumInstances = c_nMaxInstances;
                        }

                        #ifndef NDEBUG
                            const st_uint32 uiNumIndicesSoFar = st_uint32(pIndicesPtr - &aIndices[0]);
                            const st_uint32 uiNumIndicesToAdd = st_uint32(nNumInstances * (m_bQuadPrimitiveSupported ? 4 : 6));
                            const st_uint32 uiMaxIndices = pBillboardIboData->m_tIbo.NumIndices( );
                            assert(uiNumIndicesSoFar + uiNumIndicesToAdd <= uiMaxIndices);
                        #endif

                        st_uint32 uiVboOffset = st_uint32(pBaseTree->GetBillboardVboBlockNumVertices( ) * nBillboardBlock);
                        uiMinIndex = st_min(uiMinIndex, st_uint32(uiVboOffset));
                        if (m_bQuadPrimitiveSupported)
                        {
                            const st_int32 c_nNumIndices = nNumInstances * 4;
                            for (st_int32 i = 0; i < c_nNumIndices; ++i)
                                *pIndicesPtr++ = uiVboOffset++;
                        }
                        else
                        {
                            for (st_int32 i = 0; i < nNumInstances; ++i)
                            {
                                *pIndicesPtr++ = uiVboOffset + 0;
                                *pIndicesPtr++ = uiVboOffset + 1;
                                *pIndicesPtr++ = uiVboOffset + 2;
                                *pIndicesPtr++ = uiVboOffset + 0;
                                *pIndicesPtr++ = uiVboOffset + 2;
                                *pIndicesPtr++ = uiVboOffset + 3;

                                uiVboOffset += 4;
                            }
                        }
                        uiMaxIndex = st_max(uiMaxIndex, st_uint32(uiVboOffset));
                    }
                }
            }
        }

        // populate the IB and record its length
        if (uiMinIndex < INT_MAX)
        {
            pBillboardIboData->m_uiNumIndices = st_uint32(pIndicesPtr - &aIndices[0]);
            assert(pBillboardIboData->m_uiNumIndices <= pBillboardIboData->m_tIbo.NumIndices( ));
            pBillboardIboData->m_uiMinIndex = uiMinIndex;
            pBillboardIboData->m_uiNumVertices = uiMaxIndex - uiMinIndex;

            bSuccess &= pBillboardIboData->m_tIbo.OverwriteIndices(&aIndices[0], pBillboardIboData->m_uiNumIndices, 0);
        }
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::UploadBillboardTangents

CForestRI_TemplateList
inline st_bool CForestRI_t::UploadBillboardTangents(st_float32 fCameraAzimuth) const
{
    fCameraAzimuth += c_fPi;

    if (CCoordSys::IsLeftHanded( ))
        fCameraAzimuth = c_fPi - fCameraAzimuth;

    Vec3 vBinormal = CCoordSys::UpAxis( );
    Vec3 vNormal = CCoordSys::ConvertFromStd(cosf(fCameraAzimuth), sinf(fCameraAzimuth), 0.0f);
    Vec3 vTangent = vBinormal.Cross(vNormal);

    // negate the tangent if RH/Yup or LH/Zup
    if ((CCoordSys::IsYAxisUp( ) && !CCoordSys::IsLeftHanded( )) ||
        (!CCoordSys::IsYAxisUp( ) && CCoordSys::IsLeftHanded( )))
        vTangent = -vTangent;

    st_float32 afBillboardTangents[12] = 
    {
        vNormal.x, vNormal.y, vNormal.z, 0.0f,
        vBinormal.x, vBinormal.y, vBinormal.z, 0.0f,
        vTangent.x, vTangent.y, vTangent.z, 0.0f
    };

    return m_tBillboardTangents.SetArray4f(afBillboardTangents, 3);
}
