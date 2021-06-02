////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_PROBLEM_H_INCLUDED
#define PDDL_PROBLEM_H_INCLUDED

#include "action_instance.h"
#include "pddl_domain.h"

namespace xray {
namespace ai {
namespace planning {

class pddl_domain;

class pddl_problem : private boost::noncopyable
{
public:
	inline			pddl_problem					( pddl_domain const& domain );

	inline	bool	has_action_instance				( u32 const action_type ) const;
	action_instance* pop_action_instance			( );

	// parameterless actions
	inline	void	add_action_instance				(
						action_instance& result,
						boost::function< void ( ) > const& initializer,
						boost::function< void ( ) > const& executor,
						boost::function< void ( ) > const& finalizer
					);
	inline	void	add_action_instance				(
						action_instance& result,
						void ( *initializer )( ),
						void ( *executor )( ),
						void ( *finalizer )( )
					);

	// single parameter actions
	template < typename T1, typename T2 >
	inline	void	add_action_instance1			(
						action_instance& result,
						boost::function< void ( T2 ) > const& initializer,
						boost::function< void ( T2 ) > const& executor,
						boost::function< void ( T2 ) > const& finalizer
					);
	template < typename T >
	inline	void	add_action_instance				(
						action_instance& result,
						boost::function< void ( T ) > const& initializer,
						boost::function< void ( T ) > const& executor,
						boost::function< void ( T ) > const& finalizer
					);
	template < typename T >
	inline	void	add_action_instance				(
						action_instance& result,
						void ( *initializer )( T ),
						void ( *executor )( T ),
						void ( *finalizer )( T )
					);
	template < typename T1, typename T2 >
	inline	void	add_action_instance1			(
						action_instance& result,
						void ( *initializer )( T2 ),
						void ( *executor )( T2 ),
						void ( *finalizer )( T2 )
					);

	// two parameters actions
	template < typename T1, typename T2, typename T3, typename T4 >
	inline	void	add_action_instance2			(
						action_instance& result,
						boost::function< void ( T3, T4 ) > const& initializer,
						boost::function< void ( T3, T4 ) > const& executor,
						boost::function< void ( T3, T4 ) > const& finalizer
					);
	template < typename T1, typename T2 >
	inline	void	add_action_instance				(
						action_instance& result,
						boost::function< void ( T1, T2 ) > const& initializer,
						boost::function< void ( T1, T2 ) > const& executor,
						boost::function< void ( T1, T2 ) > const& finalizer
					);
	template < typename T1, typename T2 >
	inline	void	add_action_instance				(
						action_instance& result,
						void ( *initializer )( T1, T2 ),
						void ( *executor )( T1, T2 ),
						void ( *finalizer )( T1, T2 )
					);
	template < typename T1, typename T2, typename T3, typename T4 >
	inline	void	add_action_instance2			(
						action_instance& result,
						void ( *initializer )( T3, T4 ),
						void ( *executor )( T3, T4 ),
						void ( *finalizer )( T3, T4 )
					);

	// three parameters actions
	template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
	inline	void	add_action_instance3			(
						action_instance& result,
						boost::function< void ( T4, T5, T6 ) > const& initializer,
						boost::function< void ( T4, T5, T6 ) > const& executor,
						boost::function< void ( T4, T5, T6 ) > const& finalizer
					);
	template < typename T1, typename T2, typename T3 >
	inline	void	add_action_instance				(
						action_instance& result,
						boost::function< void ( T1, T2, T3 ) > const& initializer,
						boost::function< void ( T1, T2, T3 ) > const& executor,
						boost::function< void ( T1, T2, T3 ) > const& finalizer
					);
	template < typename T1, typename T2, typename T3 >
	inline	void	add_action_instance				(
						action_instance& result,
						void ( *initializer )( T1, T2, T3 ),
						void ( *executor )( T1, T2, T3 ),
						void ( *finalizer )( T1, T2, T3 )
					);
	template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
	inline	void	add_action_instance3			(
						action_instance& result,
						void ( *initializer )( T4, T5, T6 ),
						void ( *executor )( T4, T5, T6 ),
						void ( *finalizer )( T4, T5, T6 )
					);

	action_instance const*	get_action_instance		( u32 const action_type ) const;
	
private:
	typedef	fixed_vector< action_instance*, 32 >	actions_type;

private:
	actions_type			m_action_instances;
	pddl_domain const&		m_domain;
}; // class pddl_problem

} // namespace planning
} // namespace ai
} // namespace xray

#include "pddl_problem_inline.h"

#endif // #ifndef PDDL_PROBLEM_H_INCLUDED