////////////////////////////////////////////////////////////////////////////
//	Created		: 13.11.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef WEAPON_COOK_H_INCLUDED
#define WEAPON_COOK_H_INCLUDED
#include <xray/resources_cook_classes.h>

namespace stalker2{
class game;

class weapon_cook :	public resources::translate_query_cook,
					private boost::noncopyable

{
	typedef resources::translate_query_cook super;
public:
					weapon_cook				(  game& game );
	virtual	void	translate_query			( resources::query_result_for_cook&	parent );
	virtual void	delete_resource			( resources::resource_base* resource );	

private:
			void	on_weapon_config_ready	(	resources::queries_result& data, 
												resources::query_result_for_cook* parent );

			void	on_weapon_parts_ready	(	resources::queries_result& data, 
												configs::binary_config_ptr config_ptr, 
												resources::query_result_for_cook* parent );

	game&			m_game;
}; // class weapon_cook

} // namespace stalker2

#endif // #ifndef WEAPON_COOK_H_INCLUDED