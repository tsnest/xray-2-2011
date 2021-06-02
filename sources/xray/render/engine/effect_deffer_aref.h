////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_DEFFER_AREF_H_INCLUDED
#define BLENDER_DEFFER_AREF_H_INCLUDED

#include <xray/render/engine/effect_deffer_base.h>

namespace xray {
namespace render_dx10 {


class effect_deffer_aref: public effect_deffer_base, boost::noncopyable
{
public:
	effect_deffer_aref(bool is_lmapped);

	virtual void compile(effect_compiler& compiler, const effect_compilation_options& options);
	virtual	void load(memory::reader& mem_reader);

private:
	void compile_blended(effect_compiler& compiler, const effect_compilation_options& options);

private:

	// These members are temporary. Needed to use constant local binding storage in effect_compiler when implemented.
	static u32	const	m_aref_val		= 200;
	static u32	const	m_aref_val220	= 220;

	bool	m_blend;
	bool	m_is_lmapped;
}; // class effect_deffer_aref

} // namespace render 
} // namespace xray 


#endif // #ifndef BLENDER_DEFFER_AREF_H_INCLUDED