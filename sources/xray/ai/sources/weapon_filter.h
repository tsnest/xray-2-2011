////////////////////////////////////////////////////////////////////////////
//	Created		: 24.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef WEAPON_FILTER_H_INCLUDED
#define WEAPON_FILTER_H_INCLUDED

#include "base_filter.h"
#include <xray/ai/filter_types.h>

namespace xray {
namespace ai {
namespace planning {

typedef u32 filtered_weapon_id;

class weapon_filter : public base_filter
{
public:
					weapon_filter		( bool need_to_be_inverted );
	virtual			~weapon_filter		( );

	virtual	bool	is_passing_filter	( object_instance_type const& object ) const;

			void	add_filtered_id		( filtered_weapon_id const id );
	inline	void	set_filter_type		( weapon_filter_types_enum const type ) { m_filter_type = type; }
	
private:
	typedef list< filtered_weapon_id >	filtered_ids_type;

private:
			bool	contains_object_id	( filtered_weapon_id const id ) const;

private:
	filtered_ids_type					m_filtered_ids;
	weapon_filter_types_enum			m_filter_type;
}; // class weapon_filter

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef WEAPON_FILTER_H_INCLUDED