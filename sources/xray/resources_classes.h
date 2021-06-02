////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_CLASSES_H_INCLUDED
#define XRAY_RESOURCES_CLASSES_H_INCLUDED

namespace xray {
namespace resources {

enum class_id_enum
{ 
	unknown_data_class,
	fs_iterator_class,
	fs_iterator_recursive_class,
	raw_data_class,
	raw_data_class_no_reuse,// this class stands for managed resource that will not be associated with fat-it and reused therefore
	sub_fat_class,
	vfs_sub_fat_class,
	texture_wrapper_class,
	texture_class,
	texture_converter_class,
	texture_tga_to_argb_class,
	shader_binary_source_class,
	render_effect_class,
	
	material_effects_instance_class,
	speedtree_class,
	speedtree_instance_class,
	speedtree_render_instance_class,
	
	static_model_instance_class,			// final class for user
	skeleton_model_instance_class,			// final class for user
	static_render_model_class,				// static render model class (shared between instances)
	skeleton_render_model_class,			// skeleton render model class (shared between instances)
	static_render_model_instance_class,		// static_render_model_instance
	skeleton_render_model_instance_class,	// skeleton_render_model_instance
	composite_render_model_class,			// composite_render_model
	composite_render_model_instance_class,	// composite_render_model_instance
	user_mesh_class,						// user mesh instance (not shared). terrain cursor, and other user-defined geometry
	
	converted_model_class,
	collision_geometry_class,

	collision_bt_shape_class_dynamic,	// bullet shape impl
	collision_bt_shape_class_static,	// bullet shape impl

	material_class,
	config_class,
	binary_config_class,
	ltx_config_class,
	lua_config_class,

	test_resource_class,
	test_resource_class1,
	test_resource_class2,
	test_resource_class3,
	sound_scene_class,
	ogg_sound_wrapper_class,
	ogg_sound_class,
	sound_rms_class,
	sound_panning_lut_class,
	encoded_sound_interface_class,
	ogg_encoded_sound_interface_class,
	wav_encoded_sound_interface_class,
	ogg_file_contents_class,
	ogg_class,
	ogg_converter_class,
	ogg_options_converter_class,
	sound_spl_class,
	spl_class,

	skeleton_class,
	bi_spline_skeleton_animation_class,
	cubic_spline_skeleton_animation_class,
	animation_class,
	animation_controller_class,

	texture_options_lua_class,
	texture_options_binary_class,
	terrain_model_class,
	terrain_instance_class,
	terrain_mesh_class,
	dialog_resources_class,
	particle_world_class,
	particle_system_class,
	particle_system_binary_class,
	preview_particle_system_class,
	particle_system_instance_class,
	sound_class,
	sound_stream_class,
	ogg_raw_file,
	dialog_class,
	game_dialog_class,
	game_project_class,
	game_cell_class,
	game_object_class,
	game_object_scene_class,
	brain_unit_class,
	human_npc_class,
	weapon_class,
	sound_player_logic_class,
	sound_player_editor_class,
	unmanaged_allocation_class,
	behaviour_class,

	render_animated_model_instance_class,
	physics_animated_model_instance_class,
	game_animated_model_instance_class,
	editor_animated_model_instance_class,

	scene_class,
	scene_view_class,
	render_output_window_class,
	
	renderer_class,
	
	//these class ids are exported to lua and therefore must have the same value during engine life
	single_sound_class	= 256,
	composite_sound_class,
	sound_collection_class,

	//these class ids are exported to lua and therefore must have the same value during engine life
	single_animation_class = 512,
	composite_animation_class,
	animation_collection_class,
	
	last_resource_class,
};

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_CLASSES_H_INCLUDED