////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_DEBUG_CHECK_INVARIANTS_INLINE
#define XRAY_DEBUG_CHECK_INVARIANTS_INLINE

namespace xray {

inline
no_multithread::no_multithread () : m_current_user_thread_id(threading::atomic32_value_type(-1)), m_current_num_uses(0)
{
}

inline
void   no_multithread::on_function_start () const
{
#ifndef MASTER_GOLD
	threading::atomic32_type const current_thread_id	=	threading::current_thread_id();
	threading::atomic32_type const previous_thread_id	=	threading::interlocked_exchange(m_current_user_thread_id, current_thread_id);
	
	if ( previous_thread_id != threading::atomic32_value_type(-1) )
		R_ASSERT_CMP_T					(previous_thread_id, ==, current_thread_id, xray::assert_untyped, "MULTITHREATING! more then one thread use object in parallel");
#endif // #ifndef MASTER_GOLD
		
	threading::interlocked_increment	(m_current_num_uses);
}

inline
void   no_multithread::on_function_end () const
{
	if ( !threading::interlocked_decrement(m_current_num_uses) )
		threading::interlocked_exchange	(m_current_user_thread_id, 0);
}

namespace debug {
namespace detail {

inline
bool   check_multithreaded_safety ()
{
	return		!g_dont_check_multithreaded_safety;
}

class check_no_multithread_raii
{
public:
	check_no_multithread_raii (no_multithread const * const object)
	{
		if ( check_multithreaded_safety() )
		{
			m_object				=	object;
			m_object->on_function_start ();
		}
	}

	~check_no_multithread_raii ()
	{
		if ( check_multithreaded_safety() )
		{
			if ( m_object ) 
				m_object->on_function_end ();
		}
	}

private:
	no_multithread const *	m_object;
};

} // namespace detail
} // namespace debug
} // namespace xray



#endif // #ifndef DEBUG_CHECK_INVARIANTS