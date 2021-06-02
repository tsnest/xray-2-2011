////////////////////////////////////////////////////////////////////////////
//	Module 		: threading_mutex.h
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : threading mutex class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_THREADING_MUTEX_H_INCLUDED
#define CS_CORE_THREADING_MUTEX_H_INCLUDED

namespace threading {

class CS_CORE_API mutex {
public:
					mutex	();
					~mutex	();

public:
			void	lock	();
			void	unlock	();
			bool	trylock	();

private:
#if CS_PLATFORM_WINDOWS_32
	char	m_mutex[24];		// sizeof(CRITICAL_SECTION);
#elif CS_PLATFORM_WINDOWS_64	// #if CS_PLATFORM_WINDOWS_32
	char	m_mutex[40];		// sizeof(CRITICAL_SECTION);
#elif CS_PLATFORM_XBOX_360		// #if CS_PLATFORM_WINDOWS_64
	char	m_mutex[28];		// sizeof(CRITICAL_SECTION);
#elif CS_PLATFORM_PS3			// #elif CS_PLATFORM_XBOX_360
	char	m_mutex[24];		// sizeof(sys_lwmutex_t);
#else 							// #elif CS_PLATFORM_PS3
#	error define array with correct size here
#endif							// #elif CS_PLATFORM_PS3
};

}

#endif // #ifndef CS_CORE_THREADING_MUTEX_H_INCLUDED