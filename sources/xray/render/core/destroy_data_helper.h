////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_DESTROY_DATA_HELPER_H_INCLUDED
#define XRAY_RENDER_CORE_DESTROY_DATA_HELPER_H_INCLUDED

namespace xray {
namespace render {

struct destroy_data_helper_base {
	virtual void	destroy	( pcvoid data ) = 0;
};

template < class T >
struct destroy_data_helper : public destroy_data_helper_base {
	virtual	void	destroy	( pcvoid data )
	{
		static_cast<T*>(data)->~T	( );
	}
};

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_DESTROY_DATA_HELPER_H_INCLUDED