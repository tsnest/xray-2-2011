////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_compound.h"
#include "tool_compound.h"

#include "level_editor.h"
#include <xray/render/base/world.h>
#include <xray/editor/world/engine.h>
#include <xray/strings_stream.h>
#include <xray/collision/api.h>
#include "collision_object_dynamic.h"

using xray::editor::object_base;
using xray::editor::object_compound;
using xray::editor::tool_compound;

object_compound::object_compound(tool_compound^ t)
:super(t),
m_tool_compound(t),
m_block_collision(false)
{
	m_pivot_offset =	 NEW(float3)(0.0f, 0.0f, 0.0f);
	m_objects			= gcnew object_list;
	image_key			= "joint";
}

object_compound::~object_compound()
{
	DELETE					(m_pivot_offset);
}

void object_compound::set_visible(bool bvisible)
{
	super::set_visible		(bvisible);

	for each (object_base^ o in m_objects)
		o->set_visible			(bvisible);
}

void object_compound::set_selected(bool selected)
{
	super::set_selected		(selected);

	for each (object_base^ o in m_objects)
		o->set_selected			(selected);
}

object_base^ object_compound::get_object(u32 i)				
{
	return safe_cast<object_base^>(m_objects[i]);
}

void object_compound::load(xray::configs::lua_config_value const& t)
{
	super::load					(t);

	(*m_pivot_offset)			= t["pivot_offset"];
	
	xray::configs::lua_config_value t_objects = t["objects"];

	configs::lua_config::const_iterator it	 = t_objects.begin();
	configs::lua_config::const_iterator it_e = t_objects.end();

	block_collision_rebuild			();
	for(; it!=it_e; ++it)
	{
		xray::configs::lua_config_value current = *it;
		System::String^ tool_name	= gcnew System::String(current["tool_name"]);
		tool_base^ tool				= owner()->get_level_editor()->get_tool(tool_name);
		object_base^ o				= tool->load_object(current);
		m_objects->Add				(o);
		o->m_collision.on_collision_initialized += gcnew collision_initialized(this, &object_compound::on_child_object_collision_initialized);
	}
	unblock_collision_rebuild			();


	get_property_holder()->clear();
	initialize_property_holder(this);
	for each (object_base^ o in m_objects)
	{
		get_property_holder()->add_property_holder("child","","",
			o->get_property_holder()
			);
	}

}

void object_compound::save(xray::configs::lua_config_value t)
{
	super::save						(t);

	t["pivot_offset"]				= *m_pivot_offset;

	xray::configs::lua_config_value t_objects = t["objects"];
	
	int i=0;
	for each (object_base^ o in m_objects)
	{
		xray::configs::lua_config_value current = t_objects[i];
		o->save						(current);
		++i;
	}
}

void object_compound::initialize_collision()
{
	for each (object_base^ o in m_objects)
	{
		if(!o->m_collision.initialized())
			return;
	}

	if( m_collision.initialized() )
		m_collision.destroy			();

	vectora< collision::geometry_instance* > vec( g_allocator );
	vec.reserve					( m_objects->Count );
	for each (object_base^ o in m_objects)
	{
		vec.push_back			( &*create_geometry_instance( g_allocator, 
										o->get_transform(), 
										o->m_collision.get_geometry()) 
								);
		o->m_collision.remove	( );
	}
	non_null<xray::collision::geometry>::ptr geom = collision::create_compound_geometry(g_allocator, vec);
	m_collision.create_from_geometry	(this, &*geom, xray::editor::collision_type_dynamic);
	m_collision.insert					();
	m_collision.set_matrix				(m_transform);
}

void object_compound::render()
{
	super::render					();

	float4x4 pivot_transform		= (*m_transform) * create_translation(*m_pivot_offset);

	for each (object_base^ o in m_objects)
	{
		float4x4 child_transform	= o->get_transform() * pivot_transform;
		o->render					();//, child_transform);
	}
	//todo : draw pivot here

	if(get_selected())
	{
		float3 cone_size				(1,1,1);
		get_editor_renderer().draw_cone		( pivot_transform, cone_size, math::color_xrgb(255, 255, 0) );
	}
	//if(m_collision.initialized())
	//	m_collision.m_collision_obj->render( render_world );
}

bool object_compound::add_object(object_base^ o)
{
	if(m_objects->Contains(o))
	{
		LOG_WARNING				("object [%s] already a member of compound [%s]", o->get_name(), get_name());
		return					false;
	}

	// calc object relative transformation
	float4x4 pivot_transform	= get_transform() * create_translation(*m_pivot_offset);
	float4x4					ti;
	ti.try_invert				(pivot_transform);

	float4x4 trel				= ti * o->get_transform();
	o->set_transform			(trel);
	m_aabbox->modify				(trel); 
	m_objects->Add				(o);

	if(o->m_collision.initialized())
		initialize_collision	();
	else
		o->m_collision.on_collision_initialized += gcnew collision_initialized(this, &object_compound::on_child_object_collision_initialized);

	return					true;
}

void object_compound::on_child_object_collision_initialized(xray::editor::object_collision^)
{
//	ASSERT(m_objects->Contains(object));
	if(!m_block_collision)
		initialize_collision	();
}

bool object_compound::remove_object(object_base^ o, bool b_destroy)
{
	if(!m_objects->Contains(o))
	{
		LOG_WARNING	("object [%s] is not a member of compound [%s]", o->get_name(), get_name());
		return		false;
	}

	m_objects->Remove	(o);
	if(b_destroy)
		o->owner()->destroy_object(o);

	initialize_collision	();
	return					true;
}

void object_compound::set_pivot(float4x4 const& m)
{
	XRAY_UNREFERENCED_PARAMETER		( m );
	//float4x4 prev_pivot_rtansform	= *m_transform * *m_pivot_transform;
	//
	//*m_pivot_transform				= m;
	//float4x4 pivot_transform		= *m_transform * *m_pivot_transform;;
	//float4x4						ti;
	//ti.try_invert					(pivot_transform);

	//for each (object_base^ o in m_objects)
	//{
	//	float4x4 child_transform	= prev_pivot_rtansform * o->get_transform();

	//	float4x4 trel				= ti * child_transform;
	//	o->set_transform			(trel);
	//}
}

void object_compound::block_collision_rebuild()
{
	m_block_collision = true;
}

void object_compound::unblock_collision_rebuild()
{
	if(m_block_collision)
	{
		m_block_collision		= false;
		initialize_collision	();
	}
}
