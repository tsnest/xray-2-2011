////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PARTICLE_API_H_INCLUDED
#define XRAY_PARTICLE_API_H_INCLUDED

#ifndef XRAY_PARTICLE_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_PARTICLE_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_PARTICLE_BUILDING
#			define XRAY_PARTICLE_API		XRAY_DLL_EXPORT
#		else // #ifdef XRAY_PARTICLE_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_PARTICLE_API	XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_PARTICLE_API	XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_PARTICLE_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_PARTICLE_API

namespace xray {
namespace particle {

XRAY_PARTICLE_API	void	initialize	( );
XRAY_PARTICLE_API	void	finalize	( );

} // namespace particle
} // namespace xray

#endif // #ifndef XRAY_PARTICLE_API_H_INCLUDED
