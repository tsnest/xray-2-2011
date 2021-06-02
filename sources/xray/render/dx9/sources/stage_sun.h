////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SUN_H_INCLUDED
#define STAGE_SUN_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render{

class stage_sun: public stage
{
public:
	enum sun_cascade
	{
		cascade_near	= 0,
		cascade_middle	= 1,
		cascade_far		= 2
	};

			stage_sun				( scene_context* context);
			~stage_sun				();

			void	execute			( sun_cascade cascade);

private:
			float4x4 calc_sun_matrix	(const float4x4& frustum, const float3& dir);
			void	phase_sun_mask		(const float3& light_dir);

	static	void	rm_near			();
	static	void	rm_normal		();
	static	void	rm_far			();

private:
	ref_shader	m_sh_accum_direct;
}; // class stage_sun


} // namespace render
} // namespace xray



#endif // #ifndef STAGE_SUN_H_INCLUDED