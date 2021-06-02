////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_RESOURCE_INTRUSIVE_BASE_H_INCLUDED
#define XRAY_RENDER_CORE_RESOURCE_INTRUSIVE_BASE_H_INCLUDED

namespace xray {
namespace render {

class resource_intrusive_base //: private boost::noncopyable
{
public:
			inline			resource_intrusive_base	( );

	template < typename T >
	static	inline void		destroy					( T const* instance );

public:
	u32	mutable				m_reference_count;
};

} // namespace render
} // namespace xray

#include <xray/render/core/resource_intrusive_base_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_RESOURCE_INTRUSIVE_BASE_H_INCLUDED