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
//  CGeometryBufferDirectX10::CGeometryBufferDirectX10

inline CGeometryBufferDirectX10::CGeometryBufferDirectX10( ) :
	m_pVertexLayout(NULL),
	m_pVertexBuffer(NULL),
	m_pIndexBuffer(NULL),
	m_eIndexFormat(INDEX_FORMAT_UNSIGNED_32BIT),
	m_uiCurrentVertexBufferSize(0),
	m_uiCurrentIndexBufferSize(0)
{
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::~CGeometryBufferDirectX10

inline CGeometryBufferDirectX10::~CGeometryBufferDirectX10( )
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

	if (m_pVertexLayout)
	{
		m_pVertexLayout->Release( );
		m_pVertexLayout = NULL;
	}
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::CreateVertexBuffer

inline st_bool CGeometryBufferDirectX10::CreateVertexBuffer(st_bool bDynamic, const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiVertexSize)
{
	st_bool bSuccess = false;

	if (pVertexData && uiNumVertices > 0 && uiVertexSize > 0)
	{
		assert(DX10::Device( ));

		D3D10_BUFFER_DESC sBufferDesc;
		sBufferDesc.ByteWidth = uiNumVertices * uiVertexSize;
		sBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		sBufferDesc.MiscFlags = 0;
		if (bDynamic)
		{
			// We rarely replace the entire vertex buffer (we replace just a small section of billboards at
			// a time). So, to avoid having to use D3D10_MAP_WRITE_DISCARD, we use UpdateSubresource. That
			// function, however, requires a buffer with default usage rather than dynamic.
			sBufferDesc.Usage = D3D10_USAGE_DEFAULT;
			sBufferDesc.CPUAccessFlags = 0;
		}
		else
		{
			sBufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
			sBufferDesc.CPUAccessFlags = 0;
		}

		D3D10_SUBRESOURCE_DATA sInitData;
		sInitData.pSysMem = pVertexData;

		if (DX10::Device( )->CreateBuffer(&sBufferDesc, &sInitData, &m_pVertexBuffer) == S_OK)
		{
			m_uiCurrentVertexBufferSize = uiNumVertices * uiVertexSize;
			bSuccess = true;
		}
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::OverwriteVertices

inline st_bool CGeometryBufferDirectX10::OverwriteVertices(const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiVertexSize, st_uint32 uiOffset)
{
	st_bool bSuccess = false;

	// should have been taken care of in CGeometryBufferRI::ReplaceVertices, but be sure
	assert(pVertexData);
	assert(uiNumVertices > 0);
	assert(VertexBufferIsValid( ));

	// Use UpdateSubresource since we'll probably only be updating a small section of the buffer
	D3D10_BOX boxUpdate;
	boxUpdate.left = uiOffset * uiVertexSize;
	boxUpdate.top = 0;
	boxUpdate.front = 0;
	boxUpdate.right = boxUpdate.left + uiNumVertices * uiVertexSize;
	boxUpdate.bottom = 1;
	boxUpdate.back = 1;
	DX10::Device( )->UpdateSubresource(m_pVertexBuffer, 0, &boxUpdate, pVertexData, 0, 0);

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::VertexBufferIsValid

inline st_bool CGeometryBufferDirectX10::VertexBufferIsValid(void) const
{
	return (m_pVertexBuffer != NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::EnableFormat

inline st_bool CGeometryBufferDirectX10::EnableFormat(void) const
{
	assert(DX10::Device( ));

	DX10::Device( )->IASetInputLayout(m_pVertexLayout);

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::DisableFormat

inline st_bool CGeometryBufferDirectX10::DisableFormat(void)
{
	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::BindVertexBuffer

inline st_bool CGeometryBufferDirectX10::BindVertexBuffer(st_int32 nVertexSize, EGeometryProcessingType /*eProcessingType*/) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (VertexBufferIsValid( ) && IsFormatSet( ))
	{
#endif
		assert(DX10::Device( ));

		st_uint32 uiStride = nVertexSize;
		st_uint32 uiOffset = 0;
		DX10::Device( )->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &uiStride, &uiOffset);
		bSuccess = true;

#ifdef _DEBUG
	}
	else
		CCore::SetError("CGeometryBufferDirectX10::BindVertexBuffer, vertex buffer is not valid, cannot bind");
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::UnBindVertexBuffer

inline st_bool CGeometryBufferDirectX10::UnBindVertexBuffer(void)
{
	assert(DX10::Device( ));

	//ID3D10Buffer* aNullBuffers[] = { NULL };
	//UINT uiNULL = 0;
	//DX10::Device( )->IASetVertexBuffers(0, 1, aNullBuffers, &uiNULL, &uiNULL);

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::SetIndexFormat

inline st_bool CGeometryBufferDirectX10::SetIndexFormat(EIndexFormat eFormat)
{
	m_eIndexFormat = eFormat;

	return (m_eIndexFormat == INDEX_FORMAT_UNSIGNED_16BIT || m_eIndexFormat == INDEX_FORMAT_UNSIGNED_32BIT);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::CreateIndexBuffer

inline st_bool CGeometryBufferDirectX10::CreateIndexBuffer(st_bool bDynamic, const void* pIndexData, st_uint32 uiNumIndices)
{
	st_bool bSuccess = false;

	assert(pIndexData);
	assert(uiNumIndices > 0);
	assert(DX10::Device( ));
	st_uint32 c_uiNumBytes = uiNumIndices * IndexSize( );

	D3D10_BUFFER_DESC sBufferDesc;
	sBufferDesc.ByteWidth = c_uiNumBytes;
	sBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	sBufferDesc.MiscFlags = 0;
	if (bDynamic)
	{
		// Unlike the vertex buffer, the index buffer is updated as a whole. Therefore,
		// we can use the faster dynamic usage and D3D10_MAP_WRITE_DISCARD
		sBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		sBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	}
	else
	{
		sBufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
		sBufferDesc.CPUAccessFlags = 0;
	}

	D3D10_SUBRESOURCE_DATA sInitData;
	sInitData.pSysMem = pIndexData;

	if (DX10::Device( )->CreateBuffer(&sBufferDesc, &sInitData, &m_pIndexBuffer) == S_OK)
	{
		m_uiCurrentIndexBufferSize = c_uiNumBytes;
		bSuccess = true;
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::OverwriteIndices

inline st_bool CGeometryBufferDirectX10::OverwriteIndices(const void* pIndexData, st_uint32 uiNumIndices, st_uint32 uiOffset)
{
	st_bool bSuccess = false;

	// should have been taken care of in CGeometryBufferRI::ReplaceVertices, but be sure
	assert(pIndexData);
	assert(uiNumIndices > 0);
	assert(IndexBufferIsValid( ));

	// lock and fill the buffer
	void* pIndexBufferContents = NULL;
	const st_uint32 c_uiOffsetInBytes = uiOffset * IndexSize( );
	const st_uint32 c_uiSizeInBytes = uiNumIndices * IndexSize( );
	assert(c_uiOffsetInBytes + c_uiSizeInBytes <= m_uiCurrentIndexBufferSize);
	if (m_pIndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast<void**>(&pIndexBufferContents)) == S_OK)
	{
		memcpy((st_byte*)pIndexBufferContents + c_uiOffsetInBytes, pIndexData, c_uiSizeInBytes);
		m_pIndexBuffer->Unmap( );

		bSuccess = true;
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::ClearIndices

inline st_bool CGeometryBufferDirectX10::ClearIndices(void)
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
//  CGeometryBufferDirectX10::IndexBufferIsValid

inline st_bool CGeometryBufferDirectX10::IndexBufferIsValid(void) const
{
	return (m_pIndexBuffer != NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::BindIndexBuffer

inline st_bool CGeometryBufferDirectX10::BindIndexBuffer(void) const
{
	assert(DX10::Device( ));

	// CGeometryBufferRI class checks if the index buffer is valid before calling
	// BindIndexBuffer; no need to check twice
	DX10::Device( )->IASetIndexBuffer(m_pIndexBuffer, 
										(m_eIndexFormat == INDEX_FORMAT_UNSIGNED_16BIT ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT), 
										0);

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::UnBindIndexBuffer

inline st_bool CGeometryBufferDirectX10::UnBindIndexBuffer(void) const
{
	assert(DX10::Device( ));

	//DX10::Device( )->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);

	return true;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::RenderIndexed

inline st_bool CGeometryBufferDirectX10::RenderIndexed(EPrimitiveType ePrimType, 
													   st_uint32 uiStartIndex, 
													   st_uint32 uiNumIndices,
													   st_uint32 /*uiMinIndex*/,		// DX9
													   st_uint32 /*uiNumVertices*/,		// DX9
													   EGeometryProcessingType /*eProcessingType*/) const // PS3
{
	st_bool bSuccess = false;

	if (uiNumIndices > 0)
	{
		D3D10_PRIMITIVE_TOPOLOGY eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
		switch (ePrimType)
		{
		case PRIMITIVE_TRIANGLE_STRIP:
			eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		case PRIMITIVE_TRIANGLES:
			eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case PRIMITIVE_POINTS:
			eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;
		case PRIMITIVE_LINE_STRIP:
			eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
			break;
		case PRIMITIVE_LINES:
			eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
			break;
		case PRIMITIVE_TRIANGLE_FAN:
			CCore::SetError("CGeometryBufferDirectX10::RenderIndexed, PRIMITIVE_TRIANGLE_FAN type is not supported under the DirectX10 render interface");
			break;
		case PRIMITIVE_QUAD_STRIP:
			CCore::SetError("CGeometryBufferDirectX10::RenderIndexed, PRIMITIVE_QUAD_STRIP type is not supported under the DirectX10 render interface");
			break;
		case PRIMITIVE_QUADS:
			CCore::SetError("CGeometryBufferDirectX10::RenderIndexed, PRIMITIVE_QUADS type is not supported under the DirectX10 render interface");
			break;
		case PRIMITIVE_LINE_LOOP:
			CCore::SetError("CGeometryBufferDirectX10::RenderIndexed, PRIMITIVE_LINE_LOOP type is not supported under the DirectX10 render interface");
			break;
		default:
			assert(false);
		}

		if (eDirectX10Primitive != D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED)
		{
			assert(DX10::Device( ));
			DX10::Device( )->IASetPrimitiveTopology(eDirectX10Primitive);
			DX10::Device( )->DrawIndexed(uiNumIndices, uiStartIndex, 0);
			bSuccess = true;
		}
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::RenderArrays

inline st_bool CGeometryBufferDirectX10::RenderArrays(EPrimitiveType ePrimType, st_uint32 uiStartVertex, st_uint32 uiNumVertices, EGeometryProcessingType /*eProcessingType*/) const
{
	st_bool bSuccess = false;

#ifdef _DEBUG
	if (VertexBufferIsValid( ))
	{
#endif
		D3D10_PRIMITIVE_TOPOLOGY eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
		switch (ePrimType)
		{
		case PRIMITIVE_TRIANGLE_STRIP:
			eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		case PRIMITIVE_TRIANGLES:
			eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case PRIMITIVE_POINTS:
			eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;
		case PRIMITIVE_LINE_STRIP:
			eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
			break;
		case PRIMITIVE_LINES:
			eDirectX10Primitive = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
			break;
		case PRIMITIVE_TRIANGLE_FAN:
			CCore::SetError("CGeometryBufferDirectX10::RenderArrays, PRIMITIVE_TRIANGLE_FAN type is not supported under the DirectX10 render interface");
			break;
		case PRIMITIVE_QUAD_STRIP:
			CCore::SetError("CGeometryBufferDirectX10::RenderArrays, PRIMITIVE_QUAD_STRIP type is not supported under the DirectX10 render interface");
			break;
		case PRIMITIVE_QUADS:
			CCore::SetError("CGeometryBufferDirectX10::RenderArrays, PRIMITIVE_QUADS type is not supported under the DirectX10 render interface");
			break;
		case PRIMITIVE_LINE_LOOP:
			CCore::SetError("CGeometryBufferDirectX10::RenderArrays, PRIMITIVE_LINE_LOOP type is not supported under the DirectX10 render interface");
			break;
		default:
			assert(false);
		}

		if (eDirectX10Primitive != D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED)
		{
			assert(DX10::Device( ));
			DX10::Device( )->IASetPrimitiveTopology(eDirectX10Primitive);
			DX10::Device( )->Draw(uiNumVertices, uiStartVertex);
			bSuccess = true;
		}

#ifdef _DEBUG
	}
#endif

	return bSuccess;
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::IsPrimitiveTypeSupported

inline st_bool CGeometryBufferDirectX10::IsPrimitiveTypeSupported(EPrimitiveType ePrimType)
{
	return (ePrimType == PRIMITIVE_TRIANGLE_STRIP ||
			ePrimType == PRIMITIVE_TRIANGLES);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::IsFormatSet

inline st_bool CGeometryBufferDirectX10::IsFormatSet(void) const
{
	return (m_pVertexLayout != NULL);
}


///////////////////////////////////////////////////////////////////////  
//  CGeometryBufferDirectX10::IndexSize

inline st_uint32 CGeometryBufferDirectX10::IndexSize(void) const
{
	return (m_eIndexFormat == INDEX_FORMAT_UNSIGNED_16BIT) ? 2 : 4;
}

