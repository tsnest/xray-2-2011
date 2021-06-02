////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_SATISFACTION_TREES_H_INCLUDED
#define GAME_RESMAN_SATISFACTION_TREES_H_INCLUDED

#include <xray/resources.h>
#include <boost/intrusive/set.hpp>

namespace xray {
namespace resources {

class compare_by_target_satisfaction	{
public: bool	operator () (resource_base const & left, resource_base const & right) const;
};

typedef boost::intrusive::set_member_hook< boost::intrusive::link_mode<boost::intrusive::auto_unlink> >		satisfaction_tree_member_hook;

typedef boost::intrusive::member_hook
<
	resource_base, 
	satisfaction_tree_member_hook,
	& resource_base::grm_satisfaction_tree_hook 
> satisfaction_tree_helper_option;

typedef	boost::intrusive::multiset< resource_base, 
							  	 	satisfaction_tree_helper_option, 
							  	 	boost::intrusive::compare< compare_by_target_satisfaction >,
							  	 	boost::intrusive::constant_time_size<false> >	target_satisfaction_tree_type;

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_SATISFACTION_TREES_H_INCLUDED