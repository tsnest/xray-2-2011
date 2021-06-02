////////////////////////////////////////////////////////////////////////////
//	Created		: 20.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_SPL_COOK_H_INCLUDED
#define SOUND_SPL_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class sound_spl_cook : 	public resources::translate_query_cook,
						private boost::noncopyable 
{
	typedef resources::translate_query_cook	super;
public:
					sound_spl_cook		( );
	virtual			~sound_spl_cook		( );

	virtual	void	translate_query		( resources::query_result_for_cook& parent );
	virtual void	delete_resource		( resources::resource_base* res );
private:
	void	on_config_loaded( resources::queries_result& data, resources::query_result_for_cook* parent );	
}; // class sound_spl_cook

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_SPL_COOK_H_INCLUDED