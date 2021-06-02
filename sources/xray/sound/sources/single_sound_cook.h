////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SINGLE_SOUND_COOK_H_INCLUDED
#define SINGLE_SOUND_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class single_sound_cook :
	public resources::translate_query_cook,
	private boost::noncopyable
{
	typedef resources::translate_query_cook	super;

public:
					single_sound_cook		( );

	virtual			~single_sound_cook	( );

	virtual	void	translate_query			( resources::query_result_for_cook& parent );
	virtual void	delete_resource			( resources::resource_base* res );

private:
			void	on_sub_resources_loaded	( resources::queries_result& data );
}; // class single_sound_cook

} // namespace sound
} // namespace xray

#endif // #ifndef SINGLE_SOUND_COOK_H_INCLUDED