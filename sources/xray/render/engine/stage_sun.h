////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SUN_H_INCLUDED
#define STAGE_SUN_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render_dx10 {

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

			void		execute			( sun_cascade cascade);

private:
			float4x4	calc_sun_matrix	( const float4x4& frustum, const float3& dir);
			void		phase_sun_mask	( float3 const & light_dir, float3 const & light_color, float light_spec);

	static	void	rm_near			();
	static	void	rm_normal		();
	static	void	rm_far			();

private:
	ref_effect	m_sh_accum_direct;

	constant_host*	m_ldynamic_dir;
	constant_host*	m_ldynamic_color;
	constant_host*	m_ldynamic_pos;
	constant_host*	m_sunmask;

}; // class stage_sun


} // namespace render
} // namespace xray



#endif // #ifndef STAGE_SUN_H_INCLUDED