////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_logic.h"
#include "object_scene.h"
#include "tool_scripting.h"
#include "project_items.h"
#include "level_editor.h"
#include "logic_editor_panel.h"
#include "lua_config_value_editor.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/collision/collision_object.h>
#pragma managed( pop )

namespace xray{
namespace editor{

object_logic::object_logic( tool_scripting^ t )
:super(t), 
m_tool_scripting(t)
{
}

object_logic::~object_logic( )
{
}

void object_logic::destroy_collision	( )
{
	if ( m_collision->initialized() )
		m_collision->destroy( &debug::g_mt_allocator );
}

void object_logic::load_props( configs::lua_config_value const& t )
{
	System::String^				name = gcnew System::String(t["name"]);
	set_name					( name, true );
	//set_lib_name				( gcnew System::String(t["lib_name"]) );
	math::float4x4 m;
	load_transform				( t, m );
	set_transform				( m );
}


void object_logic::save( configs::lua_config_value cfg )
{
	//super::save				( t );

	//cfg["type"]					= "object";
	//cfg["name"]					= unmanaged_string( get_name()).c_str();
	//cfg["lib_name"]				= unmanaged_string( get_lib_name() ).c_str();
	//save_transform				( cfg, *m_transform );
	cfg["type"]			= "object";
	set_c_string		(cfg["name"], get_name());
	set_c_string		(cfg["lib_name"], get_library_name() );
	set_c_string		(cfg["lib_guid"], m_lib_guid.ToString());
	save_transform		( cfg, *m_transform );
	set_c_string		(cfg["tool_name"], owner_tool()->name());
}


void object_logic::initialize_collision( )
{
	//ASSERT( !m_collision->initialized() );
	//float3 extents				(0.3f,0.3f,0.3f);

	//collision::geometry* geom	= &*collision::create_box_geometry( &debug::g_mt_allocator, extents );
	//m_collision->create_from_geometry(
	//				true,
	//				this,
	//				geom,
	//				editor_base::collision_object_type_dynamic | editor_base::collision_object_type_touch );
	//m_collision->insert				( );
	//update_collision_transform		( );
}

void object_logic::load_contents			( )
{
	//if(!m_collision->initialized())
	//	initialize_collision();
}

void object_logic::unload_contents	( bool )
{
	destroy_collision				( );
}

void object_logic::on_selected				( bool bselected )
{
	super::on_selected( bselected );
}

void object_logic::set_name					( System::String^ name, bool refresh_ui )
{
	super::set_name(name, refresh_ui);	
}

void object_logic::parent_scene::set( object_scene^ scene )
{
	if (m_parent_scene == scene)
		return;

	if (m_parent_scene != nullptr)
		m_parent_scene->owner_project_item->remove(this->owner_project_item);

	m_parent_scene = scene;
	
	if ( m_parent_scene->owner_project_item != nullptr )
		m_parent_scene->owner_project_item->add(this->owner_project_item);
	on_parent_scene_changed( scene );
}

object_composite^ object_logic::parent_composite::get()
{
	if ( this->parent_scene == nullptr )
		return m_parent_composite;

	object_logic^ logic_object = this;
	while ( logic_object->parent_scene != nullptr )
	{
		logic_object = logic_object->parent_scene;
	}

	return logic_object->parent_composite;
}


}
}

