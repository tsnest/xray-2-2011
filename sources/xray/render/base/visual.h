////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_VISUAL_H_INCLUDED
#define XRAY_RENDER_BASE_VISUAL_H_INCLUDED

namespace xray {

namespace collision {
	struct geometry;
} // namespace collision

namespace render {

class visual : public resources::unmanaged_resource {
public:
	virtual	void				render			( )	= 0;
	virtual	void				render_selected	( ) = 0;
	virtual	collision::geometry const* get_collision_geometry	( )	= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( visual )
}; // class visual

typedef	resources::resource_ptr<
			visual,
			resources::unmanaged_intrusive_base
		> visual_ptr;

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_BASE_VISUAL_H_INCLUDED