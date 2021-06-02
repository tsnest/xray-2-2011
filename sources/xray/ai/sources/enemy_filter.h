////////////////////////////////////////////////////////////////////////////
//	Created		: 24.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ENEMY_FILTER_H_INCLUDED
#define ENEMY_FILTER_H_INCLUDED

#include "base_filter.h"
#include <xray/ai/filter_types.h>

namespace xray {
namespace ai {
namespace planning {

typedef u32 filtered_id;

class enemy_filter : public base_filter
{
public:
					enemy_filter		( bool need_to_be_inverted );
	virtual			~enemy_filter		( );

			void	add_filtered_id		( filtered_id const id );
	inline	void	set_filter_type		( enemy_filter_types_enum const type ) { m_filter_type = type; }
	
private:
	typedef list< filtered_id >			filtered_ids_type;

private:
	virtual	bool	is_passing_filter	( object_instance_type const& object ) const;
			bool	contains_object_id	( filtered_id const id ) const;

private:
	filtered_ids_type					m_filtered_ids;
	enemy_filter_types_enum				m_filter_type;
}; // class enemy_filter

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef ENEMY_FILTER_H_INCLUDED