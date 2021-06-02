////////////////////////////////////////////////////////////////////////////
//	Module 		: callback.h
//	Created 	: 30.06.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : callback class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_SCRIPT_CALLBACK_H_INCLUDED
#define CS_SCRIPT_CALLBACK_H_INCLUDED

#include <cs/script/callback_impl.h>

#define CS_SCRIPT_CALLBACK_MAX_ARITY 15

namespace cs {
namespace script {

template <typename _return_type>
struct callback : public callback_impl<_return_type> {
	#define	CS_SCRIPT_CALLBACK_RETURN_TYPE _return_type
	#include <cs/script/callback_generator.h>
	#undef CS_SCRIPT_CALLBACK_GENERATOR_H_INCLUDED
}; // struct callback

template <>
struct callback<void> : public callback_impl<void> {
	#include <cs/script/callback_generator.h>
}; // struct callback<void>

} // namespace script
} // namespace cs

#undef CS_SCRIPT_CALLBACK_MAX_ARITY

#endif // #ifndef CS_SCRIPT_CALLBACK_H_INCLUDED