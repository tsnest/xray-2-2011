#include "pch.h"
#include "test_application.h"

//#include "xray/buffer_string.h"
//#include "xray/engine/api.h"
//#include <xray/core/sources/fs_file_system.h>

#include <xray/core/core.h>
#include <xray/intrusive_list.h>
//#include <boost/type_traits/alignment_of.hpp>
//#include <boost/type_traits/intrinsics.hpp>

#include <stdio.h>

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>
using namespace xray;

#include <xray/core/simple_engine.h>
#include <xray/uninitialized_reference.h>
//#include <xray/type_variant.h>
//#include <xray/fs_utils.h>
//#include <xray/memory_stack_allocator.h>

xray::memory::doug_lea_allocator_type		g_test_allocator;

using namespace xray::resources;
using namespace xray::fs;

static xray::core::simple_engine		core_engine;


enum percept_memory_object_types
{	
	percept_memory_object_type_danger		= 0,
	percept_memory_object_type_enemy,
	percept_memory_object_type_order,
	percept_memory_object_type_disturbance,
	percept_memory_object_type_path_info,
	percept_memory_object_type_pickup_item,

	// all the new knowledge types must be inserted BEFORE this one
	percept_memory_object_types_count
}; // enum percept_memory_object_types

struct percept_memory_object : private boost::noncopyable
{
	percept_memory_object		( percept_memory_object_types knowledge_type ) :
		owner_position			( memory::uninitialized_value< float >(), memory::uninitialized_value< float >(), memory::uninitialized_value< float >() ),
		target_position			( memory::uninitialized_value< float >(), memory::uninitialized_value< float >(), memory::uninitialized_value< float >() ),
		update_time				( memory::uninitialized_value< u32 >() ),
		type					( knowledge_type ),
		confidence				( 0.f ),
		next					( 0 )
	{
	}

	float3						owner_position;
	float3						target_position;

	u32							update_time;
	
	percept_memory_object_types	type;
	float						confidence;

	percept_memory_object*		next;
}; // struct percept_memory_object

void   application::initialize (u32 , pstr const* )
{
	xray::core::preinitialize					(& core_engine, 
												 core::create_log, 
												 GetCommandLine(), 
												 command_line::contains_application_true,
												 "test",
												 __DATE__);
	g_test_allocator.user_current_thread_id		();
	g_test_allocator.do_register				(1 * 1024 * 1024,	"test allocator");
	xray::memory::allocate_region				();
	xray::core::initialize						( "../../resources/sources/", "test", core::perform_debug_initialization);
}

void   application::finalize ()
{

	xray::core::finalize						();
}

pcstr _wav_ext					= ".wav";
pcstr _options_ext				= ".options";

struct wav_fmt
{
	s16		format;
	s16		channels;
	s32		samplerate;
	s32		bytespersec;
	s16		align;
	s16		samplesize;
	u32		mask;
};

static u32 read_u32_le(unsigned char* buf)
{
	return (((buf)[3]<<24)|((buf)[2]<<16)|((buf)[1]<<8)|((buf)[0]&0xff));
}


int wav_id(unsigned char *buf, int len)
{
	using namespace	fs_new;
	fs_new::synchronous_device_interface const &	device	=	resources::get_synchronous_device();
	
	if ( device->open( &wav_file, wav_file_path.c_str(), file_mode::open_existing, file_access::read ) )
	{
		{
			unsigned char buf[12];
			s32 buf_filled			= 0;
			s32 ret					= 0;

			ret						= device->read( wav_file, buf, 12 );
			buf_filled				+= ret;

			u32 result				= wav_id(buf, buf_filled);
			R_ASSERT				( result == 1 );

		}

		unsigned int len		= 0;
		unsigned char buf[40];

		u32 result			= find_wav_chunk(wav_file, "fmt ", &len);
		R_ASSERT			( result, "Error: invalid wav file" );
		
		R_ASSERT	( len == 16 || len == 18 && len == 40, "INVALID format chunk in wav header." );

		size_t bytes_readed		= device->read( wav_file, buf, len );
		R_ASSERT				( bytes_readed == len );


		format.format		= read_u16_le(buf); 
		format.channels		= read_u16_le(buf+2); 
		format.samplerate	= read_u32_le(buf+4);
		format.bytespersec	= read_u32_le(buf+8);
		format.align		= read_u16_le(buf+12);
		format.samplesize	= read_u16_le(buf+14);

		device->close		(wav_file);
 		
	}



	return format;
}

void create_default_config ( fs_new::native_path_string const& wav_file_path )
{
	wav_fmt format								= get_wav_format( wav_file_path );
	fs_new::native_path_string	options_path				= wav_file_path;
	change_substring							( options_path, _wav_ext, _options_ext );
	configs::lua_config_ptr	config				= configs::create_lua_config( options_path.c_str() );
	configs::lua_config_value config_options	= config->get_root()["options"];
	config_options["name"]						= "sound";
	config_options["number_of_chanels"]			= format.channels;
	config_options["bits_per_sample"]			= format.samplesize;
	config_options["sample_rate"]				= format.samplerate;
	config_options["bit_rate"]					= format.bytespersec / 1024;
	config_options["high_quality_conversion_options"]	["sample_rate"]	= format.samplerate;
	config_options["high_quality_conversion_options"]	["bit_rate"]	= format.bytespersec	>> 10;
	config_options["medium_quality_conversion_options"]	["sample_rate"]	= format.samplerate		>> 1;
	config_options["medium_quality_conversion_options"]	["bit_rate"]	= format.bytespersec	>> 11;
	config_options["low_quality_conversion_options"]	["sample_rate"]	= format.samplerate		>> 2;
	config_options["low_quality_conversion_options"]	["bit_rate"]	= format.bytespersec	>> 12;
	config->save								( );
}

void   application::execute ()
{
	create_default_config(fs_new::native_path_string::convert("Z:/test/ogg/see_counter_3.wav"));
	printf("hello!");
	
	m_exit_code								=	0;
}
