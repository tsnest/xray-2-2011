////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_STORAGE_H_INCLUDED
#define TEXTURE_STORAGE_H_INCLUDED

namespace xray {
namespace render {

class res_texture;
class texture_pool;

class texture_storage {
public:

#if 0
	void initialize( xray::configs::lua_config_value  const & initial_params, size_t memory_amount, D3D_USAGE usage);
#endif

	texture_storage();
	~texture_storage();

	res_texture*	get		( u32 width, u32 height, DXGI_FORMAT format);
	void			release	( res_texture const* texture);

private:

	typedef map<u32, texture_pool*> pools;
	pools	m_pools;
	bool	m_initialized;

}; // class texture_storage

} // namespace render
} // namespace xray

#endif // #ifndef TEXTURE_STORAGE_H_INCLUDED