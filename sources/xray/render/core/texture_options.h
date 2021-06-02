////////////////////////////////////////////////////////////////////////////
//	Created		: 28.12.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_TEXTURE_OPTIONS_H_INCLUDED
#define XRAY_RENDER_CORE_TEXTURE_OPTIONS_H_INCLUDED

namespace xray {
namespace render {

namespace texture_options{
	enum ETType{
		sky = 0,
		bump,
		normal_map,
		terrain,
		terrain_bump,
		terrain_color,
		type_2d
	};
	
	enum ETPackMode{
		packed			= 0,
		unpacked,
	};

	enum ETTextureDimension
	{
		texture_1D		= 0,
		texture_2D,
		texture_3D,
		texture_cube,
		texture_2D_array,
	};

	enum ETAlphaBlendMode{
		none			= 0,
		reference,
		blend,
	};

	enum ETBumpMode{
		paralax			= 0,
		stip_paralax,
		displacement,
	};

}// namespace texture_options

//struct texture_options
//{
//	enum ETType{
//		sky = 0,
//		bump,
//		normal_map,
//		terrain,
//		terrain_bump,
//		terrain_color,
//		type_2d
//	};
//	enum ETBumpMode{
//		paralax			= 0,
//		stip_paralax,
//		displacement,
//	};
//	enum ETAlphaBlendMode{
//		none			= 0,
//		reference,
//		blend,
//	};
//	enum ETPackMode{
//		packed			= 0,
//		unpacked,
//	};
//	enum ETTextureDimension
//	{
//		texture_1D		= 0,
//		texture_2D,
//		texture_3D,
//		texture_cube,
//		texture_2D_array,
//	};
//	void		load			( configs::binary_config_ptr config );
//	void		make_default	( );
//public:
////.	ETType						type;
//	xray::fs_new::virtual_path_string		format;//+
//	ETTextureDimension			texture_dimension; //+
////.	ETAlphaBlendMode			alpha_blend_mode;
////.	ETPackMode					pack_mode;
//	bool						has_mip_levels;//+
////.	bool						is_cubemap;
////.	xray::fs_new::virtual_path_string		normal_name;
////.	xray::fs_new::virtual_path_string		color_name;
////.	float						detail_scale;
////.	ETBumpMode					bump_mode;
////.	xray::fs_new::virtual_path_string		bump_name;
////.	int							tile;
//}; // struct texture_options

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_TEXTURE_OPTIONS_H_INCLUDED