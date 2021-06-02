////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STATIC_TYPE_H_INCLUDED
#define STATIC_TYPE_H_INCLUDED

namespace xray {
namespace render {

struct static_type {
	static u16 type_id_counter;
	template<class T> static u16 get_type_id()
	{
		static u16 current_id = ++static_type::type_id_counter;
		return current_id;
	}
};

} // namespace render
} // namespace xray

#endif // #ifndef STATIC_TYPE_H_INCLUDED