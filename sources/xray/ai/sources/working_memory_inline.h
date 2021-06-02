////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef WORKING_MEMORY_INLINE_H_INCLUDED
#define WORKING_MEMORY_INLINE_H_INCLUDED

namespace xray {
namespace ai {

template < class Predicate >
inline void working_memory::for_each( percept_memory_object_types knowledge_type, Predicate& pred ) const
{
	knowledge const& knowledge_by_type	= m_percept_objects[knowledge_type];
	knowledge_by_type.for_each			( pred );
}

} // namespace ai
} // namespace xray

#endif // #ifndef WORKING_MEMORY_INLINE_H_INCLUDED