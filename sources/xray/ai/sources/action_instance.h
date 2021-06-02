////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_INSTANCE_H_INCLUDED
#define ACTION_INSTANCE_H_INCLUDED

#include <xray/ai/parameter_types.h>
#include "generalized_action.h"
#include "action_parameter.h"
#include "base_filter.h"

namespace xray {
namespace ai {
namespace planning {

class action_instance;
class generalized_action;

typedef void( *initialize_binder_function_ptr )( action_instance const& action, property_values_type const& values );
typedef void( *execute_binder_function_ptr )( action_instance const& action, property_values_type const& values );
typedef void( *finalize_binder_function_ptr )( action_instance const& action, property_values_type const& values );

class action_instance : private boost::noncopyable
{
	friend class pddl_problem;

public:
	typedef boost::function< void () >	function_storage_type;

public:	
								action_instance		( generalized_action const* const prototype );

	inline	action_type			get_type			( ) const	{ return m_prototype->get_type(); }
	inline	u32					get_parameters_count( ) const	{ return m_parameters.size(); }
	
	inline	void				add_parameter		( action_parameter* parameter );
	inline	parameter_type		get_parameter_type	( u32 const index ) const;
	inline	action_parameter*	get_parameter		( u32 const index ) const;
			action_parameter*	pop_parameter		( );

			void				add_filters_list	( parameters_filters_type* parameters_filters );
	parameters_filters_type*	pop_filter_list		( );
			bool			are_parameters_suitable	( property_values_type const& objects )  const;
	
	inline	function_storage_type const& initialize_storage	( ) const	{ return m_initialize_storage; }
	inline	function_storage_type const& execute_storage	( ) const	{ return m_execute_storage; }
	inline	function_storage_type const& finalize_storage	( ) const	{ return m_finalize_storage; }

			void		initialize		( property_values_type const& values ) const;
			void		execute			( property_values_type const& values ) const;
			void		finalize		( property_values_type const& values ) const;

	static	bool	is_object_suitable	( parameter_filters_type& filters, object_instance_type const& object );

public:
	typedef fixed_vector< action_parameter*, 4 >			parameters_type;
	typedef intrusive_list_item< parameters_filters_type* >	filter_set_type;
	typedef intrusive_list< filter_set_type,
							filter_set_type*,
							&filter_set_type::next
						  >									filters_set_type;
	
private:
	parameters_type						m_parameters;
	filters_set_type					m_filters_set;
	function_storage_type				m_initialize_storage;	
	function_storage_type				m_execute_storage;		
	function_storage_type				m_finalize_storage;	// NEVER call directly these 3 function_storages!
	initialize_binder_function_ptr		m_initialize_binder;
	execute_binder_function_ptr			m_execute_binder;
	finalize_binder_function_ptr		m_finalize_binder;
	generalized_action const*			m_prototype;
}; // class action_instance

} // namespace planning
} // namespace ai
} // namespace xray

#include "action_instance_inline.h"

#endif // #ifndef ACTION_INSTANCE_H_INCLUDED