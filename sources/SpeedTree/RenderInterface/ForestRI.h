///////////////////////////////////////////////////////////////////////  
//  ForestRI.h
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
#include "Forest/Forest.h"
#include "Core/Set.h"
#include "Core/List.h"
#include "Core/Timer.h"


//  When SPEEDTREE_WORLD_TRANSLATE is #defined, the SDK switches into a mode 
//  where the camera stays at the origin and the scene geometry is translated
//  to it, as opposed to translating the camera through the scene. This helps
//  mitigate depth buffer resolution break down with large coordinate values
//  and is recommended.

#define SPEEDTREE_WORLD_TRANSLATE
//#define SPEEDTREE_ADVANCED_WIND


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  Class CRenderStateRI

    enum EAlphaFunc
    {
        ALPHAFUNC_LESS,
        ALPHAFUNC_GREATER
    };
    enum ERenderStyle
    {
        RENDERSTYLE_SOLID,
        RENDERSTYLE_WIREFRAME
    };
    enum EDepthTestFunc
    {
        DEPTHTEST_NEVER,
        DEPTHTEST_LESS,
        DEPTHTEST_EQUAL,
        DEPTHTEST_LESS_EQUAL,
        DEPTHTEST_GREATER,
        DEPTHTEST_NOT_EQUAL,
        DEPTHTEST_GREATER_EQUAL,
        DEPTHTEST_ALWAYS
    };

    template<class TRenderStatePolicy>
    class ST_STORAGE_CLASS CRenderStateRI
    {
    public:

    static  void       ST_CALL_CONV Initialize(void);
    static  void       ST_CALL_CONV ApplyStates(void);
    static  void       ST_CALL_CONV Destroy(void);

    static  void       ST_CALL_CONV ClearScreen(st_bool bClearColor = true, st_bool bClearDepth = true);
    static  void       ST_CALL_CONV SetAlphaFunction(EAlphaFunc eFunc, st_float32 fValue);  // fValue range = [0.0,255.0]
    static  void       ST_CALL_CONV SetAlphaTesting(st_bool bFlag);
    static  void       ST_CALL_CONV SetAlphaToCoverage(st_bool bFlag);
    static  void       ST_CALL_CONV SetBlending(st_bool bFlag);
    static  void       ST_CALL_CONV SetColorMask(st_bool bRed, st_bool bGreen, st_bool bBlue, st_bool bAlpha);
    static  void       ST_CALL_CONV SetDepthMask(st_bool bFlag);
    static  void       ST_CALL_CONV SetDepthTestFunc(EDepthTestFunc eDepthTestFunc);
    static  void       ST_CALL_CONV SetDepthTesting(st_bool bFlag);
    static  void       ST_CALL_CONV SetFaceCulling(ECullType eCullType);
    static  void       ST_CALL_CONV SetMultisampling(st_bool bMultisample);
    static  void       ST_CALL_CONV SetPointSize(st_float32 fSize);
    static  void       ST_CALL_CONV SetPolygonOffset(st_float32 fFactor = 0.0f, st_float32 fUnits = 0.0f); // both zeros deactivates polygon offset
    static  void       ST_CALL_CONV SetRenderStyle(ERenderStyle eStyle);

            TRenderStatePolicy      m_tRenderStatePolicy;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CTextureRI

    template<class TTexturePolicy>
    class ST_STORAGE_CLASS CTextureRI
    {
    public:
            
            // loading
            st_bool                 Load(const char* pFilename, st_int32 nMaxAnisotropy = 0);
            st_bool                 LoadColor(st_uint32 uiColor); // 0xrrggbbaa
            st_bool                 LoadAlphaNoise(st_int32 nWidth, st_int32 nHeight);
            st_bool                 Unload(void);

            // render
            st_bool                 FixedFunctionBind(void);
            const TTexturePolicy&   GetPolicy(void) const;

            // other
            const char*             GetFilename(void) const;
            st_bool                 IsValid(void) const;
            CTextureRI&             operator=(const CTextureRI& cRight);
            st_bool                 operator!=(const CTextureRI& cRight) const;

            TTexturePolicy          m_tTexturePolicy;

    private:
            SpeedTree::CFixedString m_strFilename;
    };
    #define CTextureRI_t CTextureRI<TTexturePolicy>


    ///////////////////////////////////////////////////////////////////////  
    //  Class SMaterialRI

    template<class TTexturePolicy>
    struct ST_STORAGE_CLASS SMaterialRI : public SMaterial
    {
            void                    Copy(const SMaterial& sSource);

            TTexturePolicy          m_acTextureObjects[TL_NUM_TEX_LAYERS];
    };
    #define SMaterialRI_t SMaterialRI<TTexturePolicy>


    ///////////////////////////////////////////////////////////////////////  
    //  Class CGeometryRI

    enum EVertexAttribSemantic
    {
        VERTEX_ATTRIB_SEMANTIC_POS,             // generic attrib 0
        VERTEX_ATTRIB_SEMANTIC_BLENDWEIGHT,     // generic attrib 1
        VERTEX_ATTRIB_SEMANTIC_NORMAL,          // generic attrib 2
        VERTEX_ATTRIB_SEMANTIC_DIFFUSE,         // generic attrib 3
        VERTEX_ATTRIB_SEMANTIC_SPECULAR,        // generic attrib 4
        VERTEX_ATTRIB_SEMANTIC_TESSFACTOR,      // generic attrib 5
        VERTEX_ATTRIB_SEMANTIC_PSIZE,           // generic attrib 6
        VERTEX_ATTRIB_SEMANTIC_BLENDINDICES,    // generic attrib 7
        VERTEX_ATTRIB_SEMANTIC_TEXCOORD0,       // generic attrib 8
        VERTEX_ATTRIB_SEMANTIC_TEXCOORD1,       // generic attrib 9
        VERTEX_ATTRIB_SEMANTIC_TEXCOORD2,       // generic attrib 10
        VERTEX_ATTRIB_SEMANTIC_TEXCOORD3,       // generic attrib 11
        VERTEX_ATTRIB_SEMANTIC_TEXCOORD4,       // generic attrib 12
        VERTEX_ATTRIB_SEMANTIC_TEXCOORD5,       // generic attrib 13
        VERTEX_ATTRIB_SEMANTIC_TEXCOORD6,       // generic attrib 14
        VERTEX_ATTRIB_SEMANTIC_TEXCOORD7,       // generic attrib 15
        VERTEX_ATTRIB_SEMANTIC_COUNT,
        VERTEX_ATTRIB_SEMANTIC_END
    };

    enum EVertexAttribType
    {
        VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE,
        VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE_NORMALIZED,
        VERTEX_ATTRIB_TYPE_SHORT,
        VERTEX_ATTRIB_TYPE_SHORT_NORMALIZED,
        VERTEX_ATTRIB_TYPE_UNSIGNED_SHORT,
        VERTEX_ATTRIB_TYPE_UNSIGNED_SHORT_NORMALIZED,
        VERTEX_ATTRIB_TYPE_INT,
        VERTEX_ATTRIB_TYPE_INT_NORMALIZED,
        VERTEX_ATTRIB_TYPE_UNSIGNED_INT,
        VERTEX_ATTRIB_TYPE_UNSIGNED_INT_NORMALIZED,
        VERTEX_ATTRIB_TYPE_FLOAT,
        VERTEX_ATTRIB_TYPE_DOUBLE,
        VERTEX_ATTRIB_TYPE_COUNT
    };

    enum EPrimitiveType
    {
        PRIMITIVE_POINTS, 
        PRIMITIVE_LINE_STRIP, 
        PRIMITIVE_LINE_LOOP, 
        PRIMITIVE_LINES, 
        PRIMITIVE_TRIANGLE_STRIP, 
        PRIMITIVE_TRIANGLE_FAN, 
        PRIMITIVE_TRIANGLES, 
        PRIMITIVE_QUAD_STRIP, 
        PRIMITIVE_QUADS
    };

    struct SVertexAttribDesc
    {
            EVertexAttribSemantic   m_eSemantic;
            EVertexAttribType       m_eDataType;
            st_uint16               m_uiNumElements;    // e.g., 3 for (x,y,z)

            st_uint32               SizeOfDataType(void) const;
            st_uint32               SizeOfAttrib(void) const;
            const char*             DataTypeName(void) const;
            const char*             SemanticName(void) const;
    };

    #define VERTEX_ATTRIB_END() { VERTEX_ATTRIB_SEMANTIC_END, VERTEX_ATTRIB_TYPE_FLOAT, 0 }

    enum EIndexFormat
    {
        INDEX_FORMAT_UNSIGNED_16BIT,
        INDEX_FORMAT_UNSIGNED_32BIT
    };

    enum EGeometryProcessingType
    {
        GEOMETRY_PROCESSING_NONE = 0,
        GEOMETRY_PROCESSING_BRANCHES, 
        GEOMETRY_PROCESSING_BRANCHES_DEPTH,
        GEOMETRY_PROCESSING_BRANCHES_SHADOW,
        GEOMETRY_PROCESSING_FRONDS, 
        GEOMETRY_PROCESSING_FRONDS_DEPTH, 
        GEOMETRY_PROCESSING_FRONDS_SHADOW, 
        GEOMETRY_PROCESSING_LEAF_CARDS, 
        GEOMETRY_PROCESSING_LEAF_CARDS_DEPTH,
        GEOMETRY_PROCESSING_LEAF_CARDS_SHADOW,
        GEOMETRY_PROCESSING_LEAF_MESHES,   
        GEOMETRY_PROCESSING_LEAF_MESHES_DEPTH, 
        GEOMETRY_PROCESSING_LEAF_MESHES_SHADOW
    };

    template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
    class ST_STORAGE_CLASS CGeometryBufferRI
    {
    public:
                                        CGeometryBufferRI(st_bool bDynamicVB = false, st_bool bDynamicIB = false);
            virtual                     ~CGeometryBufferRI( );

            // vertex buffer
            st_bool                     SetVertexFormat(const SVertexAttribDesc* pAttribDesc, const TShaderTechniqueRI* pTechnique);
            st_bool                     AppendVertices(const void* pVertexData, st_uint32 uiNumVertices);
            st_bool                     EndVertices(void);
            st_bool                     OverwriteVertices(const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiOffset);
            st_uint32                   NumVertices(void) const;
            st_uint32                   VertexSize(void) const; // in bytes
            st_bool                     ClearVertices(void);

            st_bool                     EnableFormat(void) const;
    static  st_bool        ST_CALL_CONV DisableFormat(void);
            st_bool                     BindVertexBuffer(EGeometryProcessingType eProcessingType = GEOMETRY_PROCESSING_NONE) const;
    static  st_bool        ST_CALL_CONV UnBindVertexBuffer(void);

            // index buffer
            st_bool                     SetIndexFormat(EIndexFormat eFormat);
            st_bool                     AppendIndices(const void* pIndexData, st_uint32 uiNumIndices);
            st_bool                     EndIndices(void);
            st_bool                     OverwriteIndices(const void* pIndexData, st_uint32 uiNumIndices, st_uint32 uiOffset);
            st_uint32                   NumIndices(void) const;
            st_uint32                   IndexSize(void) const; // in bytes
            st_bool                     ClearIndices(void);

            st_bool                     BindIndexBuffer(void) const;
            st_bool                     UnBindIndexBuffer(void) const;

            // render functions
            st_bool                     RenderIndexed(EPrimitiveType ePrimType, 
                                                      st_uint32 uiStartIndex, 
                                                      st_uint32 uiNumIndices,
                                                      st_uint32 uiMinIndex = 0,
                                                      st_uint32 uiNumVertices = 0,
                                                      EGeometryProcessingType eProcessingType = GEOMETRY_PROCESSING_NONE) const;
            st_bool                     RenderArrays(EPrimitiveType ePrimType, st_uint32 uiStartVertex, 
                                                     st_uint32 uiNumVertices, 
                                                     EGeometryProcessingType eProcessingType = GEOMETRY_PROCESSING_NONE) const;
    static  st_bool        ST_CALL_CONV IsPrimitiveTypeSupported(EPrimitiveType ePrimType);

            // memory management
            st_bool                     Reserve(st_uint32 uiNumVertices, st_uint32 uiNumIndices);

            TGeometryBufferPolicy       m_tGeometryBufferPolicy;

    private:
            // vertex buffer
            st_bool                     m_bDynamicVB;
            CArray<st_byte>             m_aVertexData;
            st_uint32                   m_uiVertexSize;         // in bytes
            st_uint32                   m_uiNumVertices;

            // run-time checks
#ifndef NDEBUG
    mutable st_bool                     m_bFormatEnabled;
    mutable st_bool                     m_bVertexBufferBound;
    mutable st_bool                     m_bIndexBufferBound;
#endif

            // index buffer
            st_bool                     m_bDynamicIB;
            CArray<st_byte>             m_aIndexData;
            st_uint32                   m_uiIndexSize;          // in bytes
            st_uint32                   m_uiNumIndices;

            st_int32                    m_nVertexHeapHandle;
            st_int32                    m_nIndexHeapHandle;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CShaderConstantRI

    template<class TShaderConstantPolicy>
    class ST_STORAGE_CLASS CShaderConstantRI
    {
    public:
                                    CShaderConstantRI( );
                                    ~CShaderConstantRI( );
    
            st_bool                 Set1f(st_float32 x) const;
            st_bool                 Set2f(st_float32 x, st_float32 y) const;
            st_bool                 Set2fv(const st_float32 afValues[2]) const;
            st_bool                 Set3f(st_float32 x, st_float32 y, st_float32 z) const;
            st_bool                 Set3fv(const st_float32 afValues[3]) const;
            st_bool                 Set3fvPlus1f(const st_float32 afValues[3], st_float32 w) const;
            st_bool                 Set4f(st_float32 x, st_float32 y, st_float32 z, st_float32 w) const;
            st_bool                 Set4fv(const st_float32 afValues[4]) const;
            st_bool                 SetArray4f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset = 0) const;
            st_bool                 SetMatrix(const st_float32 afMatrix[16]) const;
            st_bool                 SetMatrixTranspose(const st_float32 afMatrix[16]) const;
            st_bool                 SetMatrixIndex(const st_float32 afMatrix[16], st_uint32 uiIndex) const;
            st_bool                 SetMatrix4x4Array(const st_float32* pMatrixArray, st_uint32 uiOffset, st_uint32 uiNumMatrices) const;

            st_bool                 IsValid(void) const;
            const char*             GetName(void) const;
            void                    SetName(const char* pName);

            TShaderConstantPolicy   m_tShaderConstantPolicy;

    private:
            CFixedString            m_strName;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CShaderTechniqueRI

    enum EConstantType
    {
        CONST_LOCATION_VERTEX,
        CONST_LOCATION_PIXEL,
        CONST_LOCATION_BOTH
    };

    template<class TShaderTechniquePolicy, class TShaderConstantRI>
    class ST_STORAGE_CLASS CShaderTechniqueRI
    {
    public:
                                    CShaderTechniqueRI( );
                                    ~CShaderTechniqueRI( );

            st_bool                 Bind(void);
            st_bool                 UnBind(void);
            st_bool                 CommitConstants(void);
            st_bool                 CommitTextures(void);

            st_bool                 IsValid(void) const;
            const char*             GetName(void) const;
            void                    SetName(const char* pName);

            TShaderTechniquePolicy  m_tShaderTechniquePolicy;

    private:
            CFixedString            m_strName;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CShaderLoaderRI

    template<class TShaderLoaderPolicy, class TShaderTechniqueRI, class TShaderConstantRI>
    class ST_STORAGE_CLASS CShaderLoaderRI
    {
    public:
                                    CShaderLoaderRI( );
                                    ~CShaderLoaderRI( );

            st_bool                 Init(const CArray<CFixedString>& aDefines, 
                                         const char* pEffectFilename,
                                         st_bool bUsePrecompiledShaders);
            st_bool                 GetTechnique(const char* pFilename, 
                                                 const char* pVertexShaderName, 
                                                 const char* pPixelShaderName, 
                                                 const char* pTechniqueName, 
                                                 TShaderTechniqueRI& cTechnique) const;
            st_bool                 GetConstant(const char* pName, 
                                                const TShaderTechniqueRI& cTechnique,
                                                TShaderConstantRI& cConstant, 
                                                EConstantType eType = CONST_LOCATION_BOTH) const;
            
            const char*             GetError(void) const;

            CFixedString            GetTextureConstantName(const CFixedString& strBaseName);

            void                    Release(void);
            void                    OnResetDevice(void);
            void                    OnLostDevice(void);

            TShaderLoaderPolicy     m_tShaderLoaderPolicy;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CDepthTextureRI

    template<class TDepthTexturePolicy>
    class ST_STORAGE_CLASS CDepthTextureRI
    {
    public:
                                    CDepthTextureRI( );
                                    ~CDepthTextureRI( );

            st_bool                 Create(st_int32 nWidth, st_int32 nHeight);

            void                    Release(void);
            void                    OnResetDevice(void);
            void                    OnLostDevice(void);

            st_bool                 SetAsTarget(void);
            st_bool                 ReleaseAsTarget(void);

            st_bool                 BindAsTexture(void);
            st_bool                 UnBindAsTexture(void);
            void                    EnableShadowMapComparison(st_bool bFlag);

            TDepthTexturePolicy     m_tDepthTexturePolicy;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Enumeration EStatsCategory

    enum EStatsCategory
    {
        // main geometry types
        STATS_CATEGORY_BRANCHES,
        STATS_CATEGORY_FRONDS,
        STATS_CATEGORY_LEAF_CARDS,
        STATS_CATEGORY_LEAF_MESHES,
        STATS_CATEGORY_MESHES,
        STATS_CATEGORY_BILLBOARDS,
        STATS_CATEGORY_TERRAIN,
        STATS_CATEGORY_GRASS,
        STATS_CATEGORY_SKY,

        // utility
        STATS_CATEGORY_COUNT
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Enumeration ERenderPassType

    enum ERenderPassType
    {
        RENDER_PASS_STANDARD,
        RENDER_PASS_DEPTH_ONLY,
        RENDER_PASS_SHADOW,
        RENDER_PASS_NUM_TYPES
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Enumeration ETextureAlphaRenderMode

    enum ETextureAlphaRenderMode
    {
        TRANS_TEXTURE_ALPHA_TESTING,
        TRANS_TEXTURE_ALPHA_TO_COVERAGE,
        TRANS_TEXTURE_BLENDING,
        TRANS_TEXTURE_NOTHING,
        TRANS_TEXTURE_UNASSIGNED
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CLodRenderInfo
    //
    //  Class used internally; not for client-side use.
    //
    //  When a set of instances is rendered, the render sort/order follows:
    //      1. Geometry type (branches, fronds, leaf cards, etc).
    //      2. Base tree (white_oak, baobab, pine_cluster, etc.).
    //      3. Material
    //
    //  When rendering a set of instances of the same base tree, each instance may
    //  be rendered in a different LOD state.  Each LOD may use an arbitrary set
    //  materials, possibly completely unrelated to the materials used by adjacent
    //  LODs.  The CLodRenderInfo facilitates handling this complexity in the
    //  render loop.
    //
    //  The init functions below are called when building the tree geometry. In the
    //  render loop, MaterialRefs() is called to get a list of materials shared
    //  by all LODs for a base tree.  As those materials are looped, each instance
    //  queries IsActive() to determine if that instance at its current LOD uses
    //  the active material and if so, a draw call is made.
    //
    //  CLodRenderInfo also stores render data for each LOD since all LODs are combined
    //  into single VB/IBs for a single base tree.

    class ST_STORAGE_CLASS CLodRenderInfo
    {
    public:
            struct SRef
            {
                                        SRef( );

                st_int32                m_nMaterialId;      // material id indexing into SGeometry::m_pMaterials

                st_uint32               m_uiOffset;         
                st_uint32               m_uiLength;         
                st_uint32               m_uiMinIndex;
                st_uint32               m_uiNumVertices;
            };

                                        CLodRenderInfo( );
                                        ~CLodRenderInfo( );

            // init functions
            void                        InitIndexed(const SIndexedTriangles* pLods, st_int32 nNumLods);
            void                        InitLeafCards(const SLeafCards* pLods, st_int32 nNumLods);
            void                        AddRef(st_int32 nLod, const SRef& sRef);

            // calls made during render loop
            const CLodRenderInfo::SRef* IsActive(st_int32 nLod, st_int32 nMaterialId) const;
            st_bool                     LodHasGeometry(st_int32 nLod) const;
            const CArray<st_int32>*     MaterialRefs(void) const;

    private:
            void                        AllocMemPool(void);

            st_int32                    m_nNumLods;
            st_int32                    m_nMaxMaterialId;

            CArray<SRef>                m_aRefs;
            CArray<st_bool>             m_aLodsWithGeometry;    // helps to quickly update draw stats
            CArray<st_int32>            m_aMaterialsUsed;

            st_byte*                    m_pMemPool;             // used to reduce heap allocations  
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CTreeRI

    #define CTreeRI_t CTreeRI<TTexturePolicy, TGeometryBufferPolicy, TShaderTechniquePolicy>

    template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>
    class ST_STORAGE_CLASS CTreeRI : public CTree
    {
    public:
                                            CTreeRI( );
            virtual                         ~CTreeRI( );
    
            // graphics
            st_bool                         InitGraphics(const CArray<CFixedString>& aSearchPaths, st_bool bQuadsSupported, const TShaderTechniquePolicy** pTechniques, st_int32 nMaxAnisotropy = 0);
            st_bool                         InitBillboardSystem(st_int32 nMaxNumInstancesPerCell,
                                                                st_int32 nMaxNumVisibleCells,
                                                                st_bool bHorzBillboardsAllowed);
            st_bool                         GraphicsAreInitialized(void) const;
            const TGeometryBufferPolicy*    GetGeometryBuffer(EGeometryType eType) const;

            // geometry
            const CArray<SMaterialRI_t>*    GetMaterials(void) const;
            const CLodRenderInfo*           GetLodRenderInfo(EGeometryType eType) const;

            // billboards
            TGeometryBufferPolicy*          GetBillboardVB(void);

            st_int32                        GetBillboardMaxInstancesPerCell(void) const;
            st_int32                        GetBillboardMaxVisibleCells(void) const;

            st_int32                        GetBillboardVboNumVertices(void) const;
            st_int32                        GetBillboardIboNumIndices(void) const;
            st_int32                        GetBillboardVboBlockNumVertices(void) const;

    protected:
            st_bool                         InitMaterials(const SGeometry* pGeometry, const CArray<CFixedString>& aSearchPaths, st_int32 nMaxAnisotropy = 0);
            st_bool                         InitBranches(const SGeometry* pGeometry, const TShaderTechniquePolicy* pTechnique);
            st_bool                         InitFronds(const SGeometry* pGeometry, const TShaderTechniquePolicy* pTechnique);
            st_bool                         InitLeafCards(const SGeometry* pGeometry, const TShaderTechniquePolicy* pTechnique);
            st_bool                         InitLeafMeshes(const SGeometry* pGeometry, const TShaderTechniquePolicy* pTechnique);

            // determine sizes ahead of time to reduce heap allocations
            st_bool                         ReserveGeometry(const SGeometry* pGeometry, EGeometryType eType);

            // used by indexed triangle geometry types (branches, fronds, leaf meshes)
            st_bool                         IndexedTrianglesCanFitInto16BitIB(const SIndexedTriangles* pLods, st_int32 nNumLods) const;
            st_bool                         LeafCardsCanFitInto16BitIB(const SLeafCards* pLods, st_int32 nNumLods) const;
            st_bool                         InitIndexBuffer(st_int32 nLod, 
                                                            const SIndexedTriangles* pLod, 
                                                            TGeometryBufferPolicy* pGeometryBuffer, 
                                                            CLodRenderInfo& cLodRenderInfo,
                                                            st_bool bUse16BitIndices);

            // used by leaf cards
            st_bool                         InitLeafCardRenderInfo(st_int32 nLod, const SLeafCards* pLod);

            // materials/textures
            CArray<SMaterialRI_t>           m_aMaterials;

            // geometry
            CLodRenderInfo                  m_acRenderInfo[GEOMETRY_TYPE_NUM_3D_TYPES];
            TGeometryBufferPolicy           m_atGeometryBuffers[GEOMETRY_TYPE_NUM_3D_TYPES];
            st_bool                         m_bQuadPrimitiveSupported;

            // billboards
            TGeometryBufferPolicy           m_tBillboardVB;
            st_int32                        m_nBillboardMaxInstances;
            st_int32                        m_nBillboardMaxCells;
            st_bool                         m_bHorzBillboardsAllowed;
            const TShaderTechniquePolicy*   m_pBillboardTechnique;

            // textures
            const CArray<CFixedString>*     m_pSearchPaths;

            // misc
            st_bool                         m_bGraphicsInitialized;

    private:
                                            CTreeRI(const CTreeRI& cRight);     // copying CTreeRI disabled
    };
    #define CTreeRI_t CTreeRI<TTexturePolicy, TGeometryBufferPolicy, TShaderTechniquePolicy>


    ///////////////////////////////////////////////////////////////////////  
    //  Type definitions

    typedef CArray<st_uint32> MaterialIndices;


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SRenderStats

    struct ST_STORAGE_CLASS SRenderStats
    {
    public:
                                    SRenderStats( );

            struct ST_STORAGE_CLASS SGeometryStats
            {
                                    SGeometryStats( );

                void                Reset(void);

                st_uint32           m_uiNumObjects;
                st_uint32           m_uiNumTextureBinds;
                st_uint32           m_uiNumDrawCalls;
                st_uint32           m_uiNumVboBinds;
                st_uint32           m_uiNumTriangles;
            };

            void                    Reset(void);
            void                    GetTotals(SGeometryStats& sTotals) const;

            SGeometryStats          m_aGeometries[STATS_CATEGORY_COUNT][RENDER_PASS_NUM_TYPES];
            st_int32                m_nNumCellsInFrustum;
            st_float32              m_fCullAndPopulateTime;     // microseconds
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SForestVertexBufferStats
    //
    //  All sizes are in bytes.

    struct ST_STORAGE_CLASS SForestVertexBufferStats
    {
                                    SForestVertexBufferStats( );

        // 3D tree geometry: branches, leaves, fronds, etc.
        size_t                      m_asiTreeVBs[GEOMETRY_TYPE_NUM_3D_TYPES];
        size_t                      m_asiTreeIBs[GEOMETRY_TYPE_NUM_3D_TYPES];
        size_t                      m_siTotalTreeVBs;   // sum of all m_asiTreeVBs elements
        size_t                      m_siTotalTreeIBs;   // sum of all m_asiTreeIBs elements

        // memory used by the billboard system; covering all base trees
        size_t                      m_siBillboardVBs;
        size_t                      m_siBillboardIBs;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SForestRenderInfo

    struct ST_STORAGE_CLASS SForestRenderInfo
    {
                                        SForestRenderInfo( );

            // general rendering
            CFixedString                m_strShaderPath;                // see note below
            st_int32                    m_nMaxAnisotropy;               // see note below
            st_bool                     m_bHorizontalBillboards;        // see note below
            st_float32                  m_fAlphaTestScalar;
            st_bool                     m_bZPrePass;                    // see note below
            st_int32                    m_nMaxBillboardImagesByBase;    // the maximum # of billboard images any one base tree may have
            st_float32                  m_fVisibility;

            // lighting
            st_float32                  m_fGlobalLightScalar;
            SMaterial                   m_sLightMaterial;
            st_bool                     m_bSpecularLighting;            // see note below
            st_bool                     m_bTransmissionLighting;        // see note below
            st_bool                     m_bDetailLayer;                 // see note below
            st_bool                     m_bDetailNormalMapping;         // see note below
            st_bool                     m_bAmbientContrast;             // see note below
            st_float32                  m_fTransmissionScalar;

            // fog
            st_float32                  m_fFogStartDistance;
            st_float32                  m_fFogEndDistance;
            Vec3                        m_vFogColor;

            // sky
            Vec3                        m_vSkyColor;
            st_float32                  m_fSkyFogMin;
            st_float32                  m_fSkyFogMax;

            // sun
            Vec3                        m_vSunColor;
            st_float32                  m_fSunSize;
            st_float32                  m_fSunSpreadExponent;
            st_float32                  m_fSunFogBloom;

            // shadows
            st_int32                    m_nNumShadowMaps;
            st_int32                    m_nShadowMapResolution;
            st_bool                     m_bSmoothShadows;               // see note below
            st_bool                     m_bShowShadowSplitsOnTerrain;   // see note below

            // wind
            st_bool                     m_bWindEnabled;                 // set note below
            st_bool                     m_bFrondRippling;               // see note below

            // *note: these values will be ignored if changed after CForestRI::InitGraphics() has been called
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Billboard vertex description

    struct SBillboardVertex
    {
        Vec3        m_vPosition; st_float32 m_fCornerAndRotation;       // position
        st_float32  m_fScale;
    };

    static const SVertexAttribDesc asBillboardVertexAttribs[ ] =
    {
        { VERTEX_ATTRIB_SEMANTIC_POS, VERTEX_ATTRIB_TYPE_FLOAT, 4 },
        { VERTEX_ATTRIB_SEMANTIC_TEXCOORD0, VERTEX_ATTRIB_TYPE_FLOAT, 1 },
        VERTEX_ATTRIB_END( )
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SForestCullResultsRI

    #define SForestCullResultsRI_TemplateList template<class TTexturePolicy, class TGeometryBufferPolicy, class TShaderTechniquePolicy>

    SForestCullResultsRI_TemplateList
    struct ST_STORAGE_CLASS SForestCullResultsRI : public SForestCullResults
    {
            struct SBillboardIboData
            {
                                        SBillboardIboData( );

                TGeometryBufferPolicy   m_tIbo;
                st_uint32               m_uiNumIndices;
                st_uint32               m_uiMinIndex;
                st_uint32               m_uiNumVertices;
            };
            typedef CMap<const CTree*, SBillboardIboData> TTreeIboMap;

            TTreeIboMap                 m_mBillboardIbos;

    virtual st_bool                     Reserve(const TTreeArray& aBaseTrees,
                                                st_int32 nMaxNumBaseTrees, 
                                                st_int32 nMaxNumVisibleCells,
                                                st_int32 nMaxVisibleInstancesPerBase,
                                                st_bool bHorzBillboards);
    };
    #define SForestCullResultsRI_t SForestCullResultsRI<TTexturePolicy, TGeometryBufferPolicy, TShaderTechniquePolicy>


    ///////////////////////////////////////////////////////////////////////  
    //  Class CForestRI

    #define CForestRI_TemplateList template<class TRenderStatePolicy, class TTexturePolicy, class TGeometryBufferPolicy, class TShaderLoaderPolicy, class TShaderTechniquePolicy, class TShaderConstantPolicy, class TDepthTexturePolicy>

    CForestRI_TemplateList
    class ST_STORAGE_CLASS CForestRI : public CForest
    {
    public:
            enum EShaderType
            {
                SHADER_TYPE_STANDARD,
                SHADER_TYPE_DEPTH_ONLY,
                SHADER_TYPE_SHADOW,
                SHADER_TYPE_NUMTYPES
            };

                                            CForestRI( );
                                            ~CForestRI( );
    
            // tree management
            st_bool                         InitTreeGraphics(CTreeRI_t* pTree, st_int32 nMaxNumInstancesPerCell, st_bool bHorzBillboardsAllowed, const char* pTexturePath = NULL);
#ifndef SPEEDTREE_HEAP_FRIENDLY_MODE
    virtual st_bool                         ReplaceTree(const CTreeRI_t* pOldTree, CTreeRI_t* pNewTree, const char* pTexturePath = NULL);
#endif
    virtual CTree*                          AllocateTree(void) const;

            // culling & dynamic VBO updates
    virtual void                            CullAndComputeLOD(const CView& cView, SForestCullResultsRI_t& sResults, st_bool bSort = false);

            // general graphics
            void                            SetRenderInfo(const SForestRenderInfo& sInfo);
            const SForestRenderInfo&        GetRenderInfo(void) const;
            st_bool                         InitGraphics(st_bool bUsePrecompiledShaders);

            // main render functions
            st_bool                         StartRender(void);
            st_bool                         EndRender(void);
            st_bool                         UploadViewShaderParameters(const CView& cView) const;
            st_bool                         ClearBoundTextures(void);
            st_bool                         RenderBranches(const SForestCullResults& sVisibleTrees, ERenderPassType ePassType);
            st_bool                         RenderFronds(const SForestCullResults& sVisibleTrees, ERenderPassType ePassType);
            st_bool                         RenderLeafMeshes(const SForestCullResults& sVisibleTrees, ERenderPassType ePassType);
            st_bool                         RenderLeafCards(const SForestCullResults& sVisibleTrees, ERenderPassType ePassType, const CView& cView);
            st_bool                         RenderBillboards(const SForestCullResultsRI_t& sVisibleTrees, ERenderPassType ePassType, const CView& cView);

            // shadows
            st_bool                         BeginShadowMap(st_int32 nShadowMap, const CView& cLightView);
            st_bool                         EndShadowMap(st_int32 nShadowMap);
            st_bool                         UploadShadowData(st_int32 nShadowMap);
            st_bool                         ShadowsAreEnabled(void) const;
            TDepthTexturePolicy*            GetShadowDepthTexture(st_int32 nShadowMap);
            st_bool                         UploadShadowShaderConsts(TShaderConstantPolicy tShadowSplitDistance, 
                                                                     TShaderConstantPolicy tShadowFadeParams,
                                                                     TShaderConstantPolicy tFarClip) const;

            // texturing
            void                            EnableTexturing(st_bool bFlag);
            void                            TransparentTextureModeNotify(ETextureAlphaRenderMode eMode);

            // wind
            st_bool                         UploadWindParams(const CWind& cWind) const;

            // overlays
            st_bool                         InitOverlays(void);
            st_bool                         RenderOverlays(void);

            // needed mostly for DirectX devices
            void                            OnResetDevice(void);
            void                            OnLostDevice(void);

            // statistics
            st_bool                         GetVertexBufferStats(SForestVertexBufferStats& sStats) const; // query VB/IB usage for current configuration
            SRenderStats&                   GetRenderStats(void); // query statistics for last rendered frame

            // shaders
            TShaderLoaderPolicy*            GetShaderLoader(void);
    static  TShaderTechniquePolicy&         PickTechnique(TShaderTechniquePolicy atAvailableTechniques[SHADER_TYPE_NUMTYPES], ERenderPassType ePassType);

    protected:
            // shader setup
            st_bool                         LoadShaders(st_bool bUsePrecompiledShaders);
            st_bool                         QueryUniformVariables(void);
            st_bool                         SetupOverlayShader(void);

            // shader uploads
            st_int32                        UploadMaterial(const SMaterialRI_t* pMaterial, st_bool bTextureOnly = false);
            st_bool                         UploadBaseTreeParameters(const CTreeRI_t* pBaseTree) const;
            st_bool                         UpdateLodProfile(const SLodProfile& sLodProfile) const;

            // wind
            void                            UploadWindParams(const CTreeRI_t* pBaseTree, const CWind* pWind = NULL);

            // billboards
            st_bool                         UpdateBillboards(SForestCullResultsRI_t& sVisibleTrees);
            st_bool                         AddBaseTreeBillboardData(CTreeRI_t* pBaseTree);
            void                            UpdateBillboardVbos(CTreeCell* pCell);
            st_bool                         UpdateBillboardIbos(SForestCullResultsRI_t& sVisibleTrees) const;
            st_bool                         UploadBillboardTangents(st_float32 fAzimuth) const;

            // render states
            void                            SetBackfaceStateByMaterial(const SMaterialRI_t* pMaterial, ERenderPassType eRenderPass) const;
            void                            DisableTransparentTextures(void);

            // misc rendering
            void                            InitFallbackTextures(void);

            // rendering
            SForestRenderInfo               m_sRenderInfo;
            TRenderStatePolicy              m_tRenderStatePolicy;
            TShaderLoaderPolicy             m_tShaderLoader;
            TShaderTechniquePolicy          m_atBranchTechniques[SHADER_TYPE_NUMTYPES];
            TShaderTechniquePolicy          m_atFrondTechniques[SHADER_TYPE_NUMTYPES];
            TShaderTechniquePolicy          m_atLeafCardTechniques[SHADER_TYPE_NUMTYPES];
            TShaderTechniquePolicy          m_atLeafMeshTechniques[SHADER_TYPE_NUMTYPES];
            TShaderTechniquePolicy          m_atBillboardTechniques[SHADER_TYPE_NUMTYPES];

            // texture management
            ETextureAlphaRenderMode         m_eTextureAlphaMethod;
            st_bool                         m_bTexturingEnabled;
            TTexturePolicy                  m_atLastBoundTextures[TL_NUM_TEX_LAYERS];

            // uniform shader constants (shared by multiple techniques)
            CArray<CFixedString>            m_aDefines;
            TShaderConstantPolicy           m_tProjModelviewMatrix;
            TShaderConstantPolicy           m_tCameraFacingMatrix;
            TShaderConstantPolicy           m_tTreePosAndScale;
            TShaderConstantPolicy           m_tTreeRotation;
            TShaderConstantPolicy           m_tLodProfile;
            TShaderConstantPolicy           m_tMaterialAmbient;
            TShaderConstantPolicy           m_tMaterialDiffuse;
            TShaderConstantPolicy           m_tMaterialSpecular;
            TShaderConstantPolicy           m_tMaterialEmissive;
            TShaderConstantPolicy           m_tMaterialExtras;
            TShaderConstantPolicy           m_tMaterialExtrasBBs;
            TShaderConstantPolicy           m_tMaterialTransmission;
            TShaderConstantPolicy           m_atTextures[TL_NUM_TEX_LAYERS];
            TShaderConstantPolicy           m_tLightDir;
            TShaderConstantPolicy           m_tFogParams;
            TShaderConstantPolicy           m_tFogColor;
            TShaderConstantPolicy           m_tCameraAngles;
            TShaderConstantPolicy           m_tCameraPosition;
            TShaderConstantPolicy           m_tCameraDirection;
            TShaderConstantPolicy           m_tFarClip;
            TShaderConstantPolicy           m_tSkyColor;
            TShaderConstantPolicy           m_tSunParams;

            // wind shader constants
            TShaderConstantPolicy           m_tWindDir;
            TShaderConstantPolicy           m_tWindTimes;
            TShaderConstantPolicy           m_tWindDistances;
            TShaderConstantPolicy           m_tWindLeaves;
            TShaderConstantPolicy           m_tWindFrondRipple;
            TShaderConstantPolicy           m_tWindGust;
            TShaderConstantPolicy           m_tWindGustHints;

        #ifdef SPEEDTREE_ADVANCED_WIND
            TShaderConstantPolicy           m_tWindRollingBranches;
            TShaderConstantPolicy           m_tWindRollingLeaves;
            TShaderConstantPolicy           m_tWindTwitchingLeaves;
            TShaderConstantPolicy           m_tWindTumblingLeaves;
        #endif

            // billboard shader constants
            TShaderConstantPolicy           m_tNum360Images;
            TShaderConstantPolicy           m_tCameraAzimuthTrig;
            TShaderConstantPolicy           m_t360BillboardTexCoords;
            TShaderConstantPolicy           m_tVertBillboardDimensions;
            TShaderConstantPolicy           m_tHorzBillboardDimensions;
            TShaderConstantPolicy           m_tBillboardTangents;
            TShaderConstantPolicy           m_tHorzBillboardFade;
            TShaderConstantPolicy           m_tHorzBillboardTexCoords;

            // shadows
            TDepthTexturePolicy             m_atShadowBuffers[c_nMaxNumShadowMaps];
            TShaderConstantPolicy           m_atShadowMaps[c_nMaxNumShadowMaps];
            TShaderConstantPolicy           m_tLightViewMatricesArray;
            TShaderConstantPolicy           m_tShadowSplitDistances;
            TShaderConstantPolicy           m_tShadowFadeParams;

            // overlays
            TShaderTechniquePolicy          m_tOverlayTechnique;
            TShaderConstantPolicy           m_tOverlayTextureVar;
            TGeometryBufferPolicy           m_tOverlayGeometry;
            
            // render-related
            TTexturePolicy                  m_atFallbackTextures[TL_NUM_TEX_LAYERS];
            SRenderStats                    m_sRenderStats;
            st_bool                         m_bQuadPrimitiveSupported;

    private:
                                            CForestRI(const CForestRI& cRight);     // copying CForestRI disabled
    };
    #define CForestRI_t CForestRI<TRenderStatePolicy, TTexturePolicy, TGeometryBufferPolicy, TShaderLoaderPolicy, TShaderTechniquePolicy, TShaderConstantPolicy, TDepthTexturePolicy>

    // include inline functions
    #include "TextureRI.inl"
    #include "RenderStateRI.inl"
    #include "GeometryBufferRI.inl"
    #include "ShaderRI.inl"
    #include "DepthTextureRI.inl"
    #include "TreeRI.inl"
    #include "MiscRI.inl"
    #include "ForestRI.inl"
    #include "BillboardRI.inl"
    #include "OverlayRI.inl"

} // end namespace SpeedTree

#include "Core/ExportEnd.h"
