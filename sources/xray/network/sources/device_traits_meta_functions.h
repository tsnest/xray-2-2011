////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DEVICE_TRAITS_META_FUNCTIONS_H_INCLUDED
#define DEVICE_TRAITS_META_FUNCTIONS_H_INCLUDED

#include <xray/debug/signalling_bool.h>

namespace xray
{

template <typename T>
struct is_seekable_writer
{
	typedef	typename T::size_type			size_type;

	template <void (T::*seek_func_spec)(size_type const)>
	struct seek_func_spec {};
	template <size_type (T::*position_func_spec)() const>
	struct position_func_spec {};
	
	template <typename C>
	static u8	test	(seek_func_spec<&C::seek>*,
						 position_func_spec<&C::position>*);
	template <typename C>
	static u32	test	(...);
	enum
	{
		result = (sizeof(test<T>(NULL, NULL)) == sizeof(u8))
	};
}; // struct is_seekable

template <typename T>
struct is_seekable_reader
{
	typedef	typename T::size_type			size_type;

	template <signalling_bool (T::*seek_func)(size_type const)>
	struct seek_func_spec {};
	template <size_type (T::*position_func)() const>
	struct position_func_spec {};
	template <size_type (T::*size_func)() const>
	struct size_func_spec {};

	template <typename C>
	static u8	test	(seek_func_spec<&C::seek>*,
						 position_func_spec<&C::position>*,
						 size_func_spec<&C::size>*);
	template <typename C>
	static u32	test	(...);
	enum
	{
		result = (sizeof(test<T>(NULL, NULL, NULL)) == sizeof(u8))
	};
}; // struct is_seekable_reader

template <typename T>
struct has_direct_read_access
{
	typedef	typename T::size_type				size_type;
	typedef typename T::const_pointer_type		const_pointer_type;

	template <const_pointer_type (T::*data_func)() const>
	struct data_func_spec {};
	template <const_pointer_type (T::*current_pointer_func)() const>
	struct current_pointer_func_spec {};

	template <typename C>
	static u8	test	(data_func_spec<&C::data>*,
						 current_pointer_func_spec<&C::pointer>*);
	template <typename C>
	static u32	test	(...);
	enum
	{
		result = (sizeof(test<T>(NULL, NULL)) == sizeof(u8))
	};
}; // struct struct has_direct_read_access

template <bool Condition, typename Base>
class base_if
{
};

template <typename Base>
class base_if<true, Base> : public Base
{
};

} // namespace xray

#endif // #ifndef DEVICE_TRAITS_META_FUNCTIONS_H_INCLUDED