////////////////////////////////////////////////////////////////////////////
//	Module 		: std_extensions.h
//	Created 	: 27.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : STD namespace extensions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_STD_EXTENSIONS_H_INCLUDED
#define CS_CORE_STD_EXTENSIONS_H_INCLUDED

#include <algorithm>

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <stack>
#include <map>
#include <set>

#include <cs/core/memory_allocator.h>

template <typename T>
class cs_vector : public std::vector<T,cs_memory_allocator<T> > {
private:
	typedef std::vector<T,cs_memory_allocator<T> > inherited;

public:
	inline					cs_vector	() :
		inherited	()
	{
	}

	inline	explicit		cs_vector	(size_t count) :
		inherited	(count)
	{
	}

	inline					cs_vector	(size_t count, T const& value) :
		inherited	(count, value)
	{
	}

	inline	typename inherited::const_reference operator[]	(typename inherited::size_type position) const
	{
		ASSERT		(position<size(), "index is out of bounds");
		return		inherited::operator[](position);
	}

	inline	typename inherited::reference		operator[]	(typename inherited::size_type position)
	{
		ASSERT		(position<size(), "index is out of bounds");
		return		inherited::operator[](position);
	}

	inline	u32				size		() const
	{
		return		(u32)inherited::size();
	}
};

template <typename T>									class	cs_list 			: public std::list<T,cs_memory_allocator<T> >												{ public: };
template <typename T>									class	cs_deque 			: public std::deque<T,cs_memory_allocator<T> >											{ public: };
template <typename T>									class	cs_stack 			: public std::stack<T,cs_memory_allocator<T> >											{ public: };
template <typename K, class P=std::less<K> >			class	cs_set				: public std::set<K,P,cs_memory_allocator<K> >											{ public: };
template <typename K, class P=std::less<K> >			class	cs_multiset			: public std::multiset<K,P,cs_memory_allocator<K> >										{ public: };
template <typename K, class V, class P=std::less<K> >	class	cs_map 				: public std::map<K,V,P,cs_memory_allocator<std::pair<const K,V> > >						{ public: };
template <typename K, class V, class P=std::less<K> >	class	cs_multimap			: public std::multimap<K,V,P,cs_memory_allocator<std::pair<const K,V> > >					{ public: };
//template <typename K, typename hash, typename equal >	class	cs_hash_multiset	: public stdext::hash_multiset<K,hash,equal,cs_memory_allocator<K> >							{ public: };
//template <typename K, class V, typename hash, typename equal >	class	cs_hash_multimap	: public stdext::hash_multimap<K,V,hash,equal,cs_memory_allocator<std::pair<const K,V> > >	{ public: };

typedef	 std::basic_string<char, std::char_traits<char>, cs_memory_allocator<char> >	cs_string;

#endif // #ifndef CS_CORE_STD_EXTENSIONS_H_INCLUDED