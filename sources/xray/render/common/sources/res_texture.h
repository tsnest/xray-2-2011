////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_TEXTURE_H_INCLUDED
#	define RES_TEXTURE_H_INCLUDED

namespace xray {
namespace render {


//#include "../../xrCore/res_base.h"

//class  ENGINE_API CAviPlayerCustom;
//class  CTheoraSurface;

class res_texture :
	public res_named,
	public boost::noncopyable
{
public:
	//	Since DX10 allows up to 128 unique textures, 
	//	distance between enum values should be at leas 128
	//	Don't change this since it's hardware-dependent
	enum resource_shader_type
	{
		rst_pixel = 0,	//	Default texture offset
		rst_vertex = D3DVERTEXTEXTURESAMPLER0,
		rst_geometry = rst_vertex+256
	};

public:
	res_texture();
	virtual ~res_texture();

	void apply(u32 stage)
	{
		backend::get_ref().set_texture(stage, m_surface);
	}

	//void /*__stdcall*/ apply_load(u32 stage);
	//void __stdcall apply_theora(u32 stage);
	//void __stdcall apply_avi(u32 stage);
	//void __stdcall apply_seq(u32 stage);
	//void __stdcall apply_normal(u32 stage);

	//void preload();
	//void load();
	//void postload();
	//void unload();

	ID3DBaseTexture* get_surface();

	void set_surface(ID3DBaseTexture* surface);

	//inline BOOL	isUser();// {return flags.bUser;}

	inline u32	get_width ();// {desc_enshure(); return desc.Width;}
	inline u32  get_height();// {desc_enshure(); return desc.Height;}

	//void video_sync(u32 time);// {m_play_time = time;}
	//void video_play(bool looped, u32 time=0xFFFFFFFF);
	//void video_pause(bool state);
	//void video_stop();
	//bool video_is_playing();

private:
	bool desc_valid();//   {return pSurface == desc_cache;}
	void desc_enshure();// {if (!desc_valid()) desc_update();}
	void desc_update();

#	ifdef	USE_DX10
	void apply(u32 dwStage);
	void process_staging();

	D3D10_USAGE get_usage();
#	endif	//	USE_DX10

private:
	u32					m_loaded	 : 1;	//bLoaded
	u32					m_user		 : 1;	//bUser
	u32					m_seq_cycles : 1;	//seqCycles
	u32					m_mem_usage	 : 28;	//MemoryUsage
#	ifdef	USE_DX10
	u32					m_is_loaded_as_staging: 1;//bLoadedAsStaging
#	endif	//	USE_DX10

	fastdelegate::FastDelegate1<u32>	m_bind;	//bind


	//CAviPlayerCustom*					m_avi;		//pAVI
	//CTheoraSurface*					m_theora;	//pTheora
	//float								m_material;
	//shared_string						m_bumpmap;

	//union
	//{
	//	u32								m_play_time;		//m_play_time	// sync theora time
	//	u32								m_seq_ms_per_frame;	//seq_MSPF			// Sequence data milliseconds per frame
	//};

	ID3DBaseTexture*					m_surface;		//pSurface
	//render::vector<ID3DBaseTexture*>			m_seq_data;		//seqDATA 	// Sequence data

	// Description
	ID3DBaseTexture*					m_desc_cache;	//desc_cache
	D3D_TEXTURE2D_DESC					m_desc;			//desc

#	ifdef	USE_DX10
	ID3D10ShaderResourceView*			m_sh_res_view;		//m_pSRView
	// Sequence view data
	render::vector<ID3D10ShaderResourceView*>	m_seq_sh_res_view;	//m_seqSRView
#	endif	//	USE_DX10
};
typedef intrusive_ptr<res_texture, res_base, threading::single_threading_policy> ref_texture;

} // namespace render 
} // namespace xray 


#endif // #ifndef RES_TEXTURE_H_INCLUDED

