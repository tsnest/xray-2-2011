////////////////////////////////////////////////////////////////////////////
//	Module 		: lua_studio_backend_singleton.h
//	Created 	: 12.04.2008
//  Modified 	: 12.04.2008
//	Author		: Dmitriy Iassenev
//	Description : lua studio backend singleton class
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUGGER_SINGLETON_H_INCLUDED
#define DEBUGGER_SINGLETON_H_INCLUDED

class engine;

namespace cs {
	namespace lua_studio {
		struct world;
	} // namespace cs
} // namespace lua_studio

class lua_studio_backend_singleton {
public:
					lua_studio_backend_singleton	();
					~lua_studio_backend_singleton	();
			void	cleanup							();
	cs::lua_studio::world& world					();

private:
			void	construct						();

private:
	cs::lua_studio::world*	m_world;
};

#endif // #ifndef DEBUGGER_SINGLETON_H_INCLUDED