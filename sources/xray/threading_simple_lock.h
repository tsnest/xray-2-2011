////////////////////////////////////////////////////////////////////////////
//	Created		: 10.10.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_THREADING_SIMPLE_LOCK_H_INCLUDED
#define XRAY_THREADING_SIMPLE_LOCK_H_INCLUDED

#include <xray/threading_policies.h>

namespace xray {
namespace threading {

class XRAY_CORE_API simple_lock : private core::noncopyable {
public:
	struct mutex_raii : core::noncopyable
	{
		mutex_raii	(simple_lock const & lock) : lock(lock) { lock.lock(); locked = true; }
		void	clear	() { if ( locked ) { lock.unlock(); locked = false; } }
		~mutex_raii	() { clear(); }
	private:
		simple_lock const &	lock;
		bool				locked;
	};
public:
	simple_lock	() : m_lock(0), m_thread_id(0) {}

	bool	try_lock	() const 
	{ 
		if ( interlocked_compare_exchange(m_thread_id, threading::current_thread_id(), 0) == 0 )
		{
			ASSERT		(!m_lock);
			m_lock	=	1;
			return		true;
		}

		return			false;
	}

	void	lock		() const 
	{ 
		if ( m_thread_id == (s32)threading::current_thread_id() )
		{
			++m_lock;
			return;
		}

		while ( interlocked_compare_exchange(m_thread_id, threading::current_thread_id(), 0) != 0 ) { ; } 
		ASSERT		(!m_lock);
		m_lock	=	1;
	}

	void	unlock		() const 
	{ 
		ASSERT		(m_thread_id == (s32)threading::current_thread_id());
		ASSERT		(m_lock);
		--m_lock;
		if ( !m_lock )
			interlocked_exchange	(m_thread_id , 0); 
	}

private:
	mutable u32				m_lock;
	mutable atomic32_type	m_thread_id;

}; // class simple_lock

typedef simple_lock::mutex_raii	simple_lock_raii;

} // namespace threading
} // namespace xray

#endif // #ifndef XRAY_THREADING_SIMPLE_LOCK_H_INCLUDED