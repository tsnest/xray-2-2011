////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_LAYOUT_UTILS_H_INCLUDED
#define INPUT_LAYOUT_UTILS_H_INCLUDED

#include <d3d9types.h>

namespace xray {
namespace render {
namespace decl_utils {

void	convert_vertex_declaration( const vector<D3DVERTEXELEMENT9> &declIn, vector<D3D_INPUT_ELEMENT_DESC> &declOut);

} // namespace decl_utils
} // namespace render
} // namespace xray

#endif // #ifndef INPUT_LAYOUT_UTILS_H_INCLUDED