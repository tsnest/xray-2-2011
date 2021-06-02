///////////////////////////////////////////////////////////////////////  
//  Core.h
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
//
//  The SpeedTree SDK generally depends on the following external macros to 
//  distinguish platforms:
//
//    Windows 32/64:
//      - _WIN32: defined in both 32- and 64-bit environments
//
//    XBOX 360/Xenon:
//      - _XBOX: Xenon only; no SpeedTree version currently exists for the 
//               original XBOX (SPEEDTREE_BIG_ENDIAN is #defined if detected)
//      - _WIN32: also defined for Xbox 360
//
//    PLAYSTATION(r)3:
//      - __CELLOS_LV2__ (SPEEDTREE_BIG_ENDIAN is #defined if detected)
//
//    Mac OSX:
//      - __APPLE__

#pragma once
#include <speedtree/core/exportbegin.h>
#include <speedtree/core/types.h>
#include <speedtree/core/coordsys.h>
#include <speedtree/core/matrix.h>
#include <speedtree/core/extents.h>
#include <speedtree/core/string.h>
#include <speedtree/core/wind.h>
#include <cstdlib>

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  Versioning

#define SPEEDTREE_VERSION_MAJOR     5
#define SPEEDTREE_VERSION_MINOR     2
#define SPEEDTREE_VERSION_SUBMINOR  0
#define SPEEDTREE_VERSION_STRING    "5.2.0"


///////////////////////////////////////////////////////////////////////  
//  Platform specifics

#if defined(_XBOX) || defined(__CELLOS_LV2__)
    #define SPEEDTREE_BIG_ENDIAN
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  Forward references

    class CAllocator;
    class CGeometry;


    ///////////////////////////////////////////////////////////////////////  
    //  Enumeration ETextureLayers

    enum ETextureLayers
    {
        TL_DIFFUSE,        
        TL_NORMAL,         
        TL_SPECULAR,  
        TL_DETAIL_DIFFUSE,        
        TL_DETAIL_NORMAL,

        TL_NUM_TEX_LAYERS
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Enumeration ECullType

    enum ECullType
    {
        CULLTYPE_NONE, 
        CULLTYPE_BACK, 
        CULLTYPE_FRONT
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SMaterial

    struct ST_STORAGE_CLASS SMaterial
    {
                        SMaterial( );
                        ~SMaterial( );

        Vec4            m_vAmbient;                     // alpha value is reserved
        Vec4            m_vDiffuse;                     // alpha value is reserved
        Vec4            m_vSpecular;                    // alpha value is reserved
        Vec4            m_vEmissive;                    // emissive is used for transmission effects; alpha value is reserved
        st_float32      m_fShininess;                   // value ranges [0-128]

        // special material values
        st_float32      m_fLightScalar;                 // multiplied against the diffuse color during lighting
        st_float32      m_fAlphaScalar;                 // for textures with alpha values; blending, a2c, and alpha testing all require different scalars
        st_float32      m_fAmbientContrast;             // contributes to a non-uniform ambient lighting layer
        st_float32      m_fTransmissionShadow;          // how light/dark shadows are on transmission surfaces when looking through back side
        st_float32      m_fTransmissionViewDependence;  // amount of transmission effect to use, based on look vector

        // render states
        ECullType       m_eCullType;                    // allows finer optimization control

        // textures
        CFixedString    m_astrTextureFilenames[TL_NUM_TEX_LAYERS]; // use .c_str() to access elements as const char
        Vec4            m_vTexCoords;                   // (left, bottom, right, top) used to reference special textures, forced into an atlas from the Modeler 

        // misc
        CFixedString    m_strUserData;                  // use .c_str() to access as const char
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SDrawCallInfo

    struct ST_STORAGE_CLASS SDrawCallInfo
    {
                        SDrawCallInfo( );

        st_int32        m_nMaterialIndex;

        // when SDrawCallInfo is used to describe leaf card geometry, m_nOffset and m_nLength both represent
        // the number of cards (not vertices); when SDrawCallInfo is used to describe branch, frond, and leaf
        // mesh geometry, m_nOffset and m_nLength represent vertices
        st_int32        m_nOffset;
        st_int32        m_nLength;

        // used by SIndexedTriangles (branches, fronds, and leaf meshes), but not by 
        // SLeafCards (leaf cards)
        st_int32        m_nMinIndex;
        st_int32        m_nNumVertices;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SIndexedTriangles
    //
    //  Index data builds indexed triangle lists, not strips

    struct ST_STORAGE_CLASS SIndexedTriangles
    {
                                SIndexedTriangles( );
                                ~SIndexedTriangles( );

        st_bool                 HasGeometry(void) const;

        st_int32                m_nNumMaterialGroups;       // # of draw calls
        const SDrawCallInfo*    m_pDrawCallInfo;            // m_nNumMaterialGroups number of elements
        const st_uint16*        m_pTriangleIndices16;       // 16-bit, default indices (default)
        const st_uint32*        m_pTriangleIndices32;       // if a tree is large enough, this structure will be used instead


        // vertex attribute tables
        st_int32                m_nNumVertices;
        const st_float32*       m_pCoords;                  // sets of 3 (m_nNumVertices * 3 total floats)
        const st_float32*       m_pLodCoords;               // sets of 3 (m_nNumVertices * 3 total floats)
        const st_uint8*         m_pNormals;                 // sets of 3 (m_nNumVertices * 3 total uint8s); compressed
        const st_uint8*         m_pBinormals;               // sets of 3 (m_nNumVertices * 3 total uint8s); compressed
        const st_uint8*         m_pTangents;                // sets of 3 (m_nNumVertices * 3 total uint8s); compressed
        const st_float32*       m_pTexCoordsDiffuse;        // sets of 2 (m_nNumVertices * 2 total floats);
        const st_float32*       m_pTexCoordsDetail;         // sets of 2 (m_nNumVertices * 2 total floats)
        const st_uint8*         m_pAmbientOcclusionValues;  // sets of 1 (m_nNumVertices * 1 total uint8s); compressed
        st_float32              m_fWindDataMagnitude;       // used to decompress m_pWindData.xyz values
        const st_uint8*         m_pWindData;                // sets of 6 (m_nNumVertices * 6 total floats); compressed
        const st_float32*       m_pFrondRipple;             // sets of 2 (m_nNumVertices * 2 total floats)
        const st_float32*       m_pLeafMeshWind;            // sets of 3 (m_nNumVertices * 3 total floats)
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SLeafCards

    struct ST_STORAGE_CLASS SLeafCards
    {
                                SLeafCards( );
                                ~SLeafCards( );

        st_bool                 HasGeometry(void) const;

        st_int32                m_nNumMaterialGroups;
        const SDrawCallInfo*    m_pDrawCallInfo;            // m_nNumMaterialGroups number of elements
        st_int32                m_nTotalNumCards;           // # of cards contained in all material groups in this struct

        const st_float32*       m_pPositions;               // one (x, y, z) coords for each card, m_nNumCards * 3 total floats
        const st_float32*       m_pDimensions;              // one (width, height) pair per card, m_nNumCards * 2 total floats
        const st_float32*       m_pLodScales;               // one (start_scale, end_scale) pair per card, m_nNumCards * 2 total floats
        const st_float32*       m_pPivotPoints;             // one (x, y) pair per card, m_nNumCards * 2 total floats
        const st_uint8*         m_pNormals;                 // four (x, y, z) coords for each card, m_nNumCards * 3 * 4 total uint8s; compressed
        const st_uint8*         m_pBinormals;               // four (x, y, z) coords for each card, m_nNumCards * 3 * 4 total uint8s; compressed
        const st_uint8*         m_pTangents;                // four (x, y, z) coords for each card, m_nNumCards * 3 * 4 total uint8s; compressed
        const st_float32*       m_pTexCoordsDiffuse;        // four (u, v) texcoords for each card, m_nNumCards * 2 * 4 total floats
        const st_uint8*         m_pAmbientOcclusionValues;  // one value for each card, m_nNumCards total uint8s; compressed
        st_float32              m_fWindDataMagnitude;       // used to decompress m_pWindData.xyz values
        const st_uint8*         m_pWindData;                // five values for each card, m_nNumCards * 5 total floats; compressed
        const st_float32*       m_pLeafCardOffsets;         // four values for each card, m_nNumCards * 4 total floats
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SVerticalBillboards

    struct ST_STORAGE_CLASS SVerticalBillboards
    {
                            SVerticalBillboards( );
                            ~SVerticalBillboards( );

        st_int32            m_nMaterialIndex;           // indexes SGeometry material table
        st_int32            m_nNumBillboards;           // number of 360-degree billboards generated by Compiler app
        st_float32          m_fWidth;                   // width of the billboard, governed by tree extents     
        st_float32          m_fTopCoord;                // top-most point of the billboard, governed by tree height
        st_float32          m_fBottomCoord;             // bottom-most point, can be below zero for trees with roots, etc.
        const st_float32*   m_pTexCoords;               // four values per billboard, 4 * m_nNumBillboards total; each set of
                                                        // four values are (max_s, max_t, s_width, t_height)
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SHorizontalBillboard

    struct ST_STORAGE_CLASS SHorizontalBillboard
    {
                            SHorizontalBillboard( );
                            ~SHorizontalBillboard( );

        st_bool             m_bPresent;                 // true if an overhead billboard was exported using Compiler
        st_int32            m_nMaterialIndex;           // indexes SGeometry material table
        Vec3                m_avCoords[4];              // four sets of (xyz) to render the overhead square
        st_float32          m_afTexCoords[8];           // 4 * (s,t) pairs of diffuse/normal texcoords
        Vec4                m_vTexCoordsShader;         // alternate texcoords storage (max_s, max_t, width_s, height_t)
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SCollisionObject
    //
    //  There are two collision object types: sphere and capsules; m_vCenter1 and
    //  m_vCenter2 will be equal for spheres.

    struct ST_STORAGE_CLASS SCollisionObject
    {
                            SCollisionObject( );
                            ~SCollisionObject( );
                            
        CFixedString        m_strUserData;              // any data entered by the artist in the Modeler app
        Vec3                m_vCenter1;                 // center of sphere or one end of a capsule
        Vec3                m_vCenter2;                 // other end of capsule or same as m_vCenter1 if sphere
        st_float32          m_fRadius;                  // radius of the sphere or capsule
    };

    
    ///////////////////////////////////////////////////////////////////////  
    //  Enumeration EGeometryType

    enum EGeometryType
    {
        GEOMETRY_TYPE_BRANCHES,
        GEOMETRY_TYPE_FRONDS,
        GEOMETRY_TYPE_LEAF_CARDS,
        GEOMETRY_TYPE_LEAF_MESHES,
        GEOMETRY_TYPE_NUM_3D_TYPES,
        GEOMETRY_TYPE_VERTICAL_BILLBOARDS = GEOMETRY_TYPE_NUM_3D_TYPES,
        GEOMETRY_TYPE_HORIZONTAL_BILLBOARDS,

        GEOMETRY_TYPE_COUNT
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SGeometry

    struct ST_STORAGE_CLASS SGeometry
    {
                                    SGeometry( );
                                    ~SGeometry( );

        // materials
        st_int32                    m_nNumMaterials;
        SMaterial*                  m_pMaterials;       // shared by all of the geometry types

        // branches
        st_int32                    m_nNumBranchLods;
        SIndexedTriangles*          m_pBranchLods;

        // fronds
        st_int32                    m_nNumFrondLods;
        SIndexedTriangles*          m_pFrondLods;

        // leaf meshes
        st_int32                    m_nNumLeafMeshLods;
        SIndexedTriangles*          m_pLeafMeshLods;

        // leaf cards
        st_int32                    m_nNumLeafCardLods;
        SLeafCards*                 m_pLeafCardLods;

        // billboards
        SVerticalBillboards         m_sVertBBs;
        SHorizontalBillboard        m_sHorzBB;

        // internal SDK use
        SIndexedTriangles*          m_pCompositeIndexedLods;
        void                        Clear(void);
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SLodRange

    struct ST_STORAGE_CLASS SLodProfile
    {
                        SLodProfile( );

        st_bool         IsValid(void) const;
        void            ComputeDerived(void);
        void            Scale(st_float32 fScale);
        void            Square(SLodProfile& sSquaredProfile) const;

        st_float32      m_fHighDetail3dDistance;        // distance at which LOD transition from highest 3D level begins
        st_float32      m_fLowDetail3dDistance;         // distance at which the lowest 3D level is sustained
        st_float32      m_fBillboardStartDistance;      // distance at which the billboard begins to fade in and 3D fade out
        st_float32      m_fBillboardFinalDistance;      // distance at which the billboard is fully visible and 3D is completely gone
        st_bool         m_bLodIsPresent;

        // derived values
        st_float32      m_f3dRange;                     // m_fLowDetail3dDistance - m_fHighDetail3dDistance
        st_float32      m_fBillboardRange;              // m_fBillboardFinalDistance - m_fBillboardStartDistance
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Structure SLodSnapshot

    struct ST_STORAGE_CLASS SLodSnapshot
    {
                        SLodSnapshot( );

        // characters are used here as small signed integer values since the SLodSnapshot
        // structure is a member of every CInstance class in the Forest library; it's
        // important to keep them small

        // for each, 0 is highest LOD, (num_lods - 1) is lowest, and -1 is inactive
        st_int8         m_nBranchLodIndex;
        st_int8         m_nFrondLodIndex;
        st_int8         m_nLeafCardLodIndex;
        st_int8         m_nLeafMeshLodIndex;
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CAllocatorInterface
    //
    //  By way of example, here are two ways to use the CAllocatorInterface class
    //  to specify a custom allocator to the SDK:
    //
    //  //  Method #1
    //  //
    //  //  Create a global or static allocator object and pass it into the SDK before
    //  //  main is called.  Note that the CAllocatorInterface object constructor
    //  //  takes a pointer to the allocator object.
    //
    //  static CReferenceAllocator g_cMyAllocator;
    //  static SpeedTree::CAllocatorInterface g_cAllocatorInterface(&g_cMyAllocator);
    //
    //  /////////////////////////////////////////////////////////////////////// 
    //  // main
    //  //
    //  // Method #2
    //  //
    //  // Use the SpeedTree::CAllocatorInterface class to enable or disable the
    //  // allocator object at any point.
    //
    //  void main(void)
    //  {
    //    ...
    //    // don't use a custom allocator
    //    CAllocatorInterface cOff(NULL);
    //    
    //     ... // do something
    //
    //    // turn the allocator back on
    //    CAllocatorInterface cOn(&g_cMyAllocator);
    //  }

    class ST_STORAGE_CLASS CAllocatorInterface
    {
    public:
                    CAllocatorInterface(CAllocator* pAllocator);
    };


    ///////////////////////////////////////////////////////////////////////  
    //  Class CCore
        
    class ST_STORAGE_CLASS CCore
    {
    public:
    friend class CParser;

                                            CCore( );
            virtual                         ~CCore( );

            // loading
            st_bool                         LoadTree(const st_char* pFilename, st_float32 fScale = 1.0f);
            st_bool                         LoadTree(const st_byte* pMemBlock, 
                                                     st_uint32 uiNumBytes, 
                                                     st_bool bCopyBuffer, 
                                                     st_float32 fScale = 1.0f);
            const st_char*                  GetFilename(void) const;
    static  st_byte*           ST_CALL_CONV LoadFileIntoBuffer(const st_char* pFilename, st_uint32& uiBufferSize);
    static  void               ST_CALL_CONV SetTextureFlip(st_bool bFlip);
    static  st_bool            ST_CALL_CONV GetTextureFlip(void);

            // geometry
            const SGeometry*                GetGeometry(void) const;
            st_bool                         HasGeometryType(EGeometryType eType) const;
    static  const st_char*                  GetGeometryTypeName(EGeometryType eType);
            const CExtents&                 GetExtents(void) const;

            // LOD
            const SLodProfile&              GetLodProfile(void) const;
            const SLodProfile&              GetLodProfileSquared(void) const;
            st_bool                         SetLodProfile(const SLodProfile& sLodProfile);
            void                            ComputeLodSnapshot(st_float32 fLod, SLodSnapshot& sProfile) const;
            st_float32                      ComputeLodByDistance(st_float32 fDistance) const;
            st_float32                      ComputeLodByDistanceSquared(st_float32 fDistance) const;
    static  st_float32                      ComputeShaderLerp(st_float32 fLod, st_int32 nNumDiscreteLevels);

            // clip-space settings (part of the culling system)
    static  void               ST_CALL_CONV SetClipSpaceDepthRange(st_float32 fNear, st_float32 fFar); // opengl is (-1,1), directx is (0,1)
    static  void               ST_CALL_CONV GetClipSpaceDepthRange(st_float32& fNear, st_float32& fFar);

            // wind
            CWind&                          GetWind(void);
            const CWind&                    GetWind(void) const;

            // collision
            const SCollisionObject*         GetCollisionObjects(st_int32& nNumObjects) const;

            // user strings
            enum EUserStringOrdinal
            {
                USER_STRING_0,
                USER_STRING_1,
                USER_STRING_2,
                USER_STRING_3,
                USER_STRING_4,
                USER_STRING_COUNT
            };
            const char*                     GetUserString(EUserStringOrdinal eOrdinal) const;

            // memory management
            void                            DeleteGeometry(st_bool bKeepBillboardTexCoords);
    static  size_t             ST_CALL_CONV GetSdkHeapUsage(void);  // in bytes
    static  size_t             ST_CALL_CONV GetNumHeapAllocs(void);
    static  void               ST_CALL_CONV ShutDown(void);

            // mostly internal functions used by the whole SDK to keep heap allocation count low
    static  st_byte*           ST_CALL_CONV LockTmpHeapBlock(size_t siSizeInBytes, const char* pOwner, st_int32& nHandle);
    static  st_bool            ST_CALL_CONV UnlockTmpHeapBlock(st_int32 nHandle);
    static  st_bool            ST_CALL_CONV DeleteTmpHeapBlock(st_int32 nHandle, size_t siSizeThreshold = 0); // delete block if >= siSizeThreshold (in bytes)
    static  st_bool            ST_CALL_CONV DeleteAllTmpHeapBlocks(size_t siSizeThreshold = 0); // delete anything >= siSizeThreshold (in bytes)

            // error system
    static  void               ST_CALL_CONV SetError(const st_char* pError, ...);
    static  const st_char*     ST_CALL_CONV GetError(void);
    static  st_bool            ST_CALL_CONV IsRunTimeBigEndian(void);

            // licensing/evaluation system
    static  st_bool            ST_CALL_CONV Authorize(const st_char* pKey);
    static  st_bool            ST_CALL_CONV IsAuthorized(void);
    static  const st_char*     ST_CALL_CONV Version(st_bool bShort = false);

            // utility
    static  Vec3               ST_CALL_CONV UncompressVec3(const st_uint8* pCompressedVector);
    static  st_float32         ST_CALL_CONV UncompressScalar(st_uint8 uiCompressedScalar);
    static  st_uint8           ST_CALL_CONV CompressScalar(st_float32 fUncompressedScalar);

    private:
                                            CCore(const CCore& cRight); // copying CCore disabled

            void                            InventoryGeometry(void);
            void                            ApplyScale(st_float32 fScale);

            CFixedString                    m_strFilename;
            st_byte*                        m_pSrtBuffer;
            SGeometry                       m_sGeometry;
            SLodProfile                     m_sLodProfile;
            SLodProfile                     m_sLodProfileSquared;
            st_bool                         m_abGeometryTypesPresent[GEOMETRY_TYPE_COUNT];
            CWind                           m_cWind;
            CExtents                        m_cExtents;
            st_int32                        m_nNumCollisionObjects;
            SCollisionObject*               m_pCollisionObjects;
            CString                         m_strUserString;
            char*                           m_pUserStrings[USER_STRING_COUNT];
            st_bool                         m_bOwnsSrtBuffer;
            st_bool                         m_bBillboardTexCoordsCopied;

    public:
            // UE3 utilities
            struct SSupportingData
            {
                                            SSupportingData( );

                // lod
                SLodProfile                 m_sLodProfile;
                st_int32                    m_nNumBranchLods;
                st_int32                    m_nNumFrondLods;
                st_int32                    m_nNumLeafMeshLods;
                st_int32                    m_nNumLeafCardLods;

                // collision objects
                enum
                {
                    MAX_NUM_COLLISION_OBJECTS = 20
                };
                st_int32                    m_nNumCollisionObjects;
                SCollisionObject            m_asCollisionObjects[MAX_NUM_COLLISION_OBJECTS];

                // wind
                CWind::SParams              m_sWindParams;
            };

            st_bool                         PopulateSupportingDataBlock(SSupportingData& sData) const;
            st_bool                         ApplySupportingDataBlock(const SSupportingData& sData);
    };
    typedef CCore CTree;


    /////////////////////////////////////////////////////////////////////
    //  Class CLocalArray
    //
    //  Interface is nearly identical to CArray, but a CLocalArray's size
    //  is constant after its initial creation.  It uses the TmpHeapBlock
    //  functions in Core to avoid new heap allocations upon each creation.

    template <class T>
    class ST_STORAGE_CLASS CLocalArray : public CArray<T>
    {
    public:
            CLocalArray(size_t siNumElements, const char* pOwner, bool bResize = true) :
                m_bInUse(true),
                m_nBlockHandle(-1)
            {
                size_t siBlockSize = siNumElements * sizeof(T);
                st_byte* pHeapBlock = CCore::LockTmpHeapBlock(siBlockSize, pOwner, m_nBlockHandle);
                assert(pHeapBlock);
                CArray<T>::SetExternalMemory(pHeapBlock, siBlockSize);
                if (bResize)
                    CArray<T>::resize(siNumElements);
            }

            ~CLocalArray( )
            {
                if (m_bInUse)
                {
                    assert(m_nBlockHandle > -1);
                    CCore::UnlockTmpHeapBlock(m_nBlockHandle);
                }
            }

            void Release(void)
            {
                if (m_bInUse)
                {
                    assert(m_nBlockHandle > -1);
                    CCore::UnlockTmpHeapBlock(m_nBlockHandle);
                    m_bInUse = false;
                }
            }

    private:
            bool        m_bInUse;
            int         m_nBlockHandle;
    };


    // include inline functions
    #include <speedtree/core/Core.inl>

} // end namespace SpeedTree


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif

#include <speedtree/core/ExportEnd.h>
