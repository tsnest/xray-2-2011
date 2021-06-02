////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TESTING_INLINE_H_INCLUDED
#define TESTING_INLINE_H_INCLUDED

#include <xray/construction.h>
#include <xray/uninitialized_reference.h>

namespace xray {
namespace testing {

//-----------------------------------------------
// suite_base
//-----------------------------------------------

namespace detail {

#define				XRAY_EXCEPTION_EXECUTE_HANDLER 1

XRAY_CORE_API xray::assert_enum	 set_awaited_exception		(xray::assert_enum);
XRAY_CORE_API void				 check_awaited_exception	(xray::assert_enum);
XRAY_CORE_API void				 set_current_test			(pcstr suite_name, pcstr test_name);
XRAY_CORE_API bool				 run_tests_impl				(test_base* test, pcstr suite_name);
XRAY_CORE_API void				 on_new_suite				();

} // namespace detail 

template <class derived>
inline suite_base<derived>::suite_base ()
{
	detail::on_new_suite();
}

template <class derived>
inline bool   suite_base<derived>::run_tests ()
{
	return	detail::run_tests_impl (singleton()->m_tests.pop_all_and_clear(), typeid(derived).name());
}

template <class derived>
inline void   suite_base<derived>::add_test (test_base* const test)
{
	suite_base* instance	=	singleton();
	instance->m_tests.push_back	(test);
}

template <class derived>
inline derived *   suite_base<derived>::singleton ()
{
	if ( !s_suite )
	{
		bind_pointer_to_buffer_mt_safe	(s_suite, 
										 memory_helper::memory(), 
										 s_suite_creation_flag, 
										 bind_pointer_to_buffer_mt_safe_placement_new_predicate());
	}

	return						s_suite;
}
	
template <class derived>
class suite_base<derived>::memory_helper
{
public:
	typedef char				memory_buffer	[sizeof(derived)];
	static memory_buffer&		memory () { return s_buffer; }
private:
	static  XRAY_DEFAULT_ALIGN	memory_buffer	s_buffer;
};

template <class derived>
derived*								suite_base<derived>::s_suite	=	NULL;

template <class derived>
threading::atomic32_type				suite_base<derived>::s_suite_creation_flag	=	0;

template <class derived>
typename suite_base<derived>::memory_helper::memory_buffer		suite_base<derived>::memory_helper::s_buffer;

//-----------------------------------------------
// test_class_helper
//-----------------------------------------------

template <class test_class, class suite_class>
class test_class_helper : public test_base
{
public:
					test_class_helper	(pcstr test_class_name, pcstr suite_class_name) 
										: m_test_class_name(test_class_name), m_suite_class_name(suite_class_name)
	{
		suite_class::add_test		(this);
	}

	virtual void	test				()
	{ 
		XRAY_CONSTRUCT_REFERENCE	(m_test, test_class); 
		timing::timer				timer;
		timer.start					();
		LOGI_INFO					("test", "test '%s' started", m_test_class_name);
		detail::set_current_test	(m_suite_class_name, m_test_class_name);
		m_test->test				(suite_class::singleton()); 
		u32 const test_length_ms	=	timer.get_elapsed_msec();
		LOGI_INFO					("test", "test '%s' finished (%d ms)", m_test_class_name, test_length_ms);
		XRAY_DESTROY_REFERENCE		(m_test);
	}

private:
	uninitialized_reference<test_class>	m_test;
	pcstr								m_test_class_name;
	pcstr								m_suite_class_name;
};

//-----------------------------------------------
// test_func_helper
//-----------------------------------------------

template <class test_func, class suite_class>
class test_func_helper : public test_base
{
public:
					test_func_helper	(pcstr test_func_name, pcstr suite_class_name) :
										 m_test_func_name(test_func_name), m_suite_class_name(suite_class_name)
	{
		suite_class::add_test		(this);
	}
			
	virtual	void	test				()
	{
		detail::set_current_test		(m_suite_class_name, m_test_func_name);
		suite_class::singleton()->test	( (test_func*)0 );
	}

private:
	pcstr								m_test_func_name;
	pcstr								m_suite_class_name;
};

} // namespace testing
} // namespace xray

#endif // #ifndef TESTING_INLINE_H_INCLUDED