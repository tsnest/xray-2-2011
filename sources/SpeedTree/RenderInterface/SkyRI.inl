///////////////////////////////////////////////////////////////////////  
//  SkyRI.inl
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
//  SSkyRenderInfo::SSkyRenderInfo

inline SSkyRenderInfo::SSkyRenderInfo( ) :
    m_nNumLargeStars(0),
    m_nNumSmallStars(0),
    m_fFogStartDistance(0.0f),
    m_fFogEndDistance(1000.0f),
    m_fSkyFogMin(-0.5f),
    m_fSkyFogMax(1.0f),
    m_fFarClip(1000.0f)
{
}


///////////////////////////////////////////////////////////////////////  
//  CSkyRI::CSkyRI

CSkyRI_TemplateList
inline CSkyRI_t::CSkyRI( ) :
    m_pShaderLoader(NULL)
{
}


///////////////////////////////////////////////////////////////////////  
//  CSkyRI::~CSkyRI

CSkyRI_TemplateList
inline CSkyRI_t::~CSkyRI( )
{
}


///////////////////////////////////////////////////////////////////////  
//  CSkyRI::Init

CSkyRI_TemplateList
inline st_bool CSkyRI_t::Init(void)
{
    st_bool bSuccess = true;
    
    bSuccess &= InitShaders( );
    if (bSuccess)
        bSuccess &= InitGeometryBuffer( );

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CSkyRI::SetShaderLoader

CSkyRI_TemplateList
inline void CSkyRI_t::SetShaderLoader(TShaderLoaderPolicy* pShaderLoader)
{
    m_pShaderLoader = pShaderLoader;
}


///////////////////////////////////////////////////////////////////////  
//  CSkyRI::SetRenderInfo

CSkyRI_TemplateList
inline void CSkyRI_t::SetRenderInfo(const SSkyRenderInfo& sInfo)
{
    m_sRenderInfo = sInfo;
}


///////////////////////////////////////////////////////////////////////  
//  CSkyRI::GetRenderInfo

CSkyRI_TemplateList
inline const SSkyRenderInfo& CSkyRI_t::GetRenderInfo(void) const
{
    return m_sRenderInfo;
}


///////////////////////////////////////////////////////////////////////  
//  CSkyRI::Render

CSkyRI_TemplateList
inline st_bool CSkyRI_t::Render(const Vec3& vLightDir, SRenderStats* pMainStats)
{
    st_bool bSuccess = false;

    // isolate the Sky render statistics
    SRenderStats::SGeometryStats* pStats = pMainStats ? &pMainStats->m_aGeometries[STATS_CATEGORY_SKY][RENDER_PASS_STANDARD] : NULL;

    // render the sky box
    if (m_tSkyTechnique.Bind( ) &&
        m_tGeometryBuffer.EnableFormat( ) &&
        m_tGeometryBuffer.BindVertexBuffer( ) &&
        m_tGeometryBuffer.BindIndexBuffer( ))
    {
        bSuccess = true;

        // set constants
        bSuccess &= m_tLightDir.Set3fv(-vLightDir); // user specifies light direction like DX/OpenGL, but the shader takes a negated value
        bSuccess &= m_tSunColor.Set3fv(m_sRenderInfo.m_vSunColor);
        bSuccess &= m_tSunParams.Set3f(m_sRenderInfo.m_fSunSize, m_sRenderInfo.m_fSunSpreadExponent, m_sRenderInfo.m_fSunFogBloom);
        bSuccess &= m_tSkyColor.Set3fv(m_sRenderInfo.m_vSkyColor);
        bSuccess &= m_tFogParams.Set4f(m_sRenderInfo.m_fFogEndDistance, m_sRenderInfo.m_fFogEndDistance - m_sRenderInfo.m_fFogStartDistance, -m_sRenderInfo.m_fSkyFogMin, -1.0f / (m_sRenderInfo.m_fSkyFogMax - m_sRenderInfo.m_fSkyFogMin));
        bSuccess &= m_tFogColor.Set3fv(m_sRenderInfo.m_vFogColor);
        bSuccess &= m_tFarClip.Set1f(m_sRenderInfo.m_fFarClip);

        // how much to render
        const st_uint32 c_uiNumIndices = m_tGeometryBuffer.NumIndices( );
        const st_uint32 c_uiNumVertices = m_tGeometryBuffer.NumVertices( );
        const st_uint32 c_uiNumTriangles = c_uiNumIndices / 3;

        // render state
        m_tRenderStatePolicy.SetDepthMask(false);
        m_tRenderStatePolicy.SetFaceCulling(CULLTYPE_NONE);

        bSuccess &= m_tSkyTechnique.CommitConstants( );
        bSuccess &= m_tGeometryBuffer.RenderIndexed(PRIMITIVE_TRIANGLES, 0, c_uiNumIndices, 0, c_uiNumVertices);
        bSuccess &= m_tGeometryBuffer.UnBindIndexBuffer( );
        bSuccess &= m_tSkyTechnique.UnBind( );

        if (pStats)
        {
            ++pStats->m_uiNumVboBinds;
            ++pStats->m_uiNumDrawCalls;
            ++pStats->m_uiNumObjects;
            pStats->m_uiNumTriangles += c_uiNumTriangles;
        }

        // render stars if present (assume vertex buffer is still bound)
        if (m_sRenderInfo.m_nNumSmallStars + m_sRenderInfo.m_nNumLargeStars > 0)
        {
            // render state
            m_tRenderStatePolicy.SetBlending(true);

            if (m_tStarsTechnique.Bind( ))
            {
                if (m_sRenderInfo.m_nNumSmallStars)
                {
                    m_tRenderStatePolicy.SetPointSize(1.0f);
                    m_tGeometryBuffer.RenderArrays(PRIMITIVE_POINTS, 256, m_sRenderInfo.m_nNumSmallStars);
                }

                if (m_sRenderInfo.m_nNumLargeStars)
                {
                    m_tRenderStatePolicy.SetPointSize(2.5f);
                    m_tGeometryBuffer.RenderArrays(PRIMITIVE_POINTS, 256 + m_sRenderInfo.m_nNumSmallStars, m_sRenderInfo.m_nNumLargeStars);
                }

                bSuccess &= m_tStarsTechnique.UnBind( );
            }

            // restore state
            m_tRenderStatePolicy.SetBlending(true);
        }

        // unbind resources
        bSuccess &= TGeometryBufferPolicy::DisableFormat( );
        bSuccess &= m_tGeometryBuffer.UnBindVertexBuffer( );

        // restore state
        m_tRenderStatePolicy.SetDepthMask(true);
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CSkyRI::EnableTexturing

CSkyRI_TemplateList
inline void CSkyRI_t::EnableTexturing(st_bool bFlag)
{
}


///////////////////////////////////////////////////////////////////////  
//  CSkyRI::InitGeometryBuffer

CSkyRI_TemplateList
inline st_bool CSkyRI_t::InitGeometryBuffer(void)
{
    st_bool bSuccess = false;

    SVertexAttribDesc asAttribs[ ] = 
    { 
        { VERTEX_ATTRIB_SEMANTIC_POS, VERTEX_ATTRIB_TYPE_FLOAT, 3 },
        VERTEX_ATTRIB_END( )
    };

    if (m_tGeometryBuffer.SetVertexFormat(asAttribs, &m_tSkyTechnique))
    {
        bSuccess = true;

        // set sky box vertices
        const st_uint32 c_uiSteps = 16;
        Vec3 c_avPos[c_uiSteps * c_uiSteps];
        for (st_uint32 i = 0; i < c_uiSteps; ++i)
        {
            float fAngle = c_fTwoPi * (float)i / (float)c_uiSteps;
            Vec3 vec2D(cos(fAngle), sin(fAngle), 0.0f);

            for (st_uint32 j = 0; j < c_uiSteps; ++j)
            {
                fAngle = c_fPi * (float)j / (float)c_uiSteps;
                const float c_fSin1 = sin(fAngle);
                const float c_fCos1 = cos(fAngle);
                c_avPos[i * c_uiSteps + j] = CCoordSys::ConvertToStd(vec2D.x * c_fSin1, vec2D.y * c_fSin1, c_fCos1);
            }
        }

        bSuccess &= m_tGeometryBuffer.AppendVertices(c_avPos, c_uiSteps * c_uiSteps);

        // set sky box indices
        st_uint32 auiIndices[c_uiSteps * c_uiSteps * 6];
        st_uint32* pIndex = auiIndices;
        for (st_uint32 i = 0; i < c_uiSteps; ++i)
        {
            const st_uint32 c_uiBase = i * c_uiSteps;
            const st_uint32 c_uiNextBase = ((i == c_uiSteps - 1) ? 0 : c_uiBase + c_uiSteps);

            for (st_uint32 j = 0; j < c_uiSteps - 1; ++j)
            {
                *pIndex++ = c_uiBase + j;
                *pIndex++ = c_uiBase + j + 1;
                *pIndex++ = c_uiNextBase + j;
                *pIndex++ = c_uiNextBase + j;
                *pIndex++ = c_uiBase + j + 1;
                *pIndex++ = c_uiNextBase + j + 1;
            }
        }

        bSuccess &= m_tGeometryBuffer.SetIndexFormat(INDEX_FORMAT_UNSIGNED_32BIT);
        bSuccess &= m_tGeometryBuffer.AppendIndices(auiIndices, c_uiSteps * (c_uiSteps - 1) * 6);
        bSuccess &= m_tGeometryBuffer.EndIndices( );

        // if stars are present, add them to the vertex buffer
        st_int32 nTotalStars = m_sRenderInfo.m_nNumLargeStars + m_sRenderInfo.m_nNumSmallStars;
        if (nTotalStars > 0)
        {
            // create array of stars
            CArray<Vec3> aStars(nTotalStars);

            CRandom cDice;
            for (st_int32 i = 0; i < nTotalStars; ++i)
            {
                Vec3* pVec = &aStars[i];

                // ensure uniform spherical distribution
                do 
                {
                    st_float32 x = cDice.GetFloat(-1.0f, 1.0f);
                    st_float32 y = cDice.GetFloat(-1.0f, 1.0f);
                    st_float32 z = cDice.GetFloat(-1.0f, 1.0f);

                    *pVec = CCoordSys::ConvertToStd(x, y, z);
                }
                while (pVec->MagnitudeSquared( ) > 1.0f);

                pVec->Normalize( );
            }

            bSuccess &= m_tGeometryBuffer.AppendVertices(&aStars[0], nTotalStars);
        }   

        bSuccess &= m_tGeometryBuffer.EndVertices( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CSkyRI::InitShaders

CSkyRI_TemplateList
inline st_bool CSkyRI_t::InitShaders(void)
{
    st_bool bSuccess = false;

    if (m_pShaderLoader)
    {
        // sky shader
        bSuccess = m_pShaderLoader->GetTechnique((m_sRenderInfo.m_strShaderPath + "Sky.hlsl").c_str( ), "SkyVertex", "SkyPixel", "Sky", m_tSkyTechnique);

        // star shader
        bSuccess &= m_pShaderLoader->GetTechnique((m_sRenderInfo.m_strShaderPath + "Star.hlsl").c_str( ), "StarVertex", "StarPixel", "Star", m_tStarsTechnique);

        // shader constants
        if (bSuccess)
        {
            bSuccess &= m_pShaderLoader->GetConstant("g_vLightDir", m_tSkyTechnique, m_tLightDir);
            bSuccess &= m_pShaderLoader->GetConstant("g_vSunColor", m_tSkyTechnique, m_tSunColor);
            bSuccess &= m_pShaderLoader->GetConstant("g_vSunParams", m_tSkyTechnique, m_tSunParams);
            bSuccess &= m_pShaderLoader->GetConstant("g_vSkyColor", m_tSkyTechnique, m_tSkyColor);
            bSuccess &= m_pShaderLoader->GetConstant("g_vFogColor", m_tSkyTechnique, m_tFogColor);
            bSuccess &= m_pShaderLoader->GetConstant("g_vFogParams", m_tSkyTechnique, m_tFogParams);
            bSuccess &= m_pShaderLoader->GetConstant("g_fFarClip", m_tSkyTechnique, m_tFarClip);
        }
        else
            CCore::SetError("CSkyRI::InitShader, technique 'Sky' failed to load");
    }

    return bSuccess;
}
