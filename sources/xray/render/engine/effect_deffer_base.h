////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_DEFFER_BASE_H_INCLUDED
#define BLENDER_DEFFER_BASE_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render_dx10 {

class effect_deffer_base: public effect
{
public:
	effect_deffer_base(bool use_detail, bool use_bump, bool use_steep_plax);

	void uber_deffer(effect_compiler& compiler,LPCSTR vs_base, LPCSTR ps_base, bool is_hq, bool is_aref, const effect_compilation_options& options);

protected:
	bool	m_use_detail;
	bool	m_use_bump;
	bool	m_use_steep_plax;
	bool	m_stencil_marking;
	u32		m_stencil_marker;
};	//effect_deffer_base

} // namespace render
} // namespace xray

#endif // #ifndef BLENDER_DEFFER_BASE_H_INCLUDED

