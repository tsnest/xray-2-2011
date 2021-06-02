////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SUN_MATERIALS_H_INCLUDED
#define STAGE_SUN_MATERIALS_H_INCLUDED


#include "stage.h"

namespace xray {
namespace render{

class stage_sun_materials: public stage
{
public:
	enum sun_cascade
	{
		cascade_near	= 0,
		cascade_middle	= 1,
		cascade_far		= 2
	};

			stage_sun_materials				( scene_context* context);
			~stage_sun_materials				();

			void	execute			( sun_cascade cascade);

private:
			float4x4	calc_sun_matrix	(const float4x4& frustum, const float3& dir);
			void		render_sun_mask		(const float3& light_dir, u32 material_marker);

	static	void	rm_near			();
	static	void	rm_normal		();
	static	void	rm_far			();

private:
	ref_shader	m_sh_accum_direct;
}; // class stage_sun_materials


} // namespace render
} // namespace xray



#endif // #ifndef STAGE_SUN_MATERIALS_H_INCLUDED