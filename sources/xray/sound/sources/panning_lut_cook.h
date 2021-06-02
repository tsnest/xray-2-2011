////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PANNING_LUT_COOK_H_INCLUDED
#define PANNING_LUT_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class panning_lut_cook :	public resources::translate_query_cook,
							private boost::noncopyable					
{
	typedef resources::translate_query_cook		super;
public:
								panning_lut_cook 		( );
								~panning_lut_cook 		( );

	virtual	void				translate_query			( resources::query_result_for_cook&	parent );
	virtual void				delete_resource			( resources::resource_base* resource );
}; // class panning_lut_cook

} // namespace sound
} // namespace xray

#endif // #ifndef PANNING_LUT_COOK_H_INCLUDED