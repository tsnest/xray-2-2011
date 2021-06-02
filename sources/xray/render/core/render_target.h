////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_RT_H_INCLUDED
#define RES_RT_H_INCLUDED


#include "res_texture.h"

namespace xray {
namespace render {

enum enum_rt_usage	{ enum_rt_usage_depth_stencil, enum_rt_usage_render_target};

class render_target : public resource_intrusive_base {
	friend class resource_manager;
	friend class resource_intrusive_base;
	render_target		();
	~render_target		();
	void destroy_impl	() const;

public:
	void save_as		(pcstr file_name);
	
	void create		( LPCSTR name, u32 w, u32 h, DXGI_FORMAT fmt, enum_rt_usage usage, D3D_USAGE memory_usage = D3D_USAGE_DEFAULT, u32 sample_count = 1);
	void create		( LPCSTR name, u32 w, u32 h, DXGI_FORMAT fmt, enum_rt_usage usage, ref_texture texture, u32 first_array_slice_index = 0);
	void create_3d	( pcstr name,  u32 width, u32 height, u32 depth, DXGI_FORMAT fmt, enum_rt_usage usage, D3D_USAGE memory_usage );
	
	void destroy	();

	ID3DRenderTargetView * get_target_view() const			{ return m_rt;}
	ID3DDepthStencilView * get_depth_stencil_view() const	{ return m_zrt;}
	
	inline bool	valid()	{return m_texture;}

	u32			width()		{ return m_width;}
	u32			height()	{ return m_height;}
	DXGI_FORMAT	format()	{ return m_format;}
	enum_rt_usage	usage()	{ return m_usage;}

	inline shared_string name() const { return m_name; }
	inline void	set_name	( pcstr const name ) { R_ASSERT( !m_name.c_str() ); m_name = name; }
	inline bool		is_registered	() const{ return m_is_registered; }
	inline void		mark_registered	()		{ R_ASSERT( !m_is_registered); m_is_registered = true; }
	
	ref_texture	get_texture() const { return m_texture; }
	
private:
	shared_string			m_name;

	ID3DTexture2D*			m_surface;
	ID3DTexture3D*			m_surface_3d;
	ID3DRenderTargetView*	m_rt;
	ID3DDepthStencilView*	m_zrt;
	ref_texture				m_texture;

	u32						m_width;
	u32						m_height;
	DXGI_FORMAT				m_format;
	enum_rt_usage			m_usage;

	u64						m_order;
	bool					m_is_registered;
};

typedef intrusive_ptr<render_target, resource_intrusive_base, threading::single_threading_policy> ref_rt;

} // namespace render 
} // namespace xray 


#endif // #ifndef RES_RT_H_INCLUDED