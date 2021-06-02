////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OUT_OF_MEMORY_HANDLER_TYPES_H_INCLUDED
#define OUT_OF_MEMORY_HANDLER_TYPES_H_INCLUDED

#define USE_OUT_OF_MEMORY_HANDLER

#ifdef USE_OUT_OF_MEMORY_HANDLER
	typedef void const* out_of_memory_handler_parameter_type;
	typedef char (__stdcall *out_of_memory_handler_type)	(void*, out_of_memory_handler_parameter_type, int);
#endif // #ifdef USE_OUT_OF_MEMORY_HANDLER

#endif // #ifndef OUT_OF_MEMORY_HANDLER_TYPES_H_INCLUDED