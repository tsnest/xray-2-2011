////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_CONFIGS_BINARY_CONFIG_H_INCLUDED
#define XRAY_CONFIGS_BINARY_CONFIG_H_INCLUDED

#include <xray/configs.h>
#include <xray/resources.h>
#include <xray/configs_binary_config_value.h>

namespace xray {
namespace configs {

class XRAY_CORE_API binary_config :
	public core::noncopyable,
	public resources::unmanaged_resource
{
public:
	typedef binary_config_value::const_iterator			const_iterator;

public:
	virtual								~binary_config	( )	{ }
	inline	binary_config_value const&	get_root		( ) const					{ return *m_root; }
	inline	bool						empty			( ) const					{ return get_root().empty(); }
	inline	u32							size			( ) const					{ return get_root().size(); }
	inline	const_iterator				begin			( ) const					{ return get_root().begin(); }
	inline	const_iterator				end				( ) const					{ return get_root().end(); }
	inline	binary_config_value const&	operator[]		( u32 const index ) const	{ return get_root()[ index ]; }
	inline	binary_config_value const&	operator[]		( int const index ) const	{ return get_root()[ index ]; }
	inline	binary_config_value const&	operator[]		( pcstr const key ) const	{ return get_root()[ key ]; }

protected:
	binary_config_value*				m_root;
}; // class binary_config

typedef	resources::resource_ptr	<
	binary_config, 
	resources::unmanaged_intrusive_base
> binary_config_ptr;

template class XRAY_CORE_API resources::resource_ptr <
	binary_config, 
	resources::unmanaged_intrusive_base
>;

typedef std::pair< binary_config_value const*, binary_config_ptr > binary_config_value_ptr;

} // namespace configs
} // namespace xray

#endif // #ifndef XRAY_CONFIGS_BINARY_CONFIG_H_INCLUDED