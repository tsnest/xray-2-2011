////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_TERRAIN_NEW_H_INCLUDED
#define BLENDER_TERRAIN_NEW_H_INCLUDED

#include <xray/render/engine/effect_deffer_base.h>

namespace xray {
namespace render_dx10 {

class effect_terrain_NEW : public effect_deffer_base
{
public:
	effect_terrain_NEW();

	virtual void compile(effect_compiler& compiler, const effect_compilation_options& options);
	virtual	void load(memory::reader& mem_reader);

private:
	bool	m_blend;
	bool	m_not_an_tree;
}; // class effect_terrain_NEW

} // namespace render
} // namespace xray


#endif // #ifndef BLENDER_TERRAIN_NEW_H_INCLUDED