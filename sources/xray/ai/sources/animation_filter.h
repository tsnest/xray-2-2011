////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_FILTER_H_INCLUDED
#define ANIMATION_FILTER_H_INCLUDED

#include "base_filter.h"
#include <xray/ai/filter_types.h>

namespace xray {
namespace ai {
namespace planning {

class animation_filter : public base_filter
{
public:
					animation_filter	( bool need_to_be_inverted );
	virtual			~animation_filter	( );

			void	add_filtered_item	( pcstr item );
	inline	void	set_filter_type		( animation_filter_types_enum const type ) { m_filter_type = type; }
	
private:
	typedef list< fs_new::virtual_path_string >	filtered_items_type;

private:
	virtual	bool	is_passing_filter	( object_instance_type const& object ) const;
			bool	contains_item		( fs_new::virtual_path_string const& item ) const;

private:
	filtered_items_type					m_filtered_items;
	animation_filter_types_enum			m_filter_type;
}; // class animation_filter

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef ANIMATION_FILTER_H_INCLUDED
