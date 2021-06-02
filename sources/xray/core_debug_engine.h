////////////////////////////////////////////////////////////////////////////
//	Created		: 07.11.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CORE_DEBUG_ENGINE_H_INCLUDED
#define CORE_DEBUG_ENGINE_H_INCLUDED

#include <xray/debug/engine.h>

namespace xray {
namespace core {

class core_debug_engine : public debug::engine
{
public:
	virtual bool	dont_check_multithreaded_safety	( ) const;
	virtual bool	core_initialized		( ) const;
	virtual bool	terminate_on_error		( ) const;
	virtual void	on_terminate			( ) const;
	virtual int		get_exit_code			( ) const;
	virtual u32		build_station_build_id	( ) const;
	virtual void	generate_debug_file_name	( string_path &				file_name,
												  _SYSTEMTIME const * const	date_time,
												  pcstr	const				report_id,
												  pcstr	const				extension ) const;

	virtual void	flush_log_file			( pcstr file_name ) const;
	virtual bool	is_testing				( ) const;
	virtual void	on_testing_exception	( assert_enum			assert_type, 
											  pcstr					description, 
											  _EXCEPTION_POINTERS*	exception_information,
											  bool					is_assertion ) const;
	virtual pcstr	current_directory		( ) const;
	virtual bool	create_folder_r			( pcstr path, bool create_last ) const;
	virtual pcstr	bugtrap_application_name ( ) const;

}; // class core_debug_engine

} // namespace core
} // namespace xray

#endif // #ifndef CORE_DEBUG_ENGINE_H_INCLUDED