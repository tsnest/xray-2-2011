////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/debug.h>
#include "debug_platform.h"
#include <sys/dbg.h>
#include <cell/sysmodule.h>

#pragma comment( lib, "sysmodule_stub" )
#pragma comment( lib, "fs_stub" )

static void unhandled_exception_handler	(
		u64 const exception_type,
		sys_ppu_thread_t const thread_id,
		u64 const data_address_register_content
	)
{
#pragma message( XRAY_TODO("PS3: unhandled exception handler") )
	//SYSTEMTIME					date_time;
	//GetLocalTime				(&date_time);

	//string_path					temp;

	//xray::core::debug::platform::prologue_dump_call_stack	( exception_information );
	//xray::core::debug::detail::generate_file_name			( temp, date_time, xray::core::application_name(), "error_report", ".log");

	//string_path					log_file_name;
	//xray::strings::copy			( log_file_name, temp );

	//string_path					portable_log_file_name;
	//xray::strings::copy			( portable_log_file_name, log_file_name );
	//xray::logging::flush		( xray::fs::convert_to_portable_in_place(portable_log_file_name) );

	//xray::core::debug::bugtrap::add_file		( log_file_name );
	//xray::core::debug::platform::save_minidump	( date_time, exception_information );
}

void xray::debug::protected_call	( protected_function_type* function_to_call, pvoid argument )
{
	static bool first_time	= true;
	if ( first_time ) {
		first_time			= false;
		cellSysmoduleInitialize		( );
		cellSysmoduleLoadModule		( CELL_SYSMODULE_LV2DBG );
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_NET					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_HTTP					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_HTTP_UTIL				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SSL					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_HTTPS					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_VDEC					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_DMUX					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_VPOST					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_RTC					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SPURS					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_OVIS					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SHEAP					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYNC					);
		cellSysmoduleLoadModule		( CELL_SYSMODULE_FS						);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_JPGDEC					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_GCM_SYS				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_GCM					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_AUDIO					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_PAMF					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ATRAC3PLUS				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_NETCTL					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_NP				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_IO						);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_PNGDEC					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_FONT					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_FONTFT					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_FREETYPE				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_USBD					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SAIL					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_L10N					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_RESC					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_DAISY					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_KEY2CHAR				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_MIC          			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_CAMERA       			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_VDEC_MPEG2				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_VDEC_AVC				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC_LPCM				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC_AC3				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC_ATX				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC_AT3				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_DMUX_PAMF				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_VDEC_AL				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC_AL				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_DMUX_AL				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_LV2DBG					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_USBPSPCM				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_AVCONF_EXT				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_USERINFO		);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_SAVEDATA		);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SUBDISPLAY				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_REC			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_VIDEO_EXPORT			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_GAME_EXEC		);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_NP2			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_AP				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_NP_CLANS		);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_OSK_EXT		);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_VDEC_DIVX				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_JPGENC					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_GAME			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_BGDL					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_FREETYPE_TT			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_VIDEO_UPLOAD	);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_SYSCONF_EXT	);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_FIBER					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_NP_COMMERCE2	);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_NP_TUS			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_VOICE					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC_CELP8				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_CELP8ENC				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_LICENSEAREA	);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_MUSIC2			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_SCREENSHOT		);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_MUSIC_DECODE	);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SPURS_JQ				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_PNGENC					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_MUSIC_DECODE2	);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYNC2					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_NP_UTIL		);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_RUDP					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_NP_SNS			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_GEM					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_CELPENC				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_GIFDEC					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC_CELP				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC_M2BC				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC_M4AAC				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_ADEC_MP3				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_IMEJP					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_MUSIC			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_PHOTO_EXPORT			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_PRINT					);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_PHOTO_IMPORT			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_MUSIC_EXPORT			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_PHOTO_DECODE			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_SEARCH			);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_AVCHAT2		);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SAIL_REC				);
		//cellSysmoduleLoadModule		( CELL_SYSMODULE_SYSUTIL_NP_TROPHY		);
		sys_dbg_initialize_ppu_exception_handler	( 0 );
		sys_dbg_register_ppu_exception_handler		( &unhandled_exception_handler, SYS_DBG_PPU_THREAD_STOP | SYS_DBG_SPU_THREAD_GROUP_STOP );
	}

	( *function_to_call	)	( argument );
}