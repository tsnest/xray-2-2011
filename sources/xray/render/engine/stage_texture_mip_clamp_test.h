////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_TEXTURE_MIP_CLAMP_TEST_H_INCLUDED
#define STAGE_TEXTURE_MIP_CLAMP_TEST_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render_dx10 {


class stage_texture_mip_clamp_test: public stage
{
public:
	stage_texture_mip_clamp_test	( scene_context* context);
	~stage_texture_mip_clamp_test	( ){}

	void execute( );

private:
	ref_effect			m_sh;
	ref_declaration		m_decl_ptr;

}; // class stage_texture_mip_clamp_test

} // namespace render
} // namespace xray


#endif // #ifndef STAGE_TEXTURE_MIP_CLAMP_TEST_H_INCLUDED