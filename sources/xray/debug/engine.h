////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEBUG_ENGINE_H_INCLUDED
#define XRAY_DEBUG_ENGINE_H_INCLUDED

struct _SYSTEMTIME;
struct _EXCEPTION_POINTERS;

namespace xray {

enum assert_enum;

namespace debug {

struct XRAY_NOVTABLE engine {

	virtual			~engine						() {}
	virtual bool	terminate_on_error			() const = 0;
	virtual void	on_terminate				() const = 0;
	virtual int  	get_exit_code				() const = 0;
	virtual bool	core_initialized			() const = 0;
	virtual void	generate_debug_file_name	(string_path &				file_name,
												 _SYSTEMTIME const * const	date_time,
												 pcstr const				report_id,
												 pcstr const				extension) const = 0;
	virtual void	flush_log_file				(pcstr file_name) const = 0;
	virtual bool	is_testing					() const = 0;
	virtual void	on_testing_exception		(assert_enum			assert_type, 
 												 pcstr					description, 
												 _EXCEPTION_POINTERS*	exception_information,
												 bool					is_assertion) const = 0;

	virtual pcstr	current_directory			() const = 0;
	virtual bool	create_folder_r				(pcstr path, bool create_last) const = 0;
	virtual bool	dont_check_multithreaded_safety	() const = 0;
	virtual pcstr	bugtrap_application_name ( ) const = 0;

}; // class engine

} // namespace debug
} // namespace xray

#endif // #ifndef XRAY_DEBUG_ENGINE_H_INCLUDED