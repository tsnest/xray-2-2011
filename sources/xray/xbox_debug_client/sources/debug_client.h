////////////////////////////////////////////////////////////////////////////
//	Created		: 16.01.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_CLIENT_H_INCLUDED
#define DEBUG_CLIENT_H_INCLUDED

#include <xray/os_include.h>
#include <xbdm.h>

class debug_client
{
public:
	explicit	debug_client		(pcstr const xbox_machine_name, bool dbgout = true);
				~debug_client		();

	pcstr		error_descr			() const { return m_error_descr.c_str(); };
	bool		failed				() { return m_failed; };
	operator	bool				() { return !failed(); };
	bool		operator!			() { return failed(); };

	void		reboot_console		();
	bool		run_image			(pcstr const image_path, pcstr const work_dir, pcstr const cmd_line);
	bool		wait_hello			(u32 const seconds);
	int			wait_tests_result	(u32 const seconds);
	void		wait_title_finish	(u32 const seconds);
	bool		is_title_crashed	() const { return m_title_crashed != 0; };
	bool		is_title_finished	() const { return m_title_finished != 0; };
private:
	void		make_error_descr	(pcstr const user_descr, HRESULT const h_res = 0);
	
	typedef		xray::fixed_string512	error_string_t;
	typedef		xray::fixed_string4096	string_store_t;

	xray::threading::event	m_title_finished_event;
	volatile long			m_hello_received;
	volatile long			m_result_received;
	volatile long			m_title_crashed;
	volatile long			m_title_finished;
	int						m_tests_result;
	bool					m_failed;
	bool					m_started_title;

	error_string_t			m_error_descr;
	PDM_CONNECTION			m_connection;
	PDMN_SESSION			m_notif_session;
	string_store_t			m_string_dst;

	static debug_client*	m_single_inst; // for notification processors (they can't receive user data :(
	static	DWORD __stdcall notification_processor	(LPCSTR str_notification);
	static	DWORD __stdcall exceptions_notification	(ULONG dwNotification, DWORD dwParam);
	static	DWORD __stdcall debugoutput_notification(ULONG dwNotification, DWORD dwParam);
	static	DWORD __stdcall exec_notification		(ULONG dwNotification, DWORD dwParam);
}; // class debug_client

#endif // #ifndef DEBUG_CLIENT_H_INCLUDED