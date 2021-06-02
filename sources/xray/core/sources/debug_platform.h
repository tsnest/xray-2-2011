////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_PLATFORM_H_INCLUDED
#define DEBUG_PLATFORM_H_INCLUDED

struct _EXCEPTION_POINTERS;
struct _SYSTEMTIME;

namespace xray {
namespace core {
namespace debug {

enum error_mode;

void set_thread_stack_guarantee			( );

namespace platform {

void setup_unhandled_exception_handler	( error_mode error_mode );
void change_unhandled_exception_handler ( error_mode error_mode );
bool error_after_dialog					( );
pstr fill_format_message				( int const error_code );
void free_format_message				( pstr const buffer );
pcstr get_graphics_api_error_description( int error_code );
void setup_storage_access_handler		( error_mode, void (*storage_access_handler) ( int ) );
void change_storage_access_handler		( error_mode, void (*storage_access_handler) ( int ) );

enum error_type_enum
{
	error_type_assert,
	error_type_unhandled_exception,
};

void on_error							( bool* do_debug_break, char* const message, u32 const message_size, bool* ignore_always, _EXCEPTION_POINTERS* exception_information, error_type_enum );
template < int count >
inline void on_error					( bool* do_debug_break, char (&message)[count], bool* ignore_always, _EXCEPTION_POINTERS* exception_information, error_type_enum const error_type )
{
	COMPILE_ASSERT						( sizeof(message) == sizeof(char)*count, different_sizeof_result_got );
	on_error							( do_debug_break, message, sizeof(message), ignore_always, exception_information, error_type );
}

void on_error_message_box				( bool* do_debug_break, char* const message, u32 const message_size, bool* ignore_always, _EXCEPTION_POINTERS* exception_information, error_type_enum);
template < int count >
inline void on_error_message_box		( bool* do_debug_break, char (&message)[count], bool* ignore_always, _EXCEPTION_POINTERS* exception_information, error_type_enum const error_type )
{
	COMPILE_ASSERT						( sizeof(message) == sizeof(char)*count, different_sizeof_result_got );
	on_error_message_box				( do_debug_break, message, sizeof(message), ignore_always, exception_information, error_type );
}

void terminate							( pcstr message, int error_code );
void format_message						( );
void prologue_dump_call_stack			( _EXCEPTION_POINTERS* exception_information );
void save_minidump						( _SYSTEMTIME const& date_time, _EXCEPTION_POINTERS* exception_information );

} // namespace platform

} // namespace debug
} // namespace core
} // namespace xray

#endif // #ifndef DEBUG_PLATFORM_H_INCLUDED