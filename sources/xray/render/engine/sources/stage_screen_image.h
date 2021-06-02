////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SCREEN_IMAGE_H_INCLUDED
#define STAGE_SCREEN_IMAGE_H_INCLUDED

#include "stage.h"
#include <xray/render/core/res_texture_list.h>

namespace xray {
namespace render {

class res_texture;
class resource_intrusive_base;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;

class res_effect;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

class res_declaration;
typedef intrusive_ptr<res_declaration, resource_intrusive_base, threading::single_threading_policy>	ref_declaration;

class stage_screen_image: public stage
{
public:
			stage_screen_image	( renderer_context* context);
	virtual ~stage_screen_image	( ){}

	void execute( ref_texture image);

private:
	bool is_effects_ready() const;
	
	ref_effect			m_sh;
	ref_declaration		m_decl_ptr;
	res_texture_list	m_textures;
}; // class stage_screen_image

	} // namespace render
} // namespace xray


#endif // #ifndef STAGE_SCREEN_IMAGE_H_INCLUDED