////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <new>
#include "editor_world.h"
#include "animated_model_instance_cook.h"

#pragma managed( push, off )
#	include <xray/editor/world/api.h>
	static void enable_fpe( bool const value );
#pragma managed( pop )

using xray::editor::editor_world;

static bool					s_initialized = false;
static char					s_world_fake[ sizeof( editor_world ) ];
static editor_world&		s_world = ( editor_world& )s_world_fake;

xray::editor::allocator_type*	xray::editor::g_allocator = 0;

static xray::editor::world* create_world		( xray::editor::engine& engine, bool fpe_enabled )
{
#ifdef DEBUG
	enable_fpe				( fpe_enabled );
#else // #ifdef DEBUG
	XRAY_UNREFERENCED_PARAMETER	( fpe_enabled );
#endif // #ifdef DEBUG

	R_ASSERT				( !s_initialized );
	s_initialized			= true;
	
	typedef xray::editor::editor_world	world;
	new ( &s_world ) world	( engine );

	static xray::editor::animated_model_instance_cook s_animated_model_cook( &s_world.get_physics_world() );
	register_cook			( &s_animated_model_cook );

	return					( &s_world );
}

static void destroy_world						( xray::editor::world*& world )
{
	R_ASSERT				( s_initialized );
	R_ASSERT				( &s_world == world );
	s_initialized			= false;
	s_world.~editor_world	( );
	world					= 0;
}

static void install_bugtrap						( )
{
	// this is workaround on incorrect BugTrapN initialization in
	// case of call from unmanaged code: globals and statics are not initialized.
	// but when we call it from managed code first, all works properly
	IntelleSoft::BugTrap::ExceptionHandler();
}

static System::Reflection::Assembly^ assembly_resolve					( System::Object^ , System::ResolveEventArgs^ e )
{
#ifdef DEBUG
	return System::Reflection::Assembly::Load( System::IO::File::ReadAllBytes( e->Name->Substring( 0, e->Name->IndexOf( ',' ) ) + "-debug.dll" ) );
#else  // #ifdef DEBUG
	return System::Reflection::Assembly::Load( System::IO::File::ReadAllBytes( e->Name->Substring( 0, e->Name->IndexOf( ',' ) ) + "-release.dll" ) );
#endif // #ifdef DEBUG
}

static void install_asembly_resolver			( )
{
	// this is workaround on not understanding separate output C# assembly names by VisualStudio
	// we change the names of our assemblies at the post build step and load by hand, when the AppDomain is trying to resolve the assembly.
	System::AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler( &assembly_resolve );
}

static void collect_garbage						( )
{
	System::GC::Collect					( );
	System::GC::WaitForPendingFinalizers( );
	System::GC::WaitForFullGCComplete	( );
}

#pragma unmanaged

#ifdef DEBUG
static void enable_fpe							( bool const value )
{
	xray::debug::enable_fpe	( value );
}
#endif // #ifdef DEBUG

xray::editor::world* xray::editor::create_world	( engine& engine )
{
#ifdef DEBUG
	bool const fpe_enabled	= xray::debug::is_fpe_enabled( );
	xray::debug::enable_fpe	( false );
#else // #ifdef DEBUG
	bool const fpe_enabled	= false;
#endif // #ifdef DEBUG
	return					::create_world( engine, fpe_enabled );
}

void xray::editor::destroy_world				( xray::editor::world*& world )
{
	::destroy_world			( world );
	::collect_garbage		( );
}

void xray::editor::set_memory_allocator			( allocator_type& allocator )
{
	install_bugtrap			( );
	install_asembly_resolver( );

	ASSERT					( !g_allocator );
	g_allocator				= &allocator;
	allocator.user_current_thread_id();
}
#pragma managed