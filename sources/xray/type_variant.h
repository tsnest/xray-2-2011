////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_TYPE_VARIANT_H_INCLUDED
#define XRAY_TYPE_VARIANT_H_INCLUDED

namespace xray {

namespace detail {
	class abstract_type_helper;
} // namespace detail

#pragma warning(push)
#pragma warning(disable:4324)

template <int MaxSize>
class variant
{
public:
			variant		();
			variant		(variant const & other);
		   ~variant		();

	template <class T>
	void	set			(T const & value);

	template <class T>
	bool	try_get		(T & out_value);

	void	operator =	(variant const & other);

private:
	void	destroy_previous_variable_if_needed	();

private:
#ifdef XRAY_STATIC_LIBRARIES
	char	XRAY_DEFAULT_ALIGN			m_helper_storage[sizeof(pvoid)];
	char	XRAY_DEFAULT_ALIGN			m_storage[MaxSize];
#else // #ifdef XRAY_STATIC_LIBRARIES
	char								m_helper_storage[sizeof(pvoid)];
	char								m_storage[MaxSize];
#endif // #ifdef XRAY_STATIC_LIBRARIES

	xray::detail::abstract_type_helper*	m_helper;
	u32									m_type_id;
};

#pragma warning( pop )

} // namespace xray

#include <xray/type_variant_inline.h>

#endif // #ifndef XRAY_TYPE_VARIANT_H_INCLUDED