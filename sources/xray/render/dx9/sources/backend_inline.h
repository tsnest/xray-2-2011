////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BACKEND_INLINE_H_INCLUDED
#define BACKEND_INLINE_H_INCLUDED

namespace xray {
namespace render {


inline void backend::set_state(render_states state, DWORD value)
{
	if (m_states[state] != value)
	{
		m_states[state] = value;
		m_dirty_states.set(state);
	}
}

inline void backend::set_stencil(u32 enable, u32 func, u32 ref, u32 mask, u32 writemask, u32 fail, u32 zpass, u32 zfail)
{
	set_state(rs_stencil_enable, enable);

//	if( enable ) Need to be checked before uncommenting 
	{
		set_state(rs_stencil_func, func);
		set_state(rs_stencil_ref, ref);
		set_state(rs_stencil_mask, mask);
		set_state(rs_stencil_writemask, writemask);
		set_state(rs_stencil_fail, fail);
		set_state(rs_stencil_zpass, zpass);
		set_state(rs_stencil_zfail, zfail);
	}
}

inline void backend::set_z(u32 enable)
{
	set_state(rs_z_enable, enable);
}

inline void backend::set_alpha_ref(u32 value)
{
	set_state(rs_alpha_ref, value);
}

inline void backend::set_color_write_enable(u32 mask)
{
	set_state(rs_colorwrite_mask, mask);
}

inline void backend::set_cull_mode(u32 mode)
{
	set_state(rs_cull_mode, mode);
}

inline void backend::set_vb(ID3DVertexBuffer* vb, u32 vb_stride)
{
	if (m_vb!=vb || vb_stride!=m_vb_stride)
	{
		m_vb = vb;
		m_vb_stride = vb_stride;
		m_dirty_objects.set(ro_vertex_buffer);
	}
}

inline void backend::set_ib(ID3DIndexBuffer* ib)
{
	if (m_ib != ib)
	{
		m_ib = ib;
		m_dirty_objects.set(ro_index_buffer);
	}
}

inline void backend::set_vs(ID3DVertexShader* vs)
{
	if (m_vs != vs)
	{
		m_vs = vs;
		m_dirty_objects.set(ro_vertex_shader);
	}
}

inline void backend::set_ps(ID3DPixelShader* ps)
{
	if (m_ps != ps)
	{
		m_ps = ps;
		m_dirty_objects.set(ro_pixel_shader);
	}
}

inline void backend::set_state(ID3DState* state)
{
	if (m_state != state)
	{
		m_state = state;
		m_dirty_objects.set(ro_state);
	}
}

inline void backend::set_decl(IDirect3DVertexDeclaration9* decl)
{
	if (m_decl != decl)
	{
		m_decl = decl;
		m_dirty_objects.set(ro_decl);
	}
}

inline void backend::set_ctable(res_const_table* ctable)
{
	if (m_ctable != ctable)
	{
		if (m_ctable)
		{
			//unregister old bindings
			res_const_table::c_table::iterator	it	= m_ctable->m_table.begin();
			res_const_table::c_table::iterator	end	= m_ctable->m_table.end();
			
			for (; it!=end; ++it)
			{
				res_constant* cs = &**it;
				if (!cs->m_handler.empty())
					cs->m_handler(0);
			}
		}

		//is it needed to call const_setup_cb with 0??????? - to unbind constants
		m_ctable = ctable;
		m_dirty_objects.set(ro_ctable);

		if (!ctable)
			return;

		// bind constants
		res_const_table::c_table::iterator	it	= ctable->m_table.begin();
		res_const_table::c_table::iterator	end	= ctable->m_table.end();
		
		for (; it!=end; ++it)
		{
			res_constant* cs = &**it;
			if (!cs->m_handler.empty())
				cs->m_handler(cs);
		}
	}
}

inline void backend::set_rt(render_target_enum target, ID3DSurface* surface)
{
	if (m_targets[target] != surface)
	{
		m_targets[target] = surface;
		m_dirty_targets.set(target);
	}
}

inline void backend::flush()
{
	flush_rt();

	if (m_dirty_objects.any())
	{
		if (m_dirty_objects.test(ro_vertex_shader))
		{
			R_CHK(m_device->SetVertexShader(m_vs));
		}

		if (m_dirty_objects.test(ro_pixel_shader))
		{
			R_CHK(m_device->SetPixelShader(m_ps));
		}
		
		if (m_dirty_objects.test(ro_state) && m_state)
		{
			R_CHK(m_state->Apply());
		}

		if (m_dirty_objects.test(ro_decl))
		{
			R_CHK(m_device->SetVertexDeclaration(m_decl));
		}

		if (m_dirty_objects.test(ro_vertex_buffer))
		{
			R_CHK(m_device->SetStreamSource(0, m_vb, 0, m_vb_stride));
		}

		if (m_dirty_objects.test(ro_index_buffer))
		{
			R_CHK(m_device->SetIndices(m_ib));
		}
		
		m_dirty_objects.reset();
	}

	if (m_dirty_states.any())
	{
		ASSERT(m_dx_render_states[rs_stencil_enable] == D3DRS_STENCILENABLE);
		ASSERT(m_dx_render_states[rs_stencil_func] == D3DRS_STENCILFUNC);
		ASSERT(m_dx_render_states[rs_stencil_ref] == D3DRS_STENCILREF);
		ASSERT(m_dx_render_states[rs_stencil_mask] == D3DRS_STENCILMASK);
		ASSERT(m_dx_render_states[rs_stencil_writemask] == D3DRS_STENCILWRITEMASK);
		ASSERT(m_dx_render_states[rs_stencil_fail] == D3DRS_STENCILFAIL);
		ASSERT(m_dx_render_states[rs_stencil_zpass] == D3DRS_STENCILPASS);
		ASSERT(m_dx_render_states[rs_stencil_zfail] == D3DRS_STENCILZFAIL);
		ASSERT(m_dx_render_states[rs_colorwrite_mask] == D3DRS_COLORWRITEENABLE);
		ASSERT(m_dx_render_states[rs_stencil_enable] == D3DRS_STENCILENABLE);
		ASSERT(m_dx_render_states[rs_cull_mode] == D3DRS_CULLMODE);
		ASSERT(m_dx_render_states[rs_z_enable] == D3DRS_ZENABLE);
		ASSERT(m_dx_render_states[rs_alpha_ref] == D3DRS_ALPHAREF);

		for (u32 i = 0; i < rs_count; ++i)
		{
			if (m_dirty_states.test(i))
			{
				DWORD value = m_states[i];
				R_CHK(m_device->SetRenderState(m_dx_render_states[i], value));
			}
		}
		m_dirty_states.reset();
	}

	flush_stages();

	flush_c_cache();
}

inline void backend::flush_rt()
{
	ASSERT(target_zb == target_count-1);
	for (u32 i = 0; i < target_count-1; ++i)
	{
		if (m_dirty_targets.test(i))
		{
			m_device->SetRenderTarget(i, m_targets[i]);
			m_dirty_targets.reset(i);
		}
	}

	if (m_dirty_targets.test(target_zb))
	{
		m_device->SetDepthStencilSurface(m_targets[target_zb]);
		m_dirty_targets.reset(target_zb);
	}
}

template <typename T>
inline void backend::set_c(res_constant* c, const T& arg)
{
	if (c && c->m_destination&1) {m_a_vertex.set(c, c->m_vs, arg);}
	if (c && c->m_destination&2) {m_a_pixel.set(c, c->m_ps, arg);}
}

inline void backend::set_c(res_constant* c, float x, float y, float z, float w)
{
	set_c(c, math::float4(x, y, z, w));
}

template <typename T>
inline void backend::set_ca(res_constant* c, u32 index, const T& arg)
{
	if (c && c->m_destination&1) {m_a_vertex.seta(c, c->m_vs, index, arg);}
	if (c && c->m_destination&2) {m_a_pixel.seta(c, c->m_ps, index, arg);}
}

inline void backend::set_ca(res_constant* c, u32 index, float x, float y, float z, float w)
{
	set_ca(c, index, math::float4(x, y, z, w));
}

template <typename T>
inline void backend::set_ca(shared_string name, u32 index, const T& arg)
{
	if (!m_ctable)
		return;

	if (ref_constant c = m_ctable->get(name))
		set_ca(&*ñ, index, arg);
}

template <typename T>
inline void backend::set_c(shared_string name, const T& arg)
{
	if (!m_ctable)
		return;

	if (ref_constant c = m_ctable->get(name))
		set_c(&*c, arg);
//	else
//		LOG_WARNING(" !Constant %s is not used in shader", name.c_str());
}

inline void backend::set_c(shared_string name, float x, float y, float z, float w)
{
	if (!m_ctable)
		return;

	if (ref_constant c = m_ctable->get(name))
		set_c(&*c, x, y, z, w);
//	else
//		LOG_WARNING(" !Constant %s is not used in shader", name.c_str());
}

inline void backend::set_ca(shared_string name, u32 index, float x, float y, float z, float w)
{
	if (!m_ctable)
		return;

	if (ref_constant c = m_ctable->get(name))
		set_ca(&*c, index, x, y, z, w);
//	else
//		LOG_WARNING(" !Constant %s is not used in shader", name.c_str());
}


inline void backend::set_texture(u32 stage, ID3DTexture* texture)
{
	if (m_stages[stage] != texture)
	{
		m_stages[stage] = texture;
		m_dirty_stages.set(stage);
	}
}

inline void backend::flush_stages()
{
	for(u32 i = 0; i < texture_stage_count; ++i)
	{
		if (m_dirty_stages.test(i))
		{
			m_device->SetTexture(i, m_stages[i]);
			m_dirty_stages.reset(i);
		}
	}
}

inline void backend::reset_stages()
{
	for(u32 i = 0; i < texture_stage_count; ++i)
	{
		set_texture(i, 0);
	}
}

inline void backend::invalidate()
{
	m_ctable = NULL;
	set_stencil(FALSE);
	set_z(TRUE);
	set_color_write_enable();
	set_cull_mode(D3DCULL_CCW);

	set_decl(0);
	set_state(0);
	set_vs(0);
	set_ps(0);
	set_ctable(0);
	set_vb(0, 0);
	set_ib(0);

	set_rt(target_rt0, m_base_rt);
	set_rt(target_rt1, 0);
	set_rt(target_rt2, 0);
	set_rt(target_zb, m_base_zb);
}

inline void backend::clear(u32 flags, D3DCOLOR color, float z_value, u32 stencil_value)
{
	flush_rt();
	R_CHK(m_device->Clear(0, 0, flags, color, z_value, stencil_value));
}

inline void	backend::render(D3DPRIMITIVETYPE type, u32 base_vertex, u32 start_vertex, u32 vertex_count, u32 start_index, u32 prim_count)
{
	flush();
	//stat.calls			++;
	//stat.verts			+= 3*PC;
	//stat.polys			+= PC;
	R_CHK(m_device->DrawIndexedPrimitive(type, base_vertex, start_vertex, vertex_count, start_index, prim_count));
}

inline void	backend::render(D3DPRIMITIVETYPE type, u32 start_vertex, u32 prim_count)
{
	flush();
	//stat.calls			++;
	//stat.verts			+= 3*PC;
	//stat.polys			+= PC;
	R_CHK(m_device->DrawPrimitive(type, start_vertex, prim_count));
}

} // namespace render 
} // namespace xray 


#endif // #ifndef BACKEND_INLINE_H_INCLUDED