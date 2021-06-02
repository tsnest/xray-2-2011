////////////////////////////////////////////////////////////////////////////
//	Created		: 16.12.2009
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ASYNC_CALLBACK_PROXIES_H_INCLUDED
#define ASYNC_CALLBACK_PROXIES_H_INCLUDED

namespace xray {
namespace network {

struct callback_proxy_base
{
	callback_proxy_base(bool volatile & ready_flag) : 
		m_callback_ready(ready_flag)
	{
	}
	callback_proxy_base & operator=(callback_proxy_base const &)
	{
		UNREACHABLE_CODE();
	};
	bool volatile & m_callback_ready;
};


struct callback_proxy0 : public callback_proxy_base
{
	typedef void (*function_t)();

	callback_proxy0	(function_t orig_funct, bool volatile & ready_flag) :
		callback_proxy_base(ready_flag),
		m_func(orig_funct)
	{
	}
		
	~callback_proxy0()
	{
	}

	static void		invoke_original	(callback_proxy_base* this_ptr)
	{
		callback_proxy0* me		= static_cast_checked<callback_proxy0*>(this_ptr);
		me->m_func();
	}

	void			invoke_proxy	()
	{
		m_callback_ready		= true;
	}
	function_t	m_func;

	struct functor
	{
		callback_proxy0*	m_cb_proxy_ptr;
		void				operator()()
		{
			m_cb_proxy_ptr->invoke_proxy();
		}
		template<typename DesiredType>
		DesiredType cast()
		{
			DesiredType result(
				m_cb_proxy_ptr,
				&callback_proxy0::invoke_proxy);
			return result;
		}
	}; // struct functor
}; // struct callback_proxy0


template<typename T1>
struct callback_proxy1 : public callback_proxy_base
{
	typedef void (*function_t)(T1);

	callback_proxy1	(function_t orig_funct, bool volatile & ready_flag) :
		callback_proxy_base(ready_flag),
		m_func(orig_funct)
	{
	}
		
	~callback_proxy1()
	{
	}

	static void		invoke_original	(callback_proxy_base* this_ptr)
	{
		callback_proxy1* me		= static_cast_checked<callback_proxy1*>(this_ptr);
		me->m_func(me->m_arg1);
	}
	void			invoke_proxy	(T1 arg1)
	{
		m_arg1				= arg1;
		m_callback_ready	= true;
	}
	function_t	m_func;
	T1			m_arg1;

	struct functor
	{
		callback_proxy1*	m_cb_proxy_ptr;
		void				operator()(T1 arg1)
		{
			m_cb_proxy_ptr->invoke_proxy(arg1);
		}
		template<typename DesiredType>
		DesiredType cast()
		{
			DesiredType result(
				m_cb_proxy_ptr,
				&callback_proxy1::invoke_proxy);
			return result;
		}
	}; // struct functor
}; // struct callback_proxy1

template<typename T1, typename T2>
struct callback_proxy2 : public callback_proxy_base
{
	typedef void (*function_t)(T1, T2);

	callback_proxy2(function_t orig_funct, bool volatile & ready_flag) :
		callback_proxy_base(ready_flag),
		m_func(orig_funct)
	{
	}
		
	~callback_proxy2()
	{
	}

	static void		invoke_original	(callback_proxy_base* this_ptr)
	{
		callback_proxy2* me		= static_cast_checked<callback_proxy2*>(this_ptr);
		me->m_func(me->m_arg1, me->m_arg2);
	}
	void			invoke_proxy	(T1 arg1, T2 arg2)
	{
		m_arg1				= arg1;
		m_arg2				= arg2;
		m_callback_ready	= true;
	}
	function_t	m_func;
	T1			m_arg1;
	T2			m_arg2;

	struct functor
	{
		callback_proxy2*	m_cb_proxy_ptr;
		void				operator()(T1 arg1, T2 arg2)
		{
			m_cb_proxy_ptr->invoke_proxy(arg1, arg2);
		}
		template<typename DesiredType>
		DesiredType cast()
		{
			DesiredType result(
				m_cb_proxy_ptr,
				&callback_proxy2::invoke_proxy);
			return result;
		}
	}; // struct functor
}; // struct callback_proxy2

template<typename TClass>
struct callback_proxy_mem_fun_0 : public callback_proxy_base
{
	typedef void (TClass::*function_t)();

	callback_proxy_mem_fun_0	(function_t orig_funct,
								 bool volatile & ready_flag,
								 TClass* tobind_this_ptr) :
		callback_proxy_base(ready_flag),
		m_mem_func(orig_funct),
		m_this_ptr(tobind_this_ptr)
	{
	}
		
	~callback_proxy_mem_fun_0	()
	{
	}

	static void		invoke_original	(callback_proxy_base* this_ptr)
	{
		callback_proxy_mem_fun_0* me	= static_cast_checked<
			callback_proxy_mem_fun_0*>(this_ptr);
		(me->m_this_ptr->*me->m_mem_func)();
	}

	void			invoke_proxy	()
	{
		m_callback_ready = true;
	}

	function_t	m_mem_func;
	TClass*		m_this_ptr;

	struct functor
	{
		callback_proxy_mem_fun_0*	m_cb_proxy_ptr;
		void						operator()()
		{
			m_cb_proxy_ptr->invoke_proxy();
		}
		template<typename DesiredType>
		DesiredType cast()
		{
			DesiredType result(
				m_cb_proxy_ptr,
				&callback_proxy_mem_fun_0::invoke_proxy);
			return result;
		}
	}; // struct functor
}; // struct callback_proxy_mem_fun_0

template<typename TClass, typename TArg1>
struct callback_proxy_mem_fun_1 : public callback_proxy_base
{
	typedef void (TClass::*function_t)(TArg1);

	callback_proxy_mem_fun_1	(function_t orig_funct,
								 bool volatile & ready_flag,
								 TClass* tobind_this_ptr) :
		callback_proxy_base(ready_flag),
		m_mem_func(orig_funct),
		m_this_ptr(tobind_this_ptr)
	{
	}
		
	~callback_proxy_mem_fun_1	()
	{
	}

	static void		invoke_original	(callback_proxy_base* this_ptr)
	{
		callback_proxy_mem_fun_1* me		= static_cast_checked<
			callback_proxy_mem_fun_1*>(this_ptr);
		(me->m_this_ptr->*me->m_mem_func)	(me->m_arg1);
	}

	void			invoke_proxy	(TArg1 arg1)
	{
		m_arg1				= arg1;
		m_callback_ready	= true;
	}

	function_t	m_mem_func;
	TClass*		m_this_ptr;
	TArg1		m_arg1;

	struct functor
	{
		callback_proxy_mem_fun_1*	m_cb_proxy_ptr;
		void						operator()(TArg1 arg1)
		{
			m_cb_proxy_ptr->invoke_proxy(arg1);
		}
		template<typename DesiredType>
		DesiredType cast()
		{
			DesiredType result(
				m_cb_proxy_ptr,
				&callback_proxy_mem_fun_1::invoke_proxy);
			return result;
		}
	}; // struct functor
}; // struct callback_proxy_mem_fun_1

template<typename TClass, typename TArg1, typename TArg2>
struct callback_proxy_mem_fun_2 : public callback_proxy_base
{
	typedef void (TClass::*function_t)(TArg1, TArg2);

	callback_proxy_mem_fun_2	(function_t orig_funct,
								 bool volatile & ready_flag,
								 TClass* tobind_this_ptr) :
		callback_proxy_base(ready_flag),
		m_mem_func(orig_funct),
		m_this_ptr(tobind_this_ptr)
	{
	}
		
	~callback_proxy_mem_fun_2	()
	{
	}

	static void		invoke_original	(callback_proxy_base* this_ptr)
	{
		callback_proxy_mem_fun_2* me		= static_cast_checked<
			callback_proxy_mem_fun_2*>(this_ptr);
		(me->m_this_ptr->*me->m_mem_func)	(me->m_arg1, me->m_arg2);
	}

	void			invoke_proxy	(TArg1 arg1, TArg2 arg2)
	{
		m_arg1				= arg1;
		m_arg2				= arg2;
		m_callback_ready	= true;
	}

	function_t	m_mem_func;
	TClass*		m_this_ptr;
	TArg1		m_arg1;
	TArg2		m_arg2;

	struct functor
	{
		callback_proxy_mem_fun_2*	m_cb_proxy_ptr;
		void						operator()(TArg1 arg1, TArg2 arg2)
		{
			m_cb_proxy_ptr->invoke_proxy(arg1, arg2);
		}
		template<typename DesiredType>
		DesiredType cast()
		{
			DesiredType result(
				m_cb_proxy_ptr,
				&callback_proxy_mem_fun_2::invoke_proxy);
			return result;
		}
	}; // struct functor
}; // struct callback_proxy_mem_fun_1

} // namespace network
} // namespace xray

#endif // #ifndef ASYNC_CALLBACK_PROXIES_H_INCLUDED