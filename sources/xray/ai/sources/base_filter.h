////////////////////////////////////////////////////////////////////////////
//	Created		: 24.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_FILTER_H_INCLUDED
#define BASE_FILTER_H_INCLUDED

#include <xray/ai/parameter_types.h>

namespace xray {
namespace ai {
namespace planning {

template < typename IntrusiveListType >
struct intrusive_list_item
{
	inline	intrusive_list_item					( IntrusiveListType new_list ) :
		list									( new_list )
	{
	}
	
	IntrusiveListType							list;
	intrusive_list_item< IntrusiveListType >*	next;
};

class base_filter;
typedef intrusive_list_item< base_filter* >		intrusive_filters_list;

class base_filter : private boost::noncopyable
{
public:
					base_filter					( bool need_to_be_inverted );
	
			void	add_subfilter				( base_filter* filter );
	base_filter*	pop_subfilter				( );
	
			bool	is_object_available			( object_instance_type const& object ) const;

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR		( base_filter );

public:
	base_filter*	m_next;
	
protected:
	typedef intrusive_list< intrusive_filters_list,
							intrusive_filters_list*,
							&intrusive_filters_list::next,
							threading::single_threading_policy,
							size_policy >		subfilters_type;

protected:
	subfilters_type								m_subfilters;

private:
	virtual	bool	is_passing_filter			( object_instance_type const& object ) const = 0;

private:
	bool										m_is_inverted;
}; // class base_filter

typedef intrusive_list< intrusive_filters_list,
						intrusive_filters_list*,
						&intrusive_filters_list::next,
						threading::single_threading_policy,
						size_policy >					parameter_filters_type;

typedef intrusive_list_item< parameter_filters_type* >	parameter_filters_item_type;
typedef intrusive_list< parameter_filters_item_type,
						parameter_filters_item_type*,
						&parameter_filters_item_type::next,
						threading::single_threading_policy,
						size_policy
					  >									parameters_filters_type;

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef BASE_FILTER_H_INCLUDED