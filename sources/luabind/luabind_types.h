////////////////////////////////////////////////////////////////////////////
//	Module 		: luabind_types.h
//	Created 	: 18.03.2007
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : luabind types
////////////////////////////////////////////////////////////////////////////

#ifndef LUABIND_TYPES_H_INCLUDED
#define LUABIND_TYPES_H_INCLUDED

#include <memory>
#include <string>
#include <vector>
#include <map>

#include <luabind/luabind_memory_allocator.h>

namespace luabind {
	typedef void* memory_allocation_function_parameter;
	typedef void* (CS_CALL *memory_allocation_function_pointer) (memory_allocation_function_parameter parameter, void const *, size_t);

	template	<typename T>									class	vector 		: public std::vector<T,memory_allocator<T> >							{ public: inline unsigned int size() const { return (unsigned int)(std::vector<T,memory_allocator<T> >::size());} };
	template	<typename K, class V, class P=std::less<K> >	class	map 		: public std::map<K,V,P,memory_allocator<std::pair<const K,V> > >		{ public: };

	typedef		std::basic_string<char, std::char_traits<char>, memory_allocator<char> >	string;
} // namespace luabind

#endif // #ifndef LUABIND_TYPES_H_INCLUDED