////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef THREADING_FUNCTIONS_H_INCLUDED
#define THREADING_FUNCTIONS_H_INCLUDED

namespace xray {
namespace threading {

struct thread_entry_params {
	thread_function_type		function_to_call;
	volatile pcstr				thread_name_for_logging;
	volatile pcstr				thread_name_for_debugger;
	u32							hardware_thread;
	threading::tasks_awareness	tasks_awareness;
	threading::atomic32_type	processed;
};

} // namespace threading
} // namespace xray

#endif // #ifndef THREADING_FUNCTIONS_H_INCLUDED