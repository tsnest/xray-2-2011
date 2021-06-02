////////////////////////////////////////////////////////////////////////////
//	Module 		: world_inline.h
//	Created 	: 15.06.2005
//  Modified 	: 15.06.2005
//	Author		: Dmitriy Iassenev
//	Description : script engine class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef WORLD_INLINE_H_INCLUDED
#define WORLD_INLINE_H_INCLUDED

inline lua_State* cs::script::detail::world::state			() const
{
	ASSERT				(m_state, "lua state hasn't been initialized");
	return				m_state;
}

inline cs::script::engine& cs::script::detail::world::engine() const
{
	return				m_engine;
}

inline cs::script::detail::world& lua_world					(lua_State* const& state)
{
	luabind::object		result = cs::script::detail::world::object("cs.script.world", luabind::globals(state));
	return				*luabind::object_cast<cs::script::detail::world*>(result);
}

inline cs::script::detail::world* const& cs::script::detail::world::next_world	() const
{
	return				m_next_world;
}

#if CS_DEBUG_LIBRARIES
inline void cs::script::detail::world::current_thread		(thread* thread)
{
	ASSERT				(!thread || !m_current_thread, "script thread cannot be explicitly changed");
	ASSERT				(thread || m_current_thread, "script thread cannot be explicitly changed");
	m_current_thread	= thread;
}

inline cs::script::detail::thread* cs::script::detail::world::current_thread	() const
{
	return				m_current_thread;
}
#endif // #if CS_DEBUG_LIBRARIES

#endif // #ifndef WORLD_INLINE_H_INCLUDED