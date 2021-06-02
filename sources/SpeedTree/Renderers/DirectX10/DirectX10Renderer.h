///////////////////////////////////////////////////////////////////////  
//  DirectX10Renderer.h
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
//  Preprocessor

#pragma once
#define VC_EXTRALEAN
#include <d3d10.h>
#include <d3dx10.h>
#include <dxerr.h>
#include "RenderInterface/ForestRI.h"
#include "RenderInterface/TerrainRI.h"
#include "RenderInterface/GrassRI.h"
#include "RenderInterface/SkyRI.h"


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

	///////////////////////////////////////////////////////////////////////  
	//  Forward references

	class CShaderLoaderDirectX10;


	///////////////////////////////////////////////////////////////////////  
	//  Class DX10

	class ST_STORAGE_CLASS DX10
	{
	public:
	static	ID3D10Device*			ST_CALL_CONV	Device(void);
	static	void					ST_CALL_CONV	SetDevice(ID3D10Device* pDevice);

	static	ID3D10RenderTargetView*	ST_CALL_CONV	MainRenderTargetView(void);
	static	void					ST_CALL_CONV	SetMainRenderTargetView(ID3D10RenderTargetView* pTarget);

	static	ID3D10DepthStencilView* ST_CALL_CONV	MainDepthStencilView(void);
	static	void					ST_CALL_CONV	SetMainDepthStencilView(ID3D10DepthStencilView* pTarget);

	private:
	static	ID3D10Device*							m_pDx10;
	static	ID3D10RenderTargetView*					m_pMainRenderTargetView;
	static	ID3D10DepthStencilView*					m_pMainDepthStencilView;
	};


	///////////////////////////////////////////////////////////////////////  
	//  Class CRenderStateDirectX9

	class ST_STORAGE_CLASS CRenderStateDirectX10
	{
	public:

	static	void   	   	ST_CALL_CONV 	Initialize(void);
	static	st_bool		ST_CALL_CONV	NeedsStateApplication(void);
	static	void		ST_CALL_CONV 	ApplyStates(void);
	static	void		ST_CALL_CONV 	Destroy(void);

	static	void   	   	ST_CALL_CONV 	ClearScreen(st_bool bClearColor = true, st_bool bClearDepth = true);
	static	void   	   	ST_CALL_CONV 	SetAlphaFunction(EAlphaFunc eFunc, st_float32 fValue);	// fValue range = [0.0, 255.0]
	static	void   	   	ST_CALL_CONV 	SetAlphaTesting(st_bool bFlag);
	static	void   	   	ST_CALL_CONV 	SetAlphaToCoverage(st_bool bFlag);
	static	void   	   	ST_CALL_CONV 	SetBlending(st_bool bFlag);
	static	void       	ST_CALL_CONV 	SetColorMask(st_bool bRed, st_bool bGreen, st_bool bBlue, st_bool bAlpha);
	static	void   	   	ST_CALL_CONV 	SetDepthMask(st_bool bFlag);
	static	void	   	ST_CALL_CONV 	SetDepthTestFunc(EDepthTestFunc eDepthTestFunc);
	static	void   	   	ST_CALL_CONV 	SetDepthTesting(st_bool bFlag);
	static	void   	   	ST_CALL_CONV 	SetFaceCulling(ECullType eCullType);
	static	void   	   	ST_CALL_CONV 	SetPointSize(st_float32 fSize);
	static	void   	   	ST_CALL_CONV 	SetPolygonOffset(st_float32 fFactor = 0.0f, st_float32 fUnits = 0.0f);
	static	void   	   	ST_CALL_CONV 	SetMultisampling(st_bool bMultisample);
	static	void   	   	ST_CALL_CONV 	SetRenderStyle(ERenderStyle eStyle);

	private:
			typedef CMap<D3D10_DEPTH_STENCIL_DESC, ID3D10DepthStencilState*> DepthStencilMap;
			typedef CMap<D3D10_RASTERIZER_DESC, ID3D10RasterizerState*> RasterizerMap;
			typedef CMap<D3D10_BLEND_DESC, ID3D10BlendState*> BlendMap;

	static	st_bool						m_bDepthChanged;
	static	D3D10_DEPTH_STENCIL_DESC	m_sCurrentDepthStateDesc;
	static	DepthStencilMap*			m_pDepthStates;

	static	st_bool						m_bRasterizerChanged;
	static	D3D10_RASTERIZER_DESC		m_sCurrentRasterizerStateDesc;
	static	RasterizerMap*				m_pRasterizerStates;

	static	st_bool						m_bBlendChanged;
	static	D3D10_BLEND_DESC			m_sCurrentBlendDesc;
	static	BlendMap*					m_pBlendStates;

	static	st_uint32					m_uiRefCount;
	};
	typedef CRenderStateRI<CRenderStateDirectX10> CRenderState;


	///////////////////////////////////////////////////////////////////////  
	//  Class CTextureDirectX10

	class ST_STORAGE_CLASS CTextureDirectX10
	{
	public:
										CTextureDirectX10( );
	virtual								~CTextureDirectX10( );

			// loading
			st_bool						Load(const char* pFilename, st_int32 nMaxAnisotropy = 0);
			st_bool						LoadColor(st_uint32 ulColor);
			st_bool						LoadAlphaNoise(st_int32 nWidth, st_int32 nHeight);
			st_bool						Unload(void);
			st_bool						IsValid(void) const;

			// render
			st_bool						FixedFunctionBind(void);
			ID3D10ShaderResourceView*	GetTextureObject(void) const;

			// other
			CTextureDirectX10&		operator=(const CTextureDirectX10& cRight);
			st_bool					operator!=(const CTextureDirectX10& cRight) const;

	private:
			// caching
			ID3D10ShaderResourceView*	SearchCache(const CString& strFilename);
			void						AddToCache(const CString& strFilename, ID3D10ShaderResourceView* pTexture);
			st_bool						RemoveFromCache(const CString& strFilename);

			ID3D10ShaderResourceView*	m_pTexture;
			CString						m_strFilename;

			struct SCacheEntry
			{
											SCacheEntry( ) :
												m_pTexture(NULL),
												m_nRefCount(0)
											{
											}

				ID3D10ShaderResourceView*	m_pTexture;
				st_int32					m_nRefCount;
			};

			typedef CMap<CString, SCacheEntry> TextureCacheMap;

	static	TextureCacheMap*		m_pCache;
	};
	typedef CTextureRI<CTextureDirectX10> CTexture;


	///////////////////////////////////////////////////////////////////////  
	//  Class CDepthTextureDirectX10

	class ST_STORAGE_CLASS CDepthTextureDirectX10
	{
	public:
										CDepthTextureDirectX10( );
										~CDepthTextureDirectX10( );

			st_bool						Create(st_int32 nWidth, st_int32 nHeight);

			void						Release(void);
			void						OnResetDevice(void);
			void						OnLostDevice(void);

			st_bool						SetAsTarget(void);
			st_bool						ReleaseAsTarget(void);

			st_bool						BindAsTexture(void);
			st_bool						UnBindAsTexture(void);

			void						EnableShadowMapComparison(st_bool bFlag);
			
			ID3D10ShaderResourceView*	GetTextureObject(void) const;

	protected:
			ID3D10ShaderResourceView*	m_pColorTexture;
			ID3D10ShaderResourceView*	m_pDepthTexture;

			ID3D10RenderTargetView*		m_pDepthTextureView;
			ID3D10DepthStencilView*		m_pDepthStencilView;
			ID3D10RenderTargetView*		m_pOldDepthTextureView;
			ID3D10DepthStencilView*		m_pOldDepthStencilView;

			D3D10_VIEWPORT				m_sOldViewport;

			st_int32					m_nBufferWidth;
			st_int32					m_nBufferHeight;
			st_bool						m_bInitSucceeded;
			st_bool						m_bBoundAsTarget;
	};
	typedef CDepthTextureRI<CDepthTextureDirectX10> CDepthTexture;


	///////////////////////////////////////////////////////////////////////  
	//  Class CShaderConstantDirectX10

	class ST_STORAGE_CLASS CShaderConstantDirectX10
	{
	public:
			friend class CShaderLoaderDirectX10;

									CShaderConstantDirectX10( );
	virtual							~CShaderConstantDirectX10( );

			st_bool					IsValid(void) const;

			st_bool					Set1f(st_float32 x) const;
			st_bool					Set2f(st_float32 x, st_float32 y) const;
			st_bool					Set2fv(const st_float32 afValues[2]) const;
			st_bool					Set3f(st_float32 x, st_float32 y, st_float32 z) const;
			st_bool					Set3fv(const st_float32 afValues[3]) const;
			st_bool					Set3fvPlus1f(const st_float32 afValues[3], st_float32 w) const;
			st_bool					Set4f(st_float32 x, st_float32 y, st_float32 z, st_float32 w) const;
			st_bool					Set4fv(const st_float32 afValues[4]) const;
			st_bool					SetArray4f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset = 0) const;
			st_bool					SetMatrix(const st_float32 afMatrix[16]) const;
			st_bool					SetMatrixTranspose(const st_float32 afMatrix[16]) const;
			st_bool					SetMatrixIndex(const st_float32 afMatrix[16], st_uint32 uiIndex) const;
			st_bool					SetMatrix4x4Array(const st_float32* pMatrixArray, st_uint32 uiNumMatrices) const;

			ID3D10EffectVariable*	m_pParameter;
	};


	///////////////////////////////////////////////////////////////////////  
	//  Class CShaderConstant

	class ST_STORAGE_CLASS CShaderConstant : public CShaderConstantRI<CShaderConstantDirectX10>
	{
	public:
			st_bool				SetTexture(const CTexture& cTexture) const;
			st_bool				SetTexture(const CDepthTexture& cDepthTexture) const;
	};


	///////////////////////////////////////////////////////////////////////  
	//  Class CShaderTechniqueDirectX10

	class ST_STORAGE_CLASS CShaderTechniqueDirectX10
	{
	public:
			friend class CShaderLoaderDirectX10;

									CShaderTechniqueDirectX10( );
	virtual							~CShaderTechniqueDirectX10( );

			st_bool					GetConstant(const char* /*pName*/, CShaderConstant& /*cConstant*/, EConstantType /*eType*/) const { return false; }

			st_bool					Bind(void);
			st_bool					UnBind(void);
			st_bool					CommitConstants(void);
			st_bool					CommitTextures(void);

			st_bool					IsValid(void) const;

			ID3D10EffectTechnique*	GetDX10Technique(void) const;

	private:
			ID3D10EffectTechnique*	m_pTechnique;
	};
	typedef CShaderTechniqueRI<CShaderTechniqueDirectX10, CShaderConstant> CShaderTechnique;


	///////////////////////////////////////////////////////////////////////  
	//  Class CShaderLoaderDirectX10

	class ST_STORAGE_CLASS CShaderLoaderDirectX10
	{
	public:
								CShaderLoaderDirectX10( );
	virtual						~CShaderLoaderDirectX10( );

			
			st_bool				Init(const CArray<CFixedString>& aDefines, const char* pEffectFilename, st_bool bUsePrecompiledShaders);
			void				Release(void);
			const char*			GetError(void) const;

			st_bool				GetTechnique(const char* pName, CShaderTechnique& cShader) const;
			st_bool				GetConstant(const char* pName, CShaderConstant& cConstant) const;
			CFixedString		GetTextureConstantName(const CFixedString& strBaseName);

			void				OnResetDevice(void);
			void				OnLostDevice(void);

			// this renderer uses effects files
			st_bool				RendererUsesEffects(void) const { return true; }
			st_bool				GetVertexShader(const char* /*pFilename*/, const char* /*pName*/, CShaderTechnique& /*cShader*/) const { return false; }
			st_bool				GetPixelShader(const char* /*pFilename*/, const char* /*pName*/, CShaderTechnique& /*cShader*/) const { return false; }

	private:
			ID3D10Effect*		m_pEffect;
			CString				m_strError;
	};
	typedef CShaderLoaderRI<CShaderLoaderDirectX10, CShaderTechnique, CShaderConstant> CShaderLoader;


	///////////////////////////////////////////////////////////////////////  
	//  Class CGeometryBufferDirectX10

	class ST_STORAGE_CLASS CGeometryBufferDirectX10
	{
	public:
											CGeometryBufferDirectX10( );
	virtual									~CGeometryBufferDirectX10( );

			// vertex buffer
			st_bool							SetVertexFormat(const SVertexAttribDesc* pAttribDesc, const CShaderTechnique* pTechnique);
			st_bool							CreateVertexBuffer(st_bool bDynamic, const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiVertexSize);
			st_bool							OverwriteVertices(const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiVertexSize, st_uint32 uiOffset);
			st_bool							VertexBufferIsValid(void) const;

			st_bool							EnableFormat(void) const;
	static	st_bool		       ST_CALL_CONV DisableFormat(void);

			st_bool							BindVertexBuffer(st_int32 nVertexSize, EGeometryProcessingType eProcessingType) const;
	static	st_bool		       ST_CALL_CONV UnBindVertexBuffer(void);

			// index buffer
			st_bool							SetIndexFormat(EIndexFormat eFormat);
			st_bool							CreateIndexBuffer(st_bool bDynamic, const void* pIndexData, st_uint32 uiNumIndices);
			st_bool							OverwriteIndices(const void* pIndexData, st_uint32 uiNumIndices, st_uint32 uiOffset);
			st_bool							IndexBufferIsValid(void) const;
			st_bool							ClearIndices(void);
			st_uint32						IndexSize(void) const;

			st_bool							BindIndexBuffer(void) const;
			st_bool							UnBindIndexBuffer(void) const;

			// render functions
			st_bool							RenderIndexed(EPrimitiveType ePrimType, 
														  st_uint32 uiStartIndex, 
														  st_uint32 uiNumIndices, 
														  st_uint32 uiMinIndex = 0,
														  st_uint32 uiNumVertices = 0,
														  EGeometryProcessingType eProcessingType = GEOMETRY_PROCESSING_NONE) const;
			st_bool							RenderArrays(EPrimitiveType ePrimType, 
														 st_uint32 uiStartVertex, 
														 st_uint32 uiNumVertices, 
														 EGeometryProcessingType eProcessingType = GEOMETRY_PROCESSING_NONE) const;
	static	st_bool							IsPrimitiveTypeSupported(EPrimitiveType ePrimType);

	private:
			st_bool							IsFormatSet(void) const;

			ID3D10InputLayout*				m_pVertexLayout;
			ID3D10Buffer*					m_pVertexBuffer;
			ID3D10Buffer*					m_pIndexBuffer;
			EIndexFormat					m_eIndexFormat;
			st_uint32						m_uiCurrentVertexBufferSize;	// in bytes
			st_uint32						m_uiCurrentIndexBufferSize;	// # of indices
	};
	typedef CGeometryBufferRI<CGeometryBufferDirectX10, CShaderTechnique> CGeometryBuffer;


	///////////////////////////////////////////////////////////////////////  
	//  Class CForestRender (DirectX10 specific)

	typedef SForestCullResultsRI<CTexture, 
								 CGeometryBuffer,
								 CShaderTechnique> SForestCullResultsRender;


	///////////////////////////////////////////////////////////////////////  
	//  Class CForestRender (DirectX10 specific)

	typedef CForestRI<CRenderState, 
					  CTexture, 
					  CGeometryBuffer, 
					  CShaderLoader, 
					  CShaderTechnique, 
					  CShaderConstant,
					  CDepthTexture> CForestRender;


	///////////////////////////////////////////////////////////////////////  
	//  Class CTreeRender (DirectX10 specific)

	typedef CTreeRI<CTexture, 
					CGeometryBuffer,
					CShaderTechnique> CTreeRender;


	///////////////////////////////////////////////////////////////////////  
	//  Class CTerrainRender (DirectX10 specific)

	typedef CTerrainRI<CRenderState, 
					   CTexture, 
					   CGeometryBuffer, 
					   CShaderLoader, 
					   CShaderTechnique, 
					   CShaderConstant,
					   CDepthTexture> CTerrainRender;


	///////////////////////////////////////////////////////////////////////  
	//  Class CGrassRender (DirectX10 specific)

	typedef CGrassRI<CRenderState, 
					 CTexture, 
					 CGeometryBuffer, 
					 CShaderLoader, 
					 CShaderTechnique, 
					 CShaderConstant,
					 CDepthTexture> CGrassRender;


	///////////////////////////////////////////////////////////////////////  
	//  Class CSkyRender (DirectX10 specific)

	typedef CSkyRI<CRenderState, 
				   CTexture, 
				   CGeometryBuffer, 
				   CShaderLoader, 
				   CShaderTechnique, 
				   CShaderConstant,
				   CDepthTexture> CSkyRender;


	// include inline functions
	#include "Renderers/DirectX10/RenderState.inl"
	#include "Renderers/DirectX10/Texture.inl"
	#include "Renderers/DirectX10/GeometryBuffer.inl"
	#include "Renderers/DirectX10/Shaders.inl"
	#include "Renderers/DirectX10/DepthTexture.inl"

} // end namespace SpeedTree



// Macintosh export control
#ifdef __APPLE__
#pragma export off
#endif
