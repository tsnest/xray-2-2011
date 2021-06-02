////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_DOMAIN_H_INCLUDED
#define PDDL_DOMAIN_H_INCLUDED

#include "pddl_predicate.h"
#include "generalized_action.h"

namespace xray {
namespace ai {
namespace planning {

class pddl_problem;
class generalized_action;

typedef u32 pddl_predicate_id;

class pddl_domain : private boost::noncopyable
{
public:
	inline			pddl_domain					( );
					~pddl_domain				( );
	
	template < typename T >
	inline	void	register_parameter_type		( parameter_type const type_to_be_assigned );

	// no parameters predicates
	inline	void	add_predicate				(
						pddl_predicate_id const required_type,
						pcstr name,
						boost::function< bool ( ) > const& predicate
					);
	inline	void	add_predicate				(
						pddl_predicate_id const required_type,
						pcstr name,
						bool ( *predicate )( )
					);

	// single parameter predicates
	template < typename T1, typename T2 >
	inline	void	add_predicate1				(
						pddl_predicate_id const required_type,
						pcstr name,
						boost::function< bool ( T2 ) > const& predicate
					);
	template < typename T >
	inline	void	add_predicate				(
						pddl_predicate_id const required_type,
						pcstr name,
						boost::function< bool ( T ) > const& predicate
					);
	template < typename T1, typename T2 >
	inline void		add_predicate1				(
						pddl_predicate_id const required_type,
						pcstr name,
						bool ( *predicate )( T2 )
					);
	template < typename T >
	inline	void	add_predicate				(
						pddl_predicate_id const required_type,
						pcstr name,
						bool ( *predicate )( T )
					);

	// two parameters predicates
	template < typename T1, typename T2, typename T3, typename T4 >
	inline	void	add_predicate2				(
						pddl_predicate_id const required_type,
						pcstr name,
						boost::function< bool ( T3, T4 ) > const& predicate
					);
	template < typename T1, typename T2 >
	inline	void	add_predicate				(
						pddl_predicate_id const required_type,
						pcstr name,
						boost::function< bool ( T1, T2 ) > const& predicate
					);
	template < typename T1, typename T2, typename T3, typename T4 >
	inline	void	add_predicate2				(
						pddl_predicate_id const required_type,
						pcstr name,
						bool ( *predicate )( T3, T4 )
					);
	template < typename T1, typename T2 >
	inline	void	add_predicate				(
						pddl_predicate_id const required_type,
						pcstr name,
						bool ( *predicate )( T1, T2 )
					);

	// three parameters predicates
	template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
	inline	void	add_predicate3				(
						pddl_predicate_id const required_type,
						pcstr name,
						boost::function< bool ( T4, T5, T6 ) > const& predicate
					);
	template < typename T1, typename T2, typename T3 >
	inline	void	add_predicate				(
						pddl_predicate_id const required_type,
						pcstr name,
						boost::function< bool ( T1, T2, T3 ) > const& predicate
					);
	template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
	inline	void	add_predicate3				(
						pddl_predicate_id const required_type,
						pcstr name,
						bool ( *predicate )( T4, T5, T6 )
					);
	template < typename T1, typename T2, typename T3 >
	inline	void	add_predicate				(
						pddl_predicate_id const required_type,
						pcstr name,
						bool ( *predicate )( T1, T2, T3 )
					);

	pddl_predicate const*	operator []			( pddl_predicate_id const type ) const;
	
	pddl_predicate const*	get_predicate		( u32 const index ) const;
	inline	u32				get_predicates_count( ) const { return m_predicates.size(); }
	
	template < typename T >
	inline parameter_type	get_registered_type	( ) const;
	
			parameter_type	get_registered_type	( pcstr registered_typename ) const;

			void			add_action			( generalized_action* action_to_be_added );
	inline	u32				get_actions_count	( ) const	{ return m_actions.size(); }
	generalized_action*		pop_action			( );
	generalized_action const* get_action_by_type( u32 const type ) const;
	generalized_action const* get_actions		( ) const { return m_actions.front(); }

private:
	template < typename T >
	inline	void	deduce_parameter_type		( pddl_predicate& predicate ) const;
	
	inline	bool	has_predicate				( pddl_predicate_id const type ) const;
	
private:
	typedef intrusive_list< generalized_action,
							generalized_action*,
							&generalized_action::next,
							threading::single_threading_policy,
							size_policy >		actions_type;

	typedef map< pddl_predicate_id, pddl_predicate* >	predicates_type;
	typedef map< pcstr, parameter_type >				registered_parameter_types;

private:
	registered_parameter_types					m_registered_types;
	predicates_type								m_predicates;
	actions_type								m_actions;
}; // class pddl_domain

} // namespace planning
} // namespace ai
} // namespace xray

#include "pddl_domain_inline.h"

#endif // #ifndef PDDL_DOMAIN_H_INCLUDED