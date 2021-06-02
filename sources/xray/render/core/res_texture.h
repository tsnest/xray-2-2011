////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_TEXTURE_DX11_H_INCLUDED
#define RES_TEXTURE_DX11_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>

namespace xray {
namespace render {

class noncopyable_protected
{
protected:
	noncopyable_protected() {}
	~noncopyable_protected() {}

	noncopyable_protected( const noncopyable_protected& ) {}
	const noncopyable_protected& operator=( const noncopyable_protected& ) { return *this;}
};

class res_texture :
	public resource_intrusive_base,
	public noncopyable_protected // Needed in "clone" function.
{
	friend class resource_intrusive_base;
	friend class resource_manager;
	friend class render_target;
	friend class res_render_output;

	res_texture	( bool pool_texture = false);
	virtual ~res_texture();
	void destroy_impl	() const;
	
	// For friend classes calls only.
	void set_hw_texture	( ID3DBaseTexture* surface, u32 mip_level_cut = 0, bool staging = false, bool srgb = false);

public:
	
	void save_as(pcstr file_name);
	
	ID3DBaseTexture *			hw_texture	();
	ID3DShaderResourceView *	view		()		{ return m_sh_res_view;}
	D3D_TEXTURE2D_DESC const &	description	()		{ return m_desc;}

	u32				width			() const { desc_enshure(); return m_desc.Width;}
	u32				height			() const { desc_enshure(); return m_desc.Height;}
	u32				mips_count		() const { desc_enshure(); return m_desc.MipLevels;}
	u32				array_size		() const { desc_enshure(); return m_desc.ArraySize;}
	DXGI_FORMAT		format			() const { desc_enshure(); return m_desc.Format;}
	bool			pool_texture1	() const { return	m_pool_texture;}

	inline bool		is_registered	() const{ return m_is_registered; }
	inline void		mark_registered	()		{ R_ASSERT( !m_is_registered); m_is_registered = true; }

	
//  	template<typename T>
// 	T*				map2D			( u32 start_x, u32 start_y, u32 width, u32 height, u32 mip_level, D3D_MAPPED_TEXTURE2D & buffer);	

	void*			map2D			( D3D_MAP mode, u32 mip_level, u32& row_pitch);	
	void			unmap2D			( u32 mip_level);	

	void*			map3D			( D3D_MAP mode, u32 mip_level, u32& row_pitch);	
	void			unmap3D			( u32 mip_level);	

	inline pcstr	name( ) const { return m_name.c_str(); }
	inline void		set_name		( pcstr const name ) { m_name = name; }

public:
	// This function is needed to to replace the content of the texture with another that the 
	// texture can bound to any shader(s) that we can't rebound shader to another one.
	void			clone 			( res_texture* other);

	// For texture color renormalization in pixel shader.
	float4			m_rescale_min;
	float4			m_rescale_max;

private:
//	bool			desc_valid		();   {return m_surface == m_desc_cache_surface;}
	void			desc_enshure	() const	{}// {if (!desc_valid()) desc_update();}
	void			desc_update		();

	D3D_USAGE		get_usage		();
	
private:
	u32				m_loaded	 : 1;	//bLoaded
	u32				m_user		 : 1;	//bUser
	u32				m_seq_cycles : 1;	//seqCycles
	u32				m_mem_usage	 : 28;	//MemoryUsage
		
#	ifdef	USE_DX10_OLD
	u32					m_is_loaded_as_staging: 1;//bLoadedAsStaging
#	endif	//	USE_DX10

	fastdelegate::FastDelegate1<u32>	m_bind;	
	D3D_TEXTURE2D_DESC					m_desc;
	D3D_TEXTURE3D_DESC					m_desc_3d;
	fs_new::virtual_path_string						m_name;
	
	ID3DBaseTexture*					m_surface;		
	ID3DBaseTexture*					m_desc_cache_surface;	
	ID3DShaderResourceView*				m_sh_res_view;		
	u32									m_mip_level_cut;
	bool								m_desc_valid;
	bool								m_desc_3d_valid;
	bool								m_pool_texture;
	bool								m_is_registered;
};

typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;

} // namespace render 
} // namespace xray 

#endif // #ifndef RES_TEXTURE_DX11_H_INCLUDED