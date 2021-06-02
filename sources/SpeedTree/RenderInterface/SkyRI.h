///////////////////////////////////////////////////////////////////////  
//  SkyRI.h
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
#include "Core/ExportBegin.h"
#include "Core/Random.h"
#include "RenderInterface/ForestRI.h"


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  Structure SSkyRenderInfo

    struct ST_STORAGE_CLASS SSkyRenderInfo
    {
                            SSkyRenderInfo( );

            // shader file location
            CFixedString    m_strShaderPath;

            // stars
            st_int32        m_nNumLargeStars;
            st_int32        m_nNumSmallStars;

            // sun
            Vec3            m_vSunColor;
            st_float32      m_fSunSize;
            st_float32      m_fSunSpreadExponent;
            st_float32      m_fSunFogBloom;

            // sky/fog
            Vec3            m_vSkyColor;
            Vec3            m_vFogColor;
            st_float32      m_fFogStartDistance;
            st_float32      m_fFogEndDistance;
            st_float32      m_fSkyFogMin;
            st_float32      m_fSkyFogMax;
            st_float32      m_fFarClip;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CSkyRI

    #define CSkyRI_TemplateList template<class TRenderStatePolicy, class TTexturePolicy, class TGeometryBufferPolicy, class TShaderLoaderPolicy, class TShaderTechniquePolicy, class TShaderConstantPolicy, class TDepthTexturePolicy>

    CSkyRI_TemplateList
    class ST_STORAGE_CLASS CSkyRI
    {
    public:
                                            CSkyRI( );
                                            ~CSkyRI( );

            // graphics initialization
            st_bool                         Init(void);
            void                            SetShaderLoader(TShaderLoaderPolicy* pShaderLoader);
            void                            SetRenderInfo(const SSkyRenderInfo& sInfo);
            const SSkyRenderInfo&           GetRenderInfo(void) const;

            st_bool                         Render(const Vec3& vLightDir, SRenderStats* pStats = NULL);

            // render states
            void                            EnableTexturing(st_bool bFlag);

    private:
            // init support
            st_bool                         InitGeometryBuffer(void);
            st_bool                         InitShaders(void);

            SSkyRenderInfo                  m_sRenderInfo;

            // rendering
            TRenderStatePolicy              m_tRenderStatePolicy;

            // shaders
            TShaderLoaderPolicy*            m_pShaderLoader;
            TShaderTechniquePolicy          m_tSkyTechnique;
            TShaderTechniquePolicy          m_tStarsTechnique;

            // shader constants
            TShaderConstantPolicy           m_tLightDir;
            TShaderConstantPolicy           m_tSunColor;
            TShaderConstantPolicy           m_tSunParams;
            TShaderConstantPolicy           m_tSkyColor;
            TShaderConstantPolicy           m_tFogColor;
            TShaderConstantPolicy           m_tFogParams;
            TShaderConstantPolicy           m_tFarClip;

            // geometry
            TGeometryBufferPolicy           m_tGeometryBuffer;
    };
    #define CSkyRI_t CSkyRI<TRenderStatePolicy, TTexturePolicy, TGeometryBufferPolicy, TShaderLoaderPolicy, TShaderTechniquePolicy, TShaderConstantPolicy, TDepthTexturePolicy>

    // include inline functions
    #include "SkyRI.inl"

} // end namespace SpeedTree

#include "Core/ExportEnd.h"

