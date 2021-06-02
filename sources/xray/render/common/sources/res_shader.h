////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_SHADER_H_INCLUDED
#define RES_SHADER_H_INCLUDED

namespace xray {
namespace render {


typedef render::vector<shared_string>	sh_list;
//class	blender_compile;
//class	blender;

//#define	SHADER_PASSES_MAX		2

//#pragma pack(push,4)

class res_texture_list	:
	public res_flagged,
	public render::vector< std::pair<u32,ref_texture> >,
	public res_id
{
public:
	typedef render::vector< std::pair<u32,ref_texture> > inherited_vec;

public:
	~res_texture_list();

	bool equal(const res_texture_list& base) const;

	void apply();

	void clear();
	void clear_not_free();

	//	Avoid using this function.
	//	If possible use precompiled texture list.
	u32 find_texture_stage(const shared_string &tex_name) const;
};
typedef intrusive_ptr<res_texture_list, res_base, threading::single_threading_policy> ref_texture_list;

class base_vertex_buffer {
public:
	virtual	void lock	() = 0;
	virtual	void unlock	() = 0;
};

class res_geometry : public res_flagged,
	public boost::noncopyable
{
public:
	res_geometry(IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib, ref_declaration dcl, u32 stride):
		m_vb(vb), m_ib(ib), m_dcl(dcl), m_vb_stride(stride)
	{}
	
	~res_geometry();
	
	bool equal(const res_geometry& other) const;
	
	void apply();
	
	u32	get_stride() {return m_vb_stride;}

private:
	ID3DVertexBuffer*	m_vb;			//vb;
	ID3DIndexBuffer*	m_ib;			//ib;
	ref_declaration		m_dcl;			//dcl;
	u32					m_vb_stride;	//vb_stride;
};
typedef	intrusive_ptr<res_geometry, res_base, threading::single_threading_policy>	ref_geometry;

//////////////////////////////////////////////////////////////////////////
class res_pass : public res_flagged, public boost::noncopyable
{
public:
	res_pass(const ref_vs& vs, const ref_ps& ps, 
			const ref_const_table& const_table,
			const ref_state& state,
			const ref_texture_list& tex_list):
	  m_state(state), m_ps(ps), m_vs(vs), m_constants(const_table), m_tex_list(tex_list)
	{}

	~res_pass			();

#	ifdef	USE_DX10
	//bool equal(ref_state& state, ref_ps& ps, ref_vs& vs, ref_gs& gs, ref_ctable& ctable, ref_texture_list& tex_list, ref_matrix_list& mat_list, ref_constant_list& const_list);
#	else	//	USE_DX10
	//bool equal(ref_state& state, ref_ps& ps, ref_vs& vs, ref_const_table& ctable, ref_texture_list& tex_list);
	bool equal(const res_pass& other) const;
	bool equal(const res_pass* other) const {return equal(*other);}
#	endif	USE_DX10

	void apply();

	u64 get_priority()
	{
#ifdef DEBUG
		const int vs_id_bit_width = 7;
#endif // #ifdef DEBUG
		const int ps_id_bit_width = 7;
		const int state_id_bit_width = 7;
		const int tex_list_id_bit_width = 12;

		u64 qword = 0;
		u64 vs_id = m_vs->get_id();
		ASSERT(vs_id < (1<<vs_id_bit_width));
		qword = vs_id;

		qword <<= ps_id_bit_width;
		u64 ps_id = m_ps->get_id();
		ASSERT(ps_id < (1<<ps_id_bit_width));
		qword |= ps_id;

		qword <<= state_id_bit_width;
		u64 state_id = m_state->get_id();
		ASSERT(state_id < (1<<state_id_bit_width));
		qword |= state_id;

		qword <<= tex_list_id_bit_width;
		u64 tlist_id = m_tex_list->get_id();
		ASSERT(tlist_id < (1<<tex_list_id_bit_width));
		qword |= tlist_id;
		
		return qword;
	}

public:
	ref_state			m_state;			//state;		// Generic state, like Z-Buffering, samplers, etc
	ref_ps				m_ps;				//ps;			// may be NULL = FFP, in that case "state" must contain TSS setup
	ref_vs				m_vs;				//vs;			// may be NULL = FFP, in that case "state" must contain RS setup, *and* FVF-compatible declaration must be used

#	ifdef	USE_DX10
	ref_gs				m_gs;				//gs;			// may be NULL = don't use geometry shader at all
#	endif	//	USE_DX10

	ref_const_table		m_constants;		//constants;	// may be NULL
	ref_texture_list	m_tex_list;			//T;
};
typedef intrusive_ptr<res_pass, res_base, threading::single_threading_policy>	ref_pass;

class res_shader_technique : public res_flagged/*, public boost::noncopyable*/
{
public:
	struct sflags
	{
		u32	priority		: 2; //iPriority
		u32	strict_b2f		: 1; //bStrictB2F
		u32	has_emissive	: 1; //bEmissive
		u32	has_distort		: 1; //bDistort
		u32	has_wmark		: 1; //bWmark
	};

public:
	res_shader_technique	() {}
	~res_shader_technique	();

	bool equal(const res_shader_technique& other) const;
	bool equal(const res_shader_technique* other) const {return equal(*other);}

public:
	static const int SHADER_PASSES_MAX = 2;

	sflags										m_flags;
	fixed_vector<ref_pass,SHADER_PASSES_MAX>	m_passes;
};

typedef intrusive_ptr<res_shader_technique, res_base, threading::single_threading_policy>	ref_shader_technique;

//////////////////////////////////////////////////////////////////////////
class res_shader :
	public res_flagged/*,
	public boost::noncopyable*/
{
public:
	~res_shader();

	bool equal(const res_shader& other) const;
	bool equal(const res_shader* other) const {return equal(*other);}
	
	void apply(u32 technique_id = 0, u32 pass_id = 0) {select_technique(technique_id); apply_pass(pass_id);}

	void select_technique(u32 technique_id) {ASSERT(technique_id<m_sh_techniques.size()); m_cur_technique = technique_id;}
	void apply_pass(u32 pass_id);

public:
	static const int SHADER_TECHNIQUES_MAX = 6;

	u32	m_cur_technique;
	fixed_vector<ref_shader_technique, SHADER_TECHNIQUES_MAX> m_sh_techniques;
	//R1-0=norm_lod0(det), 1=norm_lod1(normal), 2=L_point, 3=L_spot, 4=L_for_models,	
	//R2-0=deffer, 1=norm_lod1(normal), 2=psm, 3=ssm, 4=dsm
};

typedef intrusive_ptr<res_shader, res_base, threading::single_threading_policy> ref_shader;
//class 	 ref_shader	: public intrusive_ptr<res_shader, res_base>
//{
//public:
//	ref_shader() {}
//	ref_shader(res_shader* s) {}
//	void create(LPCSTR shader=0, LPCSTR textures=0);
//	void create(blender* desc, LPCSTR shader=0, LPCSTR textures=0);
//	//void destroy()	{/* _set(NULL);		*/}
//};

//#define		SE_R2_NORMAL_HQ		0	// high quality/detail
//#define		SE_R2_NORMAL_LQ		1	// low quality
//#define		SE_R2_SHADOW		2	// shadow generation
//	E[3] - can use for night vision but need to extend SE_R1. Will need 
//	Extra shader element.
//	E[4] - distortion or self illumination(self emission). 
//	E[4] Can use for lightmap capturing.

//#pragma pack(pop)

} // namespace render 
} // namespace xray 


#endif // #ifndef RES_SHADER_H_INCLUDED
