////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED

#include <xray/render/common/sources/res_const_table.h>
#include <xray/render/common/sources/quasi_singleton.h>
#include <xray/render/dx9/sources/constant_cache.h>
#include <xray/render/dx9/sources/vertex_stream.h>
#include <xray/render/dx9/sources/index_stream.h>

namespace xray {
namespace render {


enum render_target_enum
{
	target_rt0 = 0,
	target_rt1,
	target_rt2,
	//target_rt3,
	target_zb,
	target_count
};

class backend : public quasi_singleton<backend>
{
public:
	inline void invalidate();
	inline void flush();

	inline void set_stencil(u32 enable, u32 func=D3DCMP_ALWAYS, u32 ref=0x00, u32 mask=0x00, u32 writemask=0x00, u32 fail=D3DSTENCILOP_KEEP, u32 zpass=D3DSTENCILOP_KEEP, u32 zfail=D3DSTENCILOP_KEEP);
	inline void set_z(u32 enable);
	inline void set_alpha_ref(u32 value);
	inline void set_color_write_enable(u32 mask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	inline void set_cull_mode(u32 mode);

	inline void set_vb(ID3DVertexBuffer* vb, u32 vb_stride);
	inline void set_ib(ID3DIndexBuffer* ib);
	inline void set_vs(ID3DVertexShader* vs);
	inline void set_ps(ID3DPixelShader* ps);
	inline void set_state(ID3DState* state);
	inline void set_decl(IDirect3DVertexDeclaration9* state);
	inline void set_ctable(res_const_table* ctable);
	inline void set_rt(render_target_enum target, ID3DSurface* surface);
	inline void set_texture(u32 stage, ID3DTexture* texture);
	inline void reset_stages();
	
	inline u32  get_texture_stage_count() {return texture_stage_count;}
	
	template <typename T>
	void set_c(res_constant* c, const T& arg);

	template <typename T>
	void set_ca(res_constant* c, u32 index, const T& arg);

	template <typename T>
	void set_ca(shared_string name, u32 index, const T& arg);

	template <typename T>
	void set_c(shared_string name, const T& arg);

	void set_c(res_constant* c, float x, float y, float z, float w);
	void set_ca(res_constant* c, u32 index, float x, float y, float z, float w);
	void set_c(shared_string name, float x, float y, float z, float w);
	void set_ca(shared_string name, u32 index, float x, float y, float z, float w);

	inline void clear(u32 flags, D3DCOLOR color, float z_value, u32 stencil_value);

	inline void	render(D3DPRIMITIVETYPE type, u32 base_vertex, u32 start_vertex, u32 vertex_count, u32 start_index, u32 prim_count);
	inline void	render(D3DPRIMITIVETYPE type, u32 start_vertex, u32 prim_count);

	void on_device_create(ID3DDevice* device);
	void on_device_destroy();

public:
	vertex_stream	vertex;
	index_stream	index;

private:
	void flush_c_cache();
	inline void flush_rt();
	inline void flush_stages();

private:
	enum render_states
	{
		rs_stencil_enable = 0,
		rs_stencil_func,
		rs_stencil_ref,
		rs_stencil_mask,
		rs_stencil_writemask,
		rs_stencil_fail,
		rs_stencil_zpass,
		rs_stencil_zfail,
		rs_colorwrite_mask,
		rs_cull_mode,
		rs_z_enable,
		rs_alpha_ref,
		rs_count
	};
	
	static const D3DRENDERSTATETYPE m_dx_render_states[rs_count];

	std::bitset<rs_count>			m_dirty_states;
	fixed_vector<DWORD, rs_count>	m_states;

	ID3DDevice*	m_device;

	inline void set_state(render_states state, DWORD value);

	enum render_objects
	{
		ro_vertex_shader,
		ro_pixel_shader,
		ro_vertex_buffer,
		ro_index_buffer,
		ro_state,
		ro_decl,
		ro_ctable,
		//ro_target_start,
		//ro_target_end = ro_target_start+target_count,
		ro_count// = ro_target_end
	};

	res_const_table*				m_ctable;
	ID3DState*						m_state;
	ID3DVertexBuffer*				m_vb;
	ID3DIndexBuffer*				m_ib;
	ID3DVertexShader*				m_vs;
	ID3DPixelShader*				m_ps;
	IDirect3DVertexDeclaration9*	m_decl;
	ID3DSurface*					m_targets[target_count];
	ID3DSurface*					m_base_rt;
	ID3DSurface*					m_base_zb;

	u32	m_vb_stride;

	std::bitset<ro_count>		m_dirty_objects;
	std::bitset<target_count>	m_dirty_targets;

	XRAY_ALIGN(16) constant_array	m_a_pixel;
	XRAY_ALIGN(16) constant_array	m_a_vertex;

	enum {texture_stage_count = 16};

	std::bitset<texture_stage_count>	m_dirty_stages;
	ID3DTexture*						m_stages[texture_stage_count];

//#ifdef	USE_DX10
//	enum	MaxTextures
//	{
//		//	Actually these values are 128
//		mt_max_pixel_shader_textures = 16,
//		mt_max_vertex_shader_textures = 4,
//		mt_max_geometry_shader_textures = 16
//	};
//	enum
//	{
//		max_cbuffers	= 14
//	};
//#else	//	USE_DX10
//	enum	max_textures
//	{
//		mt_max_pixel_shader_textures = 16,
//		mt_max_vertex_shader_textures = 4,
//	};
//#endif	//	USE_DX10
//
//
//
//public:            
//	// Dynamic geometry streams
//	//_VertexStream					Vertex;
//	//_IndexStream					Index;
//	//ID3DIndexBuffer*				QuadIB;
//	//ID3DIndexBuffer*				old_QuadIB;
//	//R_xforms						xforms;
//	//R_hemi							hemi;
//	//R_tree							tree;
//
//#ifdef	USE_DX10
//	ref_cbuffer						m_aVertexConstants[MaxCBuffers];
//	ref_cbuffer						m_aPixelConstants[MaxCBuffers];
//	ref_cbuffer						m_aGeometryConstants[MaxCBuffers];
//	D3D10_PRIMITIVE_TOPOLOGY		m_PrimitiveTopology;
//	ID3D10InputLayout*				m_pInputLayout;
//	DWORD							dummy0;	//	Padding to avoid warning	
//	DWORD							dummy1;	//	Padding to avoid warning	
//	DWORD							dummy2;	//	Padding to avoid warning	
//#endif
//private:
//	// Render-targets
//	ID3DRenderTargetView*		m_rts[4]	//pRT[4];
//	ID3DDepthStencilView*		m_zb;		//pZB;
//
//	// vertices_type/indices_type/etc
//#ifdef	USE_DX10
//	SDeclaration*					decl;
//#else	//	USE_DX10
//	IDirect3DVertexDeclaration9*	m_decl;//decl;
//#endif	//	USE_DX10
//	ID3DVertexBuffer*		m_vb;	//vb;
//	ID3DIndexBuffer*		m_ib;	//ib;
//	u32						m_cb_stride;	//vb_stride;
//
//	// Pixel/Vertex constants
//	//XRAY_ALIGN(16)	R_constants			constants;
//	res_constant_table*				m_const_table;//ctable;
//
//	// Shaders/State
//	ID3DState*						m_state;//state;
//	ID3DPixelShader*				m_ps;//ps;
//	ID3DVertexShader*				m_vs;//vs;
//#ifdef	USE_DX10
//	ID3DGeometryShader*				m_gs;//gs;
//#endif	//	USE_DX10
//
//#ifdef DEBUG
//	LPCSTR							ps_name;
//	LPCSTR							vs_name;
//#ifdef	USE_DX10
//	LPCSTR							gs_name;
//#endif	//	USE_DX10
//#endif
//	u32								stencil_enable;
//	u32								stencil_func;
//	u32								stencil_ref;
//	u32								stencil_mask;
//	u32								stencil_writemask;
//	u32								stencil_fail;
//	u32								stencil_pass;
//	u32								stencil_zfail;
//	u32								colorwrite_mask;
//	u32								cull_mode;
//	u32								z_enable;
//	u32								z_func;
//	u32								alpha_ref;
//
//	// Lists
//	STextureList*					T;
//	SMatrixList*					M;
//	SConstantList*					C;
//
//	// Lists-expanded
//	CTexture*						textures_ps	[mtMaxPixelShaderTextures];	// stages
//	//CTexture*						textures_vs	[5	];	// dmap + 4 vs
//	CTexture*						textures_vs	[mtMaxVertexShaderTextures];	// 4 vs
//#ifdef	USE_DX10
//	CTexture*						textures_gs	[mtMaxGeometryShaderTextures];	// 4 vs
//#endif	//	USE_DX10
//
//	void							invalidate	();
//public:
//	struct _stats
//	{
//		u32								polys;
//		u32								verts;
//		u32								calls;
//		u32								vs;
//		u32								ps;
//#ifdef	DEBUG
//		u32								decl;
//		u32								vb;
//		u32								ib;
//		u32								states;			// Number of times the shader-state changes
//		u32								textures;		// Number of times the shader-tex changes
//		u32								matrices;		// Number of times the shader-xform changes
//		u32								constants;		// Number of times the shader-consts changes
//#endif
//		u32								xforms;
//		u32								target_rt;
//		u32								target_zb;
//
//		R_statistics					r	;
//	}									stat;
//public:
//	IC	CTexture*					get_ActiveTexture			(u32 stage)
//	{
//		if (stage<CTexture::rstVertex)			return textures_ps[stage];
//		else if (stage<CTexture::rstGeometry)	return textures_vs[stage-CTexture::rstVertex];
//#ifdef	USE_DX10
//		else									return textures_gs[stage-CTexture::rstGeometry];
//#else	//	USE_DX10
//		VERIFY(!"Invalid texture stage");
//		return 0;
//#endif	//	USE_DX10
//	}
//
//#ifdef	USE_DX10
//	IC	void						get_ConstantDirect	(shared_str& n, u32 DataSize, void** pVData, void** pGData, void** pPData);
//#else	//USE_DX10
//	//IC	R_constant_array&			get_ConstantCache_Vertex	()			{ return constants.a_vertex;	}
//	//IC	R_constant_array&			get_ConstantCache_Pixel		()			{ return constants.a_pixel;		}
//#endif	//	USE_DX10

	// API
	//IC	void						set_xform			(u32 ID, const Fmatrix& M);
	//IC	void						set_xform_world		(const Fmatrix& M);
	//IC	void						set_xform_view		(const Fmatrix& M);
	//IC	void						set_xform_project	(const Fmatrix& M);
	//IC	const Fmatrix&				get_xform_world		();
	//IC	const Fmatrix&				get_xform_view		();
	//IC	const Fmatrix&				get_xform_project	();

//	void						set_RT				(ID3DRenderTargetView* RT, u32 ID=0);
//	void						set_ZB				(ID3DDepthStencilView* ZB);
//	ID3DRenderTargetView*		get_RT				(u32 ID=0);
//	ID3DDepthStencilView*		get_ZB				();
//
//	void						set_Constants		(R_constant_table* C);
//	void						set_Constants		(ref_ctable& C)						{ set_Constants(&*C);			}
//
//	void						set_Textures		(STextureList* T);
//	void						set_Textures		(ref_texture_list& T)				{ set_Textures(&*T);			}
//
//	void						set_Element			(ShaderElement* S, u32	pass=0);
//	void						set_Element			(ref_selement& S, u32	pass=0)		{ set_Element(&*S,pass);		}
//
//	void						set_Shader			(Shader* S, u32 pass=0);
//	void						set_Shader			(ref_shader& S, u32 pass=0)			{ set_Shader(&*S,pass);			}
//
//	void						set_States			(ID3DState* _state);
//	void						set_States			(ref_state& _state)					{ set_States(_state->state);	}
//
//#ifdef	USE_DX10
//	ICF  void						set_Format			(SDeclaration* _decl);
//#else	//	USE_DX10
//	void						set_Format			(IDirect3DVertexDeclaration9* _decl);
//#endif	//	USE_DX10
//
//	void						set_PS				(ID3DPixelShader* _ps, LPCSTR _n=0);
//	void						set_PS				(ref_ps& _ps)						{ set_PS(_ps->ps,_ps->cName.c_str());				}
//
//#ifdef	USE_DX10
//	void						set_GS				(ID3DGeometryShader* _gs, LPCSTR _n=0);
//	void						set_GS				(ref_gs& _gs)						{ set_GS(_gs->gs,_gs->cName.c_str());				}
//#endif	//	USE_DX10
//
//	void						set_VS				(ref_vs& _vs);
//#ifdef	USE_DX10
//	void						set_VS				(SVS* _vs);
//protected:	//	In DX10 we need input shader signature which is stored in ref_vs
//#endif	//	USE_DX10
//	void						set_VS				(ID3DVertexShader* _vs, LPCSTR _n=0);
//#ifdef	USE_DX10
//public:
//#endif	//	USE_DX10

	//void						set_Vertices		(ID3DVertexBuffer* _vb, u32 _vb_stride);
	//void						set_Indices			(ID3DIndexBuffer* _ib);
	//void						set_Geometry		(SGeometry* _geom);
	//void						set_Geometry		(ref_geom& _geom)					{	set_Geometry(&*_geom);		}
	//void						set_Stencil			(u32 _enable, u32 _func=D3DCMP_ALWAYS, u32 _ref=0x00, u32 _mask=0x00, u32 _writemask=0x00, u32 _fail=D3DSTENCILOP_KEEP, u32 _pass=D3DSTENCILOP_KEEP, u32 _zfail=D3DSTENCILOP_KEEP);
	//void						set_Z				(u32 _enable);
	//void						set_ZFunc			(u32 _func);
	//void						set_AlphaRef		(u32 _value);
	//void						set_ColorWriteEnable(u32 _mask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	//void						set_CullMode		(u32 _mode);
	//u32							get_CullMode		(){return cull_mode;}
	//void						set_ClipPlanes		(u32 _enable, Fplane*	_planes=NULL, u32 count=0);
	//void						set_ClipPlanes		(u32 _enable, Fmatrix*	_xform =NULL, u32 fmask=0xff);
	//void						set_Scissor			(Irect*	rect=NULL);

	// constants
//	ICF	ref_constant				get_c				(LPCSTR			n)													{ if (ctable)	return ctable->get(n);else return 0;}
//	ICF	ref_constant				get_c				(shared_str&	n)													{ if (ctable)	return ctable->get(n);else return 0;}
//
//	// constants - direct (fast)
//	ICF	void						set_c				(R_constant* C, const Fmatrix& A)									{ if (C)		constants.set(C,A);					}
//	ICF	void						set_c				(R_constant* C, const Fvector4& A)									{ if (C)		constants.set(C,A);					}
//	ICF	void						set_c				(R_constant* C, float x, float y, float z, float w)					{ if (C)		constants.set(C,x,y,z,w);			}
//	ICF	void						set_ca				(R_constant* C, u32 e, const Fmatrix& A)							{ if (C)		constants.seta(C,e,A);				}
//	ICF	void						set_ca				(R_constant* C, u32 e, const Fvector4& A)							{ if (C)		constants.seta(C,e,A);				}
//	ICF	void						set_ca				(R_constant* C, u32 e, float x, float y, float z, float w)			{ if (C)		constants.seta(C,e,x,y,z,w);		}
//#ifdef	USE_DX10
//	ICF	void						set_c				(R_constant* C, float A)											{ if (C)		constants.set(C,A);					}
//	ICF	void						set_c				(R_constant* C, int A)												{ if (C)		constants.set(C,A);					}
//#endif	//	USE_DX10
//
//
//	// constants - LPCSTR (slow)
//	ICF	void						set_c				(LPCSTR n, const Fmatrix& A)										{ if(ctable)	set_c	(&*ctable->get(n),A);		}
//	ICF	void						set_c				(LPCSTR n, const Fvector4& A)										{ if(ctable)	set_c	(&*ctable->get(n),A);		}
//	ICF	void						set_c				(LPCSTR n, float x, float y, float z, float w)						{ if(ctable)	set_c	(&*ctable->get(n),x,y,z,w);	}
//	ICF	void						set_ca				(LPCSTR n, u32 e, const Fmatrix& A)									{ if(ctable)	set_ca	(&*ctable->get(n),e,A);		}
//	ICF	void						set_ca				(LPCSTR n, u32 e, const Fvector4& A)								{ if(ctable)	set_ca	(&*ctable->get(n),e,A);		}
//	ICF	void						set_ca				(LPCSTR n, u32 e, float x, float y, float z, float w)				{ if(ctable)	set_ca	(&*ctable->get(n),e,x,y,z,w);}
//#ifdef	USE_DX10
//	ICF	void						set_c				(LPCSTR n, float A)											{ if(ctable)	set_c	(&*ctable->get(n),A);		}
//	ICF	void						set_c				(LPCSTR n, int A)												{ if(ctable)	set_c	(&*ctable->get(n),A);		}
//#endif	//	USE_DX10
//
//	// constants - shared_str (average)
//	ICF	void						set_c				(shared_str& n, const Fmatrix& A)									{ if(ctable)	set_c	(&*ctable->get(n),A);			}
//	ICF	void						set_c				(shared_str& n, const Fvector4& A)									{ if(ctable)	set_c	(&*ctable->get(n),A);			}
//	ICF	void						set_c				(shared_str& n, float x, float y, float z, float w)					{ if(ctable)	set_c	(&*ctable->get(n),x,y,z,w);	}
//	ICF	void						set_ca				(shared_str& n, u32 e, const Fmatrix& A)							{ if(ctable)	set_ca	(&*ctable->get(n),e,A);		}
//	ICF	void						set_ca				(shared_str& n, u32 e, const Fvector4& A)							{ if(ctable)	set_ca	(&*ctable->get(n),e,A);		}
//	ICF	void						set_ca				(shared_str& n, u32 e, float x, float y, float z, float w)			{ if(ctable)	set_ca	(&*ctable->get(n),e,x,y,z,w);}
//#ifdef	USE_DX10
//	ICF	void						set_c				(shared_str& n, float A)											{ if(ctable)	set_c	(&*ctable->get(n),A);		}
//	ICF	void						set_c				(shared_str& n, int A)												{ if(ctable)	set_c	(&*ctable->get(n),A);		}
//#endif	//	USE_DX10

	//void	Render(D3DPRIMITIVETYPE T, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC);
	//void	Render(D3DPRIMITIVETYPE T, u32 startV, u32 PC);

	// Device create / destroy / frame signaling
	//void							RestoreQuadIBData	();	// Igor: is used to test bug with rain, particles corruption
	//void							CreateQuadIB		();
	//void							OnFrameBegin		();
	//void							OnFrameEnd			();

	//backend() {invalidate();};

//#ifdef	USE_DX10
//private:
	//	DirectX 10 internal functionality
	//void CreateConstantBuffers();
	//void DestroyConstantBuffers();
	//void	ApplyVertexLayout();
	//void	ApplyRTandZB();
	//void	ApplyPrimitieTopology( D3D10_PRIMITIVE_TOPOLOGY Topology );
	//bool	CBuffersNeedUpdate(ref_cbuffer	buf1[MaxCBuffers], ref_cbuffer	buf2[MaxCBuffers], u32	&uiMin, u32	&uiMax);

//private:
//	ID3DBlob*				m_pInputSignature;
//
//	bool					m_bChangedRTorZB;
//#endif	//	USE_DX10
}; // class backend

} // namespace render 
} // namespace xray 


#include "backend_inline.h"

#endif // #ifndef BACKEND_H_INCLUDED