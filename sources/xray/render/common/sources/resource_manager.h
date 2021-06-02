////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_MANAGER_H_INCLUDED
#define RESOURCE_MANAGER_H_INCLUDED

//using namespace xray::render::base;

#include <xray/resources.h>
#include <xray/resources_fs.h>
#include <xray/resources_queries_result.h>
#include <xray/resources_query_result.h>

//#include "res_common.h"
//#include "res_const_table.h"
//#include "res_atomic.h"
//#include ""

namespace xray {
namespace render {


struct shader_compilation_opts
{
	int	m_bones_count;
	int	m_msaa_samples_count;
};

class resource_manager : public quasi_singleton<resource_manager>
{
public:
	struct str_pred : public std::binary_function<char*, char*, bool>
	{
		inline bool operator()(LPCSTR x, LPCSTR y) const {return strcmp(x, y) < 0;}
	};

	typedef map<const char*, res_vs*, str_pred>	map_vs;
	typedef map<const char*, res_ps*, str_pred>	map_ps;
	typedef map<const char*, res_rt*, str_pred>	map_rt;
	typedef map<const char*, res_texture*, str_pred>	map_texture;

	typedef render::vector<u8> shader_source;
		
public:
	resource_manager(bool render_test_scene);
	~resource_manager();
	//void _GetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps);
	//void _DumpMemoryUsage();
	//void DBG_VerifyGeoms();
	//void DBG_VerifyTextures();
	//void Dump(bool bBrief);

	IDirect3DBaseTexture9* create_texture_platform(const u8* ptr, u32 size);
	res_texture*	create_texture(LPCSTR name);
	res_texture*	create_texture( char const* user_name, math::uint2 size, D3DFORMAT format = D3DFMT_DXT1, u32 usage = 0, u32 mip_levels = D3DX_DEFAULT);
	void			delete_texture(const res_texture* texture);

	bool			copy_texture_from_file( ref_texture dest_texture, math::rectangle<math::int2> dest_rect, char const* src_name);

	res_const_table*	create_const_table(res_const_table& proto);
	void				delete_const_table(const res_const_table* const_table);
	void				register_const_setup(shared_string name, const_setup_cb setup);
//
//#ifdef	USE_DX10
//	dx10ConstantBuffer*				_CreateConstantBuffer(ID3D10ShaderReflectionConstantBuffer* pTable);
//	void							_DeleteConstantBuffer(const dx10ConstantBuffer* pBuffer);
//
//	SInputSignature*				_CreateInputSignature(ID3DBlob* pBlob);
//	void							_DeleteInputSignature(const SInputSignature* pSignature);
//	SGS*							_CreateGS			(LPCSTR Name);
//	void							_DeleteGS			(const SGS*	GS	);
//#endif	//	USE_DX10

	res_rt*	create_rt(LPCSTR name, u32 w, u32 h, D3DFORMAT fmt);
	void	delete_rt(const res_rt*	rt);

	res_vs*	create_vs(const char* name, shader_defines_list& defines);
	void	delete_vs(const res_vs* vs);

	res_ps*	create_ps(const char* name, shader_defines_list& defines);
	void	delete_ps(const res_ps* ps);

	res_state*	create_state(simulator_states& state_code);
	void		delete_state(const res_state* state);

	res_declaration*	create_decl(D3DVERTEXELEMENT9 const* dcl);
	res_declaration*	create_decl(DWORD fvf);
	void				delete_decl(res_declaration const* dcl);

	//void			OnDeviceCreate			(/*IReader* F*/);
	//void			OnDeviceCreate			(LPCSTR name);
	//void			OnDeviceDestroy			(BOOL   bKeepTextures);

	void			evict();

	//void			reset_begin				();
	//void			reset_end				();

	//void	deferred_load(bool is_deffered) {m_deffered_loading = is_deffered;}
	//void	deferred_upload();

	//void			StoreNecessaryTextures	();
	//void			DestroyNecessaryTextures();

	static const char* get_shader_path();

public:
	typedef map<fs::path_string, shader_source>	map_shader_sources;

private:
	void load_shader_sources		( bool render_test_scene );
	
	void process_files				(resources::fs_iterator it);
	
	void load_raw_files				(resources::queries_result& data);
	void load_texture				(resources::queries_result& data);
	void copy_texture_from_file_cb	(resources::queries_result& data);

private:
	map_vs	m_vs_registry;
	map_ps	m_ps_registry;
	map_rt	m_rt_registry;
	map_texture	m_texture_registry;

	render::vector<res_const_table*>	m_const_tables;
	render::vector<res_state*>			m_states;
	render::vector<res_declaration*>	m_declarations;

	render::vector<u32>	m_vs_ids;
	render::vector<u32>	m_ps_ids;
	render::vector<u32>	m_gs_ids;

	map_shader_sources		m_sources;
	render::vector<fs::path_string>	m_files_list;

	bool	m_loading_incomplete;

	const_bindings	m_bindings;
	//bool	m_deffered_loading;
}; // class resource_manager

} // namespace render 
} // namespace xray 


#endif // #ifndef RESOURCE_MANAGER_H_INCLUDED