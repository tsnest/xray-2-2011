////////////////////////////////////////////////////////////////////////////
//	Created		: 23.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEBUG_LOG_CALLBACK_H_INCLUDED
#define XRAY_DEBUG_LOG_CALLBACK_H_INCLUDED

#include <xray/debug/api.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace xray {
namespace debug {

typedef		boost::function< void (	pcstr	initiator, 
									bool	is_error_verbosity, 
									bool	log_only_user_string, 
									pcstr	message )	>		log_callback;

XRAY_DEBUG_API	log_callback	get_log_callback		( );
XRAY_DEBUG_API	log_callback	set_log_callback		( log_callback const & callback );
XRAY_DEBUG_API	void			disable_log_callback	( );
XRAY_DEBUG_API	void			enable_log_callback		( );


} // namespace debug
} // namespace xray

#endif // #ifndef XRAY_DEBUG_LOG_CALLBACK_H_INCLUDED