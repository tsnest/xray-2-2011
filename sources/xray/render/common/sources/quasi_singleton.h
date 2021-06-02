////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef QUASI_SINGLETON_H_INCLUDED
#define QUASI_SINGLETON_H_INCLUDED

#include <xray/debug_static_cast_checked.h>

namespace xray{

template <class T>
class quasi_singleton : boost::noncopyable
{
public:
	quasi_singleton();
	~quasi_singleton();

	static T*	get_ptr() { return pinst;}
	static T&	get_ref();

private:
	static T*	pinst;
}; // class quasi_singleton

template <class T>
quasi_singleton<T>::quasi_singleton()
{
	ASSERT(!pinst);
	pinst = static_cast_checked<T*>(this);
}

template <class T>
quasi_singleton<T>::~quasi_singleton()
{
	ASSERT(pinst);
	pinst = NULL;
}

template <class T>
T*	quasi_singleton<T>::pinst = 0;

template <class T>
T&	quasi_singleton<T>::get_ref()
{
	ASSERT(pinst);
	return *pinst;
}

} // namespace xray

#endif // #ifndef QUASI_SINGLETON_H_INCLUDED