////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATED_MODEL_INSTANCE_COOK_H_INCLUDED
#define ANIMATED_MODEL_INSTANCE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace stalker2 {

class animated_model_instance_cook : public resources::translate_query_cook
{
	typedef resources::translate_query_cook			super;

public:
					animated_model_instance_cook	( );
	
	virtual	void	translate_request_path			( pcstr request, fs_new::virtual_path_string & new_request ) const;
	virtual	void	translate_query					( resources::query_result_for_cook&	parent );
	virtual void	delete_resource					( resources::resource_base* resource );
	
private:
			void	on_config_loaded				( resources::queries_result& data );
			void	on_subresources_loaded			( resources::queries_result& data );
}; // class animated_model_instance_cook

} // namespace stalker2

#endif // #ifndef ANIMATED_MODEL_INSTANCE_COOK_H_INCLUDED