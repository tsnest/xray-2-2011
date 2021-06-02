////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_PROJECT_H_INCLUDED
#define GAME_PROJECT_H_INCLUDED

#include "cell.h"

namespace stalker2 {

class game_project :	public resources::unmanaged_resource,
						public boost::noncopyable
{

public:
	configs::binary_config_ptr		m_config;
	cell_container					m_cells;			// main cell storage

}; // class game_project

typedef	xray::resources::resource_ptr<
			game_project,
			resources::unmanaged_intrusive_base
		> game_project_ptr;

} // namespace stalker2

#endif // #ifndef GAME_PROJECT_H_INCLUDED