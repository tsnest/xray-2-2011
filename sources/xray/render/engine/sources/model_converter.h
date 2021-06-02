////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_CONVERTER_H_INCLUDED
#define MODEL_CONVERTER_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace render {

class converted_model_cook : public resources::translate_query_cook 
{
private:
	typedef resources::translate_query_cook	super;

public:
					converted_model_cook	( );

	virtual	void	translate_query			( resources::query_result_for_cook& parent );
	virtual void	delete_resource			( resources::resource_base* resource );

private:
	void			request_converted_file	( resources::query_result_for_cook* parent );
	void			on_converted_loaded		( resources::queries_result& data, resources::query_result_for_cook* parent );

#ifndef MASTER_GOLD
	void			request_convertation	( resources::query_result_for_cook* parent );
	void			on_sources_loaded		( resources::queries_result& data, resources::query_result_for_cook* parent );
	void			on_fs_iterators_ready	( resources::queries_result& data, resources::query_result_for_cook* parent );
#endif //#ifndef MASTER_GOLD
}; // class converted_model_cook

} // namespace render
} // namespace xray

#endif // #ifndef MODEL_CONVERTER_H_INCLUDED