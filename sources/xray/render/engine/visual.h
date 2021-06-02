////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef VISUAL_H_INCLUDED
#define VISUAL_H_INCLUDED

#include <xray/render/base/world.h>
#include <xray/render/base/visual.h>
#include <xray/render/base/common_types.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/ogf.h>

namespace xray {

namespace collision {
	struct geometry;
} // namespace collision

namespace render_dx10 {


// class simple_static_effect: public effect
// {
// public:
// 	void compile(effect_compiler& compiler, const effect_compilation_options& options)
// 	{
// 		XRAY_UNREFERENCED_PARAMETER	( options );
// 
// 		shader_defines_list	defines;
// 
// 		make_defines(defines);
// 
// 		compiler.begin_technique(/*LEVEL*/);
// 		compiler.begin_pass("level", "level", defines)
// 				.set_depth(TRUE)
// 				.set_alpha_blend(FALSE)
// 			.end_pass()
// 		.end_technique();
// 	}
// 
// private:
// };

enum enum_flag_visual
{
	enum_flag_visual_managed	= 1,
};

struct render_mesh;

class render_visual : public render::visual
{
public:
	render_visual();
	virtual	~render_visual();

	virtual	void						load(memory::chunk_reader& r);
	virtual	void						render() = 0;
	virtual	void						render_selected();
	virtual	collision::geometry const*	get_collision_geometry	()		{ return m_collision_geom; }
			bool						set_shader(char const* shader, char const* texture);
	
	virtual render_mesh* mesh()	{return NULL;}
	
	u16	type;
	u32	flags;
	
	math::sphere	m_sphere;
	math::aabb		m_aabb;

	ref_effect		m_effect;
	ref_effect		m_notexture_shader;

	xray::collision::geometry*	m_collision_geom;

};//class visual

struct render_mesh : private boost::noncopyable
{
	ref_geometry	geom;

	u32			vertex_buffer_id;
	u32			vertex_base;
	u32			vertex_count;

	u32			index_buffer_id;
	u32			index_base;
	u32			index_count;
	u32			primitive_count;
};//struct mesh

class simple_visual		: public render_visual
{
	typedef render_visual				super;
public:
					simple_visual		();
	virtual			~simple_visual		();
	virtual	void	load				(memory::chunk_reader& r);
	virtual	void	render				();
	virtual render_mesh* mesh			()	{return &m_mesh;}

	//virtual	void	recalculate_memory_usage_impl ( ) { m_memory_usage_self.unmanaged = get_size(); }

	// This is temporary, used only for terrain painters.
	// Do not use this.
	render_mesh		m_mesh;

	res_buffer*		m_vb;
	constant_host*	m_c_L_material;
	constant_host*	m_c_hemi_cube_pos_faces;
	constant_host*	m_c_hemi_cube_neg_faces;
	constant_host*	m_c_dt_params;

}; //class fvisual

class simple_visual_pm : public simple_visual
{
	typedef simple_visual				super;
public:
	virtual			~simple_visual_pm	();
	virtual	void	load				(memory::chunk_reader& r);
	virtual	void	render				();

	//virtual	void	recalculate_memory_usage_impl ( ) { m_memory_usage_self.unmanaged = get_size(); }

protected:
	slide_window_item	m_swi;
};

class hierrarchy_visual :	public render_visual
{
	typedef render_visual			super;
public:
	virtual			~hierrarchy_visual	();
	virtual	void	load				(memory::chunk_reader& r);
	virtual	void	render				();
	virtual void	render_selected		();

	//virtual	void	recalculate_memory_usage_impl ( ) { m_memory_usage_self.unmanaged = get_size(); }

public:
	typedef render::vector<render_visual*>		childs;
	typedef childs::iterator			 childs_it;
	childs								 m_childs;
	childs								 m_managed_visuals;
}; //class hierrarchy_visual

class tree_visual :	public render_mesh, 
					public render_visual
{
	typedef render_visual				super;
public:
					tree_visual		();
	virtual			~tree_visual		();
	virtual	void	load				(memory::chunk_reader& r);
	virtual	void	render				();
	virtual render_mesh* mesh			()	{return this;}

	//virtual	void	recalculate_memory_usage_impl ( ) { m_memory_usage_self.unmanaged = get_size(); }

protected:
	float4x4		m_transform;

	constant_host*	m_c_L_material;
}; //class fvisual

class terrain_visual :	public simple_visual
{
public:
	typedef	fixed_vector<int, render::terrain_texture_max_count>	Textures;
	Textures			textures;
	u32 				vertex_row_size;
	float 				phisical_size;
	fixed_string<16>	name;
	res_buffer *		vb;
};

} // namespace render 
} // namespace xray 


#endif //VISUAL_H_INCLUDED