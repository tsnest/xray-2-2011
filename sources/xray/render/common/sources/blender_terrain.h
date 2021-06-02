////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_TERRAIN_H_INCLUDED
#define BLENDER_TERRAIN_H_INCLUDED

#include "blender_deffer_base.h"

namespace xray {
namespace render{

class blender_terrain : public blender_deffer_base
{
public:
	blender_terrain();

	virtual void compile(blender_compiler& compiler, const blender_compilation_options& options);
	virtual	void load(memory::reader& mem_reader);

private:
	bool	m_blend;
	bool	m_not_an_tree;
}; // class blender_terrain

} // namespace render
} // namespace xray


#endif // #ifndef BLENDER_TERRAIN_H_INCLUDED