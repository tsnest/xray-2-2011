///////////////////////////////////////////////////////////////////////  
//  GeometryBufferRI.inl
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
//  SVertexAttribDesc::SizeOfDataType

inline st_uint32 SVertexAttribDesc::SizeOfDataType(void) const
{
    ///////////////////////////////////////////////////////////////////////
    //  Size table
    //
    //  Size in bytes of each of the data types

    static st_uint32 auiSizeTable[VERTEX_ATTRIB_TYPE_COUNT] = 
    {
        1, // VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE,
        1, // VERTEX_ATTRIB_TYPE_UNSIGNED_BYTE_NORMALIZED,
        2, // VERTEX_ATTRIB_TYPE_SHORT,
        2, // VERTEX_ATTRIB_TYPE_SHORT_NORMALIZED,
        2, // VERTEX_ATTRIB_TYPE_UNSIGNED_SHORT,
        2, // VERTEX_ATTRIB_TYPE_UNSIGNED_SHORT_NORMALIZED,
        4, // VERTEX_ATTRIB_TYPE_INT,
        4, // VERTEX_ATTRIB_TYPE_INT_NORMALIZED,
        4, // VERTEX_ATTRIB_TYPE_UNSIGNED_INT,
        4, // VERTEX_ATTRIB_TYPE_UNSIGNED_INT_NORMALIZED,
        4, // VERTEX_ATTRIB_TYPE_FLOAT,
        8  // VERTEX_ATTRIB_TYPE_DOUBLE,
    };

    return auiSizeTable[m_eDataType];
}


///////////////////////////////////////////////////////////////////////  
//  SVertexAttribDesc::SizeOfAttrib

inline st_uint32 SVertexAttribDesc::SizeOfAttrib(void) const
{
    return m_uiNumElements * SizeOfDataType( );
}


///////////////////////////////////////////////////////////////////////  
//  SVertexAttribDesc::DataTypeName

inline const char* SVertexAttribDesc::DataTypeName(void) const
{
    static const char* aszDataTypeNamesTable[VERTEX_ATTRIB_TYPE_COUNT] =
    {
        "Unsigned Byte",
        "Unsigned Byte (Normalized)",
        "Short",
        "Short (Normalized)",
        "Unsigned Short",
        "Unsigned Short (Normalized)",
        "Integer",
        "Integer (Normalized)",
        "Unsigned Integer",
        "Unsigned Integer (Normalized)",
        "Float",
        "Double"
    };

    return aszDataTypeNamesTable[m_eDataType];
}


///////////////////////////////////////////////////////////////////////  
//  SVertexAttribDesc::SemanticName

inline const char* SVertexAttribDesc::SemanticName(void) const
{
    static const char* aszSemanticNamesTable[VERTEX_ATTRIB_SEMANTIC_COUNT] =
    {
        "Position",
        "Blend Weight",
        "Normal",
        "Diffuse Color",
        "Specular Color",
        "Tessellation Factor",
        "Point Size",
        "Blend Indices",
        "Texture Coords, Layer 0",
        "Texture Coords, Layer 1",
        "Texture Coords, Layer 2",
        "Texture Coords, Layer 3",
        "Texture Coords, Layer 4",
        "Texture Coords, Layer 5",
        "Texture Coords, Layer 6",
        "Texture Coords, Layer 7"
    };

    return aszSemanticNamesTable[m_eSemantic];
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferRI::CGeometryBufferRI

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::CGeometryBufferRI(st_bool bDynamicVB, st_bool bDynamicIB)
    : m_bDynamicVB(bDynamicVB)
    , m_uiVertexSize(0)
    , m_uiNumVertices(0)
#ifndef NDEBUG
    , m_bFormatEnabled(false)
    , m_bVertexBufferBound(false)
    , m_bIndexBufferBound(false)
#endif
    , m_bDynamicIB(bDynamicIB)
    , m_uiIndexSize(4)
    , m_uiNumIndices(0)
    , m_nVertexHeapHandle(-1)
    , m_nIndexHeapHandle(-1)
{
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferRI::CGeometryBufferRI

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::~CGeometryBufferRI( )
{
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferRI::SetVertexFormat

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::SetVertexFormat(const SVertexAttribDesc* pAttribDesc, const TShaderTechniqueRI* pTechnique)
{
    st_bool bSuccess = false;

    // compute vertex size
    m_uiVertexSize = 0;
    assert(pAttribDesc);
    const SVertexAttribDesc* pAttrib = pAttribDesc;
    while (pAttrib->m_eSemantic != VERTEX_ATTRIB_SEMANTIC_END)
    {
        m_uiVertexSize += pAttrib->SizeOfAttrib( );
        ++pAttrib;
    }

    if (m_uiVertexSize > 0)
    {
        // graphics API needs to know about the format
        bSuccess = m_tGeometryBufferPolicy.SetVertexFormat(pAttribDesc, pTechnique);
    }

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::AppendVertices

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::AppendVertices(const void* pVertexData, st_uint32 uiNumVertices)
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    if (m_uiVertexSize > 0)
    {
        if (pVertexData && uiNumVertices > 0)
        {
#endif
            size_t siStartIndex = m_aVertexData.size( );
            m_aVertexData.resize(siStartIndex + m_uiVertexSize * uiNumVertices);

            memcpy(&m_aVertexData[siStartIndex], pVertexData, uiNumVertices * m_uiVertexSize);
            m_uiNumVertices += uiNumVertices;

            bSuccess = true;
#ifndef NDEBUG
        }
    }
    else
        CCore::SetError("CGeometryBufferRI::AppendVertices, SetFormat() must be called before AppendVertices");
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::EndVertices

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::EndVertices(void)
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    if (m_uiVertexSize > 0 && !m_aVertexData.empty( ))
    {
#endif
        bSuccess = m_tGeometryBufferPolicy.CreateVertexBuffer(m_bDynamicVB, &m_aVertexData[0], st_uint32(m_aVertexData.size( ) / m_uiVertexSize), m_uiVertexSize);

        // don't need the client-side copy since the buffer's been created
        m_aVertexData.clear( );

        if (m_nVertexHeapHandle > -1)
        {
            CCore::UnlockTmpHeapBlock(m_nVertexHeapHandle);
            m_nVertexHeapHandle = -1;
        }

#ifndef NDEBUG
    }
    else
        CCore::SetError("CGeometryBufferRI::EndVertices, SetFormat() and AppendVertices() must be called before EndVertices");
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::OverwriteVertices

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::OverwriteVertices(const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiOffset)
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    if (pVertexData && uiNumVertices > 0)
    {
#endif
        if (m_tGeometryBufferPolicy.VertexBufferIsValid( ))
        {
            bSuccess = m_tGeometryBufferPolicy.OverwriteVertices(pVertexData, uiNumVertices, m_uiVertexSize, uiOffset);
        }
        else
        {
            CCore::SetError("CGeometryBufferRI::OverwriteVertices, cannot be called until a VB is established with AppendVertices() + EndVertices()");
        }
#ifndef NDEBUG
    }
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::NumVertices

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_uint32 CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::NumVertices(void) const
{
    return m_uiNumVertices;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::VertexSize

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_uint32 CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::VertexSize(void) const
{
    return m_uiVertexSize;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::EnableFormat

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::EnableFormat(void) const
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    // no need to check IsFormatSet() since VertexBufferIsValid can't return true is the format isn't already set
    if (m_uiVertexSize > 0 && m_tGeometryBufferPolicy.VertexBufferIsValid( ))
    {
#endif

        bSuccess = m_tGeometryBufferPolicy.EnableFormat( );

#ifndef NDEBUG
        m_bFormatEnabled = true;
    }
    else
        CCore::SetError("CGeometryBufferRI::EnableFormat, the vertex buffer has not been setup yet");
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::DisableFormat

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::DisableFormat(void)
{
    return TGeometryBufferPolicy::DisableFormat( );
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::BindVertexBuffer

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::BindVertexBuffer(EGeometryProcessingType eProcessingType) const
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    if (m_uiVertexSize > 0 && m_tGeometryBufferPolicy.VertexBufferIsValid( ))
    {
#endif
        bSuccess = m_tGeometryBufferPolicy.BindVertexBuffer(m_uiVertexSize, eProcessingType);

#ifndef NDEBUG
        if (bSuccess)
            m_bVertexBufferBound = true;
    }
    else
        CCore::SetError("CGeometryBufferRI::BindVertexBuffer, the vertex buffer has not been setup yet");
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::UnBindVertexBuffer

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::UnBindVertexBuffer(void)
{
    return TGeometryBufferPolicy::UnBindVertexBuffer( );
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::SetIndexFormat

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::SetIndexFormat(EIndexFormat eFormat)
{
    if (eFormat == INDEX_FORMAT_UNSIGNED_16BIT)
        m_uiIndexSize = 2;
    else 
        m_uiIndexSize = 4;

    return m_tGeometryBufferPolicy.SetIndexFormat(eFormat);
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::AppendIndices

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::AppendIndices(const void* pIndexData, st_uint32 uiNumIndices)
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    if (pIndexData && uiNumIndices > 0)
    {
#endif
        if (m_uiIndexSize > 0)
        {
            // grow the internal representation by the amount requested to append
            size_t siStartIndex = m_aIndexData.size( );
            m_aIndexData.resize(siStartIndex + uiNumIndices * m_uiIndexSize);

            // copy into the array
            memcpy(&m_aIndexData[siStartIndex], pIndexData, uiNumIndices * m_uiIndexSize);

            // m_uiNumIndices is used instead of m_aIndexData.size() because m_aIndexData will
            // be cleared after EndIndices() is called
            m_uiNumIndices += uiNumIndices;

            bSuccess = true;
        }
        else
            CCore::SetError("CGeometryBufferRI::AppendIndices, index buffer format must be set first");
#ifndef NDEBUG
    }
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::ReplaceIndices

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::OverwriteIndices(const void* pIndexData, st_uint32 uiNumIndices, st_uint32 uiOffset)
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    if (pIndexData && uiNumIndices > 0)
    {
#endif
        // if index buffer is in place, replace the buffer contents
        if (m_tGeometryBufferPolicy.IndexBufferIsValid( ))
        {
            bSuccess = m_tGeometryBufferPolicy.OverwriteIndices(pIndexData, uiNumIndices, uiOffset);
        }
#ifndef NDEBUG
    }
#endif

    if (bSuccess && uiNumIndices > m_uiNumIndices)
        m_uiNumIndices = uiNumIndices;

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::EndIndices

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::EndIndices(void)
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    if (!m_aIndexData.empty( ))
    {
#endif
        m_uiNumIndices = st_uint32(m_aIndexData.size( ) / m_uiIndexSize);
        bSuccess = m_tGeometryBufferPolicy.CreateIndexBuffer(m_bDynamicIB, &m_aIndexData[0], m_uiNumIndices);
        
        // don't need the client-side copy since the buffer's been created
        m_aIndexData.clear( );

        if (m_nIndexHeapHandle > -1)
        {
            CCore::UnlockTmpHeapBlock(m_nIndexHeapHandle);
            m_nIndexHeapHandle = -1;
        }

#ifndef NDEBUG
    }
    else
        CCore::SetError("CGeometryBufferRI::EndIndices, AppendIndices() must be called before EndVertices");
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::NumIndices

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_uint32 CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::NumIndices(void) const
{
    return m_uiNumIndices;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::IndexSize

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_uint32 CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::IndexSize(void) const
{
    return m_tGeometryBufferPolicy.IndexSize( );
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::ClearIndices

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::ClearIndices(void)
{
    m_aIndexData.clear( ); // should already be cleared
    m_uiNumIndices = 0;

    return m_tGeometryBufferPolicy.ClearIndices( );
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::BindIndexBuffer

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::BindIndexBuffer(void) const
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    if (m_tGeometryBufferPolicy.IndexBufferIsValid( ))
    {
#endif

        bSuccess = m_tGeometryBufferPolicy.BindIndexBuffer( );

#ifndef NDEBUG
        if (bSuccess)
            m_bIndexBufferBound = true;
    }
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::UnBindIndexBuffer

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::UnBindIndexBuffer(void) const
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    if (m_tGeometryBufferPolicy.IndexBufferIsValid( ))
    {
#endif

        bSuccess = m_tGeometryBufferPolicy.UnBindIndexBuffer( );

#ifndef NDEBUG
        if (bSuccess)
            m_bIndexBufferBound = false;
    }
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::RenderIndexed

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::RenderIndexed(EPrimitiveType ePrimType, 
                                                                                           st_uint32 uiStartIndex, 
                                                                                           st_uint32 uiNumIndices, 
                                                                                           st_uint32 uiMinIndex,
                                                                                           st_uint32 uiNumVertices,
                                                                                           EGeometryProcessingType eProcessingType) const
                                                                                           
{
    return m_tGeometryBufferPolicy.RenderIndexed(ePrimType, uiStartIndex, uiNumIndices, uiMinIndex, uiNumVertices, eProcessingType);
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::RenderArrays

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::RenderArrays(EPrimitiveType ePrimType, st_uint32 uiStartVertex, st_uint32 uiNumVertices, EGeometryProcessingType eProcessingType) const
{
    st_bool bSuccess = false;

#ifndef NDEBUG
    if (m_bVertexBufferBound)
    {
#endif

        bSuccess = m_tGeometryBufferPolicy.RenderArrays(ePrimType, uiStartVertex, uiNumVertices, eProcessingType);

#ifndef NDEBUG
    }
    else
        CCore::SetError("CGeometryBufferRI::RenderArrays, the vertex buffer is not bound");
#endif

    return bSuccess;
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::IsPrimitiveTypeSupported

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::IsPrimitiveTypeSupported(EPrimitiveType ePrimType)
{
    return TGeometryBufferPolicy::IsPrimitiveTypeSupported(ePrimType);
}


///////////////////////////////////////////////////////////////////////
//  CGeometryBufferRI::Reserve

template<class TGeometryBufferPolicy, class TShaderTechniqueRI>
inline st_bool CGeometryBufferRI<TGeometryBufferPolicy, TShaderTechniqueRI>::Reserve(st_uint32 uiNumVertices, st_uint32 uiNumIndices)
{
    st_bool bSuccess = true;

    // vertices
    if (m_uiVertexSize > 0)
    {
        if (uiNumVertices > 0)
        {
            // block size
            const size_t c_siBlockSize = uiNumVertices * m_uiVertexSize;

            // get block
            st_byte* pBlock = CCore::LockTmpHeapBlock(c_siBlockSize, "CGeometryBufferRI_Vertices", m_nVertexHeapHandle);
            assert(pBlock && m_nVertexHeapHandle > -1);

            // assign block
            m_aVertexData.SetExternalMemory(pBlock, c_siBlockSize);
        }
    }
    else if (uiNumVertices > 0)
    {
        CCore::SetError("CGeometryBufferRI::Reserve failed, vertex size not yet known");
        bSuccess = false;
    }

    // indices
    if (m_uiIndexSize > 0)
    {
        if (uiNumIndices > 0)
        {
            // block size
            const size_t c_siBlockSize = uiNumIndices * m_uiIndexSize;

            // get block
            st_byte* pBlock = CCore::LockTmpHeapBlock(c_siBlockSize, "CGeometryBufferRI_Index", m_nIndexHeapHandle);
            assert(pBlock && m_nIndexHeapHandle > -1);

            // assign block
            m_aIndexData.SetExternalMemory(pBlock, c_siBlockSize);
        }

        bSuccess = true;
    }
    else if (uiNumIndices > 0)
    {
        CCore::SetError("CGeometryBufferRI::Reserve failed, index size not yet known");
        bSuccess = false;
    }

    return bSuccess;
}
