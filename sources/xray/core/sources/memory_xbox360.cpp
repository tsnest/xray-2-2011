////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "memory.h"
#include "memory_platform.h"
#include <xray/os_include.h>

using xray::Kb;
using xray::Mb;

static u32 s_size_16mb					= 0;
static u32 s_allocated_size_16mb		= 0;
static pvoid s_address_16mb				= 0;

static u32 s_size_64kb					= 0;
static u32 s_allocated_size_64kb		= 0;
static pvoid s_address_64kb				= 0;

static u32 s_size_4kb					= 0;
static u32 s_allocated_size_4kb			= 0;
static pvoid s_address_4kb				= 0;

static u64 const resources_arena_granularity	= 64*Kb;
static u32 const memory_must_be_left			= 9*Mb + 256*Kb + 128*Kb + 64*Kb + 8*Kb + 4*Kb
												+ 4*Mb; // for shader compilation

static inline bool inside					( pvoid const buffer, pvoid const arena, u64 const arena_size )
{
	if ( buffer < arena )
		return						false;

	if ( buffer >= ((pbyte)arena + arena_size) )
		return						false;

	return							true;
}

static bool check							(
		pvoid const buffer,
		u64 const buffer_size,
		pvoid const arena,
		u64 const arena_size,
		u32& allocated_arena_size
	)
{
	if ( !allocated_arena_size ) {
		R_ASSERT					( !inside(buffer, arena, arena_size) );
		return						false;
	}

	if ( !inside(buffer, arena, arena_size) )
		return						false;

	R_ASSERT_CMP					( allocated_arena_size, >=, buffer_size );
	allocated_arena_size			-= (u32)buffer_size;

	if ( !allocated_arena_size )
		XPhysicalFree				( buffer );

	return							true;
}

void xray::memory::platform::free_region	( pvoid const buffer, u64 const buffer_size )
{
	if ( check(buffer, buffer_size, s_address_16mb, s_size_16mb, s_allocated_size_16mb) )
		return;

	if ( check(buffer, buffer_size, s_address_64kb, s_size_64kb, s_allocated_size_64kb) )
		return;

	if ( !check( buffer, buffer_size, s_address_4kb, s_size_4kb, s_allocated_size_4kb ) )
		UNREACHABLE_CODE			( );
}

static void allocate_memory	(
		u32 min_value,
		u32 max_value,
		u32 const page_size,
		u32 const flags,
		u32& result_size,
		pvoid& result_address
	)
{
	result_address					= XPhysicalAlloc( max_value*page_size, MAXULONG_PTR, 0, PAGE_READWRITE | flags );
	if ( result_address ) {
		result_size					= max_value*page_size;
		return;
	}

	if ( min_value ) {
		result_address				= XPhysicalAlloc( min_value*page_size, MAXULONG_PTR, 0, PAGE_READWRITE | flags );
		R_ASSERT					( result_address );
		XPhysicalFree				( result_address );
	}

	for ( ; (min_value + 1) < max_value; ) {
		u32 const average			= (min_value + max_value)/2;
		pvoid const buffer			= XPhysicalAlloc( average*page_size, MAXULONG_PTR, 0, PAGE_READWRITE | flags );
		if ( !buffer ) {
			max_value				= average;
			continue;
		}

		XPhysicalFree				( buffer );
		min_value					= average;
	}

	result_size						= min_value*page_size;
	result_address					= XPhysicalAlloc( result_size, MAXULONG_PTR, 0, PAGE_READWRITE | flags );
}

static u64 select_best_region	(
		u64 largest_but_two,
		u64 largest_but_one,
		u64 largest,
		xray::memory::platform::regions_type& resource_arenas
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
	u64 const test_size			= xray::math::align_up( (u64)(largest_but_one*share), resources_arena_granularity );
	if ( test_size > largest )
		return					largest + xray::math::align_up( (u64)(largest*share), resources_arena_granularity );

	return						largest_but_one + test_size;
}

static xray::memory::platform::regions_type::iterator& select_best_region(
		u64 const largest_but_two_size,
		xray::memory::platform::regions_type::iterator& largest_but_one,
		xray::memory::platform::regions_type::iterator& largest,
		u64 const size,
		xray::memory::platform::regions_type& resource_arenas
	)
{
	u64 const result0			= select_best_region( largest_but_two_size, (*largest_but_one).size - size, (*largest).size, resource_arenas );
	u64 const result1			= select_best_region( largest_but_two_size, (*largest_but_one).size, (*largest).size - size, resource_arenas );
	return						result0 >= result1 ? largest_but_one : largest;
}

void xray::memory::platform::allocate_arenas	(
		u64 const reserved_memory_size,
		u64 const reserved_address_space,
		regions_type& arenas,
		region& managed_arena,
		region& unmanaged_arena
	)
{
	R_ASSERT_CMP_U					( reserved_memory_size, ==, 0 );
	R_ASSERT_CMP_U					( reserved_address_space, ==, 0 );

	// calculate fixed memory size
	u64 engine_fixed_memory			= 0;
	regions_type::const_iterator i	= arenas.begin( );
	regions_type::const_iterator const e = arenas.end( );
	for ( ; i != e; ++i )
		engine_fixed_memory			+= (*i).size;
	
	u64 const free_memory			= 512*1024*1024;
	R_ASSERT_CMP					( free_memory, >, engine_fixed_memory );

	float const unmanaged_proportion = 1.f/3.f;
	u64 const resources_size		= free_memory - engine_fixed_memory;
	unmanaged_arena.size			= math::align_up( (u64)(resources_size*unmanaged_proportion), resources_arena_granularity );
	managed_arena.size				= resources_size - unmanaged_arena.size;

	regions_type				resource_arenas( ALLOCA(2*sizeof(region)), 2 );
	resource_arenas.push_back	( managed_arena );
	resource_arenas.push_back	( unmanaged_arena );

	R_ASSERT					( !arenas.empty() );

	std::sort					( arenas.begin(), arenas.end() );
	std::reverse				( arenas.begin(), arenas.end() );
	while ( !arenas.empty() && !arenas.back().size )
		arenas.pop_back			( );

	std::reverse				( arenas.begin(), arenas.end() );

	R_ASSERT					( !arenas.empty() );

	// allocate maximum memory
	u32 const max_available_memory	= 512*Mb - 32*Mb;

	u32 const page_size_16mb	= 16*Mb;
	allocate_memory				( 0,	max_available_memory/page_size_16mb,							page_size_16mb,	MEM_16MB_PAGES,		s_size_16mb,	s_address_16mb );

	u32 const page_size_64kb	= 64*Kb;
	allocate_memory				( 0,	(max_available_memory - s_size_16mb)/page_size_64kb,				page_size_64kb,	MEM_LARGE_PAGES,	s_size_64kb,	s_address_64kb );

	u32 const page_size_4kb		= 4*Kb;
	allocate_memory				( 0,	(max_available_memory - s_size_16mb - s_size_64kb)/page_size_4kb,	page_size_4kb,	0,					s_size_4kb,	s_address_4kb );

	u32 os_memory				= memory_must_be_left;
	if ( s_size_4kb >= os_memory )
		s_size_4kb				-= os_memory;
	else {
		os_memory				-= s_size_4kb;
		s_size_4kb				= 0;
		os_memory				= math::align_up( os_memory, page_size_64kb );
		if ( s_size_64kb >= os_memory )
			s_size_64kb			-= os_memory;
		else {
			os_memory			-= s_size_64kb;
			s_size_64kb			= 0;
			os_memory			= math::align_up( os_memory, page_size_16mb );
			R_ASSERT_CMP		( s_size_16mb, >=, os_memory );
			s_size_16mb			-= os_memory;
		}
	}
	
	u32 const regions_count		= (s_size_16mb ? 1 : 0) + (s_size_64kb ? 1 : 0) + (s_size_4kb ? 1 : 0);

	using xray::memory::platform::regions_type;
	using xray::memory::platform::region;

	// fill regions
	regions_type				regions( ALLOCA(regions_count*sizeof(region)), regions_count );

	for (int iteration=0; iteration < 2; ++iteration ) {
		s_allocated_size_16mb		= 0;
		s_allocated_size_64kb		= 0;
		s_allocated_size_4kb		= 0;
		regions.clear			( );

		if ( s_size_16mb ) {
			region const value	= { s_size_16mb, s_address_16mb, &s_allocated_size_16mb };
			regions.push_back	( value );
		}

		if ( s_size_64kb ) {
			region const value	= { s_size_64kb, s_address_64kb, &s_allocated_size_64kb };
			regions.push_back	( value );
		}

		if ( s_size_4kb ) {
			region const value	= { s_size_4kb, s_address_4kb, &s_allocated_size_4kb };
			regions.push_back	( value );
		}

		std::sort				( regions.begin(), regions.end() );

		// sort resource arenas
		if ( resource_arenas.front().size > resource_arenas.back().size )
			std::swap			( resource_arenas.front(), resource_arenas.back() );

		u64 const min_buffer_size	= arenas.front().size;

		regions_type::reverse_iterator i		= arenas.rbegin();
		regions_type::reverse_iterator const e	= arenas.rend();
		for ( ; i != e; ++i ) {
			regions_type::iterator const b	= regions.begin();
			regions_type::iterator const e	= regions.end();
			regions_type::iterator j	= std::lower_bound( b, e, *i );
			R_ASSERT					( j != regions.end() );
			R_ASSERT_CMP				( (*j).size, >=, (*i).size );

			// try to use 64kb pages when possible
			if ( (j == b) && ((*j).data == &s_allocated_size_4kb) )
				++j;

			regions_type::iterator k	= j + 1;
			if ( (k != e) && ( (k + 1) == e) )
				j					= select_best_region( j == b ? 0 : (*(j-1)).size, j, k, (*i).size, resource_arenas );

			(*i).address			= (*j).address;
			(pbyte&)((*j).address )	+= (*i).size;
			*(u32*)(*j).data		+= (u32)(*i).size;
			(*j).size				-= (*i).size;

			if ( j != regions.begin() ) {
				if ( (*j).size >= (*(j-1)).size )
					continue;
			}
			else {
				if ( (*j).size >= min_buffer_size )
					continue;

				regions.erase		( j );
				continue;
			}

			region temp				= *j;

			regions.erase			( j );
			if ( temp.size >= min_buffer_size )
				regions.insert(
					std::lower_bound( regions.begin(), regions.end(), temp ),
					temp
				);
		}

		R_ASSERT					( !regions.empty() );
		i							= regions.rbegin();

		R_ASSERT_CMP				( resource_arenas.size(), ==, 2 );
		region& minimum				= resource_arenas.front();
		region& maximum				= resource_arenas.back();
		R_ASSERT					( minimum.size <= maximum.size );
		u64 const resource_arenas_size	= minimum.size + maximum.size;

		for ( ; ; ) {
			if ( (*i).size >= resource_arenas_size ) {
				*(u32*)(*i).data	+= (u32)(minimum.size + maximum.size);
				(*i).size			-= minimum.size + maximum.size;
				minimum.address		= (*i).address;
				maximum.address		= (pbyte)(*i).address + minimum.size;
				break;
			}

			regions_type::reverse_iterator j = i + 1;
			float const share		= (float)minimum.size / (float)(maximum.size + minimum.size);
			if ( (regions.size() == 1) || ((*i).size*share >= (*j).size) ) {
				*(u32*)(*i).data	+= (u32)(*i).size;
				minimum.size		= xray::math::align_down( (u64)((*i).size*share), (u64)resources_arena_granularity );
				maximum.size		= (*i).size - minimum.size;
				minimum.address		= (*i).address;
				maximum.address		= (pbyte)(*i).address + minimum.size;
				(*i).size			= 0;
				break;
			}

			if ( ((*i).size >= maximum.size) && ((*j).size >= minimum.size) ) {
				*(u32*)(*j).data	+= (u32)minimum.size;
				minimum.address		= (*j).address;
				*(u32*)(*i).data	+= (u32)maximum.size;
				maximum.address		= (*i).address;
				(*j).size			-= minimum.size;
				(*i).size			-= maximum.size;
				break;
			}

			float const proportion	= (float)minimum.size / (float)maximum.size;
			u64 const test_size		= xray::math::align_up( (u64)((*j).size*proportion), (u64)resources_arena_granularity );
			if ( test_size > (*i).size ) {
				minimum.size		= xray::math::align_up( (u64)((*i).size*proportion), (u64)resources_arena_granularity );
				minimum.address		= (*j).address;
				*(u32*)(*j).data	+= (u32)minimum.size;

				maximum.size		= (*i).size;
				maximum.address		= (*i).address;
				*(u32*)(*i).data	+= (u32)(*i).size;

				(*j).size			-= minimum.size;
				(*i).size			-= maximum.size;
				break;
			}

			minimum.size			= (*j).size;
			minimum.address			= (*j).address;
			*(u32*)(*j).data		+= (u32)(*j).size;

			maximum.size			= test_size;
			maximum.address			= (*i).address;
			*(u32*)(*i).data		+= (u32)test_size;

			(*j).size				-= minimum.size;
			(*i).size				-= maximum.size;
			break;
		}

		if ( iteration )
			break;

		if ( s_address_4kb )
			XPhysicalFree			( s_address_4kb );

		if ( s_address_64kb )
			XPhysicalFree			( s_address_64kb );

		if ( s_address_16mb )
			XPhysicalFree			( s_address_16mb );

		if ( s_address_16mb ) {
			s_size_16mb				= s_allocated_size_16mb;
			if ( s_size_16mb ) {
				s_address_16mb		= XPhysicalAlloc( s_size_16mb, MAXULONG_PTR, 0, PAGE_READWRITE | MEM_16MB_PAGES );	
#ifndef MASTER_GOLD
				memory::fill32		( s_address_16mb, s_size_16mb, memory::uninitialized_value<u32>(), s_size_16mb/sizeof(u32) );
#endif // #ifndef MASTER_GOLD
			}
		}

		if ( s_address_64kb ) {
			s_size_64kb				= s_allocated_size_64kb;
			if ( s_size_64kb ) {
				s_address_64kb		= XPhysicalAlloc( s_size_64kb, MAXULONG_PTR, 0, PAGE_READWRITE | MEM_LARGE_PAGES );	
#ifndef MASTER_GOLD
				memory::fill32		( s_address_64kb, s_size_64kb, memory::uninitialized_value<u32>(), s_size_64kb/sizeof(u32) );
#endif // #ifndef MASTER_GOLD
			}
		}

		if ( s_address_4kb ) {
			s_size_4kb				= s_allocated_size_4kb;
			if ( s_size_4kb ) {
				s_address_4kb			= XPhysicalAlloc( s_size_4kb, MAXULONG_PTR, 0, PAGE_READWRITE );	
#ifndef MASTER_GOLD
				memory::fill32		( s_address_4kb, s_size_4kb, memory::uninitialized_value<u32>(), s_size_4kb/sizeof(u32) );
#endif // #ifndef MASTER_GOLD
			}
		}
	}

	if ( resource_arenas.front().data == &managed_arena ) {
		managed_arena			= resource_arenas.front();
		unmanaged_arena			= resource_arenas.back();
	}
	else {
		R_ASSERT_CMP			( resource_arenas.front().data, ==, &unmanaged_arena );
		managed_arena			= resource_arenas.back();
		unmanaged_arena			= resource_arenas.front();
	}
}

void xray::memory::register_debug_allocator	( u64 const maximum_memory_size )
{
#if XRAY_DEBUG_ALLOCATOR
	//DWORD console_memory_configuration;
	//HRESULT const result			= DmGetConsoleDebugMemoryStatus( &console_memory_configuration );
	//switch ( console_memory_configuration ) {
	//	case DM_CONSOLEMEMCONFIG_NOADDITIONALMEM : return;
	//	case DM_CONSOLEMEMCONFIG_ADDITIONALMEMDISABLED : return;
	//	case DM_CONSOLEMEMCONFIG_ADDITIONALMEMENABLED : break;
	//	default : NODEFAULT();
	//}
	//size_t const size				= 448*Mb;
	//pvoid const address				= DmDebugAlloc( size );
	//g_mt_allocator.initialize	( address, size, "debug multithreaded allocator" );
	u64 const max_memory_size		= 64*Mb;
	u64 memory_size					= math::max( maximum_memory_size ? maximum_memory_size : max_memory_size, 1*Mb );
	memory_size						= math::min( memory_size, max_memory_size );
	xray::debug::g_mt_allocator.do_register( memory_size, "debug multithreaded allocator" );
#else // #if XRAY_DEBUG_ALLOCATOR
	XRAY_UNREFERENCED_PARAMETER		( maximum_memory_size );
#endif // #if XRAY_DEBUG_ALLOCATOR
}

void xray::memory::on_after_memory_initialized	( )
{
}