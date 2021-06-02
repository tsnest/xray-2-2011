////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_HELPER_INLINE_H_INCLUDED
#define FS_HELPER_INLINE_H_INCLUDED

namespace xray {
namespace fs {

template <class Ordinal>
void	  reverse_bytes (Ordinal & res)
{
	char* const	it_begin			=	reinterpret_cast<char*>(&res);
	char* const	it_end				=	it_begin + sizeof(Ordinal);
	std::reverse						(it_begin, it_end);
}

} // namespace fs
} // namespace xray 

#endif // FS_HELPER_INLINE_H_INCLUDED