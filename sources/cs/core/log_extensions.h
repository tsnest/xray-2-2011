////////////////////////////////////////////////////////////////////////////
//	Module 		: log_extensions.h
//	Created 	: 27.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : log extensions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_LOG_EXTENSIONS_H_INCLUDED
#define CS_CORE_LOG_EXTENSIONS_H_INCLUDED

#include <fastdelegate/fastdelegate.h>
#include <cs/core/logging.h>

namespace logging {
	typedef fastdelegate::FastDelegate<void (cs::message_initiator const& , cs::core::message_type const& , pcstr)>	callback_type;

				void				initialize		();
				void				uninitialize	();

	CS_CORE_API	void				callback		(callback_type const& callback);
	CS_CORE_API	callback_type const& callback		();
	CS_CORE_API	void				log				(
										cs::message_initiator const& initiator,
										cs::core::message_type const& message_type,
										pcstr message
									);
	CS_CORE_API	void				flush			();
} // logging

	CS_CORE_API	int					vsmsg			(
										cs::message_initiator const& initiator,
										cs::core::message_type const& message_type,
										pcstr format,
										va_list mark
									);
	CS_CORE_API	int		__cdecl		msg				(
										cs::message_initiator const& initiator,
										cs::core::message_type const& message_type,
										pcstr format,
										...
									);

#endif // #ifndef CS_CORE_LOG_EXTENSIONS_H_INCLUDED