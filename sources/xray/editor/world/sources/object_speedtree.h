////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_SPEEDTREE_H_INCLUDED
#define OBJECT_SPEEDTREE_H_INCLUDED

#include "object_base.h"
#include <xray/render/facade/model.h>
#include <xray/render/engine/base_classes.h>

namespace xray {
namespace editor {

ref class tool_speedtree;

ref class object_speedtree :public object_base
{
	typedef object_base		super;
public:
					object_speedtree			( tool_speedtree^ t, render::scene_ptr const& scene );
	virtual			~object_speedtree			( );
	virtual	void	set_transform				( float4x4 const& transform )			override;
	virtual	void	on_selected					( bool selected )						override;
	virtual	void	set_visible					( bool bvisible )						override;
	virtual void	save						( configs::lua_config_value t )			override;
	virtual void	set_library_name			( System::String^ ln )					override;
	virtual void	destroy_collision			( ) override;
	virtual enum_terrain_interaction get_terrain_interaction( )							override	{return enum_terrain_interaction::tr_vertical_snap;}
	
	virtual void	load_contents				( )	override;
	virtual void	unload_contents				( bool ) override;

protected:
	void			on_resource_loaded			( resources::queries_result& data );
	void			initialize_collision		( );
	void			update_speedtree_instance	( );
	
	System::String^								m_reference_name;

public:
	query_result_delegate*						m_model_query;
	xray::render::speedtree_instance_ptr*		m_speedtree_instance_ptr;
	render::scene_ptr*							m_scene;
	tool_speedtree^								m_tool_speedtree;
	bool										m_selected;
public:
}; // class object_speedtree

} // namespace editor
} // namespace xray


#endif // #ifndef OBJECT_SPEEDTREE_H_INCLUDED