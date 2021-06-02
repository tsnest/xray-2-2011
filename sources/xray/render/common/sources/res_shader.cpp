////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "res_shader.h"

//#include "Shader.h"
#include "hw_wrapper.h"
#include "resource_manager.h"
//
//#include "dxRenderDeviceRender.h"
#include <xray/render/common/sources/shader_manager.h>

namespace xray {
namespace render {


res_texture_list::~res_texture_list()
{
	shader_manager::get_ref().delete_texture_list(this);
}

void res_texture_list::clear()
{
	//iterator it  = this->begin();
	//iterator end = this->end();
	
	//for(; it != end; ++it)
	//	(*it).second.destroy();

	this->erase(this->begin(), this->end());
}

void res_texture_list::clear_not_free()
{
	//iterator it  = this->begin();
	//iterator end = this->end();

	//for(; it != end; ++it)
	//	(*it).second.destroy();

	this->erase(this->begin(), this->end());
}

u32 res_texture_list::find_texture_stage(const shared_string &tex_name) const
{
	u32	texture_stage = 0;

	res_texture_list::const_iterator it  = this->begin();
	res_texture_list::const_iterator end = this->end();

	for (; it != end; ++it)
	{
		const std::pair<u32,ref_texture>& loader = *it;

		//	Shadowmap texture always uses 0 texture unit
		if (loader.second->m_name == tex_name)
		{
			//	Assign correct texture
			texture_stage	= loader.first;
			break;
		}
	}

	ASSERT(it != end);

	return texture_stage;
}

bool res_texture_list::equal(const res_texture_list& base) const
{
	if (size() != base.size())
		return false;

	for (u32 cmp = 0; cmp < size(); ++cmp)
	{
		if ((*this)[cmp].first  != base[cmp].first)		return false;
		if ((*this)[cmp].second != base[cmp].second)	return false;
	}

	return true;
}

void res_texture_list::apply()
{
	//backend::get_ref().reset_stages();
	backend&	be = backend::get_ref();
	u32			stage = 0;
	u32			stage_count = be.get_texture_stage_count();

	for (u32 i = 0; i < size(); ++i)
	{
		while (stage < at(i).first)
		{
			be.set_texture(stage++, 0);
		}

		ASSERT(at(i).first == stage);
		at(i).second->apply(stage++);
	}

	
	while (stage < stage_count)
	{
		be.set_texture(stage++, 0);
	}
	ASSERT(stage == stage_count);
}


res_geometry::~res_geometry()
{
	shader_manager::get_ref().delete_geometry(this);
}

bool res_geometry::equal(const res_geometry& other) const
{
	return m_vb==other.m_vb && m_ib==other.m_ib &&
		m_dcl==other.m_dcl && m_vb_stride==other.m_vb_stride;
}

void res_geometry::apply()
{
	m_dcl->apply();

	backend& be = backend::get_ref();
	be.set_vb(m_vb, m_vb_stride);
	be.set_ib(m_ib);
}

res_pass::~res_pass()
{
	shader_manager::get_ref().delete_pass(this);
}

bool res_pass::equal(const res_pass& other) const
{
	return m_state == other.m_state &&
			m_ps   == other.m_ps &&
			m_vs   == other.m_vs &&
			m_constants	== other.m_constants &&
			m_tex_list  == other.m_tex_list;
//#ifdef	USE_DX10
//	if (gs			!= _gs)			return FALSE;
//#endif	//	USE_DX10
}

void res_pass::apply()
{
	m_vs->apply();
	m_ps->apply();
	m_state->apply();
	m_tex_list->apply();

	backend::get_ref().set_ctable(m_constants.c_ptr());
}

res_shader_technique::~res_shader_technique()
{
	shader_manager::get_ref().delete_shader_technique(this);
}
//SGeometry::~SGeometry					()			{	DEV->DeleteGeom			(this);			}
bool res_shader_technique::equal(const res_shader_technique& other) const
{
	if (m_flags.priority != other.m_flags.priority) return false;
	if (m_flags.strict_b2f != other.m_flags.strict_b2f) return false;
	if (m_flags.has_emissive != other.m_flags.has_emissive) return false;
	if (m_flags.has_wmark != other.m_flags.has_wmark) return false;
	if (m_flags.has_distort != other.m_flags.has_distort) return false;
	
	if (m_passes.size() != other.m_passes.size()) return false;

	for (u32 p = 0; p < m_passes.size(); ++p)
		if (m_passes[p] != other.m_passes[p])
			return false;

	return true;
}

res_shader::~res_shader()
{
	shader_manager::get_ref().delete_shader(this);
}

bool res_shader::equal(const res_shader& other) const
{
	if (m_sh_techniques.size() != other.m_sh_techniques.size())
		return false;

	for (u32 i = 0; i < m_sh_techniques.size(); ++i)
		if (m_sh_techniques[i] != other.m_sh_techniques[i])
			return false;

	return true;

	//return m_sh_techniques[0]->equal(*other.m_sh_techniques[0]) &&
	//	m_sh_techniques[1]->equal(*other.m_sh_techniques[1]) &&
	//	m_sh_techniques[2]->equal(*other.m_sh_techniques[2]) &&
	//	m_sh_techniques[3]->equal(*other.m_sh_techniques[3]) &&
	//	m_sh_techniques[4]->equal(*other.m_sh_techniques[4]) &&
	//	m_sh_techniques[5]->equal(*other.m_sh_techniques[5]);
}

void res_shader::apply_pass(u32 id)
{
	ref_shader_technique	technique = m_sh_techniques[m_cur_technique];

	ASSERT(id < technique->m_passes.size());

	ref_pass pass = technique->m_passes[id];
	
	pass->apply();
}

//void ref_shader::create(LPCSTR shader, LPCSTR textures)
//{
//
//}
//
//void ref_shader::create(blender* desc, LPCSTR shader, LPCSTR textures)
//{
//	ref_shader	temp = *this;
//	temp = shader_manager::get_ref().create_shader(desc, shader, textures);
//	operator=(temp);
//}

//																							 
////////////////////////////////////////////////////////////////////////////					 
//void	resptrcode_shader::create		(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
//{
//	_set(DEV->Create		(s_shader,s_textures,s_constants,s_matrices));
//}
//void	resptrcode_shader::create		(IBlender* B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
//{
//	_set(DEV->Create		(B,s_shader,s_textures,s_constants,s_matrices));
//}
//
////////////////////////////////////////////////////////////////////////////
//void	resptrcode_geom::create			(u32 FVF , ID3DVertexBuffer* vb, ID3DIndexBuffer* ib)
//{
//	_set(DEV->CreateGeom		(FVF,vb,ib));
//}
//void	resptrcode_geom::create			(D3DVERTEXELEMENT9* decl, ID3DVertexBuffer* vb, ID3DIndexBuffer* ib)
//{
//	_set(DEV->CreateGeom		(decl,vb,ib));
//}
//
////////////////////////////////////////////////////////////////////////
//// Construction/Destruction
////////////////////////////////////////////////////////////////////////
//#ifdef	USE_DX10
//BOOL	SPass::equal	(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_gs& _gs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C)
//#else	//	USE_DX10
//BOOL	SPass::equal	(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C)
//#endif	//	USE_DX10
//{
//	if (state		!= _state)		return FALSE;
//	if (ps			!= _ps)			return FALSE;
//	if (vs			!= _vs)			return FALSE;
//#ifdef	USE_DX10
//	if (gs			!= _gs)			return FALSE;
//#endif	//	USE_DX10
//	if (constants	!= _ctable)		return FALSE;	// is this nessesary??? (ps+vs already combines)
//
//	if (T != _T)					return FALSE;
//	if (C != _C)					return FALSE;
//#ifdef _EDITOR
//	if (M != _M)					return FALSE;
//#endif
//	return TRUE;
//}
//
////
//ShaderElement::ShaderElement()
//{
//	flags.iPriority		= 1;
//	flags.bStrictB2F	= FALSE;
//	flags.bEmissive		= FALSE;
//	flags.bDistort		= FALSE;
//	flags.bWmark		= FALSE;
//}
//
//BOOL ShaderElement::equal	(ShaderElement& S)
//{
//	if (flags.iPriority		!= S.flags.iPriority)	return FALSE;
//	if (flags.bStrictB2F	!= S.flags.bStrictB2F)	return FALSE;
//	if (flags.bEmissive		!= S.flags.bEmissive)	return FALSE;
//	if (flags.bWmark		!= S.flags.bWmark)		return FALSE;
//	if (flags.bDistort		!= S.flags.bDistort)	return FALSE;
//	if (passes.size() != S.passes.size())			return FALSE;
//	for (u32 p=0; p<passes.size(); p++)
//		if (passes[p] != S.passes[p])				return FALSE;
//	return TRUE;
//}
//
//BOOL ShaderElement::equal	(ShaderElement* S)
//{	
//	if (0==S && 0==this)	return TRUE;
//	if (0==S || 0==this)	return FALSE;
//	return	equal	(*S);	
//}
//
////
//BOOL Shader::equal	(Shader& S)
//{
//	return
//		E[0]->equal(&*S.E[0]) &&
//		E[1]->equal(&*S.E[1]) &&
//		E[2]->equal(&*S.E[2]) &&
//		E[3]->equal(&*S.E[3]) &&
//		E[4]->equal(&*S.E[4]);
//}
//BOOL Shader::equal	(Shader* S)
//{	return	equal(*S);	}
//

} // namespace render 
} // namespace xray 
