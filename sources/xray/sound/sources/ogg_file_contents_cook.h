////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OGG_FILE_CONTENTS_COOK_H_INCLUDED
#define OGG_FILE_CONTENTS_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class ogg_file_contents_cook :	public resources::translate_query_cook,
								private boost::noncopyable
{
	typedef resources::translate_query_cook	super;
public:
							ogg_file_contents_cook	( );
	virtual					~ogg_file_contents_cook	( );

	virtual void			delete_resource			( resources::resource_base* res);
	virtual void			translate_query			( resources::query_result_for_cook& parent );

private:
			void			on_sub_resources_loaded	( resources::queries_result& data ); // raw ogg file loaded
}; // class ogg_file_contents_cook


} // namespace sound
} // namespace xray

#endif // #ifndef OGG_FILE_CONTENTS_COOK_H_INCLUDED