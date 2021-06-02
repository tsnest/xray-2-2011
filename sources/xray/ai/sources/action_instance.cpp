////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "action_instance.h"

namespace xray {
namespace ai {
namespace planning {

action_instance::action_instance	( generalized_action const* const prototype ) :
	m_prototype						( prototype ),
	m_initialize_binder				( 0 ),
	m_execute_binder				( 0 ),
	m_finalize_binder				( 0 )
{
}

void action_instance::add_filters_list	( parameters_filters_type* parameters_filters )
{
	R_ASSERT						( parameters_filters->size() == m_parameters.size() );
	
	filter_set_type* new_item		= NEW( filter_set_type )( parameters_filters );
	m_filters_set.push_back			( new_item );
}

void action_instance::initialize	( property_values_type const& values ) const
{
	if ( m_initialize_binder )
		( *m_initialize_binder )	( *this, values );
}

void action_instance::execute		( property_values_type const& values ) const
{
	if ( m_execute_binder )
		( *m_execute_binder )		( *this, values );
}

void action_instance::finalize		( property_values_type const& values ) const
{
	if ( m_finalize_binder )
		( *m_finalize_binder )		( *this, values );
}

action_parameter* action_instance::pop_parameter( )
{
	if ( m_parameters.empty() )
		return									0;

	parameters_type::iterator it_begin			= m_parameters.begin();
	action_parameter* result					= *it_begin;
	m_parameters.erase							( it_begin );
	return										result;
}

bool action_instance::is_object_suitable	( parameter_filters_type& filters, object_instance_type const& object )
{
	for ( intrusive_filters_list* it_filter = filters.front(); it_filter; it_filter = filters.get_next_of_object( it_filter ) )
		if ( it_filter->list->is_object_available( object ) )
			return							true;

	return									filters.empty();
}

struct parameters_suitability_checker_predicate : private boost::noncopyable
{
	inline parameters_suitability_checker_predicate	( property_values_type const& objects ) :
		instances									( objects )
	{
	}

	bool operator ()						( action_instance::filter_set_type* current_filters_set ) const
	{
		R_ASSERT							( current_filters_set->list->size() == instances.size() );
		u32 index							= 0;
		for ( parameter_filters_item_type* it_filter = current_filters_set->list->front(); it_filter; it_filter = current_filters_set->list->get_next_of_object( it_filter ), ++index )
		{
			if ( !action_instance::is_object_suitable( *( it_filter->list ), instances[index] ) )
				return						false;
		}
		return true;
	}

	property_values_type const&				instances;
};

bool action_instance::are_parameters_suitable	( property_values_type const& objects )  const
{
	if ( m_filters_set.empty() )
		return									true;

	parameters_suitability_checker_predicate const suitability_checker( objects );
	return m_filters_set.find_if				( suitability_checker ) != 0;
}

parameters_filters_type* action_instance::pop_filter_list( )
{
	if ( m_filters_set.empty() )
		return									0;

	filter_set_type* filter_set					= m_filters_set.pop_front();
	parameters_filters_type* result				= filter_set->list;
	DELETE										( filter_set );
	return										result;
}

} // namespace planning
} // namespace ai
} // namespace xray