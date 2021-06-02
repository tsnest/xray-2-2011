////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef STATE_SIMULATOR_H_INCLUDED
#define STATE_SIMULATOR_H_INCLUDED

namespace xray {
namespace render {

class state_descriptor
{
	friend class resource_manager;
public:

	state_descriptor();

	// State initialization functions
	void	reset		( );
	
	// State setup functions
	void	set_depth				( bool enable, bool write_enable, D3D_COMPARISON_FUNC cmp_func = D3D_COMPARISON_LESS_EQUAL);
	void	set_stencil				( BOOL enable, u32 ref=0x00, u8 mask=0x00, u8 writemask=0x00);
	void	set_stencil_frontface	( D3D_COMPARISON_FUNC func = D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP fail=D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP pass=D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP zfail=D3D_STENCIL_OP_KEEP);
	void	set_stencil_backface	( D3D_COMPARISON_FUNC func = D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP fail=D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP pass=D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP zfail=D3D_STENCIL_OP_KEEP);
	void	set_stencil				( BOOL enable, u32 ref, u8 mask, u8 writemask, D3D_COMPARISON_FUNC func, D3D_STENCIL_OP fail, D3D_STENCIL_OP pass, D3D_STENCIL_OP zfail);
	void	set_stencil_ref			( u32 ref);
	void	set_fill_mode			( D3D_FILL_MODE fill_mode);
	
	void	set_alpha_blend			(BOOL is_blend_enabled /* Temporary !!!*/, 
									 D3D_BLEND src_blend = D3D_BLEND_ONE, 
									 D3D_BLEND dest_blend = D3D_BLEND_ZERO, 
									 D3D_BLEND_OP blend_op = D3D_BLEND_OP_ADD, 
									 D3D_BLEND src_alpha_blend = D3D_BLEND_ONE,
									 D3D_BLEND dest_alpha_blend = D3D_BLEND_ZERO,
									 D3D_BLEND_OP blend_alpha_op = D3D_BLEND_OP_ADD);
	
	void	set_alpha_to_coverage	( BOOL is_enabled );
	// Needed functionality to set color write mode for each render target.
	void	color_write_enable		( D3D_COLOR_WRITE_ENABLE mode = D3D_COLOR_WRITE_ENABLE_ALL);

	void	set_cull_mode			( D3D_CULL_MODE mode);



	// This need to be passed through a shader constant.
	//void	set_alpha_ref			( u32 ref);

private:

// 	void reset		( D3D_RASTERIZER_DESC &desc);
// 	void reset		( D3D_DEPTH_STENCIL_DESC &desc);
// 	void reset		( D3D_BLEND_DESC &desc);

private:
	D3D_RASTERIZER_DESC			m_rasterizer_desc;
	D3D_DEPTH_STENCIL_DESC		m_depth_stencil_desc;
	D3D_BLEND_DESC				m_effect_desc;

	u32							m_stencil_ref;

	bool						m_rasterizer_desc_updated;
	bool						m_depth_stencil_desc_updated;
	bool						m_effect_desc_updated;

}; // class state_descriptor

} // namespace render
} // namespace xray

#endif // #ifndef STATE_SIMULATOR_H_INCLUDED