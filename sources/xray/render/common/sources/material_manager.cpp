////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_manager.h"

namespace xray {
namespace render {


#define THM_CHUNK_VERSION				0x0810
#define THM_CHUNK_DATA					0x0811
#define THM_CHUNK_TEXTUREPARAM			0x0812
#define THM_CHUNK_TYPE					0x0813
#define THM_CHUNK_TEXTURE_TYPE			0x0814
#define THM_CHUNK_DETAIL_EXT			0x0815
#define THM_CHUNK_MATERIAL				0x0816
#define THM_CHUNK_BUMP					0x0817
#define THM_CHUNK_EXT_NORMALMAP			0x0818
#define THM_CHUNK_FADE_DELAY			0x0819

enum
{
	ttImage	= 0,
	ttCubeMap,
	ttBumpMap,
	ttNormalMap,
	ttTerrain,
};

enum
{
	tbmResereved	= 0,
	tbmNone,
	tbmUse,
	tbmUseParallax,
};

enum
{
	flDiffuseDetail		= (1<<23),
	flBumpDetail		= (1<<26),
};

float r__dtex_range = 50;

class cl_dt_scaler
{
public:
	cl_dt_scaler(float s): m_scale(s)	{}

	void setup(res_constant* c)
	{
		backend::get_ref().set_c(c, m_scale, m_scale, m_scale, 1.0f/r__dtex_range);
	}

private:
	float	m_scale;
	static const float m_dtex_range;
};

const float cl_dt_scaler::m_dtex_range	= 50;

material::~material()
{
	if (m_setup) DELETE(m_setup);
}

const_setup_cb  material::get_detail_setup() const
{
	return const_setup_cb(m_setup, &cl_dt_scaler::setup);
}

void material::load(memory::chunk_reader& data)
{
	if (data.chunk_exists(THM_CHUNK_TEXTUREPARAM))
	{
		memory::reader in = data.open_reader(THM_CHUNK_TEXTUREPARAM);
		in.advance(4);
		u32 flags = in.r_u32();

		if ((flags&flDiffuseDetail) != 0)
			m_usage |= usg_detail;

		if ((flags&flBumpDetail) != 0)
			m_usage |= usg_bump_detail;
	}

	if (data.chunk_exists(THM_CHUNK_DETAIL_EXT))
	{
		memory::reader in = data.open_reader(THM_CHUNK_DETAIL_EXT);
		fixed_string<256> nm(in.r_string());
		utils::fix_path(nm.get_buffer());
		m_detail_name = /*in.r_string()*/nm.c_str();
		m_setup = NEW(cl_dt_scaler)(in.r_float());
	}

	if (data.chunk_exists(THM_CHUNK_MATERIAL))
	{
		memory::reader in = data.open_reader(THM_CHUNK_MATERIAL);
		m_material_id = in.r_u32() + in.r_float();
	}

	if (data.chunk_exists(THM_CHUNK_BUMP))
	{
		memory::reader in = data.open_reader(THM_CHUNK_BUMP);
		in.advance(4);
		u32 bump_mode = in.r_u32();
		if (bump_mode == tbmUseParallax)
			m_use_steep_parallax = true;
		
		fixed_string<256> nm(in.r_string());
		utils::fix_path(nm.get_buffer());
		m_bump_name = nm.c_str();
	}
}

void material_manager::load()
{
	#ifdef DEBUG
		//CTimer					TT;
		//TT.Start				();
	#endif // #ifdef DEBUG
	
	load_thm("resources/textures");
	
	#ifdef DEBUG
		//Msg("load time=%d ms",TT.GetElapsed_ms());
	#endif // #ifdef DEBUG
}

void material_manager::unload()
{
	m_materials.clear();
}

void material_manager::load_thm(LPCSTR base_dir)
{
	m_loading_incomplete = true;

	resources::query_fs_iterator(base_dir, 
		boost::bind(&material_manager::process_files, this, _1),
		::xray::render::g_allocator);

	while (m_loading_incomplete)
	{
		if ( threading::g_debug_single_thread )
			resources::tick				();

		resources::dispatch_callbacks();
	}
}

void material_manager::process_files(resources::fs_iterator begin)
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

				u32 pos = file_name.length()-4;
				if (file_name.substr(pos) == ".thm")
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
		boost::bind(&material_manager::load_raw_files, this, _1),
		::xray::render::g_allocator);
}

void material_manager::load_raw_files(resources::queries_result& data)
{
	ASSERT(data.size() == m_files_list.size());

	m_loading_incomplete = false;

	for (u32 i = 0; i < data.size(); ++i)
	{
		resources::pinned_ptr_const<u8>	ptr (data[i].get_managed_resource());
		u32	const	size = data[i].get_managed_resource()->get_size();
		memory::chunk_reader	ch_it(& * ptr, size, memory::chunk_reader::chunk_type_sequential);

		if (ch_it.chunk_exists(THM_CHUNK_TEXTURE_TYPE))
		{
			memory::reader in = ch_it.open_reader(THM_CHUNK_TEXTURE_TYPE);
			u32 type = in.r_u32();

			if (ttImage == type || ttTerrain == type || ttNormalMap == type)
			{
				u32 start = strings::length("resources/textures/");
				u32 len = m_files_list[i].length() - start - 4;
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!HACK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				shared_string name = m_files_list[i].substr(start, len).c_str();
				material*	mtl = NEW(material);
				m_materials[name] = mtl;
				mtl->load(ch_it);
			}
		}
	}

	//release file names list
	{
		vector<fs::path_string>	empty;
		m_files_list.swap(empty);
	}
}

ref_material material_manager::get_material(const shared_string& name)
{
	map_materials::const_iterator	it  = m_materials.find(name);
	map_materials::const_iterator	end = m_materials.end();

	if (it != end)
	{
		return it->second;
	}
	
	ASSERT( false, "Material \"%s\"not found !", name.c_str() );
	return 0;
}

} // namespace render 
} // namespace xray 

