////////////////////////////////////////////////////////////////////////////
//	Created 	: 25.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "memory.h"
#include "memory_platform.h"
#include <xray/platform_extensions_win.h>

#include <xray/os_preinclude.h>
#define _WIN32_WINNT 0x0501
#undef	NOUSER
#undef	NOMSG
#undef	NOGDI
#undef  NONLS
#undef  NOMB
#include <xray/os_include.h>
#include <psapi.h>
#pragma comment( lib, "psapi.lib" )
#include <d3d9.h>

using xray::memory::platform::regions_type;
using xray::memory::platform::region;
using xray::Kb;
using xray::Mb;
using xray::Gb;
using xray::Tb;

static pvoid s_single_block_arena		= 0;
static u64 s_single_block_arena_size	= 0;

static xray::command_line::key	s_max_resources_size("max_resources_size", "", "resources", "setup maximum size for resource arenas, Mb");
static xray::command_line::key	s_fill_arenas_with_garbage("fill_arenas_with_garbage", "", "memory", "fills all the memory in all the arenas with garbage; could slowdown startup significantly!");
static xray::command_line::key	s_no_warning_on_page_file_size("no_warning_on_page_file_size", "", "memory", "suppress warning about too many programs open or not enough page file size");

pvoid allocate_region							( u64 const size, pvoid const address, u32 const additional_flags, bool assert_on_failure = true )
{
	pvoid const result		= 
		VirtualAlloc	(
			address,
#if XRAY_PLATFORM_64_BIT
			size,
#else // #if XRAY_PLATFORM_64_BIT
			(SIZE_T)size,
#endif // #if XRAY_PLATFORM_64_BIT
			MEM_RESERVE | MEM_COMMIT | additional_flags,
			PAGE_READWRITE // PAGE_EXECUTE_READWRITE
		);
	if ( assert_on_failure )
		R_ASSERT_CMP_U		( result, ==, address );

#ifndef MASTER_GOLD
	if ( result && s_fill_arenas_with_garbage.is_set() )
		xray::memory::fill32( result, (size_t)size, xray::memory::uninitialized_value<u32>(), (size_t)size/sizeof(u32) );
#endif // #ifndef MASTER_GOLD

	return					result;
}

static void free_region_impl					( pvoid const buffer )
{
	BOOL result					= 
		VirtualFree				(
			buffer,
			0,
			MEM_RELEASE
		);
	R_ASSERT_U					(result);
}

void xray::memory::platform::free_region		( pvoid buffer, u64 const buffer_size )
{
	if ( s_single_block_arena_size ) {
		R_ASSERT_CMP			( s_single_block_arena_size, >=, buffer_size );
		s_single_block_arena_size	-= buffer_size;
		if ( s_single_block_arena_size )
			return;

		buffer					= s_single_block_arena;
	}

	free_region_impl			( buffer );
}

static u32 allocation_granularity				( )
{
	SYSTEM_INFO					system_info;
	GetSystemInfo				( &system_info );
	return						system_info.dwAllocationGranularity;
}

bool try_to_allocate_arenas_as_a_single_block	(
		regions_type& arenas,
		regions_type& resource_arenas,
		u64 start_address,
		u32 const additional_flags,
		bool assert_on_failure
	)
{
	u64 total_size				= 0;
	{
		regions_type::const_iterator i		= arenas.begin( );
		regions_type::const_iterator e		= arenas.end( );
		for ( ; i != e; ++i )
			total_size			+= (*i).size;

		for (i = resource_arenas.begin(), e = resource_arenas.end(); i != e; ++i )
			total_size			+= (*i).size;
	}

	u64 const pure_total_size	= total_size;
	total_size					= xray::math::align_up( total_size, (u64)allocation_granularity( ) );
#if XRAY_PLATFORM_WINDOWS_32
	total_size					= xray::math::min( total_size, (u64)4*Gb - (u64)allocation_granularity( ) );
#endif // #if XRAY_PLATFORM_WINDOWS_32
	pbyte arena					= (pbyte)::allocate_region( total_size, *(pvoid*)&start_address, additional_flags, false );
	R_ASSERT_U					( !assert_on_failure || arena );
	if ( !arena )
		return					false;

#ifndef MASTER_GOLD
	if ( s_fill_arenas_with_garbage.is_set() )
		xray::memory::fill32	( arena, (size_t)pure_total_size, xray::memory::uninitialized_value<u32>(), (size_t)pure_total_size/sizeof(u32) );
#endif // #ifndef MASTER_GOLD

	s_single_block_arena		= arena;
	s_single_block_arena_size	= pure_total_size;
	{
		regions_type::iterator i = arenas.begin( );
		regions_type::iterator e = arenas.end( );
		for ( ; i != e; ++i ) {
			if ( !(*i).size )
				continue;

			(*i).address		= arena;
			arena				+= (*i).size;
		}

		for (i = resource_arenas.begin(), e = resource_arenas.end(); i != e; ++i ) {
			(*i).address		= arena;
			arena				+= (*i).size;
		}
	}

	return						true;
}

template < typename P >
void iterate_regions		( u32 const start_address, u32 const allocation_granularity, u64 const min_buffer_size, P& predicate )
{
	MEMORY_BASIC_INFORMATION	memory_info;
	u64 i						= start_address;
#if XRAY_PLATFORM_32_BIT
	u64 const max_address		= u64(1) << 32;
#elif XRAY_PLATFORM_64_BIT // #if XRAY_PLATFORM_WINDOWS_32
	u64 const max_address		= u64(1) << 40;
#else // #if XRAY_PLATFORM_64_BIT
	u64 const max_address		= u64(1) << 40;
#endif // #if XRAY_PLATFORM_WINDOWS_32

	for ( ; i < max_address; ) {
		size_t const			return_value =
			VirtualQuery(
				( pcvoid )i,
				&memory_info,
				sizeof( memory_info )
			);

		if ( !return_value ) {
			i					+= allocation_granularity;
			continue;
		}

		if ( memory_info.RegionSize < min_buffer_size ) {
			i					+= xray::math::align_up( memory_info.RegionSize, (SIZE_T)allocation_granularity );
			continue;
		}

		i						+= xray::math::align_up( memory_info.RegionSize, (SIZE_T)allocation_granularity );

		if (memory_info.State != MEM_FREE)
			continue;

		predicate				( memory_info.RegionSize, memory_info.BaseAddress );
	}
}


static u64 select_best_region	(
		u64 largest_but_two,
		u64 largest_but_one,
		u64 largest,
		xray::memory::platform::regions_type& resource_arenas,
		u64 const allocation_granularity
	)
{
	xray::memory::platform::region& minimum	= resource_arenas.front();
	xray::memory::platform::region& maximum	= resource_arenas.back();
	R_ASSERT					( minimum.size <= maximum.size );

	if ( largest < largest_but_one )
		std::swap				( largest, largest_but_one );

	if ( largest_but_one < largest_but_two )
		std::swap				( largest_but_one, largest_but_two );

	if ( largest >= (maximum.size + minimum.size) )
		return					maximum.size + minimum.size;

	if ( (largest >= maximum.size) && (largest_but_one >= minimum.size) )
		return					maximum.size + minimum.size;

	if ( largest*((float)minimum.size/float(minimum.size + maximum.size)) >= largest_but_one )
		return					largest;

	float const share			= (float)minimum.size / maximum.size;
	u64 const test_size			= xray::math::align_up( (u64)(largest_but_one*share), allocation_granularity );
	if ( test_size > largest )
		return					largest + xray::math::align_up( (u64)(largest*share), allocation_granularity );

	return						largest_but_one + test_size;
}

static xray::memory::platform::regions_type::iterator& select_best_region(
		u64 const largest_but_two_size,
		xray::memory::platform::regions_type::iterator& largest_but_one,
		xray::memory::platform::regions_type::iterator& largest,
		u64 const size,
		xray::memory::platform::regions_type& resource_arenas,
		u32 const allocation_granularity
	)
{
	u64 const result0			= select_best_region( largest_but_two_size, (*largest_but_one).size - size, (*largest).size, resource_arenas, allocation_granularity );
	u64 const result1			= select_best_region( largest_but_two_size, (*largest_but_one).size, (*largest).size - size, resource_arenas, allocation_granularity );
	return						result0 >= result1 ? largest_but_one : largest;
}

struct regions_count {
	inline	regions_count	( ) :
		m_region_count	( 0 )
	{
	}

	inline	void	operator( )	( u64 const size, pvoid const address )
	{
		XRAY_UNREFERENCED_PARAMETERS	( size, address );
		++m_region_count;
	}

	u32 m_region_count;
};

struct regions_filler : private boost::noncopyable {
	inline	regions_filler	( xray::memory::platform::regions_type& regions ) :
		m_regions	( regions )
	{
	}

	inline	void	operator( )	( u64 const size, pvoid const address )
	{
		xray::memory::platform::region const value = { size, address, 0 };
		m_regions.push_back	( value );
	}

	xray::memory::platform::regions_type& m_regions;
};

static bool allocate_arenas					(
		xray::memory::platform::regions_type& arenas,
		xray::memory::platform::regions_type& resource_arenas,
		u32 const start_address,
		bool only_resources
	)
{
	R_ASSERT					( !arenas.empty() );

	std::sort					( arenas.begin(), arenas.end() );
	std::reverse				( arenas.begin(), arenas.end() );
	while ( !arenas.empty() && !arenas.back().size )
		arenas.pop_back			( );
	std::reverse				( arenas.begin(), arenas.end() );

	R_ASSERT					( !arenas.empty() );
	
	regions_count				counter;
	u64 const min_buffer_size	= arenas.front().size;
	u32 const allocation_granularity = ::allocation_granularity( );
	iterate_regions				( start_address, allocation_granularity, min_buffer_size, counter );

	using xray::memory::platform::regions_type;
	using xray::memory::platform::region;

	regions_type				regions( ALLOCA(counter.m_region_count*sizeof(region)), counter.m_region_count );
	regions_filler				filler(regions);
	iterate_regions				( start_address, allocation_granularity, min_buffer_size, filler );

	std::sort					( regions.begin(), regions.end() );

	// sort resource arenas
	if ( resource_arenas.front().size > resource_arenas.back().size )
		std::swap				( resource_arenas.front(), resource_arenas.back() );

	regions_type::reverse_iterator i		= arenas.rbegin();
	regions_type::reverse_iterator const e	= arenas.rend();
	if ( !only_resources ) {
		for ( ; i != e; ++i ) {
			(*i).size			= ( ((*i).size - 1)/allocation_granularity + 1 )*allocation_granularity;

			regions_type::iterator const regions_b	= regions.begin();
			regions_type::iterator const regions_e	= regions.end();
			regions_type::iterator j		= std::lower_bound( regions_b, regions_e, *i );
			R_ASSERT			( j != regions_e );
			R_ASSERT_CMP		( (*j).size, >=, (*i).size );

			regions_type::iterator k	= j + 1;
			if ( (k != regions_e) && ( (k + 1) == regions_e) )
				j				= select_best_region( j == regions_b ? 0 : (*(j-1)).size, j, k, (*i).size, resource_arenas, allocation_granularity );

			(*i).address		= allocate_region( (*i).size, (*j).address, 0 );
			(pbyte&)((*j).address )	+= (*i).size;
			(*j).size			-= (*i).size;

			if ( j != regions.begin() ) {
				if ( (*j).size >= (*(j-1)).size )
					continue;
			}
			else {
				if ( (*j).size >= min_buffer_size )
					continue;

				regions.erase	( j );
				continue;
			}

			region temp			= *j;

			regions.erase		( j );
			if ( temp.size >= min_buffer_size )
				regions.insert(
					std::lower_bound( regions.begin(), regions.end(), temp ),
					temp
				);
		}
	}

	R_ASSERT					( !regions.empty() );
	i							= regions.rbegin();

	R_ASSERT_CMP				( resource_arenas.size(), ==, 2 );
	region& minimum				= resource_arenas.front();
	region& maximum				= resource_arenas.back();
	R_ASSERT					( minimum.size <= maximum.size );
	u64 const resource_arenas_size	= minimum.size + maximum.size;

	if ( (*i).size >= resource_arenas_size ) {
		minimum.address			= allocate_region( minimum.size, (*i).address, 0, false );
		if ( !minimum.address )
			return				false;

		maximum.address			= allocate_region( maximum.size, (pbyte)(*i).address + minimum.size, 0, false );
		if ( !maximum.address ) {
			xray::memory::platform::free_region	( minimum.address, minimum.size );
			minimum.address		= 0;
			return				false;
		}

		return					true;
	}

	regions_type::reverse_iterator j = i + 1;
	float const share			= (float)minimum.size / (float)(maximum.size + minimum.size);
	if ( (regions.size() == 1) || ((*i).size*share >= (*j).size) ) {
		minimum.size			= xray::math::align_down( (u64)((*i).size*share), (u64)allocation_granularity );
		maximum.size			= (*i).size - minimum.size;
		minimum.address			= allocate_region( minimum.size, (*i).address, 0, false );
		if ( !minimum.address )
			return				false;

		maximum.address			= allocate_region( maximum.size, (pbyte)(*i).address + minimum.size, 0, false );
		if ( !maximum.address ) {
			xray::memory::platform::free_region	( minimum.address, minimum.size );
			minimum.address		= 0;
			return				false;
		}

		return					true;
	}

	if ( ((*i).size >= maximum.size) && ((*j).size >= minimum.size) ) {
		minimum.address			= allocate_region( minimum.size, (*j).address, 0, false );
		if ( !minimum.address )
			return				false;

		maximum.address			= allocate_region( maximum.size, (*i).address, 0, false );
		if ( !maximum.address ) {
			xray::memory::platform::free_region	( minimum.address, minimum.size );
			minimum.address		= 0;
			return				false;
		}

		return					true;
	}

	if ( ((*i).size + (*j).size) >= resource_arenas_size ) {
		if ( (*i).size > maximum.size ) {
			R_ASSERT_CMP		( minimum.size, >, (*j).size );
			minimum.size		= (*j).size;
			minimum.address		= allocate_region( minimum.size, (*j).address, 0, false );
			if ( !minimum.address )
				return			false;

			maximum.size		= xray::math::align_down( resource_arenas_size - (*j).size, (u64)allocation_granularity );
			maximum.address		= allocate_region( maximum.size, (*i).address, 0, false );
			if ( !maximum.address ) {
				xray::memory::platform::free_region	( minimum.address, minimum.size );
				minimum.address	= 0;
				return			false;
			}

			return				true;
		}
		
		R_ASSERT_CMP			( (*i).size, <, maximum.size );
		R_ASSERT_CMP			( (*j).size, >, minimum.size );

		minimum.size			= xray::math::align_down( resource_arenas_size - (*i).size, (u64)allocation_granularity );
		minimum.address			= allocate_region( minimum.size, (*j).address, 0, false );
		if ( !minimum.address )
			return				false;

		maximum.size			= (*i).size;
		maximum.address			= allocate_region( maximum.size, (*i).address, 0, false );
		if ( !maximum.address ) {
			xray::memory::platform::free_region	( minimum.address, minimum.size );
			minimum.address		= 0;
			return				false;
		}

		return					true;
	}

	minimum.size				= (*j).size;
	minimum.address				= allocate_region( minimum.size, (*j).address, 0 );
	if ( !minimum.address )
		return					false;

	maximum.size				= (*i).size;
	maximum.address				= allocate_region( maximum.size, (*i).address, 0 );
	if ( !maximum.address ) {
		xray::memory::platform::free_region	( minimum.address, minimum.size );
		minimum.address			= 0;
		return					false;
	}

	return						true;
}

static bool try_to_allocate_arenas	(
		xray::memory::platform::regions_type& arenas,
		xray::memory::platform::regions_type& resource_arenas,
		bool only_resources
	)
{
	if ( !only_resources && try_to_allocate_arenas_as_a_single_block(arenas, resource_arenas, 0, 0, false ) )
		return					true;

	return						allocate_arenas( arenas, resource_arenas, allocation_granularity(), only_resources );
}

static bool try_to_allocate_arenas	(
		regions_type& arenas,
		region& managed_arena,
		region& unmanaged_arena,
		bool only_resources
	)
{
	regions_type				resource_regions( ALLOCA(2*sizeof(region)), 2 );
	resource_regions.push_back	( managed_arena );
	resource_regions.push_back	( unmanaged_arena );
	if ( !try_to_allocate_arenas( arenas, resource_regions, only_resources ) )
		return					false;

	if ( resource_regions.front().data == &managed_arena ) {
		managed_arena			= resource_regions.front();
		unmanaged_arena			= resource_regions.back();
	}
	else {
		R_ASSERT_CMP			( resource_regions.front().data, ==, &unmanaged_arena );
		managed_arena			= resource_regions.back();
		unmanaged_arena			= resource_regions.front();
	}

	return						true;
}

static u64 get_minimum_kernel_memory	( )
{
	OSVERSIONINFOEX				os_version_info;
	xray::memory::zero			( &os_version_info, sizeof(os_version_info) );
	os_version_info.dwOSVersionInfoSize = sizeof(os_version_info);

	GetVersionEx				( (OSVERSIONINFO*)&os_version_info );
	switch ( os_version_info.dwMajorVersion ) {
		// Windows Server 2003 R2, Windows Server 2003, Windows XP, or Windows 2000
		case 5 :				return 128*Mb;
		case 6 : {
			switch ( os_version_info.dwMinorVersion ) {
				// Windows Vista or Windows Server 2008
				case 0 :		return 384*Mb;
				// Windows 7 or Windows Server 2008 R2
				case 1 :		return 512*Mb;
				// unknown
				default :		return 512*Mb;
			}
		}
		// we are not going to support any earlier versions (even Windows XP is under big question),
		// therefore this value would be for any future versions
		default :				return 512*Mb;
	}
}

struct memory_stats {
	u64		total_available_memory;
	u64		physical_memory;
	u64		available_address_space;
	u64		current_kernel_memory;
	u64		video_memory;
	u64		engine_fixed_memory;
	bool	does_os_use_process_address_space_for_duplicating_video_resources;
}; // struct memory_info

static u64 calculate_desirable_resource_arenas	(
		memory_stats& stats,
		float const maximum_video_share,
		u64 const minimum_video_memory_size,
		u64 const minimum_resources_size
	)
{
	R_ASSERT_CMP				( maximum_video_share, >, 0.f );
	R_ASSERT_CMP				( maximum_video_share, <, 1.f );

	u64 useful_memory			= xray::math::min( stats.total_available_memory, stats.physical_memory);

	CHECK_OR_EXIT				(
		useful_memory >= stats.current_kernel_memory,
		"Not enough memory to run X-Ray Engine v2.0\r\nClose all programs, increase paging file size or add memory bank and try again."
	);
	useful_memory				-= stats.current_kernel_memory;

	CHECK_OR_EXIT				(
		useful_memory >= stats.engine_fixed_memory,
		"Not enough memory to run X-Ray Engine v2.0\r\nClose all programs, increase paging file size or add memory bank and try again."
	);
	useful_memory				-= stats.engine_fixed_memory;

	CHECK_OR_EXIT				(
		useful_memory >= (minimum_video_memory_size + minimum_resources_size),
		"Not enough memory to run X-Ray Engine v2.0\r\nClose all programs, increase paging file size or add memory bank and try again."
	);

	u64 useful_video			= stats.video_memory;
	u64 address_space			= stats.available_address_space;
	CHECK_OR_EXIT				(
		address_space >= stats.engine_fixed_memory,
		"Not enough memory to run X-Ray Engine v2.0\r\nClose all programs, increase paging file size or add memory bank and try again."
	);

	address_space				-= stats.engine_fixed_memory;

	if ( stats.does_os_use_process_address_space_for_duplicating_video_resources ) {
		if ( useful_video/address_space > maximum_video_share ) {
			CHECK_OR_EXIT		(
				address_space*maximum_video_share >= minimum_video_memory_size,
				"Not enough memory to run X-Ray Engine v2.0\r\nClose all programs, increase paging file size or add memory bank and try again."
			);
			useful_video		= u64(address_space*maximum_video_share);
		}
		
		if ( (address_space - useful_video) < minimum_resources_size ) {
			CHECK_OR_EXIT		(
				(address_space - minimum_resources_size) >= minimum_video_memory_size,
				"Not enough memory to run X-Ray Engine v2.0\r\nClose all programs, increase paging file size or add memory bank and try again."
			);
			useful_video		= address_space - minimum_resources_size;
		}
	}

	if ( useful_video/useful_memory > maximum_video_share ) {
		CHECK_OR_EXIT			(
			useful_memory*maximum_video_share >= minimum_video_memory_size,
			"Not enough memory to run X-Ray Engine v2.0\r\nClose all programs, increase paging file size or add memory bank and try again."
		);
		useful_video			= u64(useful_memory*maximum_video_share);
	}

	if ( (useful_memory - useful_video) < minimum_resources_size ) {
		CHECK_OR_EXIT			(
			(useful_memory - minimum_resources_size) >= minimum_video_memory_size,
			"Not enough memory to run X-Ray Engine v2.0\r\nClose all programs, increase paging file size or add memory bank and try again."
		);
		useful_video			= useful_memory - minimum_resources_size;
	}

	if ( stats.does_os_use_process_address_space_for_duplicating_video_resources ) {
		R_ASSERT_CMP			( address_space, >=, useful_video );
		address_space			-= useful_video;
	}

	R_ASSERT_CMP				( useful_memory, >=, useful_video );
	useful_memory				-= useful_video;

	R_ASSERT_CMP				( useful_memory, >=, minimum_resources_size );
	useful_memory				= xray::math::min( useful_memory, address_space );

	R_ASSERT_CMP				( useful_memory, >=, minimum_resources_size );

	u64 maximum_resources_size	= u64(-1);
	if ( s_max_resources_size.is_set_as_number( &maximum_resources_size ) ) {
		maximum_resources_size	*= Mb;
		maximum_resources_size	= xray::math::max( maximum_resources_size, minimum_resources_size );
	}
	useful_memory				= xray::math::min( useful_memory, maximum_resources_size );
	return						useful_memory;
}

static void reduce_resources_arenas				(
		region& managed_arena,
		region& unmanaged_arena,
		u64 size_to_reduce,
		u64 const minimum_resources_size,
		u64 const granularity,
		float const managed_to_unmanaged_share
	)
{
	size_to_reduce				= xray::math::align_up( size_to_reduce, granularity );
	u64 const cumulative_size	= managed_arena.size + unmanaged_arena.size;
	R_ASSERT_CMP				( cumulative_size, >=, size_to_reduce );
	R_ASSERT_CMP_U				( cumulative_size, >=, size_to_reduce + minimum_resources_size );
	u64 const new_size			= cumulative_size - size_to_reduce;
	u64 const new_managed_size	= xray::math::align_up( u64( managed_to_unmanaged_share/(managed_to_unmanaged_share+1.f)*new_size ), granularity );
	u64 new_unmanaged_size = new_size - new_managed_size;
	if ( s_single_block_arena ) {
		managed_arena.size		= new_managed_size;
		unmanaged_arena.size	= new_unmanaged_size;
		if ( managed_arena.address < unmanaged_arena.address )
			unmanaged_arena.address	= (pbyte)managed_arena.address + managed_arena.size;
		else
			managed_arena.address	= (pbyte)unmanaged_arena.address + unmanaged_arena.size;

		free_region_impl		( s_single_block_arena );

		R_ASSERT_CMP			( s_single_block_arena_size, >, size_to_reduce );
		s_single_block_arena_size	-= size_to_reduce;
		s_single_block_arena	= allocate_region( s_single_block_arena_size, s_single_block_arena, 0 );
		return;
	}

	u64 previous_size		= managed_arena.size;
	if ( new_managed_size <= previous_size ) {
		managed_arena.size		= new_managed_size;
		xray::memory::platform::free_region	( managed_arena.address, previous_size );
		managed_arena.address	= allocate_region( new_managed_size, managed_arena.address, 0 );
	}
	else
		new_unmanaged_size	= new_size - managed_arena.size;

	previous_size			= unmanaged_arena.size;
	if ( new_unmanaged_size <= previous_size ) {
		unmanaged_arena.size	= new_unmanaged_size;
		xray::memory::platform::free_region	( unmanaged_arena.address, previous_size );
		unmanaged_arena.address	= allocate_region( new_unmanaged_size, unmanaged_arena.address, 0 );
	}
}

class CoInitialize_guard : private boost::noncopyable {
public:
	CoInitialize_guard	( ) :
		m_result			( CoInitializeEx(0, COINIT_APARTMENTTHREADED) )
	{
	}

	~CoInitialize_guard()
	{
		if ( SUCCEEDED(m_result) )
			CoUninitialize	( );
	}

private:
	HRESULT const m_result;
}; // class CoInitialize_guard

void xray::memory::platform::allocate_arenas	(
		u64 const reserved_memory_size,
		u64 reserved_address_space,
		regions_type& arenas,
		region& managed_arena,
		region& unmanaged_arena
	)
{
	CoInitialize_guard				guard;

	PERFORMANCE_INFORMATION			perfomance_information;
	GetPerformanceInfo				( &perfomance_information, sizeof(perfomance_information) );
	xray::platform::unload_delay_loaded_library	( "psapi.dll" );

	MEMORYSTATUSEX					memory_status;
	ZeroMemory						( &memory_status, sizeof(memory_status) );
	memory_status.dwLength			= sizeof( memory_status );
	GlobalMemoryStatusEx			( &memory_status );

	memory_stats					stats;
	CHECK_OR_EXIT					( reserved_memory_size < memory_status.ullTotalPhys, "Too much memory reserved, not enough memory for engine to proceed." );
	stats.physical_memory			= memory_status.ullTotalPhys - math::align_up( reserved_memory_size, (u64)perfomance_information.PageSize );
	stats.current_kernel_memory		= math::max( perfomance_information.KernelTotal*perfomance_information.PageSize, get_minimum_kernel_memory() );

#if XRAY_PLATFORM_32_BIT
	// due to the lack of address space
	if ( !reserved_address_space )
		reserved_address_space		+= 128*Mb;
#endif // #if XRAY_PLATFORM_32_BIT

	CHECK_OR_EXIT					( reserved_address_space < memory_status.ullAvailVirtual, "Too much virtual address space reserved, not enough address space for engine to proceed." );
	stats.available_address_space	= memory_status.ullAvailVirtual - reserved_address_space;

	// calculate fixed memory size
	u64 engine_fixed_memory			= 0;
	regions_type::const_iterator i	= arenas.begin( );
	regions_type::const_iterator const e = arenas.end( );
	for ( ; i != e; ++i )
		engine_fixed_memory			+= (*i).size;

	stats.engine_fixed_memory		= engine_fixed_memory;
	
	// all the OS we know do duplicate video resources in the process virtual address space
	stats.does_os_use_process_address_space_for_duplicating_video_resources	= true;

	// here we hope that page file will be increased with time
	// therefore we do not limit our computations and setup current maximum
	// supported page file size
	stats.total_available_memory	= 8*Tb;

	// we first try with an assumption of minimum local(onboard) video memory
	// because we would like to get as many as possible contiguous memory
	// since during local(onboard) memory detection there will be lots of libraries
	// loads into our process address space, which can fragment it
	u64 const minimal_local_video_memory_size = xray::platform::get_minimal_local_video_memory_size( );
	stats.video_memory				= minimal_local_video_memory_size;

	float const video_memory_share	= .9f;
	u64 const minimum_resources_memory_size	= 128*Mb;
	u64 const granularity		= allocation_granularity();
	u64 desirable_resources_size	=
		math::align_down(
			calculate_desirable_resource_arenas(
				stats,
				video_memory_share,
				minimal_local_video_memory_size,
				minimum_resources_memory_size
			),
			granularity
		);

	float const managed_to_unmanaged_share	= 4.f/1.f;

	managed_arena.size			= math::align_up( u64( (managed_to_unmanaged_share/(managed_to_unmanaged_share+1.f))*desirable_resources_size ), granularity );
	unmanaged_arena.size		= desirable_resources_size - managed_arena.size;

	if ( !::try_to_allocate_arenas(arenas, managed_arena, unmanaged_arena, false) ) {
		// we've just allocated fixed arenas successfully but resources arenas
		// allocation failed. let's assume that page file cannot grow and use
		// its fixed value
		GlobalMemoryStatusEx		( &memory_status );
		stats.total_available_memory = memory_status.ullAvailPageFile;
		desirable_resources_size	=
			math::align_down(
				calculate_desirable_resource_arenas(
					stats,
					video_memory_share,
					minimal_local_video_memory_size,
					minimum_resources_memory_size
				),
				granularity
			);

		managed_arena.size			= math::align_up( u64( (managed_to_unmanaged_share/(managed_to_unmanaged_share+1.f))*desirable_resources_size ), granularity );
		unmanaged_arena.size		= desirable_resources_size - managed_arena.size;

		CHECK_OR_EXIT				(
			try_to_allocate_arenas(arenas, managed_arena, unmanaged_arena, true),
			"Not enough memory to run X-Ray Engine v2.0\r\nClose all programs, increase paging file size or add memory bank and try again."
		);

		if ( !s_no_warning_on_page_file_size.is_set() )
			MessageBox(
				0,
				"You may increase engine performance by closing all programs or increasing page file size and then restarting engine.",
				XRAY_ENGINE_ID,
				MB_ICONWARNING | MB_OK
			);
	}

	// now when maximum memory is allocated we can start detecting real
	// video memory size
	for (;;) {
		stats.video_memory		= xray::platform::get_local_video_memory_size( );
		if ( stats.video_memory )
			break;

		int const result		=
			MessageBox(
				0,
				"Cannot detect local video memory size",
				XRAY_ENGINE_ID,
				MB_ICONWARNING | MB_ABORTRETRYIGNORE
			);

		switch( result ) {
			case IDABORT :		::exit( -1 );
			case IDRETRY :		continue;
			case IDIGNORE :		break;
			default :			NODEFAULT();
		}

		stats.video_memory		= minimal_local_video_memory_size;
		string4096				temp;
		xray::sprintf			( temp, "Assuming minimum local video memory size: %I64d", stats.video_memory );
		MessageBox				( 0, temp, XRAY_ENGINE_ID, MB_ICONINFORMATION );
		break;
	}
	R_ASSERT					( stats.video_memory );
	CHECK_OR_EXIT				(
		stats.video_memory >= minimal_local_video_memory_size,
		"Not enough video memory to run X-Ray Engine v2.0\r\nUpgrade your video card and try again."
	);

	u64 const old_desirable_resources_size	= desirable_resources_size;
	desirable_resources_size	=
		math::align_down(
			calculate_desirable_resource_arenas(
				stats,
				video_memory_share,
				minimal_local_video_memory_size,
				minimum_resources_memory_size
			),
			granularity
		);

	if ( desirable_resources_size == old_desirable_resources_size )
		return;

	R_ASSERT_CMP				( desirable_resources_size, <, old_desirable_resources_size );
	if ( managed_arena.size + unmanaged_arena.size <= desirable_resources_size )
		return;

	// and now reduce resources arenas with regards to new
	// desirable resources size calculated
	reduce_resources_arenas		(
		managed_arena,
		unmanaged_arena,
		managed_arena.size + unmanaged_arena.size - desirable_resources_size,
		minimum_resources_memory_size,
		granularity,
		managed_to_unmanaged_share
	);
}

#ifndef _DLL
static void set_low_fragmentation_heap	( )
{
	if ( xray::debug::is_debugger_present( ) )
		return;

	HMODULE const kernel32		= LoadLibrary( "kernel32.dll" );
	R_ASSERT					( kernel32 );

	typedef BOOL (__stdcall *HeapSetInformation_type) ( HANDLE, HEAP_INFORMATION_CLASS, PVOID, SIZE_T );
	HeapSetInformation_type const heap_set_information = 
		(HeapSetInformation_type)GetProcAddress(kernel32, "HeapSetInformation");

	if ( !heap_set_information )
		return;

	ULONG HeapFragValue			= 2;
	{
		BOOL const result		= 
			heap_set_information(
				GetProcessHeap(),
				HeapCompatibilityInformation,
				&HeapFragValue,
				sizeof(HeapFragValue)
			);

		ASSERT_U				(result, "can't set process heap low fragmentation");
	}
	{
		BOOL const result		= 
			heap_set_information(
				(pvoid)_get_heap_handle(),
				HeapCompatibilityInformation,
				&HeapFragValue,
				sizeof(HeapFragValue)
			);

		ASSERT_U				(result, "can't set process heap low fragmentation");
	}
}
#endif // #ifndef _DLL

void xray::memory::on_after_memory_initialized	( )
{
#ifndef _DLL
	set_low_fragmentation_heap	( );
#endif // #ifndef _DLL
}

#if XRAY_DEBUG_ALLOCATOR
void xray::memory::register_debug_allocator	( u64 const maximum_memory_size )
{
	u64 const max_memory_size		= 192*Mb;
	u64 memory_size					= math::max( maximum_memory_size ? maximum_memory_size : max_memory_size, 1*Mb );
	memory_size						= math::min( memory_size, max_memory_size );
	xray::debug::g_mt_allocator.do_register( memory_size, "debug multithreaded allocator" );
}
#endif // #if XRAY_DEBUG_ALLOCATOR