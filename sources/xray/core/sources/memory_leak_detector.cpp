////////////////////////////////////////////////////////////////////////////
//	Created		: 30.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "memory_leak_detector.h"

#if XRAY_USE_MEMORY_LEAK_DETECTOR

#include <xray/hash_multiset.h>
#include <xray/intrusive_double_linked_list.h>
#include <xray/logging/log_file.h>
#include <xray/logging/api.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/synchronous_device_interface.h>
#include <xray/core_entry_point.h>
#include <xray/debug/call_stack.h>

static xray::command_line::key	s_memory_leaks_detection_key("memory_leaks_detection", "", "memory", "turns on memory leaks detection");

namespace xray {
namespace memory {
namespace leak_detector {

struct allocation_info
{
	void*				stacktrace[64];
	pcstr				description;
	u32					full_hash;
	u32					alive_allocations;
	allocation_info *	next_hash_node;
};

inline bool operator ==	( allocation_info const& left, allocation_info const& right )
{
	return				memory::compare( left.stacktrace, right.stacktrace ) == 0;
}

struct housekeeping
{
	allocation_info*	info;
	housekeeping*		intrusive_prev;
	housekeeping*		intrusive_next;
};

static bool enabled			( )
{
	static bool const result = s_memory_leaks_detection_key.is_set();
	return					result;
}

class leak_detector
{
public:
		   ~leak_detector	();

	void	on_allocate		(pvoid& pointer, pcstr description);
	void	on_free			(pvoid& pointer);

	void	dump_leaks		();

private:
	allocation_info* find_stack_trace ( allocation_info const& info );
	typedef hash_multiset<	allocation_info, allocation_info *, & allocation_info::next_hash_node, xray::detail::fixed_size_policy<16384>	>	allocation_info_container;

private:

	typedef	intrusive_double_linked_list<	housekeeping, 
											housekeeping *, 
											&housekeeping::intrusive_prev, 
											&housekeeping::intrusive_next, 
											threading::mutex	>	pointers_container;

private:
	threading::mutex			m_mutex;
	allocation_info_container	m_allocation_info;
	pointers_container			m_pointers;
};

static char						s_no_monitoring_desc[]	=	"+";

static uninitialized_reference<leak_detector>	s_leak_detector;

leak_detector::~leak_detector	()
{
	for (	allocation_info_container::iterator it	=	m_allocation_info.begin();
												it	!=	m_allocation_info.end();
												)
	{
		allocation_info_container::iterator next_it	=	it;
		++next_it;
		
		m_allocation_info.erase						(it);

		allocation_info* info					=	*it;
		DEBUG_FREE								(info);

		it										=	next_it;
	}	
}

allocation_info* leak_detector::find_stack_trace ( allocation_info const& info )
{
	allocation_info_container::iterator	iterator	=	m_allocation_info.find( info.full_hash );
	if ( iterator == m_allocation_info.end() )
		return					0;

	allocation_info* found	=	*iterator;
	do {
		if ( *found == info )
			return				found;

		found				=	allocation_info_container::get_next_of_object(found);
	} while ( found );

	return						0;	
}

void   leak_detector::on_allocate (pvoid& pointer, pcstr const description)
{
	housekeeping* const housekeeping	=	(memory::leak_detector::housekeeping*)pointer;
	if ( description && *description == s_no_monitoring_desc[0] ) // no monitoring of this pointer
	{
		housekeeping->info	=	0;
		return;		
	}

	allocation_info info;
	u32				stacktrace_hash;
	xray::debug::call_stack::get_stack_trace (info.stacktrace, array_size(info.stacktrace), 62, &stacktrace_hash);

	u32 const desc_hash		=	description ? fs_new::crc32(description, strings::length(description)) : 0;
	info.full_hash			=	stacktrace_hash ^ desc_hash;
	info.description		=	description;
	info.alive_allocations	=	0;
	
	m_mutex.lock				();

	allocation_info* found	=	find_stack_trace( info );

	if ( !found )
	{
		m_mutex.unlock							();
		allocation_info* new_info			=	(allocation_info*)DEBUG_MALLOC(sizeof(allocation_info),
																			s_no_monitoring_desc);
		m_mutex.lock							();

		found								=	find_stack_trace( info );

		if ( found ) {
			housekeeping->info				=	found;
			++housekeeping->info->alive_allocations;
			m_mutex.unlock						();
			
			DEBUG_FREE							( new_info );
		}
		else {
			new (new_info)						allocation_info;
			*new_info						=	info;
			m_allocation_info.insert			(new_info->full_hash, new_info);
			housekeeping->info				=	new_info;
			++housekeeping->info->alive_allocations;
			m_mutex.unlock						();
		}
	}
	else
	{
		housekeeping->info					=	found;
		++housekeeping->info->alive_allocations;
		m_mutex.unlock							();
	}

	m_pointers.push_back						(housekeeping);
}

static inline size_t get_house_keeping_size_impl	( )
{
	return							sizeof(housekeeping);
}

pvoid get_real_pointer		( pvoid const pointer )
{
	if ( !enabled() )
		return				pointer;

	return					static_cast<pbyte>( pointer ) - get_house_keeping_size_impl();
}

void   leak_detector::on_free (pvoid& pointer)
{
	housekeeping* const housekeeping	=	(memory::leak_detector::housekeeping*)pointer;
	if ( !housekeeping->info ) // no monitoring of this pointer
	{
		return;		
	}
	--housekeeping->info->alive_allocations;
	m_pointers.erase							(housekeeping);
}

static pcstr s_full_call_stack_line_format	= "%-60s(%-3d) : %-70s : %-36s : 0x%08x\n";
static pcstr s_call_stack_line_format		= "%-60s       : %-70s : 0x%08x\n";

struct leak_logger 
{
	fs_new::file_type * m_file;
	leak_logger() : m_file(NULL)
	{
		if ( xray::logging::g_log_file_name.length() )
		{
			using namespace xray::fs_new;
			synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );
			if ( !device->open(& m_file, xray::logging::g_log_file_name.c_str(), file_mode::append_or_create, file_access::write, assert_on_fail_false) )
			{
				;
			}
		}
	}

	~leak_logger()
	{
		xray::fs_new::synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );
		if ( m_file )
			device->close					(m_file);
	}

	bool	predicate	(	u32		call_stack_id,
							u32		num_call_stack_lines,
							pcstr	module_name,
							pcstr	file_name,
							int		line_number,
							pcstr	function,
							size_t	address		)
	{
		XRAY_UNREFERENCED_PARAMETER	( num_call_stack_lines );

		if ( call_stack_id < 3 )
			return					true;

		fixed_string4096	string;
		if ( line_number > 0 )
			string.assignf			(s_full_call_stack_line_format, file_name, line_number, function, module_name, address );
		else
			string.assignf			(s_call_stack_line_format, module_name, function, address);

		output						(string);

		return						true;
	}

	void   output (buffer_string string)
	{
		xray::debug::output			(string.c_str());

		if ( logging::use_console_for_logging() )
			logging::write_to_stdstream(logging::stdstream_out, "%s", string.c_str());

		xray::fs_new::synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );

		if ( m_file )
			device->write			(m_file, string.c_str(), string.length());
	}
}; // struct helper


void leak_detector_dump_leaks	()
{
	s_leak_detector->dump_leaks	( );
}

void   leak_detector::dump_leaks ()
{
	if ( !xray::logging::g_log_file_name.length() && !xray::debug::is_debugger_present() )
		return;

	m_mutex.lock		( );

	u32 leak_index	=	1;

	leak_logger		 helper;
	bool header_printed	=	false;

	fixed_string4096 header;
	header.assignf		("------------------------------------------------------------------------------------------------\n"
						 "-      MEMORY LEAKS DETECTED !!!!!!!!!!!!!!\n"
						 "------------------------------------------------------------------------------------------------\n");

	for ( allocation_info_container::iterator	it	=	m_allocation_info.begin();
												it	!=	m_allocation_info.end();
											  ++it )
	{
		allocation_info*	const info		=	*it;
		if ( !info->alive_allocations )
			continue;

		if ( !header_printed )
		{
			helper.output						(header);
			header_printed					=	true;
		}

		fixed_string4096 leak_header;
		leak_header.assignf						("------------------------------------------------------------------------------------------------\n"
												 "LEAK #%d    description: %s, alive_allocations: %d\n"
												 "------------------------------------------------------------------------------------------------\n",
												 leak_index, info->description, info->alive_allocations);

		helper.output							(leak_header);

		xray::debug::call_stack::iterate		(NULL, 
												 info->stacktrace, 
												 boost::bind(&leak_logger::predicate, &helper, _1, _2, _3, _4, _5, _6, _7));

		++leak_index;
	}

	pcstr leak_message;

	if ( header_printed )
	{
		leak_message						=	"-      OMG! %d LEAKS DETECTED!!!\n";
	}
	else
	{
		leak_message						=	"CONGRATULATIONS! NO LEAKS DETECTED!\n";
	}

	header.assignf								("------------------------------------------------------------------------------------------------\n");
	header.appendf								(leak_message, leak_index-1);
	header.appendf								("------------------------------------------------------------------------------------------------\n");
	helper.output								(header);

	m_mutex.unlock								( );
}

void initialize			( )
{
	XRAY_CONSTRUCT_REFERENCE		(s_leak_detector, leak_detector);
}

void finalize			( )
{
	if ( enabled() )
		s_leak_detector->dump_leaks	( );
	XRAY_DESTROY_REFERENCE			( s_leak_detector );
}

void on_alloc	( pvoid& allocation_address, size_t & allocation_size, size_t const previous_size, pcstr const allocation_description )
{
	XRAY_UNREFERENCED_PARAMETER		( allocation_size );
	XRAY_UNREFERENCED_PARAMETER		( previous_size );

	if ( !enabled() )
		return;

	s_leak_detector->on_allocate	( allocation_address, allocation_description );

	static size_t const house_keeping_size	= get_house_keeping_size_impl( );
	(pbyte&)allocation_address		+= house_keeping_size;
	allocation_size					-= house_keeping_size;
}

void on_free	( pvoid& deallocation_address, bool const can_clear )
{
	XRAY_UNREFERENCED_PARAMETER		( can_clear );

	if ( !enabled() )
		return;

	static size_t const house_keeping_size	= get_house_keeping_size_impl( );
	(pbyte&)deallocation_address	-= house_keeping_size;

	s_leak_detector->on_free		( deallocation_address );
}

size_t get_house_keeping_size	( )
{
	if ( !enabled() )
		return						0;

	return							get_house_keeping_size_impl();
}

} // namespace leak_detector
} // namespace memory
} // namespace xray

#endif // #if XRAY_USE_MEMORY_LEAK_DETECTOR
