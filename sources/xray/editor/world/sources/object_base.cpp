////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_base.h"
#include "tool_base.h"
#include "project_items.h"
#include "level_editor.h"
#include "editor_world.h"
#include "project.h"
#include "command_set_object_transform.h"
#include "lua_config_value_editor.h"

#pragma managed( push, off )
#	include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

namespace xray{
namespace editor{

object_base::object_base(tool_base^ t):
m_owner_tool			( t ),
m_parent_object_base	( nullptr ),
m_lib_name				( "" ),
m_collision				( gcnew object_collision(t->get_level_editor()->get_collision_tree()) ),
m_visible				( false ),
m_needed_quality_		( false )
{
	m_project_item		= nullptr;
	image_index			= 0;
	m_transform			= NEW(float4x4)(float4x4().identity());
	m_previous_angles	= NEW(float3)(m_transform->get_angles_xyz());
	m_pivot				= NEW(float3)(0, 0, 0);
	m_aabbox			= NEW(math::aabb)(math::create_identity_aabb());
	m_name				= gcnew System::String("");
}

u32 object_base::id( ) 							
{
	R_ASSERT		( m_project_item );
	return m_project_item->id();
}

object_base::~object_base()
{
	destroy_collision			( );
	DELETE						( m_transform );
	DELETE						( m_previous_angles );
	DELETE						( m_aabbox );
	DELETE						( m_pivot );
}

//static float3 identity_position(0.0f, 0.0f, 0.0f);
//static float3 identity_rotation(0.0f, 0.0f, 0.0f);
//static float3 identity_scale(1.0f, 1.0f, 1.0f);

void load_transform( configs::lua_config_value const& t, math::float4x4& m )
{
	//math::float3 rotation		= t.value_exists("rotation") ? t["rotation"] : identity_rotation;
	//math::float3 translation	= t.value_exists("position") ? t["position"] : identity_position;
	//math::float3 scale			= t.value_exists("scale") ? t["scale"] : identity_scale;

	math::float3 rotation		= t["rotation"];
	math::float3 translation	= t["position"];
	math::float3 scale			= t["scale"];

	m = create_scale( scale ) * create_rotation( rotation ) * create_translation( translation );
}

void save_transform( configs::lua_config_value& t, math::float4x4 const& m )
{
	float3 rotation			= m.get_angles_xyz();

//	if(!math::is_similar(m.c.xyz(), identity_position))
		t["position"]			= m.c.xyz();

//	if(!math::is_similar(rotation, identity_rotation))
		t["rotation"]			= rotation;

	float3					scale;
	scale.x 				= m.i.length();
	scale.y 				= m.j.length();
	scale.z 				= m.k.length();

//	if(!math::is_similar(scale, identity_scale))
	t["scale"]				= scale;
}

void object_base::load_props( configs::lua_config_value const& t )
{
	set_name					( gcnew System::String(t["name"]), true );
	math::float4x4 m;
	load_transform				( t, m );
	set_transform				( m );

	if(t.value_exists("pivot"))
		*m_pivot			= t["pivot"];
}

void object_base::save( configs::lua_config_value cfg )
{
	cfg["type"]				= "object";
	set_c_string			(cfg["name"], get_name());
	set_c_string			(cfg["lib_name"], get_library_name());
	
	if(m_lib_guid!=System::Guid::Empty)
		set_c_string		(cfg["lib_guid"], m_lib_guid.ToString());

	save_transform			( cfg, *m_transform );
	cfg["pivot"]			= *m_pivot;

	math::aabb bb = get_aabb();
	cfg["aabb_min"]			= bb.min;
	cfg["aabb_max"]			= bb.max;
	set_c_string			(cfg["tool_name"], owner_tool()->name());

//	on_save						(cfg);
}

void object_base::set_name( System::String^ name, bool refresh_ui )
{
	m_name = name;
	if(refresh_ui && m_project_item)
		m_project_item->refresh_ui();
}

void object_base::object_name::set(System::String^ n)	
{
	m_project_item->m_parent->check_correct_existance( n, 1 );

	set_name_revertible	( n );
	on_property_changed	( "name" );
}

void object_base::set_name_revertible( System::String^ name )
{
	owner_tool()->get_level_editor()->get_command_engine()->run( gcnew command_set_object_name(owner_tool()->get_level_editor(), this->id(), name ) );
}

void object_base::set_library_name( System::String^ name )
{
	m_lib_name		= name;

	if( !name->StartsWith("base:"))
	{
		lib_item^ li	= m_owner_tool->m_library->get_library_item_by_full_path( name, true );
		m_lib_guid		= li->m_guid;
	}
}

aabb object_base::get_focus_aabb( )
{
	return get_aabb();
}

aabb object_base::get_aabb( )
{
	if( m_collision->initialized() )
		*m_aabbox = m_collision->get_aabb();

	return *m_aabbox;
}

System::String^ object_base::get_name( )
{
	return m_name;
}

System::String^ object_base::get_full_name( )
{
	if ( m_parent_object_base != nullptr )
		return m_parent_object_base->get_full_name() + "/" + get_name();
	return m_project_item->get_full_name();
}


wpf_controls::property_container^ object_base::get_property_container( )
{
	wpf_controls::property_container^ cont = gcnew wpf_controls::property_container;
	editor_base::object_properties::initialize_property_container	( this, cont );
	return cont;
}

void object_base::on_property_changed( System::String^ prop_name )
{
	get_project()->get_link_storage()->notify_on_changed(this, prop_name);
}

bool object_base::is_slave_attribute( System::String^ prop_name )
{
	return get_project()->get_link_storage()->has_links(nullptr, nullptr, this, prop_name);
}


render::editor::renderer& object_base::get_editor_renderer( )
{
	return get_level_editor()->get_editor_renderer();
}

editor_world& object_base::get_editor_world( )
{
	return get_level_editor()->get_editor_world();
}

render::debug::renderer& object_base::get_debug_renderer( )
{
	return get_level_editor()->get_editor_renderer().debug();
}

void object_base::set_parent( object_base^ p )
{
	R_ASSERT	( (!m_parent_object_base&&p) || (m_parent_object_base&&!p));
	m_parent_object_base	= p;
}

project^ object_base::get_project( )
{
	return get_level_editor()->get_project();
}

level_editor^ object_base::get_level_editor( )
{
	return m_owner_tool->get_level_editor();
}

object_base^ create_object( level_editor^ le, configs::lua_config_value cfg )
{
	System::String^ tool_name	= gcnew System::String(cfg["tool_name"]);
	tool_base^ tool				= le->get_tool(tool_name);
	object_base^ o				= tool->load_object( cfg, le->get_editor_world().scene(), le->get_editor_world().scene_view() );
	return o;
}

bool object_base::get_selectable( )
{
	return !get_project()->is_object_in_ignore_list(id());
}



System::String^	 object_base::check_valid_library_name( System::String^ name )
{
	System::String^ result		= nullptr;
	objects_library^ library	= m_owner_tool->m_library;
	lib_item^ lib_item			= library->get_library_item_by_full_path( name , false );
	subst_record^ record = nullptr;
	if(lib_item)
	{
		result = name;
		R_ASSERT(result!=nullptr);
	}else
	{ // resolve missing resource
		get_project()->set_changed();
		missing_resource_registry^ registry = get_project()->m_missing_resource_registry;

		record = registry->find(m_owner_tool->name(), name );

		if(record==nullptr)
		{
			System::String^ subst	= registry->choose_missing_resource( owner_tool(), name );
			result = subst;
			R_ASSERT(result!=nullptr);
		}else
		{// found record
			if(	record->result==subst_resource_result::ignore_one ||
				record->result==subst_resource_result::ignore_all_by_name ||
				record->result==subst_resource_result::ignore_all_any_name)
			{
				result = name;
				R_ASSERT(result!=nullptr);
			}else
			{
				result = record->dest_name;
				R_ASSERT(result!=nullptr);
			}
		}
	}

	R_ASSERT(result!=nullptr);
	return result;
}

} // namespace editor
} // namespace xray
