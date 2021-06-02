////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SCREEN_IMAGE_H_INCLUDED
#define STAGE_SCREEN_IMAGE_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render{


class stage_screen_image: public stage
{
public:
	stage_screen_image	( scene_context* context);
	~stage_screen_image	( ){}

	void execute( float x1, float y1, float x2, float y2, ref_texture image);

private:
	ref_shader		m_sh;
	ref_declaration m_decl_ptr;

}; // class stage_screen_image

	} // namespace render
} // namespace xray


#endif // #ifndef STAGE_SCREEN_IMAGE_H_INCLUDED