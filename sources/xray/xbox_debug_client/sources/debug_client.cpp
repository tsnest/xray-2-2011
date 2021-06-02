////////////////////////////////////////////////////////////////////////////
//	Created		: 16.01.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/os_include.h>
#include <xbdm.h>
#include "debug_client.h"

#define NOTIF_HANDLER_PREFIX				"xbox_debug_client"
debug_client* debug_client::m_single_inst	= NULL;

void debug_client::make_error_descr(pcstr const user_descr, HRESULT const h_res)
{
	ASSERT(user_descr);
	error_string_t		tmp_buff;

	sprintf_s(m_error_descr.get_buffer(),
		m_error_descr.max_length(), "%s, errorcode=0x%08x - ",
		user_descr,
		h_res);

	if (DmTranslateErrorA(h_res, tmp_buff.get_buffer(), 
		m_error_descr.max_length() - m_error_descr.length() - 1) == XBDM_NOERR)
	{
		xray::strings::append(m_error_descr.get_buffer(), 
			m_error_descr.max_length(), tmp_buff.c_str());
	}
}

debug_client::debug_client(pcstr const xbox_machine_name, bool dbgout) : 
	m_hello_received	(0),
	m_result_received	(0),
	m_title_crashed		(0),
	m_title_finished	(0),
	m_tests_result		(-1),
	m_failed			(true),
	m_error_descr		("not initialized"),
	m_connection		(NULL),
	m_notif_session		(NULL),
	m_started_title		(false)
{
	if (xbox_machine_name && strlen(xbox_machine_name))
	{
		HRESULT setname_result = DmSetXboxName(xbox_machine_name);
		if (setname_result != XBDM_NOERR)
		{
			make_error_descr("DmSetXboxName failed", setname_result);
			return;
		}
	}
	HRESULT conn_res	= DmOpenConnection(&m_connection);
	if (conn_res != XBDM_NOERR)
	{
		make_error_descr("DmOpenConnection failed", conn_res);
		return;
	}
	conn_res			= DmOpenNotificationSession(DM_DEBUGSESSION, &m_notif_session);
	if (conn_res != XBDM_NOERR)
	{
		make_error_descr("DmOpenNotificationSession failed", conn_res);
		return;
	}
	
	R_ASSERT(m_single_inst == NULL, "unfortunally this object must be single in your application :(");

	m_single_inst		= this;
	conn_res			= DmRegisterNotificationProcessor(m_notif_session,
		NOTIF_HANDLER_PREFIX, 
		&debug_client::notification_processor);
	if (conn_res != XBDM_NOERR)
	{
		make_error_descr("DmRegisterNotificationProcessor failed", conn_res);
		return;
	}

	conn_res			= DmNotify(m_notif_session, DM_EXCEPTION, 
		&debug_client::exceptions_notification);
	if (conn_res != XBDM_NOERR)
	{
		make_error_descr("DmNotify DM_EXCEPTION failed", conn_res);
		return;
	}
	conn_res			= DmNotify(m_notif_session, DM_ASSERT,
		&debug_client::exceptions_notification);
	if (conn_res != XBDM_NOERR)
	{
		make_error_descr("DmNotify DM_ASSERT failed", conn_res);
		return;
	}
	conn_res			= DmNotify(m_notif_session, DM_EXEC,
		&debug_client::exec_notification);
	if (conn_res != XBDM_NOERR)
	{
		make_error_descr("DmNotify DM_EXEC failed", conn_res);
		return;
	}

	if (dbgout)
	{
		conn_res			= DmNotify(m_notif_session, DM_DEBUGSTR,
			&debug_client::debugoutput_notification);
		if (conn_res != XBDM_NOERR)
		{
			make_error_descr("DmNotify DM_DEBUGSTR failed", conn_res);
			return;
		}
	}
	m_failed			= false;
	m_error_descr.clear	();
}

debug_client::~debug_client()
{
	if (m_notif_session)
	{
		DmCloseNotificationSession(m_notif_session);
	}
	if (m_connection)
	{
		DmCloseConnection(m_connection);
	}
}

void debug_client::reboot_console()
{
	ASSERT(m_connection != NULL, "connection to console not initialized");
	DmReboot(DMBOOT_COLD);
}

bool debug_client::run_image(pcstr const image_path,
							 pcstr const work_dir,
							 pcstr const cmd_line)
{
	ASSERT(m_connection != NULL, "connection to console not initialized");
	ASSERT(image_path && strlen(image_path));
	HRESULT run_res = DmRebootEx(DMBOOT_TITLE, image_path, work_dir, cmd_line);
	if (run_res != XBDM_NOERR)
	{
		m_failed = true;
		make_error_descr("DmRebootEx failed", run_res);
		return false;
	}
	return true;
}

bool debug_client::wait_hello(u32 const seconds)
{
	xray::timing::timer	wait_timer;
	wait_timer.start();
	while (m_hello_received == 0)
	{
		if (is_title_crashed())
			return false;

		if (wait_timer.get_elapsed_sec() > seconds)
			return false;
		Sleep(100);
	}
	return true;
}

int debug_client::wait_tests_result(u32 const seconds)
{
	xray::timing::timer	wait_timer;
	wait_timer.start();
	while (!is_title_crashed())
	{
		if (m_result_received)
		{
			return m_tests_result;
		}
		if (wait_timer.get_elapsed_sec() > seconds)
			return -1;

		Sleep(100);
	}
	return -1;
}

DWORD __stdcall debug_client::notification_processor(LPCSTR str_notification)
{
	ASSERT(str_notification);
	printf("\ntitle notification: %s\n", str_notification);
	if (!xray::strings::compare(str_notification, NOTIF_HANDLER_PREFIX"!hello"))
	{
		InterlockedExchange(&m_single_inst->m_hello_received, 1);
		return 0;
	}
	pcstr tmp_str	= NOTIF_HANDLER_PREFIX"!tests_exit_code";
	u32	tmp_strlen	= strlen(tmp_str);
	if (!strncmp(str_notification, tmp_str, tmp_strlen) &&
		strlen(str_notification) > (tmp_strlen + 1))
	{
		tmp_str = str_notification + tmp_strlen + 1;
		m_single_inst->m_tests_result = atoi(tmp_str);
		InterlockedExchange(&m_single_inst->m_result_received, 1);
		return 0;
	}
	return 0;
}

void debug_client::wait_title_finish(u32 const seconds)
{
	m_title_finished_event.wait(seconds * 1000);
}

DWORD __stdcall debug_client::exceptions_notification(ULONG dwNotification, DWORD dwParam)
{
	if (dwNotification == DM_EXCEPTION)
	{
		DMN_EXCEPTION* tmp_exception = reinterpret_cast<DMN_EXCEPTION*>(dwParam);
		if (tmp_exception->Flags & DM_EXCEPT_NONCONTINUABLE)
		{
			InterlockedExchange(&m_single_inst->m_title_crashed, 1);
		}
	} else if (dwNotification == DM_ASSERT)
	{
		InterlockedExchange(&m_single_inst->m_title_crashed, 1);
	}
	return 0;
}

DWORD __stdcall debug_client::exec_notification(ULONG dwNotification, DWORD dwParam)
{
	XRAY_UNREFERENCED_PARAMETERS	( dwNotification, dwParam );

	if ( dwParam == DMN_EXEC_REBOOT_TITLE && !m_single_inst->m_started_title )
	{
		m_single_inst->m_started_title	=	true;
	}
	else if ((dwParam == DMN_EXEC_STOP) || (dwParam == DMN_EXEC_REBOOT_TITLE && m_single_inst->m_started_title))
	{
		InterlockedExchange(&m_single_inst->m_title_finished, 1);
		m_single_inst->m_title_finished_event.set(true);
	}
	return 0;
}

DWORD __stdcall debug_client::debugoutput_notification(ULONG dwNotification, DWORD dwParam)
{
	XRAY_UNREFERENCED_PARAMETERS	( dwNotification, dwParam );
	ASSERT(dwParam);
	DMN_DEBUGSTR* deubg_str = reinterpret_cast<DMN_DEBUGSTR*>(dwParam);
	printf("%s", deubg_str->String);
	return 0;
}