///////////////////////////////////////////////////////////////////////  
//  TreeRI.inl
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
//  Constants

const st_float32 c_fUncompressWindScalar = 10.0f;


///////////////////////////////////////////////////////////////////////
//  CTreeRI::CTreeRI

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline CTreeRI_t::CTreeRI( ) :
    m_bQuadPrimitiveSupported(false),
    m_tBillboardVB(true, false),
    m_nBillboardMaxInstances(-1),
    m_nBillboardMaxCells(-1),
    m_bHorzBillboardsAllowed(false),
    m_pBillboardTechnique(NULL),
    m_pSearchPaths(NULL),
    m_bGraphicsInitialized(false)
{
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::~CTreeRI

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline CTreeRI_t::~CTreeRI( )
{
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::InitGraphics

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::InitGraphics(const CArray<CFixedString>& aSearchPaths, st_bool bQuadsSupported, const TShaderTechniquePolicy** pTechniques, st_int32 nMaxAnisotropy)
{
    st_bool bSuccess = false;

    if (!m_bGraphicsInitialized)
    {
        m_bQuadPrimitiveSupported = bQuadsSupported;

        // can't proceed if the geometry doesn't exist  
        const SGeometry* pGeometry = GetGeometry( );
        if (pGeometry)
        {
            bSuccess = InitMaterials(pGeometry, aSearchPaths, nMaxAnisotropy);
            bSuccess &= InitBranches(pGeometry, pTechniques[GEOMETRY_TYPE_BRANCHES]);
            bSuccess &= InitFronds(pGeometry, pTechniques[GEOMETRY_TYPE_FRONDS]);
            bSuccess &= InitLeafCards(pGeometry, pTechniques[GEOMETRY_TYPE_LEAF_CARDS]);
            bSuccess &= InitLeafMeshes(pGeometry, pTechniques[GEOMETRY_TYPE_LEAF_MESHES]);

            // dump the CCore copy of the geometry now that it's been copied and formatted
            // for the vertex and index buffers
            DeleteGeometry(true);

            // save this technique for later use in CTreeRI::InitBillboardSystem
            m_pBillboardTechnique = pTechniques[GEOMETRY_TYPE_VERTICAL_BILLBOARDS];
            assert(m_pBillboardTechnique);

            if (!bSuccess)
                CCore::SetError("CTreeRI::InitGraphics, failed to initialize material and/or geometry");
        }
        else
            CCore::SetError("CTreeRI::InitGraphics, no geometry present (CCore::DeleteGeometry called too early?");
        
        m_bGraphicsInitialized = bSuccess;
    }
    else
    {
        CCore::SetError("CTreeRI::InitGraphics called redundantly");
        bSuccess = true;
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::InitBillboardSystem

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::InitBillboardSystem(st_int32 nMaxNumInstancesPerCell, 
                                              st_int32 nMaxNumVisibleCells,
                                              st_bool bHorzBillboardsAllowed)
{
    st_bool bSuccess = true;

    // make sure this function hasn't already been called for this object
    if (m_nBillboardMaxInstances == -1 && m_nBillboardMaxCells == -1 && m_pBillboardTechnique)
    {
        // determine billboard parameters
        const st_bool c_bHasVertBillboards = HasGeometryType(GEOMETRY_TYPE_VERTICAL_BILLBOARDS);
        const st_bool c_bHasHorzBillboards = HasGeometryType(GEOMETRY_TYPE_HORIZONTAL_BILLBOARDS) && bHorzBillboardsAllowed;

        // record parameters
        m_nBillboardMaxInstances = (c_bHasVertBillboards ? nMaxNumInstancesPerCell : 0) + (c_bHasHorzBillboards ? nMaxNumInstancesPerCell : 0);
        
        m_nBillboardMaxCells = nMaxNumVisibleCells;
        m_bHorzBillboardsAllowed = bHorzBillboardsAllowed;

        if (c_bHasVertBillboards || c_bHasHorzBillboards)
        {
            if (m_tBillboardVB.SetVertexFormat(asBillboardVertexAttribs, m_pBillboardTechnique))
            {
                // setup empty VBO
                const st_int32 c_nNumVertices = GetBillboardVboNumVertices( );
                CLocalArray<SBillboardVertex> aEmptyVertices(c_nNumVertices, "InitBillboardSystem, vertices");
                bSuccess &= m_tBillboardVB.AppendVertices(&aEmptyVertices[0], c_nNumVertices);
                bSuccess &= m_tBillboardVB.EndVertices( );
                assert(m_tBillboardVB.NumVertices( ) == st_uint32(c_nNumVertices));
            }
        }
    }
    else
    {
        CCore::SetError("CTreeRI::InitBillboardSystem, function already called for this object");
        bSuccess = false;
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::GraphicsAreInitialized

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::GraphicsAreInitialized(void) const
{
    return m_bGraphicsInitialized;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::GetGeometryBuffer

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline const TGeometryBufferPolicy* CTreeRI_t::GetGeometryBuffer(EGeometryType eType) const
{
    assert(eType >= GEOMETRY_TYPE_BRANCHES && eType <= GEOMETRY_TYPE_LEAF_MESHES);

    return m_atGeometryBuffers + eType;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::GetMaterials

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline const CArray<SMaterialRI_t>* CTreeRI_t::GetMaterials(void) const
{
    return &m_aMaterials;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::GetLodRenderInfo

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline const CLodRenderInfo* CTreeRI_t::GetLodRenderInfo(EGeometryType eType) const
{
    return m_acRenderInfo + eType;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::GetBillboardVB

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline TGeometryBufferPolicy* CTreeRI_t::GetBillboardVB(void)
{
    return &m_tBillboardVB;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::GetBillboardMaxInstancesPerCell

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_int32 CTreeRI_t::GetBillboardMaxInstancesPerCell(void) const
{
    return m_nBillboardMaxInstances;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::GetBillboardMaxVisibleCells

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_int32 CTreeRI_t::GetBillboardMaxVisibleCells(void) const
{
    return m_nBillboardMaxCells;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::GetBillboardVboNumVertices

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_int32 CTreeRI_t::GetBillboardVboNumVertices(void) const
{
    return GetBillboardVboBlockNumVertices( ) * GetBillboardMaxVisibleCells( );
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::GetBillboardIboNumIndices

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_int32 CTreeRI_t::GetBillboardIboNumIndices(void) const
{
    const st_int32 c_nNumBillboardTypes = (HasGeometryType(GEOMETRY_TYPE_VERTICAL_BILLBOARDS) ? 1 : 0) +
                                         ((HasGeometryType(GEOMETRY_TYPE_HORIZONTAL_BILLBOARDS) && m_bHorzBillboardsAllowed) ? 1 : 0);
    const st_int32 c_nNumIndicesPerBillboard = m_bQuadPrimitiveSupported ? 4 : 6;

    return GetBillboardMaxVisibleCells( ) * GetBillboardMaxInstancesPerCell( ) * c_nNumIndicesPerBillboard * c_nNumBillboardTypes;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::GetBillboardVboBlockNumVertices

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_int32 CTreeRI_t::GetBillboardVboBlockNumVertices(void) const
{
    const st_int32 c_nNumBillboardTypes = (HasGeometryType(GEOMETRY_TYPE_VERTICAL_BILLBOARDS) ? 1 : 0) +
                                         ((HasGeometryType(GEOMETRY_TYPE_HORIZONTAL_BILLBOARDS) && m_bHorzBillboardsAllowed) ? 1 : 0);
    const st_int32 c_nNumVertsPerBillboard = 4;

    return GetBillboardMaxInstancesPerCell( ) * c_nNumVertsPerBillboard * c_nNumBillboardTypes;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::InitMaterials

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::InitMaterials(const SGeometry* pGeometry, const CArray<CFixedString>& aSearchPaths, st_int32 nMaxAnisotropy)
{
    assert(pGeometry);
    st_bool bSuccess = true;

    // check to see if any materials are stored
    if (pGeometry->m_nNumMaterials > 0)
    {
        printf("[%s]\n", GetFilename( ));

        // keep copy of the materials in CTreeRI that will be modified to hold the full path names of the textures
        // (determined after the path search), not just the original filename
        m_aMaterials.resize(pGeometry->m_nNumMaterials);

        // run through each material, initializing its textures
        for (st_int32 nMaterial = 0; nMaterial < pGeometry->m_nNumMaterials; ++nMaterial)
        {
            assert(pGeometry->m_pMaterials);
            m_aMaterials[nMaterial].Copy(pGeometry->m_pMaterials[nMaterial]);
            SMaterialRI_t* pMaterial = &m_aMaterials[nMaterial];

            // each material has TL_NUM_TEX_LAYERS layers; initialize each if present
            for (st_int32 nTexture = 0; nTexture < TL_NUM_TEX_LAYERS; ++nTexture)
            {
                const char* pFilename = pMaterial->m_astrTextureFilenames[nTexture].c_str( );
                if (pFilename && strlen(pFilename) > 0)
                {
                    // look for the texture at each location in the search path
                    st_bool bFound = false;
                    for (st_int32 nSearchPath = 0; nSearchPath < st_int32(aSearchPaths.size( )) && !bFound; ++nSearchPath)
                    {
                        CFixedString strSearchLocation = aSearchPaths.at(nSearchPath) + CFixedString(pFilename).NoPath( );

                        // if the Load() call succeeds, the texture was found
                        if (pMaterial->m_acTextureObjects[nTexture].Load(strSearchLocation.c_str( ), nMaxAnisotropy))
                        {
                            // replace the original no-path filename with one containing the full path
                            pMaterial->m_astrTextureFilenames[nTexture] = strSearchLocation;
                            printf("  [material %d, tex layer %d] - [%s]\n", nMaterial, nTexture, strSearchLocation.c_str( ));
                            bFound = true;

                            break;
                        }
                    }

                    bSuccess &= bFound;
                }
            }
        }
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  struct SBranchVertex

struct SBranchVertex
{
    st_float32  m_afCoord[3];                                       // position
    st_float32  m_afLodCoord[3], m_fWindScalarMag;                  // texcoord0
    st_float32  m_afDiffuseTexCoords[2], m_afDetailTexCoords[2];    // texcoord1
    st_uchar    m_aucNormal[3], m_ucAmbOcc;                         // texcoord2
    st_uchar    m_aucTangent[3], m_ucTangentPadding;                // texcoord3
    st_uchar    m_aucWindData[4];                                   // texcoord4
};


///////////////////////////////////////////////////////////////////////
//  CTreeRI::InitBranches

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::InitBranches(const SGeometry* pGeometry, const TShaderTechniquePolicy* pTechnique)
{
    assert(pGeometry);
    st_bool bSuccess = true;

    // setup some aliases
    const st_int32 c_nNumLods = pGeometry->m_nNumBranchLods;
    const SIndexedTriangles* pLods = pGeometry->m_pBranchLods;
    TGeometryBufferPolicy* pGeometryBuffer = m_atGeometryBuffers + GEOMETRY_TYPE_BRANCHES;

    if (c_nNumLods > 0 && pLods) // may not have branch geometry in this model
    {
        // setup vertex structure
        SVertexAttribDesc asBranchVertexFormat[ ] =
        {
            { VERTEX_ATTRIB_SEMANTIC_POS, VERTEX_ATTRIB_TYPE_FLOAT, 3 },                // pos.xyz
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD0, VERTEX_ATTRIB_TYPE_FLOAT, 4 },          // LOD position.xyz + wind scalar mag
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD1, VERTEX_ATTRIB_TYPE_FLOAT, 4 },          // diffuse texcoords.st + detail texcoords.st
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD2, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // normal.xyz + amb occ
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD3, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // tangent.xyz
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD4, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // wind data.xyzw
            VERTEX_ATTRIB_END( )
        };

        st_bool bUse16BitIndices = IndexedTrianglesCanFitInto16BitIB(pLods, c_nNumLods);

        if (pGeometryBuffer->SetVertexFormat(asBranchVertexFormat, pTechnique) &&
            pGeometryBuffer->SetIndexFormat(bUse16BitIndices ? INDEX_FORMAT_UNSIGNED_16BIT : INDEX_FORMAT_UNSIGNED_32BIT) &&
            ReserveGeometry(pGeometry, GEOMETRY_TYPE_BRANCHES))
        {
            assert(sizeof(SBranchVertex) == pGeometryBuffer->VertexSize( ));

            // fill out vertex and index buffers
            m_acRenderInfo[GEOMETRY_TYPE_BRANCHES].InitIndexed(pLods, c_nNumLods);
            for (st_int32 nLod = 0; nLod < c_nNumLods; ++nLod)
            {
                const SIndexedTriangles* pLod = pLods + nLod;

                // an LOD must have these values before the render path will work
                if (pLod->HasGeometry( ))
                {
                    assert(pLod->m_pNormals && pLod->m_pTexCoordsDiffuse);

                    // initialize index buffer
                    bSuccess &= InitIndexBuffer(nLod, pLod, pGeometryBuffer, m_acRenderInfo[GEOMETRY_TYPE_BRANCHES], bUse16BitIndices);

                    // fill out vertex buffer
                    CLocalArray<SBranchVertex> aVertices(pLod->m_nNumVertices, "InitBranches");
                    SBranchVertex* pVertex = &aVertices[0];
                    for (st_int32 nVertex = 0; nVertex < pLod->m_nNumVertices; ++nVertex)
                    {
                        // position
                        memcpy(pVertex->m_afCoord, pLod->m_pCoords + nVertex * 3, sizeof(pVertex->m_afCoord));

                        // LOD position
                        memcpy(pVertex->m_afLodCoord, pLod->m_pLodCoords ? pLod->m_pLodCoords + nVertex * 3 : pLod->m_pCoords + nVertex * 3, sizeof(pVertex->m_afLodCoord));

                        // normal
                        memcpy(pVertex->m_aucNormal, pLod->m_pNormals + nVertex * 3, sizeof(pVertex->m_aucNormal));

                        // ambient occlusion
                        pVertex->m_ucAmbOcc = pLod->m_pAmbientOcclusionValues ? pLod->m_pAmbientOcclusionValues[nVertex] : 255;

                        // diffuse texcoords
                        pVertex->m_afDiffuseTexCoords[0] = pLod->m_pTexCoordsDiffuse[nVertex * 2];
                        pVertex->m_afDiffuseTexCoords[1] = pLod->m_pTexCoordsDiffuse[nVertex * 2 + 1];

                        // detail texcoords
                        if (pLod->m_pTexCoordsDetail)
                        {
                            pVertex->m_afDetailTexCoords[0] = pLod->m_pTexCoordsDetail[nVertex * 2];
                            pVertex->m_afDetailTexCoords[1] = pLod->m_pTexCoordsDetail[nVertex * 2 + 1];
                        }
                        else
                            pVertex->m_afDetailTexCoords[0] = pVertex->m_afDetailTexCoords[1] = 0.0f;

                        // tangent
                        memcpy(pVertex->m_aucTangent, pLod->m_pTangents + nVertex * 3, sizeof(pVertex->m_aucTangent));
                        pVertex->m_ucTangentPadding = 0;

                        // wind data
                        if (pLod->m_pWindData)
                        {
                            pVertex->m_fWindScalarMag = pLod->m_fWindDataMagnitude;
                            memcpy(pVertex->m_aucWindData, pLod->m_pWindData + nVertex * 6, sizeof(pVertex->m_aucWindData));
                        }
                        else
                        {
                            pVertex->m_fWindScalarMag = 0.0f;
                            pVertex->m_aucWindData[0] = pVertex->m_aucWindData[1] = pVertex->m_aucWindData[2] = pVertex->m_aucWindData[3] = 0;
                        }

                        ++pVertex;
                    }

                    bSuccess &= pGeometryBuffer->AppendVertices(&aVertices[0], pLod->m_nNumVertices);
                }
            }
        }

        bSuccess &= pGeometryBuffer->EndVertices( );
        bSuccess &= pGeometryBuffer->EndIndices( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  struct SFrondVertex

struct SFrondVertex
{
    st_float32  m_afCoord[3];                                   // position
    st_float32  m_afLodCoord[3], m_fWindScalarMag;              // texcoord0
    st_float32  m_afDiffuseTexCoords[2], m_afFrondRipple[2];    // texcoord1
    st_uchar    m_aucNormal[3], m_ucAmbOcc;                     // texcoord2
    st_uchar    m_aucTangent[3], m_ucTangentPadding;            // texcoord3
    st_uchar    m_aucWindData[4];                               // texcoord4
};


///////////////////////////////////////////////////////////////////////
//  CTreeRI::InitFronds

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::InitFronds(const SGeometry* pGeometry, const TShaderTechniquePolicy* pTechnique)
{
    assert(pGeometry);
    st_bool bSuccess = true;

    // setup some aliases
    const st_int32 c_nNumLods = pGeometry->m_nNumFrondLods;
    const SIndexedTriangles* pLods = pGeometry->m_pFrondLods;
    TGeometryBufferPolicy* pGeometryBuffer = m_atGeometryBuffers + GEOMETRY_TYPE_FRONDS;

    if (c_nNumLods > 0 && pLods) // may not have frond geometry in this model
    {
        // setup vertex structure
        SVertexAttribDesc asFrondVertexFormat[ ] =
        {
            { VERTEX_ATTRIB_SEMANTIC_POS, VERTEX_ATTRIB_TYPE_FLOAT, 3 },                // pos.xyz
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD0, VERTEX_ATTRIB_TYPE_FLOAT, 4 },          // LOD position.xyz + wind scalar mag
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD1, VERTEX_ATTRIB_TYPE_FLOAT, 4 },          // diffuse texcoords.st + frond ripple
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD2, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // normal.xyz + amb occ
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD3, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // tangent.xyz
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD4, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // wind data.xyzw
            VERTEX_ATTRIB_END( )
        };

        st_bool bUse16BitIndices = IndexedTrianglesCanFitInto16BitIB(pLods, c_nNumLods);

        if (pGeometryBuffer->SetVertexFormat(asFrondVertexFormat, pTechnique) &&
            pGeometryBuffer->SetIndexFormat(bUse16BitIndices ? INDEX_FORMAT_UNSIGNED_16BIT : INDEX_FORMAT_UNSIGNED_32BIT) &&
            ReserveGeometry(pGeometry, GEOMETRY_TYPE_FRONDS))
        {
            assert(sizeof(SFrondVertex) == pGeometryBuffer->VertexSize( ));

            // fill out vertex and index buffers
            m_acRenderInfo[GEOMETRY_TYPE_FRONDS].InitIndexed(pLods, c_nNumLods);
            for (st_int32 nLod = 0; nLod < c_nNumLods; ++nLod)
            {
                const SIndexedTriangles* pLod = pLods + nLod;

                // an LOD must have these values before the render path will work
                if (pLod->HasGeometry( ))
                {
                    assert(pLod->m_pNormals && pLod->m_pTexCoordsDiffuse);

                    // initialize index buffer
                    bSuccess &= InitIndexBuffer(nLod, pLod, pGeometryBuffer, m_acRenderInfo[GEOMETRY_TYPE_FRONDS], bUse16BitIndices);

                    // fill out vertex buffer
                    CLocalArray<SFrondVertex> aVertices(pLod->m_nNumVertices, "InitFronds");
                    SFrondVertex* pVertex = &aVertices[0];
                    for (st_int32 nVertex = 0; nVertex < pLod->m_nNumVertices; ++nVertex)
                    {
                        // position
                        memcpy(pVertex->m_afCoord, pLod->m_pCoords + nVertex * 3, sizeof(pVertex->m_afCoord));

                        // LOD position
                        memcpy(pVertex->m_afLodCoord, pLod->m_pLodCoords ? pLod->m_pLodCoords + nVertex * 3 : pLod->m_pCoords + nVertex * 3, sizeof(pVertex->m_afLodCoord));

                        // normal
                        memcpy(pVertex->m_aucNormal, pLod->m_pNormals + nVertex * 3, sizeof(pVertex->m_aucNormal));

                        // ambient occlusion
                        pVertex->m_ucAmbOcc = pLod->m_pAmbientOcclusionValues ? pLod->m_pAmbientOcclusionValues[nVertex] : 255;

                        // diffuse texcoords
                        pVertex->m_afDiffuseTexCoords[0] = pLod->m_pTexCoordsDiffuse[nVertex * 2];
                        pVertex->m_afDiffuseTexCoords[1] = pLod->m_pTexCoordsDiffuse[nVertex * 2 + 1];

                        // frond ripple / ambient occlusion
                        pVertex->m_afFrondRipple[0] = pLod->m_pFrondRipple[nVertex * 2 + 0];
                        pVertex->m_afFrondRipple[1] = pLod->m_pFrondRipple[nVertex * 2 + 1];

                        // tangent
                        memcpy(pVertex->m_aucTangent, pLod->m_pTangents + nVertex * 3, sizeof(pVertex->m_aucTangent));
                        pVertex->m_ucTangentPadding = 0;

                        // wind data
                        if (pLod->m_pWindData)
                        {
                            pVertex->m_fWindScalarMag = pLod->m_fWindDataMagnitude;
                            memcpy(pVertex->m_aucWindData, pLod->m_pWindData + nVertex * 6, sizeof(pVertex->m_aucWindData));
                        }
                        else
                        {
                            pVertex->m_fWindScalarMag = 0.0f;
                            pVertex->m_aucWindData[0] = pVertex->m_aucWindData[1] = pVertex->m_aucWindData[2] = pVertex->m_aucWindData[3] = 0;
                        }

                        ++pVertex;
                    }

                    bSuccess &= pGeometryBuffer->AppendVertices(&aVertices[0], pLod->m_nNumVertices);
                }
            }
        }

        bSuccess &= pGeometryBuffer->EndVertices( );
        bSuccess &= pGeometryBuffer->EndIndices( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  struct SLeafCardVertex

struct SLeafCardVertex
{
    st_float32  m_afCardCenter[3], m_fWindScalar;                           // position
    st_uchar    m_aucNormal[3], m_ucAmbOcc;                                 // texcoord0
    st_uchar    m_aucTangent[3], m_ucTangentPadding;                        // texcoord1
    st_uchar    m_aucWindData[4];                                           // texcoord2
    st_float32  m_afCardCorner[2], m_fLodScale, m_fWindScalarMag;           // texcoord3
    st_float32  m_afDiffuseTexCoords[2], m_fPlanarOffset, m_fShadowOffset;  // texcoord4
};


///////////////////////////////////////////////////////////////////////
//  CTreeRI::InitLeafCards

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::InitLeafCards(const SGeometry* pGeometry, const TShaderTechniquePolicy* pTechnique)
{
    assert(pGeometry);
    st_bool bSuccess = true;

    // setup some aliases
    const st_int32 c_nNumLods = pGeometry->m_nNumLeafCardLods;
    const SLeafCards* pLods = pGeometry->m_pLeafCardLods;
    TGeometryBufferPolicy* pGeometryBuffer = m_atGeometryBuffers + GEOMETRY_TYPE_LEAF_CARDS;

    if (c_nNumLods > 0 && pLods) // may not have leaf card geometry in this model
    {
        // setup vertex structure
        SVertexAttribDesc asLeafCardVertexFormat[ ] =
        {
            { VERTEX_ATTRIB_SEMANTIC_POS, VERTEX_ATTRIB_TYPE_FLOAT, 4 },                // pos.xyz + wind scalar
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD0, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // normal.xyz + amb occ
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD1, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // tangent.xyz + tangent pad
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD2, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // wind data.xyzw
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD3, VERTEX_ATTRIB_TYPE_FLOAT, 4 },          // lod scales.xy, card corner.zw
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD4, VERTEX_ATTRIB_TYPE_FLOAT, 4 },          // diffuse texcoords.xy, planar offset.z, shadow offset.z
            VERTEX_ATTRIB_END( )
        };

        st_bool bUse16BitIndices = false;
        if (!m_bQuadPrimitiveSupported)
        {
            bUse16BitIndices = LeafCardsCanFitInto16BitIB(pLods, c_nNumLods);
            pGeometryBuffer->SetIndexFormat(bUse16BitIndices ? INDEX_FORMAT_UNSIGNED_16BIT : INDEX_FORMAT_UNSIGNED_32BIT);
        }

        if (pGeometryBuffer->SetVertexFormat(asLeafCardVertexFormat, pTechnique) &&
            ReserveGeometry(pGeometry, GEOMETRY_TYPE_LEAF_CARDS))
        {
            assert(sizeof(SLeafCardVertex) == pGeometryBuffer->VertexSize( ));

            // fill out a single vertex buffer to accommodate all LODs (avoids buffer state changes during render)
            m_acRenderInfo[GEOMETRY_TYPE_LEAF_CARDS].InitLeafCards(pLods, c_nNumLods);
            for (st_int32 nLod = 0; nLod < c_nNumLods; ++nLod)
            {
                const SLeafCards* pLod = pLods + nLod;

                // initialize index buffer
                bSuccess &= InitLeafCardRenderInfo(nLod, pLod);

                // an LOD must have these values before the render path will work
                if (pLod->HasGeometry( ))
                {
                    assert(pLod->m_pNormals && pLod->m_pTexCoordsDiffuse);

                    // copy this LOD into the vertex buffer
                    CLocalArray<SLeafCardVertex> aVertices(pLod->m_nTotalNumCards * 4, "InitLeafCards");
                    SLeafCardVertex* pVertex = &aVertices[0];
                    st_uint32 uiThisLodStartingVertexIndex = pGeometryBuffer->NumVertices( );
                    for (st_int32 nCard = 0; nCard < pLod->m_nTotalNumCards; ++nCard)
                    {
                        const float c_afOffsets[4][2] =  { { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.5f, -0.5f }, { -0.5f, -0.5f } };

                        // setup card values used by each vertex
                        const st_float32* c_pDimensons = pLod->m_pDimensions + nCard * 2;
                        const st_float32* c_pPivotPoint = pLod->m_pPivotPoints + nCard * 2;
                        Vec3 vPivot(c_pPivotPoint[0] * c_pDimensons[0], c_pPivotPoint[1] * c_pDimensons[1], 0.0f);

                        // determine the largest distance from the pivot point; useful for the shadow offset value
                        st_float32 fMaxDistanceFromPivotPoint = 0.0f;
                        st_int32 nCorner = 0;
                        for (nCorner = 0; nCorner < 4; ++nCorner)
                        {
                            Vec3 vCorner(c_afOffsets[nCorner][0] * c_pDimensons[0], c_afOffsets[nCorner][1] * c_pDimensons[1], 0.0f);
                            fMaxDistanceFromPivotPoint = st_max(fMaxDistanceFromPivotPoint, vCorner.Distance(vPivot));
                        }

                        for (nCorner = 0; nCorner < 4; ++nCorner)
                        {
                            // card center
                            memcpy(pVertex->m_afCardCenter, pLod->m_pPositions + nCard * 3, sizeof(pVertex->m_afCardCenter));

                            // wind scalar 
                            if (pLod->m_pWindData)
                                pVertex->m_fWindScalar = c_fUncompressWindScalar * CCore::UncompressScalar(pLod->m_pWindData[nCard * 5 + 4]);
                            else
                                pVertex->m_fWindScalar = 0.0f;

                            // normal
                            memcpy(pVertex->m_aucNormal, pLod->m_pNormals + nCard * 12 + nCorner * 3, sizeof(pVertex->m_aucNormal));

                            // ambient occlusion
                            pVertex->m_ucAmbOcc = pLod->m_pAmbientOcclusionValues ? pLod->m_pAmbientOcclusionValues[nCard] : 255;

                            // tangent
                            memcpy(pVertex->m_aucTangent, pLod->m_pTangents + nCard * 12 + nCorner * 3, sizeof(pVertex->m_aucTangent));
                            pVertex->m_ucTangentPadding = 0;

                            // wind data
                            if (pLod->m_pWindData)
                            {
                                pVertex->m_fWindScalarMag = pLod->m_fWindDataMagnitude;
                                memcpy(pVertex->m_aucWindData, pLod->m_pWindData + nCard * 5, sizeof(pVertex->m_aucWindData));
                            }
                            else
                            {
                                pVertex->m_fWindScalarMag = 0.0f;
                                pVertex->m_aucWindData[0] = pVertex->m_aucWindData[1] = pVertex->m_aucWindData[2] = pVertex->m_aucWindData[3] = 0;
                            }

                            // lod scales
                            const st_float32* pLodScales = pLod->m_pLodScales + nCard * 2;
                            if (pLodScales[1] == 0.0f)
                                pVertex->m_fLodScale = 0.0f;
                            else if (pLodScales[0] == pLodScales[1])
                                pVertex->m_fLodScale = 1.0f;
                            else
                                pVertex->m_fLodScale = pLodScales[1] / pLodScales[0];

                            // card corner
                            const st_float32 c_fStartLodScale = pLodScales[0];
                            if (CCoordSys::IsYAxisUp( ))
                            {
                                pVertex->m_afCardCorner[0] = (c_afOffsets[nCorner][1] - c_pPivotPoint[1]) * c_pDimensons[1] * c_fStartLodScale;
                                pVertex->m_afCardCorner[1] = (c_afOffsets[nCorner][0] - c_pPivotPoint[0]) * c_pDimensons[0] * c_fStartLodScale;
                            }
                            else
                            {
                                pVertex->m_afCardCorner[0] = (c_afOffsets[nCorner][0] - c_pPivotPoint[0]) * c_pDimensons[0] * c_fStartLodScale;
                                pVertex->m_afCardCorner[1] = (c_afOffsets[nCorner][1] - c_pPivotPoint[1]) * c_pDimensons[1] * c_fStartLodScale;
                            }

                            // diffuse texcoords
                            const st_float32* pTexCoords = pLod->m_pTexCoordsDiffuse + nCard * 8 + nCorner * 2;
                            pVertex->m_afDiffuseTexCoords[0] = pTexCoords[0];
                            pVertex->m_afDiffuseTexCoords[1] = pTexCoords[1];

                            // planar offset
                            pVertex->m_fPlanarOffset = pLod->m_pLeafCardOffsets ? pLod->m_pLeafCardOffsets[nCard * 4 + nCorner] : 0.0f;

                            // shadow offset
                            pVertex->m_fShadowOffset = fMaxDistanceFromPivotPoint;

                            ++pVertex;
                        }
                    }

                    bSuccess &= pGeometryBuffer->AppendVertices(&aVertices[0], st_uint32(aVertices.size( )));
                    aVertices.Release( );

                    // if quads are not a supported rendering primitive, prepare an index buffer of suitable size that will rendering using indexed
                    // triangle lists to compose the quad geometry
                    if (!m_bQuadPrimitiveSupported && pLod->m_nTotalNumCards > 0)
                    {
                        if (bUse16BitIndices)
                        {
                            CLocalArray<st_uint16> aCardIndices(pLod->m_nTotalNumCards * 6, "InitLeafCards16");
                            st_uint16* pTable = &aCardIndices[0];
                            for (st_int32 i = 0; i < pLod->m_nTotalNumCards; ++i)
                            {
                                // first half of card
                                *pTable++ = st_uint16(uiThisLodStartingVertexIndex + i * 4 + 0);
                                *pTable++ = st_uint16(uiThisLodStartingVertexIndex + i * 4 + 1);
                                *pTable++ = st_uint16(uiThisLodStartingVertexIndex + i * 4 + 2);

                                // second half of card
                                *pTable++ = st_uint16(uiThisLodStartingVertexIndex + i * 4 + 0);
                                *pTable++ = st_uint16(uiThisLodStartingVertexIndex + i * 4 + 2);
                                *pTable++ = st_uint16(uiThisLodStartingVertexIndex + i * 4 + 3);
                            }
                            assert(st_int32(pTable - &aCardIndices[0]) == pLod->m_nTotalNumCards * 6);
                            bSuccess &= pGeometryBuffer->AppendIndices(&aCardIndices[0], pLod->m_nTotalNumCards * 6);
                        }
                        else
                        {
                            CLocalArray<st_uint32> aCardIndices(pLod->m_nTotalNumCards * 6, "InitLeafCards32");
                            st_uint32* pTable = &aCardIndices[0];
                            for (st_int32 i = 0; i < pLod->m_nTotalNumCards; ++i)
                            {
                                // first half of card
                                *pTable++ = uiThisLodStartingVertexIndex + i * 4 + 0;
                                *pTable++ = uiThisLodStartingVertexIndex + i * 4 + 1;
                                *pTable++ = uiThisLodStartingVertexIndex + i * 4 + 2;

                                // second half of card
                                *pTable++ = uiThisLodStartingVertexIndex + i * 4 + 0;
                                *pTable++ = uiThisLodStartingVertexIndex + i * 4 + 2;
                                *pTable++ = uiThisLodStartingVertexIndex + i * 4 + 3;
                            }
                            assert(st_int32(pTable - &aCardIndices[0]) == pLod->m_nTotalNumCards * 6);
                            bSuccess &= pGeometryBuffer->AppendIndices(&aCardIndices[0], pLod->m_nTotalNumCards * 6);
                        }

                    }
                }
            }
        }

        bSuccess &= pGeometryBuffer->EndVertices( );
        if (!m_bQuadPrimitiveSupported)
            bSuccess &= pGeometryBuffer->EndIndices( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  struct SLeafMeshVertex

struct SLeafMeshVertex
{
    st_float32  m_afCoord[3], m_fWindScalar;            // position
    st_float32  m_afLodCoord[3], m_fWindScalarMag;      // texcoord0
    st_uchar    m_aucNormal[3], m_ucAmbOcc;             // texcoord1
    st_uchar    m_aucTangent[3], m_ucWindActiveFlag;    // texcoord2
    st_uchar    m_aucWindData[4];                       // texcoord3
    st_float32  m_afDiffuseTexCoords[2];                // texcoord4
#ifdef SPEEDTREE_ADVANCED_WIND
    st_float32  m_afLeafMeshWind[3];                    // texcoord5
#endif
};


///////////////////////////////////////////////////////////////////////
//  CTreeRI::InitLeafMeshes

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::InitLeafMeshes(const SGeometry* pGeometry, const TShaderTechniquePolicy* pTechnique)
{
    assert(pGeometry);
    st_bool bSuccess = true;

    // setup some aliases
    const st_int32 c_nNumLods = pGeometry->m_nNumLeafMeshLods;
    const SIndexedTriangles* pLods = pGeometry->m_pLeafMeshLods;
    TGeometryBufferPolicy* pGeometryBuffer = m_atGeometryBuffers + GEOMETRY_TYPE_LEAF_MESHES;

    if (c_nNumLods > 0 && pLods) // may not have leaf mesh geometry in this model
    {
        // setup vertex structure
        SVertexAttribDesc asLeafMeshVertexFormat[ ] =
        {
            { VERTEX_ATTRIB_SEMANTIC_POS, VERTEX_ATTRIB_TYPE_FLOAT, 4 },                // pos.xyz _ wind scalar
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD0, VERTEX_ATTRIB_TYPE_FLOAT, 4 },          // LOD position.xyz + wind scalar mag
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD1, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // normal.xyz + amb occ
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD2, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // tangent.xyz + wind active flag
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD3, VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE, 4 },  // wind data.xyzw
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD4, VERTEX_ATTRIB_TYPE_FLOAT, 2 },          // diffuse texcoords.st
        #ifdef SPEEDTREE_ADVANCED_WIND
            { VERTEX_ATTRIB_SEMANTIC_TEXCOORD5, VERTEX_ATTRIB_TYPE_FLOAT, 3 },          // leaf mesh wind.xyz
        #endif
            VERTEX_ATTRIB_END( )
        };

        st_bool bUse16BitIndices = IndexedTrianglesCanFitInto16BitIB(pLods, c_nNumLods);

        if (pGeometryBuffer->SetVertexFormat(asLeafMeshVertexFormat, pTechnique) &&
            pGeometryBuffer->SetIndexFormat(bUse16BitIndices ? INDEX_FORMAT_UNSIGNED_16BIT : INDEX_FORMAT_UNSIGNED_32BIT) &&
            ReserveGeometry(pGeometry, GEOMETRY_TYPE_LEAF_MESHES))
        {
            assert(sizeof(SLeafMeshVertex) == pGeometryBuffer->VertexSize( ));

            // fill out vertex and index buffers
            m_acRenderInfo[GEOMETRY_TYPE_LEAF_MESHES].InitIndexed(pLods, c_nNumLods);
            for (st_int32 nLod = 0; nLod < c_nNumLods; ++nLod)
            {
                const SIndexedTriangles* pLod = pLods + nLod;

                // an LOD must have these values before the render path will work
                if (pLod->HasGeometry( ))
                {
                    assert(pLod->m_pNormals && pLod->m_pTexCoordsDiffuse);

                    // initialize index buffer
                    bSuccess &= InitIndexBuffer(nLod, pLod, pGeometryBuffer, m_acRenderInfo[GEOMETRY_TYPE_LEAF_MESHES], bUse16BitIndices);

                    // fill out vertex buffer
                    CLocalArray<SLeafMeshVertex> aVertices(pLod->m_nNumVertices, "LeafMeshes");
                    SLeafMeshVertex* pVertex = &aVertices[0];
                    for (st_int32 nVertex = 0; nVertex < pLod->m_nNumVertices; ++nVertex)
                    {
                        // position
                        memcpy(pVertex->m_afCoord, pLod->m_pCoords + nVertex * 3, sizeof(pVertex->m_afCoord));

                        // wind scalar
                        if (pLod->m_pWindData)
                            pVertex->m_fWindScalar = c_fUncompressWindScalar * CCore::UncompressScalar(pLod->m_pWindData[nVertex * 6 + 4]);
                        else
                            pVertex->m_fWindScalar = 0.0f;

                        // LOD position
                        memcpy(pVertex->m_afLodCoord, pLod->m_pLodCoords ? pLod->m_pLodCoords + nVertex * 3 : pLod->m_pCoords + nVertex * 3, sizeof(pVertex->m_afLodCoord));

                        // normal
                        memcpy(pVertex->m_aucNormal, pLod->m_pNormals + nVertex * 3, sizeof(pVertex->m_aucNormal));

                        // ambient occlusion
                        pVertex->m_ucAmbOcc = pLod->m_pAmbientOcclusionValues ? pLod->m_pAmbientOcclusionValues[nVertex] : 255;

                        // tangent
                        memcpy(pVertex->m_aucTangent, pLod->m_pTangents + nVertex * 3, sizeof(pVertex->m_aucTangent));

                        // wind active flag
                        pVertex->m_ucWindActiveFlag = st_float32(pLod->m_pWindData[nVertex * 6 + 5]) ? 255 : 0;

                        // wind data
                        if (pLod->m_pWindData)
                        {
                            pVertex->m_fWindScalarMag = pLod->m_fWindDataMagnitude;
                            memcpy(pVertex->m_aucWindData, pLod->m_pWindData + nVertex * 6, sizeof(pVertex->m_aucWindData));
                        }
                        else
                        {
                            pVertex->m_fWindScalarMag = 0.0f;
                            pVertex->m_aucWindData[0] = pVertex->m_aucWindData[1] = pVertex->m_aucWindData[2] = pVertex->m_aucWindData[3] = 0;
                        }

                        // diffuse texcoords
                        pVertex->m_afDiffuseTexCoords[0] = pLod->m_pTexCoordsDiffuse[nVertex * 2];
                        pVertex->m_afDiffuseTexCoords[1] = pLod->m_pTexCoordsDiffuse[nVertex * 2 + 1];

                        #ifdef SPEEDTREE_ADVANCED_WIND
                            // leaf mesh wind
                            pVertex->m_afLeafMeshWind[0] = pLod->m_pLeafMeshWind[nVertex * 3];
                            pVertex->m_afLeafMeshWind[1] = pLod->m_pLeafMeshWind[nVertex * 3 + 1];
                            pVertex->m_afLeafMeshWind[2] = pLod->m_pLeafMeshWind[nVertex * 3 + 2];
                        #endif

                        ++pVertex;
                    }

                    pGeometryBuffer->AppendVertices(&aVertices[0], pLod->m_nNumVertices);
                }
            }
        }

        bSuccess &= pGeometryBuffer->EndVertices( );
        bSuccess &= pGeometryBuffer->EndIndices( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::ReserveGeometry
//
//  This function is used soley to compute the about of VB and IB space
//  necessary for use by the CGeometryBufferRI class so that we don't have
//  to use CArray::resize() a bunch of times, fragmenting the heap.

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::ReserveGeometry(const SGeometry* pGeometry, EGeometryType eType)
{
    st_bool bSuccess = false;

    if (pGeometry)
    {
        st_uint32 uiNumVertices = 0;
        st_uint32 uiNumIndices = 0;

        if (eType == GEOMETRY_TYPE_BRANCHES ||
            eType == GEOMETRY_TYPE_FRONDS ||
            eType == GEOMETRY_TYPE_LEAF_MESHES)
        {
            st_int32 nNumLods = pGeometry->m_nNumBranchLods;
            SIndexedTriangles* pLods = pGeometry->m_pBranchLods;

            if (eType == GEOMETRY_TYPE_FRONDS)
            {
                nNumLods = pGeometry->m_nNumFrondLods;
                pLods = pGeometry->m_pFrondLods;
            }
            else if (eType == GEOMETRY_TYPE_LEAF_MESHES)
            {
                nNumLods = pGeometry->m_nNumLeafMeshLods;
                pLods = pGeometry->m_pLeafMeshLods;
            }
            assert(nNumLods > 0);
            assert(pLods);

            for (st_int32 nLod = 0; nLod < nNumLods; ++nLod)
            {
                const SIndexedTriangles* pLod = pLods + nLod;

                // vertex count
                uiNumVertices += pLod->m_nNumVertices;

                // index count
                for (st_int32 nMaterialGroup = 0; nMaterialGroup < pLod->m_nNumMaterialGroups; ++nMaterialGroup)
                {
                    const SDrawCallInfo* pSrcDrawInfo = pLod->m_pDrawCallInfo + nMaterialGroup;
                    uiNumIndices += pSrcDrawInfo->m_nLength;
                }
            }
        }
        else
        {
            // assume leaf cards
            assert(eType == GEOMETRY_TYPE_LEAF_CARDS);

            const st_int32 c_nNumLods = pGeometry->m_nNumLeafCardLods;
            const SLeafCards* pLods = pGeometry->m_pLeafCardLods;

            for (st_int32 nLod = 0; nLod < c_nNumLods; ++nLod)
            {
                const SLeafCards* pLod = pLods + nLod;
                
                // vertex count
                uiNumVertices += pLod->m_nTotalNumCards * 4;

                // index count
                if (!m_bQuadPrimitiveSupported)
                    uiNumIndices += pLod->m_nTotalNumCards * 6;
            }
        }

        bSuccess = m_atGeometryBuffers[eType].Reserve(uiNumVertices, uiNumIndices);
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::IndexedTrianglesCanFitInto16BitIB
//
//  Determine if we can use 16-bit indices; only possible if:
//
//      1. All LODs use them since they all share the same index buffer
//      2. The vertices they reference are stacked in one VB, so they can't surpass
//         65k aggregately

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::IndexedTrianglesCanFitInto16BitIB(const SIndexedTriangles* pLods, st_int32 nNumLods) const
{
    st_bool bFitsInto16BitBuffer = true;

    st_uint32 uiTotalVertices = 0;
    for (st_int32 nLod = 0; nLod < nNumLods; ++nLod)
    {
        uiTotalVertices += st_uint32(pLods[nLod].m_nNumVertices);
        if (pLods[nLod].m_pTriangleIndices32)
        {
            bFitsInto16BitBuffer = false;
            break;
        }
    }

    const st_uint32 uiMax16BitIndex = 65535;
    if (uiTotalVertices > uiMax16BitIndex)
        bFitsInto16BitBuffer = false;

    return bFitsInto16BitBuffer;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::LeafCardsCanFitInto16BitIB
//
//  Determine if we can use 16-bit indices; only possible if:
//
//      1. All LODs use them since they all share the same index buffer
//      2. The vertices they reference are stacked in one VB, so they can't surpass
//         65k aggregately

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::LeafCardsCanFitInto16BitIB(const SLeafCards* pLods, st_int32 nNumLods) const
{
    st_uint32 uiTotalVertices = 0;
    for (st_int32 nLod = 0; nLod < nNumLods; ++nLod)
        uiTotalVertices += st_uint32(pLods[nLod].m_nTotalNumCards * 4);

    const st_uint32 uiMax16BitIndex = 65535;

    return (uiTotalVertices <= uiMax16BitIndex);
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::InitIndexBuffer
//
//  In addition to building a single index buffer that will store indices
//  for all LODs and materials in this tree model, this function will
//  also determine the draw offsets, indices, and material indices in
//  m_asDrawCallInfo.

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::InitIndexBuffer(st_int32 nLod, 
                                          const SIndexedTriangles* pLod, 
                                          TGeometryBufferPolicy* pGeometryBuffer, 
                                          CLodRenderInfo& cLodRenderInfo,
                                          st_bool bUse16BitIndices)
{
    assert(pLod);
    assert(pGeometryBuffer);

    st_bool bSuccess = false;

    // need to copy indices into an intermediate buffer because we're combining all of the LODs into a 
    // single index buffer, necessitating index offsets

    // figure out how large the overall buffer is
    st_int32 nBufferSize = 0;
    for (st_int32 nMaterialGroup = 0; nMaterialGroup < pLod->m_nNumMaterialGroups; ++nMaterialGroup)
    {
        const SDrawCallInfo* pInfo = pLod->m_pDrawCallInfo + nMaterialGroup;
        nBufferSize += pInfo->m_nLength;
    }
    assert(nBufferSize > 0);
    CLocalArray<st_uint32> aIndices32(nBufferSize, "InitIndexBuffer32", false);
    CLocalArray<st_uint16> aIndices16(nBufferSize, "InitIndexBuffer16", false);

    // fill out the index buffer and record other rendering hints
    for (st_int32 nMaterialGroup = 0; nMaterialGroup < pLod->m_nNumMaterialGroups; ++nMaterialGroup)
    {
        // source draw information (raw data taken from SRT file)
        const SDrawCallInfo* pSrcDrawInfo = pLod->m_pDrawCallInfo + nMaterialGroup;

        // add reference for quick LOD/material access during render loop
        CLodRenderInfo::SRef sLodRef;
        sLodRef.m_nMaterialId = pSrcDrawInfo->m_nMaterialIndex;
        sLodRef.m_uiOffset = st_uint32(pSrcDrawInfo->m_nOffset + pGeometryBuffer->NumIndices( ));
        sLodRef.m_uiMinIndex = st_uint32(pSrcDrawInfo->m_nMinIndex + pGeometryBuffer->NumVertices( ));
        sLodRef.m_uiNumVertices = st_uint32(pSrcDrawInfo->m_nNumVertices);
        sLodRef.m_uiLength = st_uint32(pSrcDrawInfo->m_nLength);
        cLodRenderInfo.AddRef(nLod, sLodRef);

        // collect indices for a single draw call (for pLod/nMaterialGroup)
        if (bUse16BitIndices)
        {
            assert(pLod->m_pTriangleIndices16);
            assert(!pLod->m_pTriangleIndices32);
            for (st_int32 i = pSrcDrawInfo->m_nOffset; i < pSrcDrawInfo->m_nOffset + pSrcDrawInfo->m_nLength; ++i)
                aIndices16.push_back(st_uint16(pLod->m_pTriangleIndices16[i] + pGeometryBuffer->NumVertices( )));
        }
        else
        {
            if (pLod->m_pTriangleIndices32)
                for (st_int32 i = pSrcDrawInfo->m_nOffset; i < pSrcDrawInfo->m_nOffset + pSrcDrawInfo->m_nLength; ++i)
                    aIndices32.push_back(pLod->m_pTriangleIndices32[i] + pGeometryBuffer->NumVertices( ));
            else
                for (st_int32 i = pSrcDrawInfo->m_nOffset; i < pSrcDrawInfo->m_nOffset + pSrcDrawInfo->m_nLength; ++i)
                    aIndices32.push_back(st_uint32(pLod->m_pTriangleIndices16[i] + pGeometryBuffer->NumVertices( )));
        }
    }

    // finally, copy the array of offset indices into the geometry buffer
    if (bUse16BitIndices)
    {
        if (!aIndices16.empty( ))
            bSuccess = pGeometryBuffer->AppendIndices((const st_byte*) &aIndices16[0], st_uint32(aIndices16.size( )));
    }
    else
    {
        if (!aIndices32.empty( ))
            bSuccess = pGeometryBuffer->AppendIndices((const st_byte*) &aIndices32[0], st_uint32(aIndices32.size( )));
    }
    
    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CTreeRI::InitLeafCardRenderInfo

template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
inline st_bool CTreeRI_t::InitLeafCardRenderInfo(st_int32 nLod, const SLeafCards* pLod)
{
    assert(nLod > -1);
    assert(pLod);

    CLodRenderInfo* pLodRenderInfo = m_acRenderInfo + GEOMETRY_TYPE_LEAF_CARDS;
    const TGeometryBufferPolicy* pGeometryBuffer = m_atGeometryBuffers + GEOMETRY_TYPE_LEAF_CARDS;

    // fill out the index buffer and record other rendering hints
    for (st_int32 nMaterialGroup = 0; nMaterialGroup < pLod->m_nNumMaterialGroups; ++nMaterialGroup)
    {
        // source draw information (raw data taken from SRT file); draw call info for cards is a little different
        // than for indexed triangles; instead of vertex counts, offset and length count the number of cards since
        // vertex count varies depending on the quads to render quads
        const SDrawCallInfo* pSrcDrawInfo = pLod->m_pDrawCallInfo + nMaterialGroup;

        // add reference for quick LOD/material access during render loop
        CLodRenderInfo::SRef sLodRef;
        sLodRef.m_nMaterialId = pSrcDrawInfo->m_nMaterialIndex;
        if (m_bQuadPrimitiveSupported)
        {
            sLodRef.m_uiOffset = (pSrcDrawInfo->m_nOffset * 4) + pGeometryBuffer->NumVertices( );
            sLodRef.m_uiLength = pSrcDrawInfo->m_nLength * 4;
            // sLodRef.m_uiMinIndex && sLod.m_uiNumVertices unused by RenderArrays()
        }
        else
        {
            sLodRef.m_uiOffset = (pSrcDrawInfo->m_nOffset * 6) + pGeometryBuffer->NumIndices( );
            sLodRef.m_uiLength = pSrcDrawInfo->m_nLength * 6;
            sLodRef.m_uiMinIndex = (2 * pGeometryBuffer->NumIndices( )) / 3;
            sLodRef.m_uiNumVertices = pSrcDrawInfo->m_nLength * 4;
        }
        pLodRenderInfo->AddRef(nLod, sLodRef);
    }

    return true;
}
