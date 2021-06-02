////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_INSTANCE_INLINE_H_INCLUDED
#define OBJECT_INSTANCE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline object_instance::object_instance	( parameter_type object_type, object_instance_type const& instance, pcstr object_caption ) :
	m_type								( object_type ),
	m_instance							( instance ),
	m_caption							( object_caption )
{
}

template < typename T >
inline void object_instance::set_instance	( object_instance_type& instance, T value )
{
	instance							= 0; // must be nulled in case of 64-bit pointers and 32-bit values
	instance							= value;
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef OBJECT_INSTANCE_INLINE_H_INCLUDED