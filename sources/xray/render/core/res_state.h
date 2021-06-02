////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_STATE_H_INCLUDED
#define RES_STATE_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>

namespace xray {
namespace render {

class res_state : public resource_intrusive_base {
	friend class resource_manager;

protected:
	// Only state_generator will be able create state instances.
	res_state(	ID3DRasterizerState*		rasterizer_state, 
					ID3DDepthStencilState*		depth_stencil_state, 
					ID3DBlendState*				blend_state, 
					u32							stencil_ref);	

public:
	~res_state	();
	void destroy_impl	() const;

	// ?
	void	apply() const;
	u32		stencil_ref			( ) { return m_stencil_ref; }

	inline bool is_registered() const { return m_is_registered; }
	inline void mark_registered() { R_ASSERT( !m_is_registered ); m_is_registered = true; }
	inline u32  id() const { return m_id; }
	inline void set_id( u32 const id ) { m_id = id; }

private:

	//	All states are supposed to live along all application lifetime
	ID3DRasterizerState		*	m_rasterizer_state;		//	Weak link
	ID3DDepthStencilState	*	m_depth_stencil_state;	//	Weak link
	ID3DBlendState			*	m_blend_state;			//	Weak link

// 	D3D_RASTERIZER_DESC		m_rasterizer_desc;
// 	D3D_DEPTH_STENCIL_DESC	m_depth_stencil_desc;
// 	D3D_BLEND_DESC			m_effect_desc;

	u32						m_stencil_ref;
	u32						m_id;
	bool					m_is_registered;
};

typedef xray::intrusive_ptr<res_state, resource_intrusive_base, threading::single_threading_policy>	ref_state;

} // namespace render
} // namespace xray

#endif // #ifndef RES_STATE_H_INCLUDED