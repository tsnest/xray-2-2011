////////////////////////////////////////////////////////////////////////////
//	Created		: 15.12.2009
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ASYNC_HANDLER_HOST_H_INCLUDED
#define ASYNC_HANDLER_HOST_H_INCLUDED

#include <list>
#include "async_callback_proxies.h"

namespace xray {
namespace network {

template<
	template<typename T> class Allocator,
	typename SyncLocker>
class async_handler_host
{
public:
	

				async_handler_host		()
				{
				}
				~async_handler_host		()
				{
				}

	callback_proxy0::functor										wrap(
		void (*callback)())
	{
		typedef callback_proxy0 callback_proxy_t;
		callback_proxy_t::functor result;
		result.m_cb_proxy_ptr = add_callback<
			callback_proxy_t, callback_proxy_t::function_t>(callback);
		return result;
	}

	template<typename TClass>
	typename callback_proxy_mem_fun_0<TClass>::functor				wrap(
		void (TClass::*callback)(), TClass* tobind_this_ptr)
	{
		typedef callback_proxy_mem_fun_0<TClass> callback_proxy_t;
		callback_proxy_t::functor result;
		result.m_cb_proxy_ptr = add_callback<
			callback_proxy_t, callback_proxy_t::function_t, TClass>(callback, tobind_this_ptr);
		return result;
	}
	
	template<typename Arg1>
	typename callback_proxy1<Arg1>::functor							wrap(
		void (*callback)(Arg1))
	{
		typedef callback_proxy1<Arg1> callback_proxy_t;
		callback_proxy_t::functor result;
		result.m_cb_proxy_ptr = add_callback<
			callback_proxy_t, callback_proxy_t::function_t>(callback);
		return result;
	}

	template<typename TClass, typename Arg1>
	typename callback_proxy_mem_fun_1<TClass, Arg1>::functor		wrap(
		void (TClass::*callback)(Arg1), TClass* tobind_this_ptr)
	{
		typedef callback_proxy_mem_fun_1<TClass, Arg1> callback_proxy_t;
		callback_proxy_t::functor result;
		result.m_cb_proxy_ptr = add_callback<
			callback_proxy_t, callback_proxy_t::function_t, TClass>(
				callback, tobind_this_ptr);
		return result;
	}

	template<typename Arg1, typename Arg2>
	typename callback_proxy2<Arg1, Arg2>::functor					wrap(
		void (*callback)(Arg1, Arg2))
	{
		typedef callback_proxy2<Arg1, Arg2> callback_proxy_t;
		callback_proxy_t::functor result;
		result.m_cb_proxy_ptr = add_callback<
			callback_proxy_t, callback_proxy_t::function_t>(callback);
		return result;
	}

	template<typename TClass, typename Arg1, typename Arg2>
	typename callback_proxy_mem_fun_2<TClass, Arg1, Arg2>::functor		wrap(
		void (TClass::*callback)(Arg1, Arg2), TClass* tobind_this_ptr)
	{
		typedef callback_proxy_mem_fun_2<TClass, Arg1, Arg2> callback_proxy_t;
		callback_proxy_t::functor result;
		result.m_cb_proxy_ptr = add_callback<
			callback_proxy_t, callback_proxy_t::function_t, TClass>(
				callback, tobind_this_ptr);
		return result;
	}

	// invokes ready callbacks in the context of current thread (host)
	void		invoke_ready_callbacks	()
	{
		m_sync_for_list.lock	();
		m_ready_list.remove_if	(wrapped_invoker());
		m_sync_for_list.unlock	();
	}
private:
	struct wrapped_cb_t 
	{
		typedef void (*origin_invoker_func_t)(callback_proxy_base*);
		wrapped_cb_t()
		{
			m_ready = false;
		}
		// default copy constructor is valid

#if XRAY_PLATFORM_32_BIT
		static u32 const		cb_proxy_max_size = 32;
#else // #if XRAY_PLATFORM_32_BIT
		static u32 const		cb_proxy_max_size = 48;
#endif // #if XRAY_PLATFORM_32_BIT
		char					m_cb_proxy_store[cb_proxy_max_size];
		origin_invoker_func_t	m_origin_invoker;
		bool volatile			m_ready;
	};
	
	typedef std::list<wrapped_cb_t, Allocator<wrapped_cb_t> >	ready_list_t;
	
	struct wrapped_invoker 
	{
		bool operator()(typename ready_list_t::value_type const & left) const
		{
			if (left.m_ready)
			{
				// left parameter isn't const really.
				// but in release configuration of stlport
				// std::list::remove_if receives const reference of element
				// but method std::list::remove_if is not const !
				// so sorry for const_cast :(
				callback_proxy_base* cbproxy = 
					const_cast<callback_proxy_base*>(
						pointer_cast<callback_proxy_base const*>(left.m_cb_proxy_store));
				left.m_origin_invoker(cbproxy);
				return true;
			}
			return false;
		}
	};

	template<typename CallbackProxy, typename CallbackFunction>
	CallbackProxy*	add_callback	(CallbackFunction callback)
	{
		wrapped_cb_t & wcb			= push_wrapped_cb();

		COMPILE_ASSERT(sizeof(CallbackProxy) <= wrapped_cb_t::cb_proxy_max_size,
			NotEnoughSpaceForCallbackProxy);

		CallbackProxy* cb_proxy		= new (wcb.m_cb_proxy_store) 
			CallbackProxy(callback, wcb.m_ready);
		wcb.m_origin_invoker = &CallbackProxy::invoke_original;
		return cb_proxy;
	}
	
	template<typename CallbackProxy, typename CallbackFunction, typename TClass>
	CallbackProxy*	add_callback	(CallbackFunction callback, TClass* tobind_this_ptr)
	{
		wrapped_cb_t & wcb			= push_wrapped_cb();

		COMPILE_ASSERT(sizeof(CallbackProxy) <= wrapped_cb_t::cb_proxy_max_size,
			NotEnoughSpaceForCallbackProxy);
		ASSERT(sizeof(CallbackProxy) <= wrapped_cb_t::cb_proxy_max_size);

		CallbackProxy* cb_proxy		= new (wcb.m_cb_proxy_store) 
			CallbackProxy(callback, wcb.m_ready, tobind_this_ptr);
		wcb.m_origin_invoker = &CallbackProxy::invoke_original;
		return cb_proxy;
	}

	wrapped_cb_t &	push_wrapped_cb	()
	{
		m_sync_for_list.lock		();
		m_ready_list.push_back		(wrapped_cb_t());
		wrapped_cb_t & wcb			= m_ready_list.back();
		m_sync_for_list.unlock		();
		return wcb;
	}


	ready_list_t	m_ready_list;
	SyncLocker		m_sync_for_list;
}; // class async_handler_host


} // namespace network
} // namespace xray

#endif // #ifndef ASYNC_HANDLER_HOST_H_INCLUDED