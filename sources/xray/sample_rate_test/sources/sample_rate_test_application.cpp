////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "sample_rate_test_application.h"
#include "sample_rate_test_memory.h"
#include <xray/core/core.h>
#include <xray/core/simple_engine.h>
#include <conio.h>

#include <xray/os_preinclude.h>		// for GetCommandLine
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>		// for GetCommandLine

#include <xaudio2.h>
#include "sound_stream.h"
#include "sound_object.h"

using xray::sample_rate_test::application;

xray::memory::doug_lea_allocator_type	xray::sample_rate_test::g_allocator;

typedef xray::core::simple_engine							core_engine_type;
static xray::uninitialized_reference< core_engine_type >	s_core_engine;

void application::initialize( )
{
	m_exit_code				= 0;

	XRAY_CONSTRUCT_REFERENCE( s_core_engine, core_engine_type );

	core::preinitialize		(
		&*s_core_engine,
		GetCommandLine(), 
		command_line::contains_application_true,
		"sample_rate_test",
		__DATE__ 
	);

	g_allocator.do_register	( 4*1024*1024, "sample_rate_test" );

	memory::allocate_region	( );

	core::initialize		( "sample_rate_test", core::create_log, core::perform_debug_initialization );
}

void application::finalize	( )
{
	core::finalize			( );

	XRAY_DESTROY_REFERENCE	( s_core_engine );
}

void application::execute	( )
{
	IXAudio2* pXAudio2 = NULL;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	HRESULT hr = XAudio2Create	( &pXAudio2, XAUDIO2_DEBUG_ENGINE, XAUDIO2_DEFAULT_PROCESSOR );
	ASSERT				( !FAILED(hr) );

	IXAudio2MasteringVoice* pMasterVoice = NULL;
	hr = pXAudio2->CreateMasteringVoice( &pMasterVoice, XAUDIO2_DEFAULT_CHANNELS, 44100, 0, 0, NULL ); 
	ASSERT				( !FAILED(hr) );

	FILE* input_file_44100 = 0;
	errno_t err = fopen_s(&input_file_44100, "Z:\\test\\ogg\\guitar_10_44100.ogg", "rb");
	ASSERT				( err == 0 );


	FILE* input_file_22050 = 0;
	err = fopen_s(&input_file_22050, "Z:\\test\\ogg\\guitar_10_22050.ogg", "rb");
	ASSERT				( err == 0 );

	FILE* input_file_11025 = 0;
	err = fopen_s(&input_file_11025, "Z:\\test\\ogg\\guitar_10_11025.ogg", "rb");
	ASSERT				( err == 0 );

	sound_stream ss44100(input_file_44100);
	sound_stream ss22050(input_file_22050);
	sound_stream ss11025(input_file_11025);

	//sound_object obj(pXAudio2, &ss);
	//obj.play();

	typedef vector<sound_object*> sounds;
	sounds vec;


//	for (int i = 0; i < 30; ++i)
//	{
	sound_object obj(pXAudio2, &ss11025, &ss22050, &ss44100);
		vec.push_back(&obj);
//	}

	for (int i = 0; i < vec.size(); ++i)
	{
		vec[i]->play();
	}

	int sample_rate = 0;
	int old_sample_rate = 0;
	xray::timing::timer tm;
	tm.start();
	while(true)
	{
		for (int i = 0; i < vec.size(); ++i)
		{
			vec[i]->update();
		}

		if (tm.get_elapsed_ms() > 29000)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(2);
			}

		}
		else if (tm.get_elapsed_ms() > 5000)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(1);
			}

		}
		else if (tm.get_elapsed_ms() > 4800)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(1);
			}

		}
		else if (tm.get_elapsed_ms() > 4700)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(0);
			}

		}
		else if (tm.get_elapsed_ms() > 4200)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(1);
			}

		}
		else if (tm.get_elapsed_ms() > 3900)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(0);
			}

		}
		else if (tm.get_elapsed_ms() > 3700)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(1);
			}

		}
		else if (tm.get_elapsed_ms() > 3500)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(0);
			}

		}
		else if (tm.get_elapsed_ms() > 3000)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(1);
			}

		}
		else if (tm.get_elapsed_ms() > 2500)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(0);
			}

		}
		else if (tm.get_elapsed_ms() > 2000)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(1);
			}

		}
		else if (tm.get_elapsed_ms() > 5000)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(0);
			}

		}
		else if (tm.get_elapsed_ms() > 1700)
		{
			for (int i = 0; i < vec.size(); ++i)
			{
				vec[i]->change_sound_quality(2);
			}

		}

		old_sample_rate = sample_rate;
	}

	m_exit_code				= 0; //s_core_engine->get_exit_code();
}