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


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::CGeometryBufferDirectX9

inline CGeometryBufferDirectX9::CGeometryBufferDirectX9( ) :
	m_pVertexDeclaration(NULL),
	m_pVertexBuffer(NULL),
	m_pIndexBuffer(NULL),
	m_eIndexFormat(INDEX_FORMAT_UNSIGNED_32BIT),
	m_uiCurrentVertexBufferSize(0),
	m_uiCurrentIndexBufferSize(0)
{
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::~CGeometryBufferDirectX9

inline CGeometryBufferDirectX9::~CGeometryBufferDirectX9( )
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release( );
		m_pVertexBuffer = NULL;
	}

	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release( );
		m_pIndexBuffer = NULL;
	}

	if (m_pVertexDeclaration)
	{
		m_pVertexDeclaration->Release( );
		m_pVertexDeclaration = NULL;
	}
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::CreateVertexBuffer

inline st_bool CGeometryBufferDirectX9::CreateVertexBuffer(st_bool /*bDynamic*/, const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiVertexSize)
{
	st_bool bSuccess = false;

	if (pVertexData && uiNumVertices > 0 && uiVertexSize > 0)
	{
		assert(DX9::Device( ));

		const st_uint32 c_uiNumBytes = uiNumVertices * uiVertexSize;

		// setup vertex buffer - testing showed that with the infrequent updates made by the SpeedTree,
		// the best performance came from using mostly default flags and not D3DUSAGE_DYNAMIC.
		//const DWORD c_dwUsageFlags = D3DUSAGE_WRITEONLY | (bDynamic ? D3DUSAGE_DYNAMIC : 0);
		//const D3DPOOL c_ePool = bDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		const DWORD c_dwUsageFlags = D3DUSAGE_WRITEONLY;
		const D3DPOOL c_ePool = D3DPOOL_MANAGED;
		if (DX9::Device( )->CreateVertexBuffer(c_uiNumBytes, c_dwUsageFlags, 0, c_ePool, &m_pVertexBuffer, NULL) == D3D_OK)
		{
			assert(m_pVertexBuffer);

			m_uiCurrentVertexBufferSize = c_uiNumBytes;
			bSuccess = OverwriteVertices(pVertexData, uiNumVertices, uiVertexSize, 0);
		}
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::OverwriteVertices

inline st_bool CGeometryBufferDirectX9::OverwriteVertices(const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiVertexSize, st_uint32 uiOffset)
{
	st_bool bSuccess = false;

	// should have been taken care of in CGeometryBufferRI::ReplaceVertices, but be sure
	assert(pVertexData);
	assert(uiNumVertices > 0);
	assert(VertexBufferIsValid( ));

	// make sure the buffer's not bound when updating
	if (DX9::Device( )->SetStreamSource(0, NULL, 0, 0) == D3D_OK)
	{
		// lock and fill the buffer
		void* pVertexBufferContents = NULL;
		const st_uint32 c_uiOffsetInBytes = uiOffset * uiVertexSize;
		const st_uint32 c_uiSizeInBytes = uiNumVertices * uiVertexSize;
		assert(c_uiOffsetInBytes + c_uiSizeInBytes <= m_uiCurrentVertexBufferSize);

		if (m_pVertexBuffer->Lock(c_uiOffsetInBytes, c_uiSizeInBytes, reinterpret_cast<void**>(&pVertexBufferContents), 0) == D3D_OK)
		{
			memcpy(pVertexBufferContents, pVertexData, c_uiSizeInBytes);

			// buffer update complete, release the lock
			bSuccess = (m_pVertexBuffer->Unlock( ) == D3D_OK);
		}
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::VertexBufferIsValid

inline st_bool CGeometryBufferDirectX9::VertexBufferIsValid(void) const
{
	return (m_pVertexBuffer != NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::EnableFormat

inline st_bool CGeometryBufferDirectX9::EnableFormat(void) const
{
	assert(DX9::Device( ));

	return (DX9::Device( )->SetVertexDeclaration(m_pVertexDeclaration) == D3D_OK);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::DisableFormat

inline st_bool CGeometryBufferDirectX9::DisableFormat(void)
{
	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::BindVertexBuffer

inline st_bool CGeometryBufferDirectX9::BindVertexBuffer(st_int32 nVertexSize, EGeometryProcessingType /*eProcessingType*/) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (VertexBufferIsValid( ) && IsFormatSet( ))
	{
#endif
		assert(DX9::Device( ));
		bSuccess = (DX9::Device( )->SetStreamSource(0, m_pVertexBuffer, 0, nVertexSize) == D3D_OK);
#ifdef _DEBUG
	}
	else
		CCore::SetError("CGeometryBufferDirectX9::BindVertexBuffer, vertex buffer is not valid, cannot bind");
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::UnBindVertexBuffer

inline st_bool CGeometryBufferDirectX9::UnBindVertexBuffer(void)
{
	assert(DX9::Device( ));

	return (DX9::Device( )->SetStreamSource(0, NULL, 0, 0) == D3D_OK);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::SetIndexFormat

inline st_bool CGeometryBufferDirectX9::SetIndexFormat(EIndexFormat eFormat)
{
	m_eIndexFormat = eFormat;

	return (m_eIndexFormat == INDEX_FORMAT_UNSIGNED_16BIT || m_eIndexFormat == INDEX_FORMAT_UNSIGNED_32BIT);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::CreateIndexBuffer

inline st_bool CGeometryBufferDirectX9::CreateIndexBuffer(st_bool /*bDynamic*/, const void* pIndexData, st_uint32 uiNumIndices)
{
	st_bool bSuccess = false;

	assert(pIndexData);
	assert(uiNumIndices > 0);
	assert(DX9::Device( ));

	st_uint32 c_uiNumBytes = uiNumIndices * IndexSize( );

	// setup index buffer - testing showed that with the infrequent updates made by the SpeedTree,
	// the best performance came from using mostly default flags and not D3DUSAGE_DYNAMIC.
	//const DWORD c_dwUsageFlags = D3DUSAGE_WRITEONLY | (bDynamic ? D3DUSAGE_DYNAMIC : 0);
	//const D3DPOOL c_ePool = bDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
	const DWORD c_dwUsageFlags = D3DUSAGE_WRITEONLY;
	const D3DPOOL c_ePool = D3DPOOL_MANAGED;
	if (DX9::Device( )->CreateIndexBuffer(c_uiNumBytes, c_dwUsageFlags, m_eIndexFormat == INDEX_FORMAT_UNSIGNED_16BIT ? D3DFMT_INDEX16 : D3DFMT_INDEX32, c_ePool, &m_pIndexBuffer, NULL) == D3D_OK)
	{
		assert(m_pIndexBuffer);

		m_uiCurrentIndexBufferSize = c_uiNumBytes;
		bSuccess = OverwriteIndices(pIndexData, uiNumIndices, 0);
	}

	if (bSuccess)
		m_uiCurrentIndexBufferSize = c_uiNumBytes;	

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::OverwriteIndices

inline st_bool CGeometryBufferDirectX9::OverwriteIndices(const void* pIndexData, st_uint32 uiNumIndices, st_uint32 uiOffset)
{
	st_bool bSuccess = false;

	// should have been taken care of in CGeometryBufferRI::ReplaceVertices, but be sure
	assert(pIndexData);
	assert(uiNumIndices > 0);
	assert(IndexBufferIsValid( ));

	// make sure the buffer's not bound when updating
	if (DX9::Device( )->SetIndices(NULL) == D3D_OK)
	{
		// lock and fill the buffer
		void* pIndexBufferContents = NULL;
		const st_uint32 c_uiOffsetInBytes = uiOffset * IndexSize( );
		const st_uint32 c_uiSizeInBytes = uiNumIndices * IndexSize( );
		assert(c_uiOffsetInBytes + c_uiSizeInBytes <= m_uiCurrentIndexBufferSize);

		if (m_pIndexBuffer->Lock(c_uiOffsetInBytes, c_uiSizeInBytes, reinterpret_cast<void**>(&pIndexBufferContents), 0) == D3D_OK)
		{
			memcpy(pIndexBufferContents, pIndexData, c_uiSizeInBytes);

			// buffer update complete, release the lock
			bSuccess = (m_pIndexBuffer->Unlock( ) == D3D_OK);
		}
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::IndexBufferIsValid

inline st_bool CGeometryBufferDirectX9::IndexBufferIsValid(void) const
{
	return (m_pIndexBuffer != NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::IndexSize

inline st_uint32 CGeometryBufferDirectX9::IndexSize(void) const
{
	return (m_eIndexFormat == INDEX_FORMAT_UNSIGNED_16BIT) ? 2 : 4;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::ClearIndices

inline st_bool CGeometryBufferDirectX9::ClearIndices(void)
{
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release( );
		m_pIndexBuffer = NULL;
	}

	m_uiCurrentIndexBufferSize = 0;

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::BindIndexBuffer

inline st_bool CGeometryBufferDirectX9::BindIndexBuffer(void) const
{
	assert(DX9::Device( ));

	// CGeometryBufferRI class checks if the index buffer is valid before calling
	// BindIndexBuffer; no need to check twice
	return (DX9::Device( )->SetIndices(m_pIndexBuffer) == D3D_OK);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::UnBindIndexBuffer

inline st_bool CGeometryBufferDirectX9::UnBindIndexBuffer(void) const
{
	assert(DX9::Device( ));

	return (DX9::Device( )->SetIndices(NULL) == D3D_OK);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::RenderIndexed

inline st_bool CGeometryBufferDirectX9::RenderIndexed(EPrimitiveType ePrimType, 
													  st_uint32 uiStartIndex, 
													  st_uint32 uiNumIndices, 
													  st_uint32 uiMinIndex,
													  st_uint32 uiNumVertices,
													  EGeometryProcessingType /*eProcessingType*/) const
{
	st_bool bSuccess = false;

	if (uiNumIndices > 0)
	{
		st_bool bValidPrimitiveType = true;

		st_int32 nNumPrimitiveElements = 1;
		D3DPRIMITIVETYPE eDirectX9Primitive = D3DPT_POINTLIST;
		switch (ePrimType)
		{
		case PRIMITIVE_TRIANGLE_STRIP:
			nNumPrimitiveElements = uiNumIndices - 2;
			eDirectX9Primitive = D3DPT_TRIANGLESTRIP;
			break;
		case PRIMITIVE_TRIANGLES:
			nNumPrimitiveElements = uiNumIndices / 3;
			eDirectX9Primitive = D3DPT_TRIANGLELIST;
			break;
		case PRIMITIVE_LINE_STRIP:
			//eDirectX9Primitive = D3DPT_LINESTRIP;
			CCore::SetError("CGeometryBufferDirectX9::RenderIndexed, PRIMITIVE_LINE_STRIP type is not supported under the DirectX9 render interface");
			break;
		case PRIMITIVE_LINES:
			//eDirectX9Primitive = D3DPT_LINELIST;
			CCore::SetError("CGeometryBufferDirectX9::RenderIndexed, PRIMITIVE_LINES type is not supported under the DirectX9 render interface");
			break;
		case PRIMITIVE_TRIANGLE_FAN:
			//eDirectX9Primitive = D3DPT_TRIANGLEFAN;
			CCore::SetError("CGeometryBufferDirectX9::RenderIndexed, PRIMITIVE_TRIANGLE_FAN type is not supported under the DirectX9 render interface");
			break;
		case PRIMITIVE_QUAD_STRIP:
			CCore::SetError("CGeometryBufferDirectX9::RenderIndexed, PRIMITIVE_QUAD_STRIP type is not supported under the DirectX9 render interface");
			bValidPrimitiveType = false;
			break;
		case PRIMITIVE_QUADS:
			CCore::SetError("CGeometryBufferDirectX9::RenderIndexed, PRIMITIVE_QUADS type is not supported under the DirectX9 render interface");
			bValidPrimitiveType = false;
			break;
		case PRIMITIVE_LINE_LOOP:
			CCore::SetError("CGeometryBufferDirectX9::RenderIndexed, PRIMITIVE_LINE_LOOP type is not supported under the DirectX9 render interface");
			bValidPrimitiveType = false;
			break;
		default:
			assert(false);
		}

		if (bValidPrimitiveType)
		{
			assert(DX9::Device( ));
			assert(uiStartIndex + uiNumIndices <= m_uiCurrentIndexBufferSize);

			bSuccess = (DX9::Device( )->DrawIndexedPrimitive(eDirectX9Primitive, 0, uiMinIndex, uiNumVertices, uiStartIndex, nNumPrimitiveElements) == D3D_OK);
		}
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::RenderArrays

inline st_bool CGeometryBufferDirectX9::RenderArrays(EPrimitiveType ePrimType, st_uint32 uiStartVertex, st_uint32 uiNumVertices, EGeometryProcessingType /*eProcessingType*/) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (VertexBufferIsValid( ))
	{
#endif
		st_bool bValidPrimitiveType = true;

		int nNumPrimitiveElements = 1;
		D3DPRIMITIVETYPE eDirectX9Primitive = D3DPT_POINTLIST;
		switch (ePrimType)
		{
		case PRIMITIVE_TRIANGLE_STRIP:
			eDirectX9Primitive = D3DPT_TRIANGLESTRIP;
			nNumPrimitiveElements = uiNumVertices - 2;
			break;
		case PRIMITIVE_TRIANGLES:
			eDirectX9Primitive = D3DPT_TRIANGLELIST;
			nNumPrimitiveElements = uiNumVertices / 3;
			break;
		case PRIMITIVE_LINE_STRIP:
			eDirectX9Primitive = D3DPT_LINESTRIP;
			nNumPrimitiveElements = uiNumVertices - 1;
			break;
		case PRIMITIVE_LINES:
			eDirectX9Primitive = D3DPT_LINELIST;
			nNumPrimitiveElements = uiNumVertices / 2;
			break;
		case PRIMITIVE_TRIANGLE_FAN:
			eDirectX9Primitive = D3DPT_TRIANGLEFAN;
			nNumPrimitiveElements = uiNumVertices - 2;
			break;
		case PRIMITIVE_POINTS:
			eDirectX9Primitive = D3DPT_POINTLIST;
			nNumPrimitiveElements = uiNumVertices;
			break;
		case PRIMITIVE_QUAD_STRIP:
			CCore::SetError("CGeometryBufferDirectX9::RenderArrays, PRIMITIVE_QUAD_STRIP type is not supported under the DirectX9 render interface");
			bValidPrimitiveType = false;
			break;
		case PRIMITIVE_QUADS:
			CCore::SetError("CGeometryBufferDirectX9::RenderArrays, PRIMITIVE_QUADS type is not supported under the DirectX9 render interface");
			bValidPrimitiveType = false;
			break;
		case PRIMITIVE_LINE_LOOP:
			CCore::SetError("CGeometryBufferDirectX9::RenderArrays, PRIMITIVE_LINE_LOOP type is not supported under the DirectX9 render interface");
			bValidPrimitiveType = false;
			break;
		default:
			// let D3DPT_POINTLIST fall through
			break;
		}

		if (bValidPrimitiveType)
		{
			assert(DX9::Device( ));
			bSuccess = (DX9::Device( )->DrawPrimitive(eDirectX9Primitive, uiStartVertex, nNumPrimitiveElements) == D3D_OK);
		}

#ifdef _DEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::IsPrimitiveTypeSupported

inline st_bool CGeometryBufferDirectX9::IsPrimitiveTypeSupported(EPrimitiveType ePrimType)
{
	return (ePrimType == PRIMITIVE_TRIANGLE_STRIP ||
			ePrimType == PRIMITIVE_TRIANGLES);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX9::IsFormatSet

inline st_bool CGeometryBufferDirectX9::IsFormatSet(void) const
{
	return (m_pVertexDeclaration != NULL);
}


