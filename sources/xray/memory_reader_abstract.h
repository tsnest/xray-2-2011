////////////////////////////////////////////////////////////////////////////
//	Created 	: 19.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMORY_READER_ABSTRACT_H_INCLUDED
#define XRAY_MEMORY_READER_ABSTRACT_H_INCLUDED

namespace xray {
namespace memory {

class XRAY_CORE_API reader_abstract {
public:
	virtual	void	r	( pvoid& buffer, u32 size ) = 0;
}; // class reader_abstract

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_MEMORY_READER_ABSTRACT_H_INCLUDED