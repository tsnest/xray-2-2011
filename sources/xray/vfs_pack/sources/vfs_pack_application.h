////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FS_PACK_APPLICATION_H_INCLUDED
#define FS_PACK_APPLICATION_H_INCLUDED

#include "vfs_pack.h"
#include <xray/vfs/pack_archive.h>
#include <xray/fs/simple_synchronous_interface.h>

namespace vfs_pack {

class application 
{
public:
					application			();
			void	initialize			(u32 argc, pstr const * argv);
			void	execute				();
			void	finalize			();
	inline	u32		get_exit_code		() const { return m_exit_code; }

private:
	bool			parse_command_line	();
	bool			pack				();
	bool			unpack				();
	bool			patch				();
	bool			fill_pack_arguments	(xray::vfs::pack_archive_args & args);

private:
	xray::vfs::debug_info_enum			m_debug_info;
	xray::vfs::archive_platform_enum	m_platform;
	
	xray::uninitialized_reference< xray::fs_new::simple_synchronous_interface >	m_fs_devices;

	xray::logging::log_format			m_log_format;
	xray::logging::log_flags_enum		m_log_flags;

private:
	HWND								m_splash_screen;
	pcstr const*						m_argv;
	u32									m_argc;

private:
	u32									m_exit_code;
}; // class application


} // namespace vfs_pack

#endif // #ifndef FS_PACK_APPLICATION_H_INCLUDED