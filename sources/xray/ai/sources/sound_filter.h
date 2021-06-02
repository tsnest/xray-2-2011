////////////////////////////////////////////////////////////////////////////
//	Created		: 25.10.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_FILTER_H_INCLUDED
#define SOUND_FILTER_H_INCLUDED

#include "base_filter.h"
#include <xray/ai/filter_types.h>

namespace xray {
namespace ai {
namespace planning {

class sound_filter : public base_filter
{
public:
					sound_filter		( bool need_to_be_inverted );
	virtual			~sound_filter		( );

			void	add_filtered_item	( pcstr item );
	inline	void	set_filter_type		( sound_filter_types_enum const type ) { m_filter_type = type; }
	
private:
	typedef list< fs_new::virtual_path_string >	filtered_items_type;

private:
	virtual	bool	is_passing_filter	( object_instance_type const& object ) const;
			bool	contains_object		( fs_new::virtual_path_string const& item ) const;

private:
	filtered_items_type					m_filtered_items;
	sound_filter_types_enum				m_filter_type;
}; // class sound_filter

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef SOUND_FILTER_H_INCLUDED