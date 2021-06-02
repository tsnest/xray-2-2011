////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_PREDICATE_H_INCLUDED
#define PDDL_PREDICATE_H_INCLUDED

#include <xray/ai/parameter_types.h>

namespace xray {
namespace ai {
namespace planning {

class pddl_predicate;
	
typedef u32 predicate_type;
typedef bool( *predicate_binder_function_ptr )( pddl_predicate const& predicate, property_values_type const& values );

class pddl_predicate
{
	friend class pddl_domain;

public:
	typedef boost::function< bool () >					function_storage_type;

public:
	inline							pddl_predicate		( predicate_type required_type, pcstr name );

	inline	predicate_type			get_type			( ) const	{ return m_type; }
	inline	u32						get_parameters_count( ) const	{ return m_parameters.size(); }
	inline	parameter_type			get_parameter		( u32 const index ) const;
	inline	pcstr					get_caption			( ) const	{ return m_caption; }
	inline function_storage_type const& function_storage( ) const	{ return m_function_storage; }
	inline	void					add_parameter		( parameter_type type );
	inline	bool					evaluate_value		( property_values_type const& values ) const;
	inline	bool					has_owner_parameter	( ) const;

public:
	typedef fixed_vector< parameter_type, 4 >			parameters_type;

private:
	predicate_type 					m_type;
	parameters_type					m_parameters;
	pcstr							m_caption;
	function_storage_type			m_function_storage;	// do not call it directly!
	predicate_binder_function_ptr	m_predicate_binder;
}; // class pddl_predicate

} // namespace planning
} // namespace ai
} // namespace xray

#include "pddl_predicate_inline.h"

#endif // #ifndef PDDL_PREDICATE_H_INCLUDED