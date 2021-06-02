////////////////////////////////////////////////////////////////////////////
//	Created		: 17.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_LIGHT_H_INCLUDED
#define TOOL_LIGHT_H_INCLUDED

#include "tool_base.h"
#include <xray/render/world.h>
#include <xray/render/facade/model.h>


namespace xray {
namespace editor {

ref class light_tool_tab;
ref class object_light;

public ref class tool_light  :public tool_base
{
	typedef tool_base	super;
public:
						tool_light				( level_editor^ le );
	virtual				~tool_light				( );

	virtual object_base^ create_library_object_instance	( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual object_base^ load_object			( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual void		destroy_object			( object_base^ o ) override;
	virtual void		on_new_project			( render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )	override;

	void				load_library			( );
	virtual void		save_library			( )	override;
	virtual tool_tab^	ui						( ) override;
	collision::geometry*		m_object_geometry;

protected:
	virtual void		on_library_loaded		( ) override;
	pcstr				get_library_module_name	( );
private:
			object_light^ create_raw_object		( System::String^ id, render::scene_ptr const& scene );
	
	light_tool_tab^								m_tool_tab;
}; // class tool_light

} // namespace editor
} // namespace xray

#endif // #ifndef TOOL_LIGHT_H_INCLUDED
