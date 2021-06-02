////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_TREE_H_INCLUDED
#define BLENDER_TREE_H_INCLUDED

#include <xray/render/engine/effect_deffer_base.h>

namespace xray {
namespace render_dx10 {

class effect_tree : public effect_deffer_base
{
public:
	effect_tree();

	virtual void compile(effect_compiler& compiler, const effect_compilation_options& options);
	virtual	void load(memory::reader& mem_reader);

private:
	bool	m_blend;
	bool	m_not_an_tree;
}; // class effect_tree

} // namespace render
} // namespace xray


#endif // #ifndef BLENDER_TREE_H_INCLUDED