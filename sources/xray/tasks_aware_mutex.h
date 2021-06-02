////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TASKS_AWARE_MUTEX_H_INCLUDED
#define TASKS_AWARE_MUTEX_H_INCLUDED

namespace xray {
namespace tasks {

class smart_mutex : private threading::mutex
{
public:
	void	lock	();
	void	unlock	();
	bool	try_lock() { return mutex::try_lock(); }
};

} // namespace tasks
} // namespace xray


#endif // #ifndef TASKS_AWARE_MUTEX_H_INCLUDED