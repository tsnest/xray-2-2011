////////////////////////////////////////////////////////////////////////////
//	Created		: 06.06.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CURVES_SERIALIZER_H_INCLUDED
#define CURVES_SERIALIZER_H_INCLUDED

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor_base {
public ref class curves_serializer
{
public:
	static void				synchronize_float_curve_config ( float_curve^ curve, configs::lua_config_value& curve_keys_config );
	static float_curve^		load_float_curve_from_config ( configs::lua_config_value const& curve_keys_config );
	
	static void				synchronize_color_curve_config( color_curve^ color_curve, configs::lua_config_value& color_curve_config );
	static color_curve^		load_color_curve_from_config ( configs::lua_config_value const& curve_keys_config );
	
	static void				synchronize_color_matrix_config( color_matrix^ color_matrix, configs::lua_config_value& color_matrix_config );
	static color_matrix^	load_color_matrix_from_config( configs::lua_config_value const& color_matrix_config );
	
};
} // namespace editor_base
} // namespace xray

#endif // #ifndef CURVES_SERIALIZER_H_INCLUDED