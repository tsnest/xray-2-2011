////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_MANAGER_H_INCLUDED
#define TERRAIN_MANAGER_H_INCLUDED

#include <xray/render/base/common_types.h>
#include <xray/render/base/visual.h>
#include "terrain_texture_pool.h"
#include <xray/resources_cook_classes.h>
#include <xray/render/core/vertex_stream.h>

namespace xray {
namespace render_dx10 {

// forward declarations
class effect_terrain_NEW;
class terrain_visual;

struct terrain_cell
{
	render::visual_ptr			visual;

	bool operator == (render::visual_ptr const& v)
	{
		return visual == v;
	}
};

class terrain : public quasi_singleton<terrain>
{
public:
	typedef render::vector<terrain_cell>	cells;

public:
				terrain				( );
				~terrain			( );
	bool		has_cell			( pcstr cell_id) const;
	render::visual_ptr	get_cell			( pcstr cell_id);

	void 		add_cell			( render::visual_ptr const v, bool beditor );
	void 		remove_cell			( render::visual_ptr const v, bool beditor );

	// Editor specific functions
	void 		update_cell_buffer	( render::visual_ptr const v, xray::vectora<render::buffer_fragment_NEW> const& fragments, float4x4 const& transform);
	void 		add_cell_texture 	( render::visual_ptr const v, render::texture_string const & texture, u32 tex_user_id);
	void 		remove_cell_texture ( render::visual_ptr const v, u32 tex_user_id);
	void 		exchange_texture	( render::texture_string const & old_texture, render::texture_string const & new_texture);

	cells const&  get_editor_cells	() const		{ return m_editor_cells; }
	cells const&  get_game_cells	() const		{ return m_game_cells; }

private:

public:
	cells									m_editor_cells;
	cells									m_game_cells;
	terrain_texture_pool					m_texture_pool;
	effect_terrain_NEW*						m_effect;
	vertex_stream							m_intermediate_vertex_stream;
}; // class terrain


struct terrain_cell_cook : public resources::unmanaged_cook
{
	typedef resources::unmanaged_cook	super;

								terrain_cell_cook		( );

	virtual	mutable_buffer		allocate_resource		(resources::query_result_for_cook &	in_query, 
														 const_buffer						raw_file_data, 
														 bool								file_exist)
	{
		XRAY_UNREFERENCED_PARAMETERS					(& in_query, & raw_file_data, file_exist);
		return					mutable_buffer::zero();
	}

	virtual void				deallocate_resource		(pvoid) {}

	virtual create_resource_if_no_file_delegate_type	get_create_resource_if_no_file_delegate	() 
	{ 
		return					create_resource_if_no_file_delegate_type(this, & terrain_cell_cook::create_resource_if_no_file);
	}
	
	void						create_resource_if_no_file	(resources::query_result_for_cook &	in_out_query, 
															 mutable_buffer						in_out_unmanaged_resource_buffer);

	virtual void				destroy_resource		(resources::unmanaged_resource *		resource);

	virtual void				create_resource			(resources::query_result_for_cook &	in_out_query, 
											 			 const_buffer				raw_file_data, 
														 mutable_buffer				in_out_unmanaged_resource_buffer);

	//virtual	void				translate_request_path	(pcstr request, fs::path_string & new_request) const;
};


} // namespace render
} // namespace xray

#endif // #ifndef TERRAIN_MANAGER_H_INCLUDED