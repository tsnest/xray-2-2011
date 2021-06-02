///////////////////////////////////////////////////////////////////////  
//  GrassRI.h
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
#include "Forest/Grass.h"
#include "RenderInterface/ForestRI.h"
#include "RenderInterface/TerrainRI.h"


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  Structure SGrassRenderInfo

    struct ST_STORAGE_CLASS SGrassRenderInfo
    {
                            SGrassRenderInfo( );

            // shader
            CFixedString    m_strShaderPath;

            // terrain matching 
            Vec4            m_avTerrainSplatColors[c_nNumTerrainSplatLayers];
            st_float32      m_fTerrainMatchFactorTop;
            st_float32      m_fTerrainMatchFactorBottom;
            st_float32      m_afSplatTileValues[c_nNumTerrainSplatLayers];
            st_float32      m_fNormalMapBlueScale;

            // lighting
            SMaterial       m_sMaterial;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CGrassRI

    #define CGrassRI_TemplateList template<class TRenderStatePolicy, class TTexturePolicy, class TGeometryBufferPolicy, class TShaderLoaderPolicy, class TShaderTechniquePolicy, class TShaderConstantPolicy, class TDepthTexturePolicy>

    CGrassRI_TemplateList
    class ST_STORAGE_CLASS CGrassRI : public CGrass
    {
    public:
                                            CGrassRI( );
                                            ~CGrassRI( );

            // graphics initialization
            void                            SetShaderLoader(TShaderLoaderPolicy* pShaderLoader);
            void                            SetRenderInfo(const SGrassRenderInfo& sInfo);
            const SGrassRenderInfo&         GetRenderInfo(void) const;
            void                            SetTerrainSplatColors(const Vec4 avSplatColors[c_nNumTerrainSplatLayers]);
            st_bool                         Init(const char* pTextureFilename, st_int32 nImageRows, st_int32 nImageCols, st_float32 fCellSize, const SVertexAttribDesc* pVertexFormat);

            // rendering functions
            st_bool                         Render(const SGrassCullResults& sCullResults, 
                                                   ERenderPassType ePassType,
                                                   const Vec3& vLightDir,
                                                   const SMaterial& sLighting, 
                                                   SRenderStats* pStats = NULL);

            // culling & LOD
            void                            Cull(const CView& cView, SGrassCullResults& sResults);

            // render states
            void                            EnableTexturing(st_bool bFlag);

    private:
            // render support
            st_bool                         BeginRender(TShaderTechniquePolicy& tTechnique, const Vec3& vLightDir, const SMaterial& sLighting);
            st_bool                         EndRender(TShaderTechniquePolicy& tTechnique);
            void                            ComputeUnitBillboard(const CView& cView);

            // VBO management
            st_bool                         InitVboPool(void);
            TGeometryBufferPolicy*          NextVbo(void);
            TGeometryBufferPolicy*          NewVbo(TGeometryBufferPolicy* pVboObjectBuffer = NULL);
            st_bool                         ResizeIndexBuffer(st_int32 nNumGrassBlades);

            st_bool                         InitShader(void);
            st_bool                         ShaderIsLoaded(void) const;

            // texturing
            TTexturePolicy                  m_tCompositeTexture;
            TTexturePolicy                  m_tWhiteTexture;

            // shaders
            TShaderLoaderPolicy*            m_pShaderLoader;
            TShaderTechniquePolicy          m_tTechnique;

            // shader constants
            TShaderConstantPolicy           m_tCompositeTextureVar;
            TShaderConstantPolicy           m_tUnitBillboard;
            TShaderConstantPolicy           m_tTerrainSplatColors;
            TShaderConstantPolicy           m_tRenderInfo;
            TShaderConstantPolicy           m_tLightDir;
            TShaderConstantPolicy           m_tLightAndMaterialAmbient;
            TShaderConstantPolicy           m_tLightAndMaterialDiffuse;
            TShaderConstantPolicy           m_tSplatMapTiles;

            // render mechanics
            TRenderStatePolicy              m_tRenderStatePolicy;
            CArray<TGeometryBufferPolicy*>  m_aVboPool;
            TGeometryBufferPolicy           m_tIndexBuffer;
            const SVertexAttribDesc*        m_pVertexFormat;

            // render states
            st_bool                         m_bTexturingEnabled;
            Mat4x4                          m_mCameraFacingUnitBillboard;
            SGrassRenderInfo                m_sRenderInfo;

#ifdef SPEEDTREE_HEAP_FRIENDLY_MODE
            st_byte*                        m_pMemPool;
#endif
    };
    #define CGrassRI_t CGrassRI<TRenderStatePolicy, TTexturePolicy, TGeometryBufferPolicy, TShaderLoaderPolicy, TShaderTechniquePolicy, TShaderConstantPolicy, TDepthTexturePolicy>

    // include inline functions
    #include "GrassRI.inl"

} // end namespace SpeedTree

#include "Core/ExportEnd.h"

