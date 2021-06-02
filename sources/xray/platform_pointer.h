////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PLATFORM_POINTER_TYPE_H_INCLUDED
#define XRAY_PLATFORM_POINTER_TYPE_H_INCLUDED

namespace xray {

enum platform_pointer_enum {
	platform_pointer_32bit, 
	platform_pointer_64bit,
#if XRAY_PLATFORM_WINDOWS
	platform_pointer_default	=	platform_pointer_64bit,
#else // #if XRAY_PLATFORM_WINDOWS
	platform_pointer_default	=	platform_pointer_32bit,
#endif // #if XRAY_PLATFORM_WINDOWS
};

template <class T, platform_pointer_enum  PointerSize >
struct platform_pointer_selector
{
	typedef T *		type;
	typedef T *		pod_type;
}; // struct platform_pointer_selector

template < typename T >
struct platform_pointer_selector_void_helper
{
	struct XRAY_CORE_API helper_pod
	{
		union
		{
			T *		pointer;
			u64		max_storage;
		};

		helper_pod &	operator =		(T * const in_pointer)	{ max_storage = 0; pointer = in_pointer; return *this; }
					operator T * &		()						{ return pointer; }
					operator T *		() const				{ return pointer; }
	};

	struct XRAY_CORE_API helper : public helper_pod
	{
 					helper				()						{ max_storage = 0; }
 					helper				(T * const in_pointer)	{ max_storage = 0; pointer = in_pointer; }
		helper &	operator =			(T * const in_pointer)	{ pointer = in_pointer; return *this; }
		helper &	operator =			(helper const & in_pointer)	{ max_storage = in_pointer.max_storage; return * this; }
	};
}; // struct platform_pointer_selector_void_helper

template <>
struct platform_pointer_selector< void, platform_pointer_64bit >
{
	typedef platform_pointer_selector_void_helper<void>::helper		type;
	typedef platform_pointer_selector_void_helper<void>::helper_pod	pod_type;
}; // struct platform_pointer_selector

template <>
struct platform_pointer_selector< void const, platform_pointer_64bit >
{
	typedef platform_pointer_selector_void_helper<void const>::helper		type;
	typedef platform_pointer_selector_void_helper<void const>::helper_pod	pod_type;
}; // struct platform_pointer_selector

template <class T>
struct platform_pointer_selector< T, platform_pointer_64bit >
{
	struct XRAY_CORE_API helper_pod
	{
		union
		{
			T *		pointer;
			u64		max_storage;
		};

		helper_pod &	operator =		(T * const in_pointer)	{ max_storage = 0; pointer = in_pointer; return * this; }

		T *			operator ->			()						{ return pointer; }
		T const *	operator ->			() const				{ return pointer; }
		T &			operator *			()						{ return * pointer; }
		T const &	operator *			() const				{ return * pointer; }

					operator T * &		()						{ return pointer; }
					operator T *		() const				{ return pointer; }
	};

	struct XRAY_CORE_API helper : public helper_pod
	{		
					helper				() { helper_pod::max_storage = 0; }
					helper				(T * const in_pointer)	{ helper_pod::max_storage = 0; helper_pod::pointer = in_pointer; }

		helper &	operator =			(T * const in_pointer)	{ helper_pod::pointer = in_pointer; return * this; }
		helper &	operator =			(helper const & in_pointer)	{ helper_pod::max_storage = in_pointer.max_storage; return * this; }
	};

	typedef helper_pod	pod_type;
	typedef helper		type;
}; // struct platform_pointer_selector

template <class T, platform_pointer_enum PointerSize>
struct platform_pointer
{
	typedef typename platform_pointer_selector<T, PointerSize>::type		type;
	typedef typename platform_pointer_selector<T, PointerSize>::pod_type	pod_type;
}; // struct platform_pointer

} // namespace xray

#endif // #ifndef XRAY_PLATFORM_POINTER_TYPE_H_INCLUDED