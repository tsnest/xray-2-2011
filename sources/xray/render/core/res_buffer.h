////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_BUFFER_H_INCLUDED
#define RES_BUFFER_H_INCLUDED

#include "res_common.h"

namespace xray {
namespace render_dx10 {

enum enum_buffer_type { enum_buffer_type_vertex, enum_buffer_type_index};

class res_buffer : public res_base{

	friend class resource_manager;
	friend class res_base;

	
	res_buffer( u32 size, void const * data, enum_buffer_type type, bool dynamic);
	~res_buffer();
	void _free()	const;

public:

	inline void*	map		( D3D_MAP mode);
	inline void		unmap	();
	
	ID3DBuffer*		hw_buffer()	{ return m_hw_buffer;}
	u32				type()		{ return m_type;}
	u32				size()		{ return m_size;}

private:

	ID3DBuffer*			m_hw_buffer;
	u32					m_size;
	enum_buffer_type	m_type;
}; // class res_buffer

typedef	intrusive_ptr<res_buffer, res_base, threading::single_threading_policy>	ref_buffer;

} // namespace render
} // namespace xray

#include "res_buffer_inline.h"

#endif // #ifndef RES_BUFFER_H_INCLUDED