////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_FORWARD_SYSTEM_H_INCLUDED
#define BLENDER_FORWARD_SYSTEM_H_INCLUDED

#include <xray/render/engine/effect_deffer_base.h>

namespace xray {
namespace render_dx10 {

class effect_forward_system : public effect
{
public:
	effect_forward_system();

	virtual void compile(effect_compiler& compiler, const effect_compilation_options& options);
	virtual	void load(memory::reader& mem_reader);

private:
}; // class effect_tree

} // namespace render
} // namespace xray

#endif // #ifndef BLENDER_FORWARD_SYSTEM_H_INCLUDED