////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_rms_cook.h"
#include "ogg_utils.h"
#include <xray/sound/sound_rms.h>


namespace xray {
namespace sound {

#define XRAY_RMS_SAMPLE_RATE	50

static xray::command_line::key s_discr_frequency (
	"rms_discreteness",
	"rms_discr",
	"sound rms",
	"set discretization frequency(floats per second). Default " XRAY_MAKE_STRING( XRAY_RMS_SAMPLE_RATE ) "."
);

enum
{
	rms_discreteness_default_value = XRAY_RMS_SAMPLE_RATE,
};

#undef XRAY_RMS_SAMPLE_RATE

sound_rms_cook::sound_rms_cook( ) 
	:super(resources::sound_rms_class, reuse_true, use_resource_manager_thread_id)
{
}

sound_rms_cook::~sound_rms_cook( )
{
}

void sound_rms_cook::translate_query( resources::query_result_for_cook& parent )
{
	// calc rms from high quality sound
	fs_new::virtual_path_string req_path;
	req_path.assignf( "%s%s.high%s", _converted_local_path, parent.get_requested_path(), _ogg_ext );
	query_resource	(
						req_path.c_str(), 
						resources::ogg_raw_file, 
						boost::bind(&sound_rms_cook::on_sub_resources_loaded, this, _1 ), 
						parent.get_user_allocator(), 
						0,
						&parent );

}

void sound_rms_cook::on_sub_resources_loaded( resources::queries_result& data )
{
	R_ASSERT(data.is_successful());
	resources::query_result_for_cook* const parent	= data.get_parent_query();

	resources::managed_resource_ptr ogg_raw_file	= data[0].get_managed_resource();

	ogg_file_source		ogg;
	ogg.resource		= ogg_raw_file;
	ogg.pointer			= 0;

	OggVorbis_File		ovf;

	ov_callbacks ovc	= {ogg_utils::ov_read_func, ogg_utils::ov_seek_func, ogg_utils::ov_close_func, ogg_utils::ov_tell_func};
 	ov_open_callbacks	(&ogg, &ovf, NULL, 0, ovc);
	vorbis_info* ovi	= ov_info(&ovf, -1);

	u32 discretization	= rms_discreteness_default_value;
	
	if(s_discr_frequency.is_set())
		s_discr_frequency.is_set_as_number(&discretization);

	float discr			= 1.0f / (float)discretization;

	//const u32	one_sec_samples	= samples_per_sec * 60;
	//u32 length_in_msec			= (u32)((ov_pcm_total(&ovf, -1) * 1000) / samples_per_sec);
	u64 samples_discr_in_pcm	= static_cast<u64>(ovi->rate * discr );

	u32 size					= math::ceil( ov_pcm_total(&ovf, -1) / (float)samples_discr_in_pcm);
	ov_clear					(&ovf);

	resources::managed_resource_ptr created_resource	=	resources::allocate_managed_resource(sizeof(sound_rms) + size * sizeof(float), resources::sound_rms_class);
	if ( !created_resource )
	{
		parent->set_out_of_memory	( resources::managed_memory, sizeof(sound_rms) + size * sizeof(float) );
		parent->finish_query		( result_out_of_memory );
		return;
	}

	resources::pinned_ptr_mutable<sound_rms> pinned_ptr = pin_for_write<sound_rms>( created_resource );
	new (pinned_ptr.c_ptr())			sound_rms(ogg_raw_file, discr );


	parent->set_managed_resource	( created_resource );
	parent->finish_query			( result_success );
}

void sound_rms_cook::delete_resource( resources::resource_base* res)
{
	sound_rms_ptr rms				= res->cast_managed_resource();
	sound_rms_pinned pinned_rms		( rms );
	pinned_rms.c_ptr()->~sound_rms	( );
}

} // namespace sound
} // namespace xray
