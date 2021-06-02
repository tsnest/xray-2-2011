////////////////////////////////////////////////////////////////////////////
//	Module 		: threading_functions.h
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : threading functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_THREADING_FUNCTIONS_H_INCLUDED
#define CS_CORE_THREADING_FUNCTIONS_H_INCLUDED

namespace threading {

typedef void thread_function_type	(pvoid);

CS_CORE_API	void	spawn			(
	thread_function_type* function,
	pcstr name,
	u32 const stack_size,
	pvoid arguments
);

CS_CORE_API	void	yield			(u32 const milliseconds = 0);
CS_CORE_API	void	debugger_name	(pcstr debugger_name);

}

#endif // #ifndef CS_CORE_THREADING_FUNCTIONS_H_INCLUDED