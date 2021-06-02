////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_NETWORK_API_H_INCLUDED
#define XRAY_NETWORK_API_H_INCLUDED

#ifndef XRAY_NETWORK_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_NETWORK_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_NETWORK_BUILDING
#			define XRAY_NETWORK_API				XRAY_DLL_EXPORT
#		else // #ifdef XRAY_NETWORK_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_NETWORK_API			XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_NETWORK_API			XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_NETWORK_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_NETWORK_API

namespace xray {
namespace network {

struct engine;
struct world;

typedef memory::doug_lea_allocator_type			allocator_type;

XRAY_NETWORK_API	world*	create_world		( engine& engine );
XRAY_NETWORK_API	void	destroy_world		( world*& world );
XRAY_NETWORK_API	void	set_memory_allocator( allocator_type& allocator );
XRAY_NETWORK_API	void	dispatch_callbacks	( );

} // namespace network
} // namespace xray

#endif // #ifndef XRAY_NETWORK_API_H_INCLUDED