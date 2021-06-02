////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_RT_H_INCLUDED
#define RES_RT_H_INCLUDED


#include "res_texture.h"

namespace xray {
namespace render_dx10 {


class res_rt : public res_named
{
	friend class resource_manager;
	friend class res_base;
	res_rt		();
	~res_rt		();
	void _free	() const;

public:
	enum enum_usage	{ enum_usage_depth_stencil, enum_usage_render_target};


	void create		( LPCSTR name, u32 w, u32 h, DXGI_FORMAT fmt, enum_usage usage);
	void destroy	();

	ID3DRenderTargetView * get_taget_view() const			{ return m_rt;}
	ID3DDepthStencilView * get_depth_stencil_view() const	{ return m_zrt;}
	
	inline bool	valid()	{return m_texture;}

	u32			width()		{ return m_width;}
	u32			height()	{ return m_height;}
	DXGI_FORMAT	format()	{ return m_format;}

private:
	ID3DTexture2D*			m_surface;
	ID3DRenderTargetView*	m_rt;
	ID3DDepthStencilView*	m_zrt;
	ref_texture				m_texture;

	u32						m_width;
	u32						m_height;
	DXGI_FORMAT				m_format;
	enum_usage				m_usage;

	u64						m_order;
};

typedef intrusive_ptr<res_rt, res_base, threading::single_threading_policy> ref_rt;

} // namespace render 
} // namespace xray 


#endif // #ifndef RES_RT_H_INCLUDED
