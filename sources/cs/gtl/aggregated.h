////////////////////////////////////////////////////////////////////////////
//	Module 		: aggregated.h
//	Created 	: 17.04.2005
//  Modified 	: 29.04.2008
//	Author		: Dmitriy Iassenev
//	Description : aggregated template class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_AGGREGATED_H_INCLUDED
#define CS_GTL_AGGREGATED_H_INCLUDED

namespace gtl {

template <typename object_type, bool use_pointer = true>
class aggregated {
public:
	typedef object_type	object_type;

private:
	object_type*			m_object;

public:
	inline				aggregated	();

protected:
	virtual aggregated*	construct	(object_type* object);

public:
	inline	object_type&	object		() const;
}; // class aggregated_static

template <typename object_type>
class aggregated<object_type,false> {
public:
	typedef object_type	object_type;

private:
	mutable object_type	m_object;

protected:
	virtual aggregated*	construct	(object_type const& object);

public:
	inline	object_type&	object		() const;
}; // class aggregated_static<object_type,false>

} // namespace gtl

#include <cs/gtl/aggregated_inline.h>

#endif // #ifndef CS_GTL_AGGREGATED_H_INCLUDED