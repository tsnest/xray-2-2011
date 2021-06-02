///////////////////////////////////////////////////////////////////////  
//  GeometryBuffer.inl
//
//	*** INTERACTIVE DATA VISUALIZATION (IDV) CONFIDENTIAL AND PROPRIETARY INFORMATION ***
//
//	This software is supplied under the terms of a license agreement or
//	nondisclosure agreement with Interactive Data Visualization, Inc. and
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
//  CGeometryBufferOpenGL::SAttribParams::SAttribParams

inline CGeometryBufferOpenGL::SAttribParams::SAttribParams( ) :
	m_nOffset(-1),
	m_nNumElements(-1),
	m_eDataType(GL_FLOAT),
	m_bNormalized(false)
{
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::SAttribParams::IsActive

inline st_bool CGeometryBufferOpenGL::SAttribParams::IsActive(void) const
{
	return (m_nOffset > -1);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::CGeometryBufferOpenGL

inline CGeometryBufferOpenGL::CGeometryBufferOpenGL( ) :
	m_uiVertexBuffer(0),
	m_uiIndexBuffer(0),
	m_eIndexFormat(INDEX_FORMAT_UNSIGNED_32BIT)
{
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::~CGeometryBufferOpenGL

inline CGeometryBufferOpenGL::~CGeometryBufferOpenGL( )
{
	if (VertexBufferIsValid( ))
		glDeleteBuffersARB(1, &m_uiVertexBuffer);

	if (IndexBufferIsValid( ))
		glDeleteBuffersARB(1, &m_uiIndexBuffer);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::CreateVertexBuffer

inline st_bool CGeometryBufferOpenGL::CreateVertexBuffer(st_bool bDynamic, const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiVertexSize)
{
	st_bool bSuccess = false;

	if (pVertexData && uiNumVertices > 0 && uiVertexSize > 0)
	{
		CRenderStateOpenGL::Lock( );
		glGenBuffersARB(1, &m_uiVertexBuffer);

		// make sure the generation was okay
		if (VertexBufferIsValid( ))
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_uiVertexBuffer);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, uiNumVertices * uiVertexSize, pVertexData, bDynamic ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB);

			// unbind this buffer
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

			bSuccess = true;
		}

		CRenderStateOpenGL::Unlock( );
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::OverwriteVertices

inline st_bool CGeometryBufferOpenGL::OverwriteVertices(const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiVertexSize, st_uint32 uiOffset)
{
	// should have been taken care of in CGeometryBufferRI::OverwriteVertices, but be sure
	assert(pVertexData);
	assert(uiNumVertices > 0);
	assert(VertexBufferIsValid( ));

	CRenderStateOpenGL::Lock( );

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_uiVertexBuffer);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, uiOffset * uiVertexSize, uiNumVertices * uiVertexSize, pVertexData);

	// unbind this buffer
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	CRenderStateOpenGL::Unlock( );

	return true; // validity checks are performed in CGeometryBufferRI::OverwriteVertices
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::VertexBufferIsValid

inline st_bool CGeometryBufferOpenGL::VertexBufferIsValid(void) const
{
	return (m_uiVertexBuffer != 0);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::EnableFormat

inline st_bool CGeometryBufferOpenGL::EnableFormat(void) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (IsVertexFormatSet( ))
	{
#endif

		// save current state to restore later in DisableFormat()
		glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

		// enable each vertex attribute set
		for (st_uint32 i = st_uint32(VERTEX_ATTRIB_SEMANTIC_POS); i < st_uint32(VERTEX_ATTRIB_SEMANTIC_COUNT); ++i)
		{
			if (m_asAttribParams[i].IsActive( ))
				glEnableVertexAttribArray(i);
			else
				glDisableVertexAttribArray(i);
		}

		bSuccess = true;

#ifndef NDEBUG
	}
	else
		CCore::SetError("CGeometryBufferOpenGL::EnableFormat, format has not been set yet");
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::DisableFormat

inline st_bool CGeometryBufferOpenGL::DisableFormat(void)
{
	glPopClientAttrib( );

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::UnBindVertexBuffer

inline st_bool CGeometryBufferOpenGL::UnBindVertexBuffer(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::SetIndexFormat

inline st_bool CGeometryBufferOpenGL::SetIndexFormat(EIndexFormat eFormat)
{
	m_eIndexFormat = eFormat;

	return (m_eIndexFormat == INDEX_FORMAT_UNSIGNED_16BIT || m_eIndexFormat == INDEX_FORMAT_UNSIGNED_32BIT);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::CreateIndexBuffer

inline st_bool CGeometryBufferOpenGL::CreateIndexBuffer(st_bool bDynamic, const void* pIndexData, st_uint32 uiNumIndices)
{
	st_bool bSuccess = false;

	if (pIndexData && uiNumIndices > 0)
	{
		CRenderStateOpenGL::Lock( );
		glGenBuffersARB(1, &m_uiIndexBuffer);

		if (IndexBufferIsValid( ))
		{
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_uiIndexBuffer);
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GLsizeiptrARB(uiNumIndices * IndexSize( )), pIndexData, bDynamic ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB);

			// unbind this buffer
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

			bSuccess = true;
		}

		CRenderStateOpenGL::Unlock( );
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::OverwriteIndices

inline st_bool CGeometryBufferOpenGL::OverwriteIndices(const void* pIndexData, st_uint32 uiNumIndices, st_uint32 uiOffset)
{
	st_bool bSuccess = false;

	if (pIndexData && uiNumIndices > 0)
	{
		if (IndexBufferIsValid( ))
		{
			CRenderStateOpenGL::Lock( );

			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_uiIndexBuffer);
			glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GLint(uiOffset * IndexSize( )), GLsizeiptrARB(uiNumIndices * IndexSize( )), pIndexData);

			// unbind this buffer
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

			CRenderStateOpenGL::Unlock( );

			bSuccess = true;
		}
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::IndexBufferIsValid

inline st_bool CGeometryBufferOpenGL::IndexBufferIsValid(void) const
{
	return (m_uiIndexBuffer != 0);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::IndexSize

inline st_uint32 CGeometryBufferOpenGL::IndexSize(void) const
{
	return (m_eIndexFormat == INDEX_FORMAT_UNSIGNED_16BIT) ? 2 : 4;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::ClearIndices

inline st_bool CGeometryBufferOpenGL::ClearIndices(void)
{
	if (IndexBufferIsValid( ))
	{
		glDeleteBuffersARB(1, &m_uiIndexBuffer);
		m_uiIndexBuffer = 0;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::BindIndexBuffer

inline st_bool CGeometryBufferOpenGL::BindIndexBuffer(void) const
{
	// CGeometryBufferRI class checks if the index buffer is valid before calling
	// BindIndexBuffer; no need to check twice
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_uiIndexBuffer);

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::UnBindIndexBuffer

inline st_bool CGeometryBufferOpenGL::UnBindIndexBuffer(void) const
{
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::RenderIndexed

inline st_bool CGeometryBufferOpenGL::RenderIndexed(EPrimitiveType ePrimType, 
													st_uint32 uiStartIndex, 
													st_uint32 uiNumIndices, 
													st_uint32 /*uiMinIndex*/, // DX9
													st_uint32 /*uiNumVertices*/, // DX9
													EGeometryProcessingType /*eProcessingType*/) const // PS3
{
	st_bool bSuccess = false;

	if (uiNumIndices > 0)
	{
		// determine the right opengl primitive type
		GLenum eOpenGLPrimitive = GL_TRIANGLE_STRIP;
		switch (ePrimType)
		{
		case PRIMITIVE_QUADS:
			eOpenGLPrimitive = GL_QUADS;
			break;
		case PRIMITIVE_TRIANGLES:
			eOpenGLPrimitive = GL_TRIANGLES;
			break;
		case PRIMITIVE_POINTS:
			eOpenGLPrimitive = GL_POINTS;
			break;
		case PRIMITIVE_LINE_STRIP:
			eOpenGLPrimitive = GL_LINE_STRIP;
			break;
		case PRIMITIVE_LINE_LOOP:
			eOpenGLPrimitive = GL_LINE_LOOP;
			break;
		case PRIMITIVE_LINES:
			eOpenGLPrimitive = GL_LINES;
			break;
		case PRIMITIVE_TRIANGLE_FAN:
			eOpenGLPrimitive = GL_TRIANGLE_FAN;
			break;
		case PRIMITIVE_QUAD_STRIP:
			eOpenGLPrimitive = GL_QUAD_STRIP;
			break;
		case PRIMITIVE_TRIANGLE_STRIP:
		default:
			break;
		}

		const st_byte* pBase = NULL;
		if (m_eIndexFormat == INDEX_FORMAT_UNSIGNED_16BIT)
			glDrawElements(eOpenGLPrimitive, uiNumIndices, GL_UNSIGNED_SHORT, pBase + uiStartIndex * sizeof(st_uint16));
		else
			glDrawElements(eOpenGLPrimitive, uiNumIndices, GL_UNSIGNED_INT, pBase + uiStartIndex * sizeof(st_uint32));

		bSuccess = true;
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::RenderArrays

inline st_bool CGeometryBufferOpenGL::RenderArrays(EPrimitiveType ePrimType, st_uint32 uiStartVertex, st_uint32 uiNumVertices, EGeometryProcessingType /*eProcessingType*/) const
{
	st_bool bSuccess = false;

#ifndef NDEBUG
	if (VertexBufferIsValid( ))
	{
#endif
		GLenum eOpenGLPrimitive = GL_TRIANGLE_STRIP;
		switch (ePrimType)
		{
		case PRIMITIVE_QUADS:
			eOpenGLPrimitive = GL_QUADS;
			break;
		case PRIMITIVE_TRIANGLES:
			eOpenGLPrimitive = GL_TRIANGLES;
			break;
		case PRIMITIVE_POINTS:
			eOpenGLPrimitive = GL_POINTS;
			break;
		case PRIMITIVE_LINE_STRIP:
			eOpenGLPrimitive = GL_LINE_STRIP;
			break;
		case PRIMITIVE_LINE_LOOP:
			eOpenGLPrimitive = GL_LINE_LOOP;
			break;
		case PRIMITIVE_LINES:
			eOpenGLPrimitive = GL_LINES;
			break;
		case PRIMITIVE_TRIANGLE_FAN:
			eOpenGLPrimitive = GL_TRIANGLE_FAN;
			break;
		case PRIMITIVE_QUAD_STRIP:
			eOpenGLPrimitive = GL_QUAD_STRIP;
			break;
		default:
			// let GL_TRIANGLE_STRIP fall through
			break;
		}

		glDrawArrays(eOpenGLPrimitive, uiStartVertex, uiNumVertices);

		bSuccess = true;
#ifndef NDEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::IsPrimitiveTypeSupported

inline st_bool CGeometryBufferOpenGL::IsPrimitiveTypeSupported(EPrimitiveType ePrimType)
{
	return (ePrimType == PRIMITIVE_QUADS ||
			ePrimType == PRIMITIVE_TRIANGLES ||
			ePrimType == PRIMITIVE_POINTS ||
			ePrimType == PRIMITIVE_LINE_STRIP ||
			ePrimType == PRIMITIVE_LINE_LOOP ||
			ePrimType == PRIMITIVE_LINES ||
			ePrimType == PRIMITIVE_TRIANGLE_FAN ||
			ePrimType == PRIMITIVE_QUADS ||
			ePrimType == PRIMITIVE_QUAD_STRIP);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferOpenGL::IsVertexFormatSet

inline st_bool CGeometryBufferOpenGL::IsVertexFormatSet(void) const
{
	return m_asAttribParams[VERTEX_ATTRIB_SEMANTIC_POS].IsActive( );
}
