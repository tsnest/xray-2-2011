////////////////////////////////////////////////////////////////////////////
//	Module 		: thread.h
//	Created 	: 24.06.2005
//  Modified 	: 24.06.2005
//	Author		: Dmitriy Iassenev
//	Description : script thread class
////////////////////////////////////////////////////////////////////////////

#ifndef SCRIPT_THREAD_H_INCLUDED
#define SCRIPT_THREAD_H_INCLUDED

#include <boost/noncopyable.hpp>

struct lua_State;

namespace cs {
namespace script {
namespace detail {

class world;

class thread : 
	public boost::noncopyable,
	public cs::script::thread
{
private:
	world&						m_world;
	lua_State*					m_thread_environment;
	int							m_lua_reference;
	bool						m_suspended;
	str_shared					m_id;

private:
	inline	int					lua_reference		() const;
			void				unreference			();
			void				resume_internal		();
			void				construct_internal	();

public:
								thread				(world* world, pcstr string_to_execute);
	virtual						~thread				();

public:
	virtual	void CS_SCRIPT_CALL	resume				();
	virtual	bool CS_SCRIPT_CALL	suspended			();

public:
	inline	lua_State*			thread_environment	() const;
	inline	bool				suspended			() const;
	inline	str_shared const&	id					() const;
};

} // namespace detail
} // namespace script
} // namespace cs

#include "thread_inline.h"

#endif // #ifndef SCRIPT_THREAD_H_INCLUDED