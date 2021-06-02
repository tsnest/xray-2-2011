////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COMPRESSOR_H_INCLUDED
#define XRAY_COMPRESSOR_H_INCLUDED

#include <xray/memory_buffer.h>
#include <xray/debug/signalling_bool.h>

namespace xray {

class XRAY_CORE_API compressor
{
public:
	virtual xray::signalling_bool	compress	(const_buffer src, mutable_buffer dest, u32& out_size) = 0;
	virtual xray::signalling_bool	decompress	(const_buffer src, mutable_buffer dest, u32& out_size) = 0;

	virtual ~compressor () {}

}; // class compressor

} // namespace xray

#endif // #ifndef XRAY_PPMD_ALLOCATOR_H_INCLUDED