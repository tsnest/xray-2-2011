////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STATE_UTILS_H_INCLUDED
#define STATE_UTILS_H_INCLUDED

namespace xray {
namespace render {

namespace state_utils
{
	//	Set description to default values
	void	reset( D3D_RASTERIZER_DESC &desc);
	void	reset( D3D_DEPTH_STENCIL_DESC &desc);
	void	reset( D3D_BLEND_DESC &desc);
	void	reset( D3D_SAMPLER_DESC &desc);

	//	State comparison ( memcmp doesn't work due to padding bytes in structure)
	bool	operator==( const D3D_RASTERIZER_DESC &desc1, const D3D_RASTERIZER_DESC &desc2);
	bool	operator==( const D3D_DEPTH_STENCIL_DESC &desc1, const D3D_DEPTH_STENCIL_DESC &desc2);
	bool	operator==( const D3D_BLEND_DESC &desc1, const D3D_BLEND_DESC &desc2);
	bool	operator==( const D3D_SAMPLER_DESC &desc1, const D3D_SAMPLER_DESC &desc2);

	//	Calculate hash values
	u32		get_hash( const D3D_RASTERIZER_DESC &desc);
	u32		get_hash( const D3D_DEPTH_STENCIL_DESC &desc);
	u32		get_hash( const D3D_BLEND_DESC &desc);
	u32		get_hash( const D3D_SAMPLER_DESC &desc);

	//	Modify state to meet DX10 automatic modifications
	void	check_validity( D3D_RASTERIZER_DESC const &desc);
	void	check_validity( D3D_DEPTH_STENCIL_DESC const &desc);
	void	check_validity( D3D_BLEND_DESC const &desc);
	void	check_validity( D3D_SAMPLER_DESC const &desc);
} // namespace state_utils

} // namespace render
} // namespace xray

#endif // #ifndef STATE_UTILS_H_INCLUDED
