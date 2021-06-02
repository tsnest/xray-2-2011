////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_DEFFER_AREF_H_INCLUDED
#define BLENDER_DEFFER_AREF_H_INCLUDED

#include "blender_deffer_base.h"

namespace xray {
namespace render {


class blender_deffer_aref: public blender_deffer_base
{
public:
	blender_deffer_aref(bool is_lmapped);

	virtual void compile(blender_compiler& compiler, const blender_compilation_options& options);
	virtual	void load(memory::reader& mem_reader);

private:
	void compile_blended(blender_compiler& compiler, const blender_compilation_options& options);

private:
	u32		m_aref_val;
	bool	m_blend;
	bool	m_is_lmapped;
}; // class blender_deffer_aref

} // namespace render 
} // namespace xray 


#endif // #ifndef BLENDER_DEFFER_AREF_H_INCLUDED