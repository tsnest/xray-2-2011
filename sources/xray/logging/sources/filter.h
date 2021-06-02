////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RULE_H_INCLUDED
#define RULE_H_INCLUDED

#include <xray/intrusive_double_linked_list.h>

namespace xray {
namespace logging {

struct initiator_filter
{
	initiator_filter *						next;
	initiator_filter *						prev;
	memory::base_allocator *				allocator;
	int										verbosity;
	u32										thread_id;
#pragma warning (push)
#pragma warning (disable:4200)
	fixed_string<128>						initiator;
#pragma warning	(pop)
};

typedef intrusive_double_linked_list<initiator_filter, initiator_filter *, & initiator_filter::prev, & initiator_filter::next>	
											filter_stack;


} // namespace logging
} // namespace xray

#endif // #ifndef RULE_H_INCLUDED