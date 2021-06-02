///////////////////////////////////////////////////////////////////////  
//  OpenGLRenderer.h
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
#include "Core/ExportBegin.h"
#include "Core/Mutex.h"
#ifdef __GNUC__
	#include <glew/glew.h>
	#include <Cg/cgGL.h>
#else
	#include "glew/glew.h"
	#include "glew/wglew.h"
	#include "Cg/cgGL.h"
#endif
#include "RenderInterface/ForestRI.h"
#include "RenderInterface/TerrainRI.h"
#include "RenderInterface/GrassRI.h"
#include "RenderInterface/SkyRI.h"


///////////////////////////////////////////////////////////////////////  
//  Cg-related macros

// arbvp1/arbfp1 are the recommended profiles since they're the most portable and
// our internals tests should them to be the best performing as well
#define SPEEDTREE_CG_VERTEX_PROFILE    cgGetProfile("arbvp1")
#define SPEEDTREE_CG_FRAGMENT_PROFILE  cgGetProfile("arbfp1")
//#define SPEEDTREE_CG_VERTEX_PROFILE    cgGLGetLatestProfile(CG_GL_VERTEX)
//#define SPEEDTREE_CG_FRAGMENT_PROFILE  cgGLGetLatestProfile(CG_GL_FRAGMENT)


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

	///////////////////////////////////////////////////////////////////////  
	//  Forward references

	class CShaderLoaderOpenGL;


	///////////////////////////////////////////////////////////////////////  
	//  Class CRenderStateOpenGL

	class ST_STORAGE_CLASS CRenderStateOpenGL
	{
	public:
	static	void				Initialize(void);
	static	void				ApplyStates(void);
	static	void				Destroy(void);

	static	void				ClearScreen(st_bool bClearColor = true, st_bool bClearDepth = true);
	static	void				SetAlphaFunction(EAlphaFunc eFunc, st_float32 fValue);	// fValue range = [0.0,255.0]
	static	void				SetAlphaTesting(st_bool bFlag);
	static	void				SetAlphaToCoverage(st_bool bFlag);
	static	void				SetBlending(st_bool bFlag);
	static	void				SetColorMask(st_bool bRed, st_bool bGreen, st_bool bBlue, st_bool bAlpha);
	static	void				SetDepthMask(st_bool bFlag);
	static	void				SetDepthTestFunc(EDepthTestFunc eDepthTestFunc);
	static	void				SetDepthTesting(st_bool bFlag);
	static	void				SetFaceCulling(ECullType eCullType);
	static	void				SetMultisampling(st_bool bMultisample);
	static	void				SetPointSize(st_float32 fSize);
	static	void				SetPolygonOffset(st_float32 fFactor = 0.0f, st_float32 fUnits = 0.0f); // both zeros deactivates polygon offset
	static	void				SetRenderStyle(ERenderStyle eStyle);

#ifdef _WIN32
	static 	void 				InitializeForThreadedOpenGL(HDC hDC, HGLRC hContext);
#endif
	static 	void 				PrepareForThreadedOpenGL(void);
	static 	void 				DoneThreadedOpenGL(void);
	static 	void 				Lock(void);
	static 	void 				Unlock(void);

	private:
	static	st_bool				m_bSupportsA2C;
	static	st_bool				m_bReadyForThreading;
	static	CMutex				m_cLock;
#ifdef _WIN32
	static	HDC					m_hDC;
	static	HGLRC				m_hContext;
#endif
	};
	typedef CRenderStateRI<CRenderStateOpenGL> CRenderState;


	///////////////////////////////////////////////////////////////////////  
	//  Class CTextureOpenGL

	class ST_STORAGE_CLASS CTextureOpenGL
	{
	public:
									CTextureOpenGL( );
	virtual							~CTextureOpenGL( );

			// loading
			st_bool					Load(const char* pFilename, st_int32 nMaxAnisotropy = 0);
			st_bool					LoadColor(st_uint32 uiColor); // 0xAABBGGRR
			st_bool					LoadAlphaNoise(st_int32 nWidth, st_int32 nHeight);
			st_bool					Unload(void);
			st_bool					IsValid(void) const;

			// render
			st_bool					FixedFunctionBind(void);
			st_uint32				GetTextureObject(void) const;

			// other
			CTextureOpenGL&			operator=(const CTextureOpenGL& cRight);
			st_bool					operator!=(const CTextureOpenGL& cRight) const;

	private:
			st_bool					LoadTGA(const CFixedString& strFilename, st_int32 nMaxAnisotropy = 0);
			st_bool					LoadDDS(const CFixedString& strFilename, st_int32 nMaxAnisotropy = 0);

			// caching
			GLuint					SearchCache(const CFixedString& strFilename);
			void					AddToCache(const CFixedString& strFilename, GLuint uiTexture);
			st_bool					RemoveFromCache(const CFixedString& strFilename);

			GLuint					m_uiTexture;
			CFixedString			m_strFilename;

			struct SCacheEntry
			{
									SCacheEntry( ) :
										m_uiTexture(0),
										m_nRefCount(0)
									{
									}

				GLuint				m_uiTexture;
				st_int32			m_nRefCount;
			};

			typedef CMap<CFixedString, SCacheEntry> TextureCacheMap;

	static	TextureCacheMap*		m_pCache;
	};
	typedef CTextureRI<CTextureOpenGL> CTexture;


	///////////////////////////////////////////////////////////////////////  
	//  Class CDepthTextureOpenGL

	class ST_STORAGE_CLASS CDepthTextureOpenGL
	{
	public:
								CDepthTextureOpenGL( );
								~CDepthTextureOpenGL( );

			st_bool				Create(st_int32 nWidth, st_int32 nHeight);
			void				Release(void);

			st_bool				SetAsTarget(void);
			st_bool				ReleaseAsTarget(void);

			st_bool				BindAsTexture(void);
			st_bool				UnBindAsTexture(void);
			void				EnableShadowMapComparison(st_bool bFlag);
			GLuint				GetTextureObject(void) const;

	protected:
			st_bool				CreateTextureObject(void);

			GLuint				m_uiFrameBufferObject;
			GLuint				m_uiTextureObject;
			st_int32			m_nBufferWidth;
			st_int32			m_nBufferHeight;
			st_bool				m_bInitSucceeded;
			st_bool				m_bBoundAsTarget;
	};
	typedef CDepthTextureRI<CDepthTextureOpenGL> CDepthTexture;


	///////////////////////////////////////////////////////////////////////  
	//  Class CShaderConstantOpenGL

	class ST_STORAGE_CLASS CShaderConstantOpenGL
	{
	public:
			friend class CShaderLoaderOpenGL;

								CShaderConstantOpenGL( );
	virtual						~CShaderConstantOpenGL( );

			st_bool				IsValid(void) const;

			st_bool				Set1f(st_float32 x) const;
			st_bool				Set2f(st_float32 x, st_float32 y) const;
			st_bool				Set2fv(const st_float32 afValues[2]) const;
			st_bool				Set3f(st_float32 x, st_float32 y, st_float32 z) const;
			st_bool				Set3fv(const st_float32 afValues[3]) const;
			st_bool				Set3fvPlus1f(const st_float32 afValues[3], st_float32 w) const;
			st_bool				Set4f(st_float32 x, st_float32 y, st_float32 z, st_float32 w) const;
			st_bool				Set4fv(const st_float32 afValues[4]) const;
			st_bool				SetArray1f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset = 0) const;
			st_bool				SetArray3f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset = 0) const;
			st_bool				SetArray4f(const st_float32* pArray, st_int32 nSize, st_int32 nOffset = 0) const;
			st_bool				SetMatrix(const st_float32 afMatrix[16]) const;
			st_bool				SetMatrixTranspose(const st_float32 afMatrix[16]) const;
			st_bool				SetMatrixIndex(const st_float32 afMatrix[16], st_uint32 uiIndex) const;
			st_bool				SetMatrix4x4Array(const st_float32* pMatrixArray, st_uint32 uiOffset, st_uint32 uiNumMatrices) const;

			CGparameter			m_cgParameter;
			CGparameter			m_cgParameterMirror; // in case the parameter needs to be set for both the pixel & vertex shader

			st_int32			m_nTexUnit;
	};


	///////////////////////////////////////////////////////////////////////  
	//  Class CShaderConstant

	class ST_STORAGE_CLASS CShaderConstant : public CShaderConstantRI<CShaderConstantOpenGL>
	{
	public:
			st_bool				SetTexture(const CTexture& cTexture) const;
			st_bool				SetTexture(const CDepthTexture& cDepthTexture) const;

			// used to set one texture in an array of textures
			st_bool				SetTextureIndex(const CTexture& cTexture, st_uint32 uiIndex) const;
			st_bool				SetTextureIndex(const CDepthTexture& cDepthTexture, st_uint32 uiIndex) const;
	};


	///////////////////////////////////////////////////////////////////////  
	//  Class CShaderTechniqueOpenGL

	class ST_STORAGE_CLASS CShaderTechniqueOpenGL
	{
	public:
			friend class CShaderLoaderOpenGL;

								CShaderTechniqueOpenGL( );
	virtual						~CShaderTechniqueOpenGL( );

			st_bool				GetConstant(const char* pName, CShaderConstant& cConstant, EConstantType eType) const;

			st_bool				Bind(void);
			st_bool				UnBind(void);
			st_bool				CommitConstants(void);
			st_bool				CommitTextures(void);

			st_bool				IsValid(void) const;

	private:
			void				ProcessParameterSemantic(CShaderConstantOpenGL& cConstant) const;

			CGprogram			m_cgVertexShader;
			CGprogram			m_cgPixelShader;
	};
	typedef CShaderTechniqueRI<CShaderTechniqueOpenGL, CShaderConstant> CShaderTechnique;


	///////////////////////////////////////////////////////////////////////  
	//  Class CShaderLoaderOpenGL

	class ST_STORAGE_CLASS CShaderLoaderOpenGL
	{
	public:
									CShaderLoaderOpenGL( );
	virtual							~CShaderLoaderOpenGL( );

			st_bool					Init(const CArray<CFixedString>& aDefines, const char* pEffectFilename, st_bool bUsePrecompiledShaders);
			void					Release(void);
			const char*				GetError(void) const;

			st_bool					GetVertexShader(const char* pFilename, const char* pName, CShaderTechnique& cShader) const;
			st_bool					GetPixelShader(const char* pFilename, const char* pName, CShaderTechnique& cShader) const;
			CFixedString			GetTextureConstantName(const CFixedString& strBaseName);
			
			void					OnResetDevice(void) { }
			void					OnLostDevice(void) { }

	static	CGcontext ST_CALL_CONV GetCgContext(void);

			// this renderer uses separate vertex/pixel shaders
			st_bool					RendererUsesEffects(void) const { return false; }
			st_bool					GetTechnique(const char* /*pName*/, CShaderTechnique& /*cShader*/) const { return false; }
			st_bool					GetConstant(const char* /*pName*/, CShaderConstant& /*cConstant*/) const { return false; }

	private:
	static	void					CreateCgContext(void);
	static	void					ReleaseCgContext(void);

	mutable	CString					m_strError;

	static	CGcontext				m_cgContext;
	static	st_uint32				m_uiCgContextRefCount;
			const char**			m_pShaderMacros;
	};
	typedef CShaderLoaderRI<CShaderLoaderOpenGL, CShaderTechnique, CShaderConstant> CShaderLoader;

	
	///////////////////////////////////////////////////////////////////////  
	//  Class CGeometryBufferOpenGL

	class ST_STORAGE_CLASS CGeometryBufferOpenGL
	{
	public:
									CGeometryBufferOpenGL( );
	virtual							~CGeometryBufferOpenGL( );

			// vertex buffer
			st_bool					SetVertexFormat(const SVertexAttribDesc* pAttribDesc, const CShaderTechnique* pTechnique);
			st_bool					CreateVertexBuffer(st_bool bDynamic, const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiVertexSize);
			st_bool					OverwriteVertices(const void* pVertexData, st_uint32 uiNumVertices, st_uint32 uiVertexSize, st_uint32 uiOffset);
			st_bool					VertexBufferIsValid(void) const;

			st_bool					EnableFormat(void) const;
	static	st_bool	ST_CALL_CONV	DisableFormat(void);
			st_bool					BindVertexBuffer(st_int32 nVertexSize, EGeometryProcessingType eProcessingType) const;
	static	st_bool	ST_CALL_CONV	UnBindVertexBuffer(void);

			// index buffer
			st_bool					SetIndexFormat(EIndexFormat eFormat);
			st_bool					CreateIndexBuffer(st_bool bDynamic, const void* pIndexData, st_uint32 uiNumIndices);
			st_bool					OverwriteIndices(const void* pIndexData, st_uint32 uiNumIndices, st_uint32 uiOffset);
			st_bool					IndexBufferIsValid(void) const;
			st_uint32				IndexSize(void) const;
			st_bool					ClearIndices(void);

			st_bool					BindIndexBuffer(void) const;
			st_bool					UnBindIndexBuffer(void) const;

			// render functions
			st_bool					RenderIndexed(EPrimitiveType ePrimType, 
												  st_uint32 uiStartIndex, 
												  st_uint32 uiNumIndices, 
												  st_uint32 uiMinIndex = 0,
												  st_uint32 uiNumVertices = 0,
												  EGeometryProcessingType eProcessingType = GEOMETRY_PROCESSING_NONE) const;
			st_bool					RenderArrays(EPrimitiveType ePrimType, 
												 st_uint32 uiStartVertex, 
												 st_uint32 uiNumVertices, 
												 EGeometryProcessingType eProcessingType) const;
	static	st_bool	ST_CALL_CONV	IsPrimitiveTypeSupported(EPrimitiveType ePrimType);

	private:
			struct ST_STORAGE_CLASS SAttribParams
			{
									SAttribParams( );

				st_bool				IsActive(void) const;

				st_int32			m_nOffset;
				st_int32			m_nNumElements;
				GLenum				m_eDataType;
				st_bool				m_bNormalized;
			};

			st_bool					IsVertexFormatSet(void) const;

			GLuint					m_uiVertexBuffer;
			GLuint					m_uiIndexBuffer;
			EIndexFormat			m_eIndexFormat;
			SAttribParams			m_asAttribParams[VERTEX_ATTRIB_SEMANTIC_COUNT];
	};
	typedef CGeometryBufferRI<CGeometryBufferOpenGL, CShaderTechnique> CGeometryBuffer;


	///////////////////////////////////////////////////////////////////////  
	//  Class CForestRender (OpenGL specific)

	typedef SForestCullResultsRI<CTexture, 
								 CGeometryBuffer,
							 	 CShaderTechnique> SForestCullResultsRender;


	///////////////////////////////////////////////////////////////////////  
	//  Class CForestRender (OpenGL specific)

	typedef CForestRI<CRenderState, 
					  CTexture, 
					  CGeometryBuffer, 
					  CShaderLoader, 
					  CShaderTechnique, 
					  CShaderConstant,
					  CDepthTexture> CForestRender;


	///////////////////////////////////////////////////////////////////////  
	//  Class CTreeRender (OpenGL specific)

	typedef CTreeRI<CTexture, 
					CGeometryBuffer,
					CShaderTechnique> CTreeRender;


	///////////////////////////////////////////////////////////////////////  
	//  Class CTerrainRender (OpenGL specific)

	typedef CTerrainRI<CRenderState, 
					   CTexture, 
					   CGeometryBuffer, 
					   CShaderLoader, 
					   CShaderTechnique, 
					   CShaderConstant,
					   CDepthTexture> CTerrainRender;


	///////////////////////////////////////////////////////////////////////  
	//  Class CGrassRender (OpenGL specific)

	typedef CGrassRI<CRenderState, 
					 CTexture, 
					 CGeometryBuffer, 
					 CShaderLoader, 
					 CShaderTechnique, 
					 CShaderConstant,
					 CDepthTexture> CGrassRender;


	///////////////////////////////////////////////////////////////////////  
	//  Class CSkyRender (OpenGL specific)

	typedef CSkyRI<CRenderState, 
				   CTexture, 
				   CGeometryBuffer, 
				   CShaderLoader, 
				   CShaderTechnique, 
				   CShaderConstant,
				   CDepthTexture> CSkyRender;


	// include inline functions
	#include "Renderers/OpenGL/Texture.inl"
	#include "Renderers/OpenGL/RenderState.inl"
	#include "Renderers/OpenGL/Shaders.inl"
	#include "Renderers/OpenGL/DepthTexture.inl"
	#include "Renderers/OpenGL/GeometryBuffer.inl"

} // end namespace SpeedTree



#include "Core/ExportEnd.h"
