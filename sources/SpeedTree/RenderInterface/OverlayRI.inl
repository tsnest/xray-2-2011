///////////////////////////////////////////////////////////////////////  
//  OverlayRI.inl
//
//  These overlays are used for debugging/diagnostic purposes and are not
//  a core element of the SpeedTree SDK.
//
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
//  CForestRI::InitOverlays

CForestRI_TemplateList
inline st_bool CForestRI_t::InitOverlays(void)
{
    st_bool bSuccess = false;

    // setup the geometry buffer for rendering the shadow map
    SVertexAttribDesc asVertexFormat[ ] =
    {
        { VERTEX_ATTRIB_SEMANTIC_POS, VERTEX_ATTRIB_TYPE_FLOAT, 3 },
        { VERTEX_ATTRIB_SEMANTIC_TEXCOORD0, VERTEX_ATTRIB_TYPE_FLOAT, 2 },
        VERTEX_ATTRIB_END( )
    };

    if (m_tOverlayGeometry.SetVertexFormat(asVertexFormat, &m_tOverlayTechnique))
    {
        bSuccess = true;

        // structure matching the vertex format
        struct SVertex
        {
            st_float32  m_afCoord[3];
            st_float32  m_afTexCoord[2];
        };

        const st_float32 c_fScale = 1.0f;
        const st_float32 c_fSize = 0.2f * c_fScale;
        const st_float32 c_fSpacing = 0.015f * c_fScale;
        for (st_int32 nSplit = 0; nSplit < m_sRenderInfo.m_nNumShadowMaps; ++nSplit)
        {
            const st_float32 c_afOffsets[2] = 
            {
                c_fSpacing + float(nSplit + 1) * c_fSpacing + nSplit * c_fSize,
                c_fSpacing + float(nSplit + 1) * c_fSpacing + (nSplit + 1) * c_fSize
            };
            SVertex asVertices[4] = 
            {
                { { 1.0f - c_afOffsets[0], c_fSpacing, 0.0f }, { 0.0f, 0.0f } },
                { { 1.0f - c_afOffsets[0], c_fSpacing + c_fSize * 1.5f, 0.0f }, { 0.0f, 1.0f } },
                { { 1.0f - c_afOffsets[1], c_fSpacing, 0.0f }, { 1.0f, 0.0f } },
                { { 1.0f - c_afOffsets[1], c_fSpacing + c_fSize * 1.5f, 0.0f }, { 1.0f, 1.0f } }
            };

            bSuccess &= m_tOverlayGeometry.AppendVertices(asVertices, 4);
        }

        bSuccess &= m_tOverlayGeometry.EndVertices( );
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::RenderOverlays

CForestRI_TemplateList
inline st_bool CForestRI_t::RenderOverlays(void)
{
    st_bool bSuccess = false;

    // setup overlay render states
    m_tRenderStatePolicy.SetDepthTesting(false);
    m_tRenderStatePolicy.SetFaceCulling(CULLTYPE_NONE);

    // setup a composite projection/view matrix for the overlay
    Mat4x4 mOverlayView;
    mOverlayView.Ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

    if (m_tProjModelviewMatrix.SetMatrix(mOverlayView) &&
        m_tOverlayTechnique.Bind( ))
    {
        for (st_int32 nSplit = 0; nSplit < m_sRenderInfo.m_nNumShadowMaps; ++nSplit)
        {
            m_atShadowBuffers[nSplit].EnableShadowMapComparison(false);
            m_tOverlayTextureVar.SetTexture(m_atShadowBuffers[nSplit]);

            if (m_tOverlayGeometry.EnableFormat( ))
            {
                if (m_tOverlayGeometry.BindVertexBuffer( ))
                {
                    bSuccess = true;

                    bSuccess &= m_tOverlayTechnique.CommitTextures( );
                    bSuccess &= m_tOverlayTechnique.CommitConstants( );
                    bSuccess &= m_tOverlayGeometry.RenderArrays(PRIMITIVE_TRIANGLE_STRIP, nSplit * 4, 4);

                    bSuccess &= m_tOverlayGeometry.UnBindVertexBuffer( );
                }

                bSuccess &= m_tOverlayGeometry.DisableFormat( );
            }

            m_atShadowBuffers[nSplit].EnableShadowMapComparison(true);
        }

        bSuccess &= m_tOverlayTechnique.UnBind( );
    }

    // restore states
    m_tRenderStatePolicy.SetDepthTesting(true);

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CForestRI::SetupOverlayShader

CForestRI_TemplateList
inline st_bool CForestRI_t::SetupOverlayShader(void)
{
    st_bool bSuccess = false;

    if (m_tShaderLoader.GetTechnique((m_sRenderInfo.m_strShaderPath + "Overlay.hlsl").c_str( ), 
                                        "OverlayVertex", "OverlayPixel", "Overlay", m_tOverlayTechnique))
    {
        // query overlay-specific uniform variables
        bSuccess = m_tShaderLoader.GetConstant(m_tShaderLoader.GetTextureConstantName("Overlay").c_str( ), m_tOverlayTechnique, m_tOverlayTextureVar, CONST_LOCATION_PIXEL);
    }

    return bSuccess;
}
