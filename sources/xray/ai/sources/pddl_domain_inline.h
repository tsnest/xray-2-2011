////////////////////////////////////////////////////////////////////////////
//	Created		: 01.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_DOMAIN_INLINE_H_INCLUDED
#define PDDL_DOMAIN_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline pddl_domain::pddl_domain	( )
{
}

inline bool pddl_domain::has_predicate	( pddl_predicate_id const type ) const
{
	return m_predicates.find( type ) != m_predicates.end();
}

template < typename T >
inline void pddl_domain::register_parameter_type( parameter_type const type_to_be_assigned )
{
	m_registered_types[typeid( T ).name()] = type_to_be_assigned;
}

template < typename T >
parameter_type pddl_domain::get_registered_type	( ) const
{
	registered_parameter_types::const_iterator iter = m_registered_types.find( typeid( T ).name() );
	return iter != m_registered_types.end() ? iter->second : parameter_type(-1);
}

template < typename T >
inline void pddl_domain::deduce_parameter_type	( pddl_predicate& predicate ) const
{
	registered_parameter_types::const_iterator iter = m_registered_types.find( typeid( T ).name() );
	R_ASSERT						( iter != m_registered_types.end(), "unregistered parameter type %s", typeid( T ).name() );
	predicate.add_parameter			( iter->second );
}

inline bool predicate_binder( pddl_predicate const& predicate, property_values_type const& values )
{
	R_ASSERT_CMP_U	( values.size(), ==, 0 );
	return			( *horrible_cast< pcvoid, boost::function< bool ( ) > const* >( &predicate.function_storage() ).second )();
}

template < typename T1, typename T2 >
inline bool predicate_binder( pddl_predicate const& predicate, property_values_type const& values )
{
	R_ASSERT_CMP	( values.size(), ==, 1 );
	return			( *horrible_cast< pcvoid, boost::function< bool ( T2 ) > const* >( &predicate.function_storage() ).second )(
						static_cast_checked< T2 >( horrible_cast< pcvoid, T1 >( values[0] ).second )
					);
}

template < typename T1, typename T2, typename T3, typename T4 >
inline bool predicate_binder( pddl_predicate const& predicate, property_values_type const& values )
{
	R_ASSERT_CMP	( values.size(), ==, 2 );
	return			( *horrible_cast< pcvoid, boost::function< bool ( T3, T4 ) > const* >( &predicate.function_storage() ).second )(
						static_cast_checked< T3 >( horrible_cast< pcvoid, T1 >( values[0] ).second ),
						static_cast_checked< T4 >( horrible_cast< pcvoid, T2 >( values[1] ).second )
					);
}

template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
inline bool predicate_binder( pddl_predicate const& predicate, property_values_type const& values )
{
	R_ASSERT_CMP	( values.size(), ==, 3 );
	return			( *horrible_cast< pcvoid, boost::function< bool ( T4, T5, T6 ) > const* >( &predicate.function_storage() ).second )(
						static_cast_checked< T4 >( horrible_cast< pcvoid, T1 >( values[0] ).second ),
						static_cast_checked< T5 >( horrible_cast< pcvoid, T2 >( values[1] ).second ),
						static_cast_checked< T6 >( horrible_cast< pcvoid, T3 >( values[2] ).second )
					);
}

inline void pddl_domain::add_predicate(
		pddl_predicate_id const required_type,
		pcstr name,
		boost::function< bool ( ) > const& predicate
	)
{
	R_ASSERT					( !has_predicate( required_type ), "attempt to add predicate of the same type as already added: %d", required_type );
	
	pddl_predicate* result		= NEW( pddl_predicate )( required_type, name );
	
	result->m_function_storage.~function< bool () >();
	new( &result->m_function_storage )boost::function< bool ( ) >( predicate );
	result->m_predicate_binder	= &predicate_binder;
	
	m_predicates[required_type]	= result;
}

template < typename T1, typename T2 >
inline void pddl_domain::add_predicate1	(
		pddl_predicate_id const required_type,
		pcstr name,
		boost::function< bool ( T2 ) > const& predicate
	)
{
	R_ASSERT					( !has_predicate( required_type ), "attempt to add predicate of the same type as already added: %d", required_type );
	pddl_predicate* result		= NEW( pddl_predicate )( required_type, name );

	result->m_function_storage.~function< bool () >();
	new( &result->m_function_storage )boost::function< bool ( T2 ) >( predicate );
	result->m_predicate_binder	= &predicate_binder< T1, T2 >;
	deduce_parameter_type< T1 >	( *result );

	m_predicates[required_type]	= result;
}

template < typename T >
inline void pddl_domain::add_predicate	(
		pddl_predicate_id const required_type,
		pcstr name,
		boost::function< bool ( T ) > const& predicate
	)
{
	add_predicate1< T, T >		( required_type, name, predicate );
}

template < typename T1, typename T2, typename T3, typename T4 >
inline void pddl_domain::add_predicate2	(
		pddl_predicate_id const required_type,
		pcstr name,
		boost::function< bool ( T3, T4 ) > const& predicate
	)
{
	R_ASSERT					( !has_predicate( required_type ), "attempt to add predicate of the same type as already added: %d", required_type );
	pddl_predicate* result		= NEW( pddl_predicate )( required_type, name );

	result->m_function_storage.~function< bool () >();
	new( &result->m_function_storage )boost::function< bool ( T3, T4 ) >( predicate );
	result->m_predicate_binder	= &predicate_binder< T1, T2, T3, T4 >;
	deduce_parameter_type< T1 >	( *result );
	deduce_parameter_type< T2 >	( *result );

	m_predicates[required_type]	= result;
}

template < typename T1, typename T2 >
inline void pddl_domain::add_predicate	(
		pddl_predicate_id const required_type,
		pcstr name,
		boost::function< bool ( T1, T2 ) > const& predicate
	)
{
	add_predicate2< T1, T2, T1, T2 >	( required_type, name, predicate );
}

template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
inline void pddl_domain::add_predicate3	(
		pddl_predicate_id const required_type,
		pcstr name,
		boost::function< bool ( T4, T5, T6 ) > const& predicate
	)
{
	R_ASSERT					( !has_predicate( required_type ), "attempt to add predicate of the same type as already added: %d", required_type );
	pddl_predicate* result		= NEW( pddl_predicate )( required_type, name );

	result->function_storage().~function< bool () >();
	new( &result->m_function_storage )boost::function< bool ( T4, T5, T6 ) >( predicate );
	result->m_predicate_binder	= &predicate_binder< T1, T2, T3, T4, T5, T6 >;
	deduce_parameter_type< T1 >	( *result );
	deduce_parameter_type< T2 >	( *result );
	deduce_parameter_type< T3 >	( *result );

	m_predicates[required_type]	= result;
}

template < typename T1, typename T2, typename T3 >
inline void pddl_domain::add_predicate	(
		pddl_predicate_id const required_type,
		pcstr name,
		boost::function< bool ( T1, T2, T3 ) > const& predicate
	)
{
	add_predicate3< T1, T2, T3, T1, T2, T3 >( required_type, name, predicate );
}

inline void pddl_domain::add_predicate	(
		pddl_predicate_id const required_type,
		pcstr name,
		bool ( *predicate )( )
	)
{
	add_predicate				( required_type, name, boost::function< bool ( ) >( predicate ) );
}

template < typename T >
inline void pddl_domain::add_predicate	(
		pddl_predicate_id const required_type,
		pcstr name,
		bool ( *predicate )( T )
	)
{
	add_predicate				( required_type, name, boost::function< bool ( T ) >( predicate ) );
}

template < typename T1, typename T2 >
inline void pddl_domain::add_predicate1	(
		pddl_predicate_id const required_type,
		pcstr name,
		bool ( *predicate )( T2 )
	)
{
	return add_predicate< T1 >( required_type, name, boost::function< bool ( T2 ) >( predicate ) );
}

template < typename T1, typename T2 >
inline void pddl_domain::add_predicate	(
		pddl_predicate_id const required_type,
		pcstr name,
		bool ( *predicate )( T1, T2 )
	)
{
	return add_predicate< T1, T2 >( required_type, name, boost::function< bool ( T1, T2 ) >( predicate ) );
}

template < typename T1, typename T2, typename T3 >
inline void pddl_domain::add_predicate	(
		pddl_predicate_id const required_type,
		pcstr name,
		bool ( *predicate )( T1, T2, T3 )
	)
{
	return add_predicate< T1, T2, T3 >( required_type, name, boost::function< bool ( T1, T2, T3 ) >( predicate ) );
}

template < typename T1, typename T2, typename T3, typename T4 >
inline void pddl_domain::add_predicate2	(
		pddl_predicate_id const required_type,
		pcstr name,
		bool ( *predicate )( T3, T4 )
	)
{
	return add_predicate< T1, T2, T3, T4 >( required_type, boost::function< bool ( T3, T4 ) >( predicate ) );
}

template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
inline void pddl_domain::add_predicate3	(
		pddl_predicate_id const required_type,
		pcstr name,
		bool ( *predicate )( T4, T5, T6 )
	)
{
	return add_predicate< T1, T2, T3, T4, T5, T6 >( required_type, boost::function< bool ( T4, T5, T6 ) >( predicate ) );
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef PDDL_DOMAIN_INLINE_H_INCLUDED