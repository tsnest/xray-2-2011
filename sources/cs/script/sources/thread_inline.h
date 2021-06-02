////////////////////////////////////////////////////////////////////////////
//	Module 		: thread_inline.h
//	Created 	: 24.06.2005
//  Modified 	: 24.06.2005
//	Author		: Dmitriy Iassenev
//	Description : script thread class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef SCRIPT_THREAD_INLINE_H_INCLUDED
#define SCRIPT_THREAD_INLINE_H_INCLUDED

inline int  cs::script::detail::thread::lua_reference		() const
{
	return		m_lua_reference;
}

inline bool cs::script::detail::thread::suspended				() const
{
	return		m_suspended;
}

inline str_shared const& cs::script::detail::thread::id			() const
{
	return		m_id;
}

inline lua_State* cs::script::detail::thread::thread_environment() const
{
	return		m_thread_environment;
}

#endif // #ifndef SCRIPT_THREAD_INLINE_H_INCLUDED