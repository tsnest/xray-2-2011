////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SCREEN_IMAGE_H_INCLUDED
#define STAGE_SCREEN_IMAGE_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render_dx10 {


class stage_screen_image: public stage
{
public:
	stage_screen_image	( scene_context* context);
	~stage_screen_image	( ){}

	void execute( ref_texture image);

private:
	ref_effect			m_sh;
	ref_declaration		m_decl_ptr;
	res_texture_list	m_textures;

}; // class stage_screen_image

	} // namespace render
} // namespace xray


#endif // #ifndef STAGE_SCREEN_IMAGE_H_INCLUDED