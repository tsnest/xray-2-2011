////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_VFS_WORLD_H_INCLUDED
#define XRAY_VFS_WORLD_H_INCLUDED

namespace xray {
namespace vfs {

struct XRAY_NOVTABLE world {
	virtual	void	tick		( ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world );
}; // class world

} // namespace vfs
} // namespace xray

#endif // #ifndef XRAY_VFS_WORLD_H_INCLUDED