////////////////////////////////////////////////////////////////////////////
//	Module 		: callback_generator.h
//	Created 	: 30.06.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : callback generator class
////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_PP_IS_ITERATING

#	ifndef CS_SCRIPT_CALLBACK_GENERATOR_H_INCLUDED
#		define CS_SCRIPT_CALLBACK_GENERATOR_H_INCLUDED

#		include <boost/preprocessor/repetition.hpp>
#		include <boost/preprocessor/iteration/iterate.hpp>

#		ifndef CS_SCRIPT_CALLBACK_RETURN_TYPE
#			define CS_SCRIPT_CALLBACK_RETURN_TYPE void
#			define CS_SCRIPT_CALLBACK_RETURN_OPERATOR (void)
#		else  // #ifndef CS_SCRIPT_CALLBACK_RETURN_TYPE
#			define CS_SCRIPT_CALLBACK_RETURN_OPERATOR return CS_SCRIPT_CALLBACK_RETURN_TYPE
#		endif // #ifndef CS_SCRIPT_CALLBACK_RETURN_TYPE

#		define CS_SCRIPT_CALLBACK_PARAMETERS(J,I,D) BOOST_PP_CAT(A,I) BOOST_PP_CAT(a,I)

// generate specializations
#		define BOOST_PP_ITERATION_LIMITS (0, CS_SCRIPT_CALLBACK_MAX_ARITY)
#		define BOOST_PP_FILENAME_1    <cs/script/callback_generator.h> // this file
#		include BOOST_PP_ITERATE()

#	endif // #ifndef CS_SCRIPT_CALLBACK_GENERATOR_H_INCLUDED

#else // #ifndef BOOST_PP_IS_ITERATING

#	define n BOOST_PP_ITERATION()

// specialization pattern
#if n > 0
template <BOOST_PP_ENUM_PARAMS(n, class A)>
#endif // #if n > 0
inline CS_SCRIPT_CALLBACK_RETURN_TYPE operator() (BOOST_PP_ENUM(n,CS_SCRIPT_CALLBACK_PARAMETERS,BOOST_PP_EMPTY)) const
{
	if (m_functor.is_valid()) {
		if (m_object.is_valid())
			CS_SCRIPT_CALLBACK_RETURN_OPERATOR (::luabind::call_function<_return_type>(m_functor, m_object BOOST_PP_ENUM_TRAILING_PARAMS(n,a)));
		else
			CS_SCRIPT_CALLBACK_RETURN_OPERATOR (::luabind::call_function<_return_type>(m_functor BOOST_PP_ENUM_TRAILING_PARAMS(n,a)));
	}
	CS_SCRIPT_CALLBACK_RETURN_OPERATOR (0);
}
	
#	if n == CS_SCRIPT_CALLBACK_MAX_ARITY
#		undef CS_SCRIPT_CALLBACK_RETURN_TYPE
#		undef CS_SCRIPT_CALLBACK_RETURN_OPERATOR
#	endif // #if n == CS_SCRIPT_CALLBACK_MAX_ARITY

#	undef n

#endif // #ifndef BOOST_PP_IS_ITERATING
