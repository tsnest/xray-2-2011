////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SPL_COOK_H_INCLUDED
#define SPL_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace sound {

class spl_cook :
	public resources::translate_query_cook,
	private boost::noncopyable 
{
	typedef resources::translate_query_cook	super;
public:
					spl_cook			( );

	virtual			~spl_cook			( );

	virtual	void	translate_query		( resources::query_result_for_cook& parent );
	virtual void	delete_resource		( resources::resource_base* res );

private:
			void	on_lua_config_loaded( resources::queries_result& data );
}; // class spl_cook

} // namespace sound
} // namespace xray

#endif // #ifndef SPL_COOK_H_INCLUDED