#ifndef XRAY_MACRO_TODO_H_INCLUDED
#define XRAY_MACRO_TODO_H_INCLUDED

#ifdef XRAY_TODO
#	error do not define XRAY_TODO macro
#endif // #ifdef XRAY_TODO

#define XRAY_TODO(message_to_show)	__FILE__ "(" XRAY_MAKE_STRING(__LINE__) "): @todo: " message_to_show

#if defined(GCC_VERSION) && ( GCC_VERSION < 4 )
	// to disable unknown pragma warning message
#	pragma warning( disable : ??? )
#endif // #if defined(GCC_VERSION) && ( GCC_VERSION < 4.5 )

#endif // #ifndef XRAY_MACRO_TODO_H_INCLUDED