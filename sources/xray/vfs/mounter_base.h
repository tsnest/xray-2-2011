////////////////////////////////////////////////////////////////////////////
//	Created		: 20.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_MOUNTER_BASE_H_INCLUDED
#define VFS_MOUNTER_BASE_H_INCLUDED

namespace xray {
namespace vfs {

class mounter;
class mounter_base
{
public:
	mounter *							prev;
	mounter *							next;
};

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_MOUNTER_BASE_H_INCLUDED