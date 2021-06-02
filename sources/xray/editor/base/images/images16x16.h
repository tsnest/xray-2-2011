////////////////////////////////////////////////////////////////////////////
//	Created		: 02.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef IMAGES16X16_H_INCLUDED
#define IMAGES16X16_H_INCLUDED

namespace xray {
namespace editor_base {

enum images16x16 {
	// lights
	ambient_light,
	script_object_tree_icon,
	misc_tree_icon,
	sun_light, //point
	object_light, //spot
	composite_tree_icon,

	// tree nodes
	folder_closed,
	folder_open,
	folder_closed_modified,
	folder_open_modified,
	node_resource,
	node_resource_modified,
	node_resource_deleted,
	node_material,
	node_material_added,
	node_material_modified,
	node_material_deleted,
	node_texture,
	node_texture_modified,
	node_speedtree,
	node_group,
	node_joint,
	node_mesh,
	node_camera,
	node_sound_file,

	// tools
	tool_lasso,
	tool_manipulator,
	tool_paint_selection,
	tool_translate,
	tool_rotate,
	tool_scale,
	tool_select,

	// terrain modifiers
	terrain_bump,
	terrain_errosion,
	terrain_flatten,
	terrain_noise,
	terrain_raise_lower,
	terrain_ramp,
	terrain_smooth,
	terrain_painter,
	terrain_diffuse_painter,

	// modes
	mode_editor,
	mode_pause,

	// miscellanious
	default_object_set,

	reload_shaders,
	// add new items here!
	terrain_uv_relax,
	terrain_selector,
	pin,
	
	terrain_debug_mode,
	view_mode_iterator,
	toggle_post_process,
	toggle_draw_grid,

	particle_tree_icon,//+
	focus_to_origin,
	npc_tree_icon,
	preview_cube,
	preview_sphere,
	setup_shared_data,
	turn_camera_light,
	object_collision,
	view_isolate_selected,
	camera_track,
	camera_wasd_move,
	debug_draw,
	reload_modified_textures,
	show_render_statistics,
	draw_portals,
	make_portals_coplanar,
	create_bsp_tree,
	show_model,
	generate_portals,
	bsp_tree_edge_mode,
	node_group_locked,
	images16x16_count,
}; // enum images16x16

} // namespace editor
} // namespace xray

#endif // #ifndef IMAGES16X16_H_INCLUDED