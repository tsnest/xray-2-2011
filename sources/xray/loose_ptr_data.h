////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOOSE_PTR_DATA_H_INCLUDED
#define XRAY_LOOSE_PTR_DATA_H_INCLUDED

namespace xray {

class loose_ptr_base;

class loose_ptr_data {
public:
	inline			loose_ptr_data	( loose_ptr_base* const pointer );
	template < typename T >
	inline	void	destroy			( T* object ) const;

public:
	loose_ptr_base*					m_pointer;
	threading::atomic32_type		m_reference_count;
};

} // namespace xray

#include <xray/loose_ptr_data_inline.h>

#endif // #ifndef XRAY_LOOSE_PTR_DATA_H_INCLUDED