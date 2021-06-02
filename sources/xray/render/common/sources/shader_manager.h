////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SHADER_MANAGER_H_INCLUDED
#define SHADER_MANAGER_H_INCLUDED

#include "blender.h"

namespace xray {
namespace render {


class shader_manager: public quasi_singleton<shader_manager>
{
public:
	shader_manager();
	~shader_manager();

	res_texture_list*		create_texture_list(res_texture_list& tex_list);
	void					delete_texture_list(const res_texture_list* tex_list);

	res_shader_technique*	create_shader_technique(const res_shader_technique& element);
	void					delete_shader_technique(const res_shader_technique* element);

//
//	Shader*							_cpp_Create			(LPCSTR		s_shader,	LPCSTR s_textures=0,	LPCSTR s_constants=0,	LPCSTR s_matrices=0);
//	Shader*							_cpp_Create			(IBlender*	B,			LPCSTR s_shader=0,		LPCSTR s_textures=0,	LPCSTR s_constants=0, LPCSTR s_matrices=0);
//	Shader*							_lua_Create			(LPCSTR		s_shader,	LPCSTR s_textures);
//	BOOL							_lua_HasShader		(LPCSTR		s_shader);
//

	//res_shader*	create(LPCSTR shader=0, LPCSTR textures=0);
	res_shader*	create_shader(LPCSTR shader, LPCSTR textures=0);
	res_shader*	create_shader(blender* desc, LPCSTR shader=0, LPCSTR textures=0);
	void		delete_shader(res_shader* shader);
//	void							Delete				(const Shader*		S	);

	res_geometry*	create_geometry(DWORD fvf, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	res_geometry*	create_geometry(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	res_geometry*	create_geometry(ref_declaration dcl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void			delete_geometry(res_geometry* geom);

	
	res_pass*	create_pass(const res_pass& pass);
	void		delete_pass(const res_pass* pass);

//#ifdef	USE_DX10
//	SPass*							_CreatePass			(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_gs& _gs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C);
//#else	//	USE_DX10
//	SPass*							_CreatePass			(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C);
//#endif	//	USE_DX10
//	void							_DeletePass			(const SPass* P	);
//
//	void							_ParseList			(sh_list& dest, LPCSTR names);
//	IBlender*						_GetBlender			(LPCSTR Name);
//	IBlender* 						_FindBlender		(LPCSTR Name);
//
//	map_Blender&					_GetBlenders		()		{	return m_blenders;	}
//
//	void							LS_Load				();//lua script load
//	void							LS_Unload			();//lua script unload
//

private:
	typedef map<fixed_string<128>, blender*>	map_blenders;
	typedef	map_blenders::iterator				map_blenders_it;
	
	typedef render::vector<res_geometry*>				vec_geoms;
	typedef vec_geoms::iterator					vec_geoms_it;

private:
	blender* find_blender(LPCSTR name, LPCSTR texture /* "texture" fortesting only */  );


	void init_blenders();
	void load_blenders();
	void load_raw_file(resources::queries_result& data);

	blender* make_blender(u64 cls);

private:
	render::vector<res_texture_list*>	m_texture_lists;
	render::vector<res_pass*>			m_passes;
	render::vector<res_shader*>			m_shaders;
	render::vector<res_shader_technique*>	m_sh_techniques;
	vec_geoms					m_geometries;

	map_blenders	m_blenders;
	map_blenders	m_blenders_by_texture;

	bool	m_loading_incomplete;
}; // class shader_manager

} // namespace render 
} // namespace xray 


#endif // #ifndef SHADER_MANAGER_H_INCLUDED