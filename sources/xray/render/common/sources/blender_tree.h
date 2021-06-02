////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_TREE_H_INCLUDED
#define BLENDER_TREE_H_INCLUDED

#include "blender_deffer_base.h"

namespace xray {
namespace render{

class blender_tree : public blender_deffer_base
{
public:
	blender_tree();

	virtual void compile(blender_compiler& compiler, const blender_compilation_options& options);
	virtual	void load(memory::reader& mem_reader);

private:
	bool	m_blend;
	bool	m_not_an_tree;
}; // class blender_tree

} // namespace render
} // namespace xray


#endif // #ifndef BLENDER_TREE_H_INCLUDED