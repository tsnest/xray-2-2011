////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CORE_INTERNAL_H_INCLUDED
#define CORE_INTERNAL_H_INCLUDED

namespace xray {
namespace fs_new {
	class	device_file_system_interface;
	class	synchronous_device_interface;
} // namespace fs_new

namespace core {
namespace platform {

void	preinitialize			( );

} // namespace platform

void	fill_user_name			( );

pcstr	user_name				( );
pcstr	current_directory		( );
pcstr	application_name		( );
pcstr   get_command_line		( );
pcstr	XRAY_CORE_API user_data_directory		( );

void	set_application_name	( pcstr application_name );
fs_new::device_file_system_interface *		get_core_device_file_system	( );
fs_new::synchronous_device_interface &		get_core_synchronous_device	( );

} // namespace core
} // namespace xray

#endif // #ifndef CORE_INTERNAL_H_INCLUDED