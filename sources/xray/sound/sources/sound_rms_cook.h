////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_RMS_COOK_H_INCLUDED
#define SOUND_RMS_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class sound_rms_cook :	public resources::translate_query_cook,
						private boost::noncopyable
{
	typedef resources::translate_query_cook	super;
public:
							sound_rms_cook			( );
	virtual					~sound_rms_cook			( );

	virtual void			delete_resource			( resources::resource_base* res);
	virtual void			translate_query			( resources::query_result_for_cook& parent );

private:
			void			on_sub_resources_loaded	( resources::queries_result& data ); // raw ogg file loaded
}; // class sound_rms_cook

//class sound_rms_cook :
//	public resources::managed_cook,
//	public boost::noncopyable
//{
//	typedef resources::managed_cook	super;
//
//public:
//					sound_rms_cook			( );
//	virtual			~sound_rms_cook			( ) { }
//
//	// raw_file_data can be NULL, when no file is found
//	virtual	u32		calculate_resource_size	(const_buffer in_raw_file_data, bool file_exist);
//
//	virtual void	create_resource			(resources::query_result_for_cook &	in_out_query, 
//												 const_buffer						raw_file_data,
//												 resources::managed_resource_ptr	out_resource);
//
//	virtual	void	destroy_resource		(resources::managed_resource *		dying_resource);
//
//private:
////			void	on_sub_resources_loaded	( resources::queries_result& data );
//
//private:
//}; // class sound_rms_cook

} // namespace sound
} // namespace xray

#endif // #ifndef RSOUND_RMS_COOK_H_INCLUDED