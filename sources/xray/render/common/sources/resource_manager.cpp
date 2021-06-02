////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "hw_wrapper.h"
#include "resource_manager.h"
#include "manager_common_inline.h"

namespace xray {
namespace render {

void register_texture_cook();
void unregister_texture_cook();

HRESULT	shader_compile(
	LPCSTR						name,
	LPCSTR						source,
	UINT						source_len,
	const shader_defines_list&	defines,
	LPD3DXINCLUDE				includes,
	LPCSTR						entry_point,
	LPCSTR						target,
	DWORD						flags,
	LPD3DXBUFFER*				out_shader,
	LPD3DXBUFFER*				out_error_msgs,
	LPD3DXCONSTANTTABLE*		out_const_table)
{
	XRAY_UNREFERENCED_PARAMETER	( name );

#ifdef	D3DXSHADER_USE_LEGACY_D3DX9_31_DLL	//	December 2006 and later
	flags |= D3DXSHADER_USE_LEGACY_D3DX9_31_DLL;
#endif

	HRESULT	result = D3DXCompileShader(source, source_len,
			defines.empty() ? NULL : &*defines.begin(), includes, entry_point,
			target, flags, out_shader, out_error_msgs, out_const_table);

	//if (SUCCEEDED(_result) && o.disasm)
	//{
	//	ID3DXBuffer*		code	= *((LPD3DXBUFFER*)_ppShader);
	//	ID3DXBuffer*		disasm	= 0;
	//	D3DXDisassembleShader		(LPDWORD(code->GetBufferPointer()), FALSE, 0, &disasm );
	//	string_path			dname;
	//	strconcat			(sizeof(dname),dname,"disasm\\",name,('v'==pTarget[0])?".vs":".ps" );
	//	IWriter*			W		= FS.w_open("$logs$",dname);
	//	W->w				(disasm->GetBufferPointer(),disasm->GetBufferSize());
	//	FS.w_close			(W);
	//	_RELEASE			(disasm);
	//}
	return result;
}

class includes_handler : public ID3DXInclude
{
public:
	includes_handler(resource_manager::map_shader_sources* source_map): m_sources(source_map)
	{
	}

	HRESULT __stdcall	Open	(D3DXINCLUDE_TYPE include_type, LPCSTR file_name, LPCVOID parent_data, LPCVOID *data_ptr, UINT *size_ptr)
	{
		XRAY_UNREFERENCED_PARAMETERS	( parent_data, include_type );

		fs::path_string	path = resource_manager::get_ref().get_shader_path();
		path += "/";
		path += file_name;
		
		utils::fix_path(path.get_buffer());

		resource_manager::map_shader_sources::iterator	it  = m_sources->find(path),
														end = m_sources->end();

		if (it == end)
		{
			return E_FAIL;
		}

		//string_path				pname;
		//strconcat				(sizeof(pname),pname,::Render->getShaderPath(),pFileName);
		//IReader*		R		= FS.r_open	("$game_shaders$",pname);
		//if (0==R)				{
		//	// possibly in shared directory or somewhere else - open directly
		//	R					= FS.r_open	("$game_shaders$",pFileName);
		//	if (0==R)			return			E_FAIL;
		//}

		//// duplicate and zero-terminate
		//u32				size	= R->length();
		//u8*				data	= xr_alloc<u8>	(size + 1);
		//CopyMemory			(data,R->pointer(),size);
		//data[size]				= 0;
		//FS.r_close				(R);

		*data_ptr = &it->second[0];
		*size_ptr = it->second.size();

		return	D3D_OK;
	}
	HRESULT __stdcall	Close	(LPCVOID	pData)
	{
		XRAY_UNREFERENCED_PARAMETER	( pData );
		//xr_free	(pData);
		return	D3D_OK;
	}

private:
	resource_manager::map_shader_sources*	m_sources;
};

//Try to get rid of this!!!!!!
void make_defines(shader_defines_list& defines)
{
	//{
	//	sprintf						(c_smapsize,"%d",u32(o.smapsize));
	//	defines[def_it].Name		=	"SMAP_size";
	//	defines[def_it].Definition	=	c_smapsize;
	//	def_it						++	;
	//}
	if (hw_wrapper::get_ref().o.fp16_filter)
	{
		defines.push_back(D3DXMACRO());
		defines.back().Name       = "FP16_FILTER";
		defines.back().Definition = "1";
	}
	if (hw_wrapper::get_ref().o.fp16_blend)
	{
		defines.push_back(D3DXMACRO());
		defines.back().Name       = "FP16_BLEND";
		defines.back().Definition = "1";
	}
	//if (o.HW_smap)			{
	//	defines[def_it].Name		=	"USE_HWSMAP";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (o.HW_smap_PCF)			{
	//	defines[def_it].Name		=	"USE_HWSMAP_PCF";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (o.HW_smap_FETCH4)			{
	//	defines[def_it].Name		=	"USE_FETCH4";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (o.sjitter)			{
	//	defines[def_it].Name		=	"USE_SJITTER";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (ps_r2_lighting == 1)
	//{
	//	defines[def_it].Name		=	"USE_OREN_NAYAR";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//else if (ps_r2_lighting == 2)
	//{
	//	defines[def_it].Name		=	"USE_COOK_TORRANCE";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (ps_r2_ls_flags_ext.test(R2FLAGEXT_TWEAK_MATERIAL))
	//{
	//	defines[def_it].Name		=	"MATERIAL_TWEAK_MODE";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	if (hw_wrapper::get_ref().get_caps().raster_major >= 3)
	{
		defines.push_back(D3DXMACRO());
		defines.back().Name       = "USE_BRANCHING";
		defines.back().Definition = "1";
	}
	//if (HW.Caps.geometry.bVTF)	{
	//	defines[def_it].Name		=	"USE_VTF";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (o.Tshadows)			{
	//	defines[def_it].Name		=	"USE_TSHADOWS";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (o.mblur)			{
	//	defines[def_it].Name		=	"USE_MBLUR";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (o.sunfilter)		{
	//	defines[def_it].Name		=	"USE_SUNFILTER";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (o.sunstatic)		{
	//	defines[def_it].Name		=	"USE_R2_STATIC_SUN";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (o.forcegloss)		{
	//	sprintf						(c_gloss,"%f",o.forcegloss_v);
	//	defines[def_it].Name		=	"FORCE_GLOSS";
	//	defines[def_it].Definition	=	c_gloss;
	//	def_it						++	;
	//}
	//if (o.forceskinw)		{
	//	defines[def_it].Name		=	"SKIN_COLOR";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}

	//// skinning
	//if (m_skinning<0)		{
	//	defines[def_it].Name		=	"SKIN_NONE";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++	;
	//}
	//if (0==m_skinning)		{
	//	defines[def_it].Name		=	"SKIN_0";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}
	//if (1==m_skinning)		{
	//	defines[def_it].Name		=	"SKIN_1";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}
	//if (2==m_skinning)		{
	//	defines[def_it].Name		=	"SKIN_2";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}
	//if (3==m_skinning)		{
	//	defines[def_it].Name		=	"SKIN_3";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}
	//if (4==m_skinning)		{
	//	defines[def_it].Name		=	"SKIN_4";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}

	//if (o.ssao_blur_on)
	//{
	//	defines[def_it].Name		=	"USE_SSAO_BLUR";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}

	////	Igor: need restart options
	//if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_SOFT_WATER))
	//{
	//	defines[def_it].Name		=	"USE_SOFT_WATER";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}

	//if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_SOFT_PARTICLES))
	//{
	//	defines[def_it].Name		=	"USE_SOFT_PARTICLES";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}

	//if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_DOF))
	//{
	//	defines[def_it].Name		=	"USE_DOF";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}

	//if (RImplementation.o.advancedpp && ps_r_sun_shafts)
	//{
	//	sprintf_s					(c_sun_shafts,"%d",ps_r_sun_shafts);
	//	defines[def_it].Name		=	"SUN_SHAFTS_QUALITY";
	//	defines[def_it].Definition	=	c_sun_shafts;
	//	def_it						++;
	//}

	//if (RImplementation.o.advancedpp && ps_r_ssao)
	//{
	//	sprintf_s					(c_ssao,"%d",ps_r_ssao);
	//	defines[def_it].Name		=	"SSAO_QUALITY";
	//	defines[def_it].Definition	=	c_ssao;
	//	def_it						++;
	//}

	//if (RImplementation.o.advancedpp && ps_r_sun_quality)
	//{
	//	sprintf_s					(c_sun_quality,"%d",ps_r_sun_quality);
	//	defines[def_it].Name		=	"SUN_QUALITY";
	//	defines[def_it].Definition	=	c_sun_quality;
	//	def_it						++;
	//}

	//if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_STEEP_PARALLAX))
	//{
	//	defines[def_it].Name		=	"ALLOW_STEEPPARALLAX";
	//	defines[def_it].Definition	=	"1";
	//	def_it						++;
	//}

	defines.push_back(D3DXMACRO());
	defines.back().Name     = 0;
	defines.back().Definition = 0;
}

resource_manager::resource_manager(bool render_test_scene) :
	m_loading_incomplete(false)
{
	register_texture_cook();

	load_shader_sources	( render_test_scene );
}

resource_manager::~resource_manager()
{
	unregister_texture_cook();
}

res_vs*	resource_manager::create_vs(const char* name, shader_defines_list& defines)
{
	fs::path_string	reg_name(name);
	
	//if (0 == local_opts.m_bones_count)	reg_name += "_0";
	//if (1 == local_opts.m_bones_count)	reg_name += "_1";
	//if (2 == local_opts.m_bones_count)	reg_name += "_2";
	//if (3 == local_opts.m_bones_count)	reg_name += "_3";
	//if (4 == local_opts.m_bones_count)	reg_name += "_4";
	
	map_vs::iterator it	= m_vs_registry.find(reg_name.get_buffer());
	
	if (it != m_vs_registry.end())
	{
		return it->second;
	}
	else
	{
		res_vs* vs	= NEW(res_vs);
		vs->make_registered();
		m_vs_registry.insert(mk_pair(vs->set_name(reg_name.get_buffer()), vs));
		vs->set_id(gen_id(m_vs_registry, m_vs_ids));
		
		if (0 == _stricmp(name, "null"))
		{
			//vs->m_vs = NULL;
			return vs;
		}

		includes_handler			includer(&m_sources);
		LPD3DXBUFFER				shader_code	= NULL;
		LPD3DXBUFFER				error_buf	= NULL;

		u32 pos = reg_name.find("(");
		fs::path_string nm(pos==fs::path_string::npos ? name : reg_name.substr(0, pos));

		fs::path_string				sh_name(get_shader_path());
		sh_name.appendf("/%s.vs", nm.c_str());

		LPCSTR						c_target = "vs_3_0";
		LPCSTR						c_entry  = "main";
	
		//shader_defines_list	defines;
		//make_defines(local_opts, defines);
		
		map_shader_sources::iterator src_it = m_sources.find(sh_name);
		ASSERT(src_it != m_sources.end()); //What to do if file not found?????
		ASSERT(!src_it->second.empty());

		HRESULT hr = shader_compile(name,
			(LPCSTR)&src_it->second[0],
			src_it->second.size(), defines, 
			&includer,
			c_entry,
			c_target,
			D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR
			/*| D3DXSHADER_PREFER_FLOW_CONTROL*/,
			&shader_code, &error_buf, NULL);

		if (SUCCEEDED(hr))
		{
			hr = vs->create_hw_shader(shader_code);
			if (FAILED(hr))
			{
				LOG_ERROR("!VS: %s", name);
				LOG_ERROR("shader creation failed");
				FATAL("!Shader is not created");
			}
		}
		else
		{
			ASSERT(error_buf);
			LOG_ERROR("! VS: %s", name);
			pcstr err_msg = (LPCSTR)error_buf->GetBufferPointer();
			LOG_ERROR("!error: %s", err_msg);
 			FATAL("!Shader is not created");
		}

		safe_release(shader_code);
		safe_release(error_buf);

		//R_CHK		(hr);
		return		vs;
	}
}

void resource_manager::delete_vs(const res_vs* vs)
{
	if (!vs->is_registered())
		return;

	LPSTR name = LPSTR(vs->m_name.c_str());
	map_vs::iterator it	= m_vs_registry.find(name);

	if (it != m_vs_registry.end())
	{
		m_vs_ids.push_back(it->second->get_id());
		m_vs_registry.erase(it);
		return;
	}

	LOG_ERROR("! ERROR: Failed to find compiled vertex-shader '%s'",vs->m_name.c_str());
}

res_ps* resource_manager::create_ps(LPCSTR name, shader_defines_list& defines)
{
	fs::path_string		reg_name(name);
	map_ps::iterator	it	= m_ps_registry.find(reg_name.get_buffer());

	if (it != m_ps_registry.end())
	{
		return it->second;
	}
	else
	{
		res_ps* ps	= NEW(res_ps);
		ps->make_registered();
		m_ps_registry.insert(mk_pair(ps->set_name(reg_name.get_buffer()), ps));
		ps->set_id(gen_id(m_ps_registry, m_ps_ids));

		if (0 == _stricmp(name, "null"))
		{
			//ps->m_ps = NULL;
			return ps;
		}

		includes_handler			includer(&m_sources);
		LPD3DXBUFFER				shader_code	= NULL;
		LPD3DXBUFFER				error_buf	= NULL;
		fs::path_string				sh_name(get_shader_path());
		u32 pos = reg_name.find("(");
		fs::path_string nm(pos==fs::path_string::npos ? name : reg_name.substr(0, pos));
		sh_name.appendf("/%s.ps", nm.c_str());

		LPCSTR						c_target = "ps_3_0";
		LPCSTR						c_entry  = "main";

		//shader_defines_list	defines;
		//make_defines(local_opts, defines);

		map_shader_sources::iterator src_it = m_sources.find(sh_name);
		ASSERT(src_it != m_sources.end()); //What to do if file not found?????
		ASSERT(!src_it->second.empty());

		HRESULT hr = shader_compile(name,
			(LPCSTR)&src_it->second[0],
			src_it->second.size(), defines, 
			&includer,
			c_entry,
			c_target,
			D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR
			/*| D3DXSHADER_PREFER_FLOW_CONTROL*/,
			&shader_code, &error_buf, NULL);

		if (SUCCEEDED(hr))
		{
			hr = ps->create_hw_shader(shader_code);
			if (FAILED(hr))
			{
				LOG_ERROR("!PS: %s", name);
				LOG_ERROR("!shader creation failed");
				FATAL("!Shader is not created");
			}
		}
		else
		{
			ASSERT(error_buf);
			LOG_ERROR("!PS: %s", name);
			pcstr err_msg = (LPCSTR)error_buf->GetBufferPointer();
			LOG_ERROR("!error: %s", err_msg);
			FATAL("!Shader is not created");
		}

		safe_release(shader_code);
		safe_release(error_buf);

		return		ps;
	}
}

void resource_manager::delete_ps(const res_ps* ps)
{
	if (!ps->is_registered())
		return;

	LPSTR name = LPSTR(ps->m_name.c_str());
	map_ps::iterator it	= m_ps_registry.find(name);

	if (it != m_ps_registry.end())
	{
		m_ps_ids.push_back(it->second->get_id());
		m_ps_registry.erase(it);
		return;
	}

	LOG_ERROR("! ERROR: Failed to find compiled pixel-shader '%s'",ps->m_name.c_str());
}

const char* resource_manager::get_shader_path()
{
	return "resources/shaders/sm_3_0";
}

void resource_manager::process_files(resources::fs_iterator	begin)
{
	vector<resources::fs_iterator>	stack;
	stack.push_back(begin);
	fs::path_string name;
	begin.get_full_path(name);
	vector<resources::request>	requests;
	while (!stack.empty())
	{
		resources::fs_iterator curDir = stack.back();
		stack.pop_back();
		
		resources::fs_iterator it  = curDir.children_begin(),
					end = curDir.children_end();

		for (; it != end; ++it)
		{
			fs::path_string name;
			if (it.is_folder())
			{
				stack.push_back(it);
			}
			else
			{
				fs::path_string	file_name;
				it.get_full_path(file_name);

				//u32 pos = file_name.size()-3;
				//if (file_name.substr(pos) == ".vs" || 
				//	file_name.substr(pos) == ".ps" ||
				//	file_name.substr(pos) == ".h"  ||
				//	file_name.substr(pos) == ".s")
					m_files_list.push_back(file_name);
			}
		}
	}

	if (m_files_list.empty())
	{
		m_loading_incomplete = false;
		return;
	}

	//to ensure that list does not reallocate
	for (u32 i = 0; i < m_files_list.size(); ++i)
	{
		resources::request	req;
		req.path = m_files_list[i].get_buffer();
		req.id   = resources::raw_data_class;
		requests.push_back(req);
	}

	resources::query_resources(&requests[0], requests.size(),
		boost::bind(&resource_manager::load_raw_files, this, _1),
		::xray::render::g_allocator);
}

void resource_manager::load_raw_files(resources::queries_result& data)
{
	ASSERT(data.is_successful());
	ASSERT(data.size() == m_files_list.size());

	m_loading_incomplete = false;

	for (u32 i = 0; i < data.size(); ++i)
	{
		resources::pinned_ptr_const<u8> src  (data[i].get_managed_resource());
		m_sources[m_files_list[i]].assign(src.c_ptr(), src.c_ptr() + src.size());
	}

	//release file names list
	{
		vector<fs::path_string>	empty;
		m_files_list.swap(empty);
	}
}


void resource_manager::load_shader_sources( bool const render_test_scene )
{
	m_loading_incomplete = true;

	if ( render_test_scene ) {
		resources::query_fs_iterator(get_shader_path(), 
			boost::bind(&resource_manager::process_files, this, _1),
			::xray::render::g_allocator);
	}
	else {
		resources::request requests[] = {
			{ "resources/shaders/sm_3_0/test.vs",	resources::raw_data_class },
			{ "resources/shaders/sm_3_0/test.ps",	resources::raw_data_class },
			{ "resources/shaders/sm_3_0/color.ps",	resources::raw_data_class },
			{ "resources/shaders/sm_3_0/common.h",	resources::raw_data_class },
			{ "resources/shaders/sm_3_0/shared/common.h",	resources::raw_data_class },
			{ "resources/shaders/sm_3_0/terrain.vs",	resources::raw_data_class },
			{ "resources/shaders/sm_3_0/terrain.ps",	resources::raw_data_class },

		};
		for ( u32 i=0, n=array_size(requests); i < n; ++i ) {
			m_files_list.push_back	( requests[i].path );
		}

		resources::query_resources(&requests[0], array_size(requests),
			boost::bind(&resource_manager::load_raw_files, this, _1),
			::xray::render::g_allocator);
	}

	while (m_loading_incomplete)
	{
		if ( threading::g_debug_single_thread )
			resources::tick				();

		resources::dispatch_callbacks	();
	}
}

res_const_table* resource_manager::create_const_table(res_const_table& proto)
{
	if (proto.empty())
		return NULL;

	m_bindings.apply(&proto);

	for (u32 it=0; it < m_const_tables.size(); ++it)
		if (m_const_tables[it]->equal(proto))
			return m_const_tables[it];

	m_const_tables.push_back(NEW(res_const_table)(proto));
	m_const_tables.back()->make_registered();
	
	return m_const_tables.back		();
}

void resource_manager::delete_const_table(const res_const_table* const_table)
{
	if (!const_table->is_registered())
		return;

	if (reclaim(m_const_tables, const_table))
		return;

	LOG_ERROR("!ERROR: Failed to find compiled constant-table");
}

void resource_manager::register_const_setup(shared_string name, const_setup_cb setup)
{
	m_bindings.add(name, setup);
}

res_texture* resource_manager::create_texture(LPCSTR name)
{
	// DBG_VerifyTextures	();
	if (0 == strcmp(name,"null"))
		return 0;

	R_ASSERT(name && name[0]);

	fs::path_string	tex_name(name);
	//fix_texture_name(tex_name);

#ifdef	DEBUG
	//simplify_texture(tex_name);
#endif	//	DEBUG

	map_texture::iterator it = m_texture_registry.find(tex_name.c_str());

	if (it!=m_texture_registry.end())
	{
		return	it->second;
	}
	else
	{
		res_texture* tex = NEW(res_texture);
		tex->make_registered();
		m_texture_registry.insert(mk_pair(tex->set_name(tex_name.c_str()), tex));
		
		if (strstr(tex->get_name(), "$user$") == 0)
		{
			fixed_string<512> path("resources/textures/");
			path += tex->get_name();
			path += ".dds";
			ASSERT(strstr(path.c_str(), "\\")==0);
			//fix_path(path.get_buffer());
			resources::query_resource(path.c_str(), 
				resources::raw_data_class, 
				boost::bind(&resource_manager::load_texture, this, _1),
				::xray::render::g_allocator);
		}

		//tex->preload();
		//if (Device.b_is_Ready && !bDeferredLoad)
		//	tex->load();
		return tex;
	}
}

void resource_manager::load_texture(resources::queries_result& data)
{
	ASSERT(data.is_successful());

	if (!data[0].get_managed_resource())
		return;

	resources::pinned_ptr_const<u8>	ptr	(data[0].get_managed_resource());

	fixed_string<512> name(data[0].get_requested_path());

	name = name.substr(19);
	name = name.substr(0, name.length()-4);

	ref_texture tex = create_texture(name.c_str());

	D3DXIMAGE_INFO	dds_info = {0};
	R_CHK(D3DXGetImageInfoFromFileInMemory(ptr.c_ptr(), ptr.size(), &dds_info));
	
	if (dds_info.ResourceType	== D3DRTYPE_CUBETEXTURE)
	{
		IDirect3DCubeTexture9*	cube_texture = 0;
		R_CHK(D3DXCreateCubeTextureFromFileInMemoryEx(
			hw_wrapper::get_ref().device(), ptr.c_ptr(), ptr.size(),
			D3DX_DEFAULT, dds_info.MipLevels,0,	dds_info.Format,
			D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,&dds_info,0,	&cube_texture));
		tex->set_surface(cube_texture);
		cube_texture->Release();
	}
	else
	{
		ID3DTexture2D*	d3d_texture = 0;
		R_CHK(D3DXCreateTextureFromFileInMemory(hw_wrapper::get_ref().device(), ptr.c_ptr(), ptr.size(), &d3d_texture));
		tex->set_surface(d3d_texture);
		d3d_texture->Release();
	}
}

res_texture* resource_manager::create_texture( char const* user_name, math::uint2 size, D3DFORMAT format, u32 usage, u32 mip_levels)
{
	XRAY_UNREFERENCED_PARAMETER	( mip_levels );

	res_texture* tex = NEW		(res_texture);
	tex->make_registered		();
	m_texture_registry.insert	( mk_pair(tex->set_name(user_name), tex));

	ID3DTexture2D*	d3d_texture = 0;
	R_CHK( D3DXCreateTexture(	hw_wrapper::get_ref().device(), 
								size.x, size.y, D3DX_DEFAULT /*needed to determine correct mip levels*/,
								usage,
								format,
								D3DPOOL_MANAGED,
								&d3d_texture));

	tex->set_surface		(d3d_texture);
	d3d_texture->Release	();

	return tex;
}

void resource_manager::delete_texture(const res_texture* texture)
{
	// DBG_VerifyTextures	();

	if (!texture->is_registered())
		return;

	LPSTR name = LPSTR(texture->get_name());
	map_texture::iterator it = m_texture_registry.find(name);

	if (it!=m_texture_registry.end())
	{
		m_texture_registry.erase(it);
		return;
	}

	LOG_ERROR("! ERROR: Failed to find texture surface '%s'", texture->get_name());
}


struct load_texture_delegate
{
	load_texture_delegate( ref_texture dest_texture, math::rectangle<int2> dest_rect, char const* user_name):
			dest_texture		( dest_texture),
			dest_rect			( dest_rect),
			user_name			( user_name)
			{}


	void execute(resources::queries_result& data)
	{
		ASSERT		(data.is_successful());

		ASSERT		( data[0].get_managed_resource());

		RECT rect;
		rect  = *( (RECT*)&dest_rect);
		RECT src_rect;
		src_rect.left = src_rect.top = 0;
		src_rect.right = dest_rect.width();
		src_rect.bottom = dest_rect.height();

		resources::pinned_ptr_const<u8> ptr  (data[0].get_managed_resource());

		ID3DSurface* surface;
		((ID3DTexture2D*)dest_texture->get_surface())->GetSurfaceLevel(0, &surface);
		D3DXLoadSurfaceFromFileInMemory( surface, NULL, &rect, ptr.c_ptr(), ptr.size(), &src_rect, D3DX_FILTER_NONE, 0, NULL);
		surface->Release();

		((ID3DTexture2D*)dest_texture->get_surface())->AddDirtyRect( &rect);
//		HRESULT res = ((ID3DTexture2D*)dest_texture->get_surface())->SetAutoGenFilterType(D3DTEXF_PYRAMIDALQUAD);
//		((ID3DTexture2D*)dest_texture->get_surface())->GenerateMipSubLevels();
		load_texture_delegate* d = this;
		DELETE				(d);
	}

private:
	math::rectangle<int2>	dest_rect;
	ref_texture				dest_texture;
	char const*				user_name;
};

bool resource_manager::copy_texture_from_file( ref_texture dest_texture, math::rectangle<int2> dest_rect, char const* src_name)
{
	fixed_string<512> path("resources/textures/");

	path += src_name;
	path += ".dds";
	ASSERT(strstr(path.c_str(), "\\")==0);
	

	load_texture_delegate* load_delegate = NEW (load_texture_delegate) ( dest_texture, dest_rect, src_name);

	resources::query_resource(path.c_str(), 
		resources::raw_data_class, 
		boost::bind(&load_texture_delegate::execute, load_delegate, _1),
		::xray::render::g_allocator);

	return true;
}

res_state* resource_manager::create_state(simulator_states& state_code)
{
	// Search equal state-code 
	for (u32 it = 0; it < m_states.size(); ++it)
	{
		res_state*	st = m_states[it];;
		simulator_states& base = st->m_state_code;
		if (base.equal(state_code))
			return st;
	}

	// Create new
	m_states.push_back(NEW(res_state)(state_code));
	m_states.back()->make_registered();
	m_states.back()->cache_states();
	m_states.back()->m_state_code	= state_code;
	m_states.back()->set_id(m_states.size());

	return m_states.back();
}
void resource_manager::delete_state(const res_state* state)
{
	if (!state->is_registered())
		return;

	if (reclaim_with_id(m_states, state))
		return;

	LOG_ERROR("!ERROR: Failed to find compiled stateblock");
}

res_declaration* resource_manager::create_decl(DWORD fvf)
{
	D3DVERTEXELEMENT9	dcl[MAX_FVF_DECL_SIZE];
	
	R_CHK(D3DXDeclaratorFromFVF(fvf, dcl));
	return create_decl(dcl);
}

res_declaration* resource_manager::create_decl(D3DVERTEXELEMENT9 const* dcl)
{
	// Search equal code
	for (u32 it=0; it<m_declarations.size(); it++)
	{
		res_declaration* decl = m_declarations[it];
		if (decl->equal(dcl))
			return decl;
	}

	// Create _new
	res_declaration* new_decl = NEW(res_declaration)(dcl);
	new_decl->make_registered();
	m_declarations.push_back(new_decl);

	return new_decl;
}

void resource_manager::delete_decl(const res_declaration* dcl)
{
	if (!dcl->is_registered())
		return;

	if (reclaim(m_declarations, dcl))
		return;

	LOG_ERROR("! ERROR: Failed to find compiled vertex-declarator");
}

res_rt*	resource_manager::create_rt(LPCSTR name, u32 w, u32 h, D3DFORMAT fmt)
{
	R_ASSERT(name && name[0] && w && h);

	// ***** first pass - search already created RT
	map_rt::iterator it = m_rt_registry.find(name);

	if (it != m_rt_registry.end())
	{
		return it->second;
	}
	else
	{
		res_rt*	rt = NEW(res_rt);
		rt->make_registered();
		m_rt_registry.insert(mk_pair(rt->set_name(name), rt));

		//if (Device.b_is_Ready)
			rt->create(name, w, h, fmt);

		return rt;
	}
}

void resource_manager::delete_rt(const res_rt*	rt)
{
	if (!rt->is_registered())
		return;

	LPCSTR name = rt->get_name();
	map_rt::iterator it	= m_rt_registry.find(name);

	if (it != m_rt_registry.end())
	{
		m_rt_registry.erase(it);
		return;
	}

	LOG_ERROR("! ERROR: Failed to find render-target '%s'",rt->m_name.c_str());
}

void resource_manager::evict()
{
	hw_wrapper::get_ref().device()->EvictManagedResources();
}

} // namespace render 
} // namespace xray 

