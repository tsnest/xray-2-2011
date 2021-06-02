////////////////////////////////////////////////////////////////////////////
//	Module 		: thread.h
//	Created 	: 29.12.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : thread interface
////////////////////////////////////////////////////////////////////////////

#ifndef CS_SCRIPT_THREAD_H_INCLUDED
#define CS_SCRIPT_THREAD_H_INCLUDED

#include <cs/script/interfaces.h>

struct lua_State;

namespace cs {
namespace script {

struct DECLSPEC_NOVTABLE thread {
	virtual	void	CS_SCRIPT_CALL	resume		() = 0;
	virtual	bool	CS_SCRIPT_CALL	suspended	() = 0;

protected:
	CS_DECLARE_PURE_VIRTUAL_DESTRUCTOR( thread )
}; // struct thread

} // namespace script
} // namespace cs

#endif // #ifndef CS_SCRIPT_THREAD_H_INCLUDED