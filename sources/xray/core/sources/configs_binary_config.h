////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_READER_H_INCLUDED
#define CONFIG_READER_H_INCLUDED

#include <xray/configs.h>

namespace xray {

namespace resources {
	class queries_result;
} // namespace resources

namespace core {
namespace configs {

class binary_config : public xray::configs::binary_config {
public:
					binary_config	( pcbyte buffer, u32 buffer_size, xray::memory::base_allocator* allocator );
	virtual		   ~binary_config	( );

	xray::memory::base_allocator*	get_allocator() const { return m_allocator; }
private:
			void	load			( pcbyte buffer, u32 buffer_size );

private:
	xray::memory::base_allocator*	m_allocator;
};

} // namespace configs
} // namespace core
} // namespace xray

#endif // #ifndef CONFIG_READER_H_INCLUDED