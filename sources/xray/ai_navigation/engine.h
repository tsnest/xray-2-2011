////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_NAVIGATION_ENGINE_H_INCLUDED
#define XRAY_AI_NAVIGATION_ENGINE_H_INCLUDED

namespace xray {
namespace ai {
namespace navigation {

struct XRAY_NOVTABLE engine
{
	virtual	void	set_navmesh_info	( pcstr text ) const = 0;
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( engine )
}; // class engine

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_NAVIGATION_ENGINE_H_INCLUDED