////////////////////////////////////////////////////////////////////////////
//	Created		: 28.02.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_VFS_PLATFORM_CONFIGURATION_H_INCLUDED
#define XRAY_VFS_PLATFORM_CONFIGURATION_H_INCLUDED

namespace xray {
namespace vfs {

// forward declarations:
template <platform_pointer_enum PointerSize = platform_pointer_default>
class base_node;

template <platform_pointer_enum PointerSize = platform_pointer_default>
class archive_folder_mount_root_node;

template <platform_pointer_enum PointerSize = platform_pointer_default>
class mount_root_node_base;

template <platform_pointer_enum PointerSize>
struct pointer_size_to_int_type
{
	typedef	u32		int_type;
};
template <>
struct pointer_size_to_int_type<platform_pointer_64bit>
{
	typedef	u64		int_type;
};

template <  
			class PointerType,
			class T, 
			platform_pointer_enum PointerSize 
		 > 
inline	PointerType	file_size_type_to_platform_pointer_impl (file_size_type value)
{
	PointerType								out_result;
	typedef typename	pointer_size_to_int_type<PointerSize>::int_type	pointer_int;
	R_ASSERT								(value < std::numeric_limits<pointer_int>::max());
	* (pointer_int *) & out_result		=	(pointer_int)value;
	return									out_result;
}

template <class T, platform_pointer_enum PointerSize> inline
typename platform_pointer<T, PointerSize>::type		file_size_type_to_platform_pointer (file_size_type value)
{
	return	file_size_type_to_platform_pointer_impl< platform_pointer<T, PointerSize>::type, T, PointerSize >(value);
}

template <class T, platform_pointer_enum PointerSize> inline
typename platform_pointer<T, PointerSize>::pod_type		file_size_type_to_platform_pointer_pod (file_size_type value)
{
	return	file_size_type_to_platform_pointer_impl< platform_pointer<T, PointerSize>::pod_type, T, PointerSize >(value);
}

} // namespace vfs
} // namespace xray

#endif // #ifndef XRAY_VFS_PLATFORM_CONFIGURATION_H_INCLUDED