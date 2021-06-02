////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_MANAGER_H_INCLUDED
#define MATERIAL_MANAGER_H_INCLUDED

#include <xray/resources_fs.h>
#include <xray/render/core/res_common.h>


namespace xray {
namespace render_dx10 {


class cl_dt_scaler;

class material: public res_base
{
	friend class material_manager;

private:
	enum usage_enum
	{
		usg_detail = 1<<0,
		usg_bump_detail = 1<<1
	};

public:
	material():
		m_setup(0), 
		m_material_id(0),
		m_usage(0), 
		m_use_steep_parallax(0)
	{}

	~material();
	void _free() const;

	shared_string	get_bump_name() const {return m_bump_name;}
	shared_string	get_detail_name() const {return m_detail_name;}
	float4 const*	get_detail_setup() const;
	float			get_material_id() const {return m_material_id;}

	bool	has_detail() const {return /*(m_usage&usg_detail) != 0*/m_detail_name.length()>0;}
	bool	has_bump() const {return   /*(m_usage&usg_bump_detail) != 0*/m_bump_name.length()>0;}
	bool	use_steep_parallax() const {return m_use_steep_parallax;}

private:
	void	load(memory::chunk_reader& data);

private:
	shared_string		m_detail_name;
	shared_string		m_bump_name;
	cl_dt_scaler*		m_setup;
	float				m_material_id;
	u8					m_usage;
	bool				m_use_steep_parallax;
};
typedef intrusive_ptr<material, res_base, threading::single_threading_policy>	ref_material;

class material_manager: public quasi_singleton<material_manager>
{
public:
	material_manager(): m_loading_incomplete(false) {}

	void load();
	void unload();

	ref_material get_material(const shared_string& name);

private:
	typedef render::map<shared_string, ref_material>	map_materials;

private:
	void load_thm(LPCSTR base_dir);
	void process_files	( resources::fs_iterator begin);
	void load_raw_files	( resources::queries_result& data);

private:
	map_materials	m_materials;

	render::vector<fs::path_string>	m_files_list;

	bool	m_loading_incomplete;
}; // class material_manager

} // namespace render 
} // namespace xray 


#endif // #ifndef MATERIAL_MANAGER_H_INCLUDED
