////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_manager_generator.h
//	Created 	: 05.01.2006
//  Modified 	: 05.01.2006
//	Author		: Dmitriy Iassenev
//	Description : memory manager generator
////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_PP_IS_ITERATING

#	ifndef CS_CORE_MEMORY_MANAGER_GENERATOR
#		define CS_CORE_MEMORY_MANAGER_GENERATOR

#		include <boost/type_traits/is_pod.hpp>
#		include <boost/preprocessor/repetition.hpp>
#		include <boost/preprocessor/iteration/iterate.hpp>

#		define PARAMETERS(J,I,D)			const BOOST_PP_CAT(P,I) & BOOST_PP_CAT(p,I)
#		define BOOST_PP_ITERATION_LIMITS	(0,CS_MEMORY_MANAGER_GENERATOR_MAX_ARITY)
#		define BOOST_PP_FILENAME_1			<cs/core/memory_manager_generator.h> // this file

#		include BOOST_PP_ITERATE()

#		undef PARAMETERS
#		undef BOOST_PP_ITERATION_LIMITS
#		undef BOOST_PP_FILENAME_1
#	endif // MEMORY_MANAGER_GENERATOR

#else // #ifndef BOOST_PP_IS_ITERATING

#	define N								BOOST_PP_ITERATION()
#	define CS_MERGE_HELPER(a,b)				a##b
#	define CS_MERGE(a,b)					CS_MERGE_HELPER(a,b)
#	define CS_NEW_DETAIL					CS_MERGE(CS_NEW_DETAIL,N)
#	define CS_NEW_DETAIL_COPY_CONSTRUCTOR	CS_MERGE(CS_NEW_DETAIL_COPY_CONSTRUCTOR,N)

template <bool>
struct CS_NEW_DETAIL {
	template <typename T BOOST_PP_ENUM_TRAILING_PARAMS(N, class P)>
	static inline T* initialize (T* result BOOST_PP_ENUM_TRAILING(N,PARAMETERS,BOOST_PP_EMPTY))
	{
		return	new (result) T(BOOST_PP_ENUM_PARAMS(N,p));
	}
};

template <>
struct CS_NEW_DETAIL<true> {
	template <typename T BOOST_PP_ENUM_TRAILING_PARAMS(N, class P)>
	static inline T* initialize (T* result BOOST_PP_ENUM_TRAILING(N,PARAMETERS,BOOST_PP_EMPTY))
	{
		return	result;
	}
};


struct CS_NEW_DETAIL_COPY_CONSTRUCTOR {
	template <typename T>
	static inline T* initialize (T* result, T const& value)
	{
		return	new (result) T(value);
	}

	template <typename T BOOST_PP_ENUM_TRAILING_PARAMS(N, class P)>
	static inline T* initialize (T* result BOOST_PP_ENUM_TRAILING(N,PARAMETERS,BOOST_PP_EMPTY))
	{
		return	CS_NEW_DETAIL<boost::is_pod<T>::value>::initialize(result BOOST_PP_ENUM_TRAILING_PARAMS(N,p));
	}
};

template <typename T BOOST_PP_ENUM_TRAILING_PARAMS(N, class P)>
inline T* cs_new (BOOST_PP_ENUM(N,PARAMETERS,BOOST_PP_EMPTY))
{
	T*			result = (T*)cs_malloc( sizeof(T), typeid(T).name() );
	return		CS_NEW_DETAIL_COPY_CONSTRUCTOR::initialize(result BOOST_PP_ENUM_TRAILING_PARAMS(N,p));
}

#	undef N
#	undef CS_MERGE_HELPER
#	undef CS_MERGE
#	undef CS_NEW_DETAIL
#	undef CS_NEW_DETAIL_COPY_CONSTRUCTOR

#endif // #ifndef BOOST_PP_IS_ITERATING