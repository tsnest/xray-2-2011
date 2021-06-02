////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PARTICLE_PARTICLE_DATA_TYPE_H_INCLUDED
#define XRAY_PARTICLE_PARTICLE_DATA_TYPE_H_INCLUDED

namespace xray {
namespace particle {

enum enum_particle_data_type {
	particle_data_type_billboard,
	particle_data_type_mesh,
	particle_data_type_trail,
	particle_data_type_beam,
	particle_data_type_decal,
	particle_data_type_unknown,
}; // enum enum_particle_data_type

enum enum_particle_screen_alignment {
	particle_screen_alignment_square,
	particle_screen_alignment_rectangle,
	particle_screen_alignment_to_path,
	particle_screen_alignment_to_axis,
}; // enum enum_particle_screen_alignment

enum enum_particle_locked_axis {
	particle_locked_axis_x,
	particle_locked_axis_y,
	particle_locked_axis_z,
	particle_locked_axis_negative_x,
	particle_locked_axis_negative_y,
	particle_locked_axis_negative_z,
	particle_locked_axis_rotate_x,
	particle_locked_axis_rotate_y,
	particle_locked_axis_rotate_z,
	particle_locked_axis_none,
}; // enum enum_particle_locked_axis

struct beamtrail_parameters {
	u32		num_sheets;
	u32		num_texture_tiles;
	u32		num_beams;
}; // struct beamtrail_parameters

enum enum_particle_subuv_method {
	particle_subuv_method_linear,
	particle_subuv_method_linear_smooth,
	particle_subuv_method_random,
	particle_subuv_method_random_smooth
}; // enum enum_particle_subuv_method

struct billboard_parameters {
	enum_particle_locked_axis		locked_axis;
	enum_particle_subuv_method		subuv_method;
	enum_particle_screen_alignment	screen_alignment;
	u32								sub_image_horizontal;
	u32								sub_image_vertical;
	u32								sub_image_changes;
}; // struct billboard_parameters

} // namespace particle
} // namespace xray

#endif // #ifndef XRAY_PARTICLE_PARTICLE_DATA_TYPE_H_INCLUDED