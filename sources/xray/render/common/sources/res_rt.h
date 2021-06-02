////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_RT_H_INCLUDED
#define RES_RT_H_INCLUDED

namespace xray {
namespace render {


class res_rt : public res_named
{
public:
	res_rt();
	~res_rt();

	void create(LPCSTR name, u32 w, u32 h, D3DFORMAT fmt);
	void destroy();
	void reset_begin();
	void reset_end();
	
	void apply(render_target_enum target) {backend::get_ref().set_rt(target, m_rt);}

	inline bool	valid()	{return m_texture;}

private:
	ID3DTexture2D*			m_surface;//pSurface;
	ID3DRenderTargetView*	m_rt;//pRT;
#ifdef	USE_DX10
	ID3DDepthStencilView*	m_zrt;//pZRT;
#endif	//	USE_DX10
	ref_texture				m_texture;//pTexture;

	u32						m_width;//dwWidth;
	u32						m_height;//dwHeight;
	D3DFORMAT				m_format;//fmt;

	u64						m_order;//_order;
};

typedef intrusive_ptr<res_rt, res_base, threading::single_threading_policy> ref_rt;

} // namespace render 
} // namespace xray 


#endif // #ifndef RES_RT_H_INCLUDED
