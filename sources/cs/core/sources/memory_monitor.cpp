/////////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_monitor.cpp
//	Created 	: 14.06.2008
//  Modified 	: 14.06.2008
//	Author		: Dmitriy Iassenev
//	Description : memory monitor client code
/////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "memory_monitor.h"

#if CS_USE_MEMORY_MONITOR

#include <time.h>
#include <direct.h>

static u32 const				s_buffer_size = 512*1024;
#if CS_PLATFORM_WINDOWS
	static pcstr					s_output_folder = "c:/memory_monitor_stats/";
#elif CS_PLATFORM_XBOX_360 // #if CS_PLATFORM_WINDOWS
	static pcstr					s_output_folder = "cache:/memory_monitor_stats/";
#else // #elif CS_PLATFORM_XBOX_360
#	error define storage for memory monitor stats here
#endif // #if CS_PLATFORM_WINDOWS
static pcstr					s_output_extension = ".bin";
static FILE*					s_file = 0;
static char						s_buffer[s_buffer_size];
static char s_mutex_fake[sizeof(threading::mutex)];
static threading::mutex*			s_mutex = (threading::mutex*)&s_mutex_fake[0];

namespace cs {
namespace core {
namespace memory_monitor {

struct mutex_guard {
	inline	mutex_guard	()
	{
		s_mutex->lock			();
	}

	inline	~mutex_guard	()
	{
		s_mutex->unlock			();
	}
}; // struct mutex_guard

} // namespace memory_monitor
} // namespace core
} // namespace cs

using cs::core::memory_monitor::mutex_guard;

void memory_monitor::initialize			()
{
	_mkdir						(s_output_folder);

	__time64_t					long_time;
	_time64						(&long_time);
	tm							new_time;
	R_ASSERT					(!_localtime64_s(&new_time, &long_time));
	
	string256					file_name;
	strftime					(file_name,sizeof(file_name),"%Y.%m.%d.%H.%M.%S",&new_time);
	
	string256					file;
	sz_cpy						(file, s_output_folder);
	sz_cat						(file, file_name);
	sz_cat						(file, s_output_extension);

	R_ASSERT					(!fopen_s(&s_file,file,"wb"));
	ASSERT						(s_file);
	setvbuf						(s_file,s_buffer,_IOFBF,s_buffer_size);

	new (s_mutex) threading::mutex( );
}

void memory_monitor::finalize			()
{
	s_mutex->~mutex				( );

	if (!s_file)
		return;

	fclose						(s_file);
}

#pragma warning(push)
#pragma warning(disable:4201)
union record_type {
	struct {
		u32	allocation			: 1;
		u32 size				: 31;
	};
	u32		allocation_size;
};
#pragma warning(pop)

void memory_monitor::add_allocation		( pcvoid pointer, size_t size, pcstr description )
{
	if (!s_file)
		return;

	mutex_guard					guard;
	record_type					temp;
	temp.allocation				= 1;
	temp.size					= u32(size);
	fwrite						(&temp,sizeof(temp),1,s_file);
	fwrite						(&pointer,sizeof(pointer),1,s_file);
	fwrite						(description,(sz_len(description) + 1)*sizeof(char),1,s_file);
}

void memory_monitor::add_deallocation	( pcvoid pointer )
{
	if (!s_file)
		return;

	if (!pointer)
		return;

	mutex_guard					guard;
	record_type					temp;
	temp.allocation				= 0;
	temp.size					= 0;
	fwrite						(&temp,sizeof(temp),1,s_file);
	fwrite						(&pointer,sizeof(pointer),1,s_file);
}

void memory_monitor::add_snapshot		( pcstr description )
{
	if (!s_file)
		return;

	mutex_guard					guard;
	ASSERT						(description);
	record_type					temp;
	temp.allocation				= 0;
	temp.size					= 1;
	fwrite						(&temp,sizeof(temp),1,s_file);
	fwrite						(description,(sz_len(description) + 1)*sizeof(char),1,s_file);
}

#endif // #if CS_USE_MEMORY_MONITOR