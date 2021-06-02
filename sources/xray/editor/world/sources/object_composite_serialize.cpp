////////////////////////////////////////////////////////////////////////////
//	Created		: 15.10.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_composite.h"
#include "tool_composite.h"
#include "level_editor.h"
#include "project_items.h"
#include "object_scene.h"
#include "project.h"
#include "tool_scripting.h"

namespace xray{
namespace editor{

void object_composite::re_load_lib( )
{
	System::String^ name	= get_name();
	System::String^ ln		= get_library_name();
	float4x4 tr				= get_transform( );
	destroy_contents		( );
	
	lib_item^ li		= m_tool_composite->m_library->get_library_item_by_full_path( ln, true );
	load_from_library	( *li->m_config );
	set_library_name	( ln );
	set_visible			( get_visible() );
	load_contents		( );
	set_transform		( tr );
	set_name			( name, true );
}


void object_composite::save_as_library( configs::lua_config_value& t )
{
	R_ASSERT							(is_edit_mode());
	t["pivot"]							= *m_pivot;
	configs::lua_config_value t_objects = t["objects"];
	
	for each (composite_object_item^ o in m_objects)
	{
		
		if ( o->m_object == logic_scene )
		{
			configs::lua_config_value current = t["logic"]["root_scene"];		
			configs::lua_config_value t_offset = current["offset"];
			save_transform				( t_offset, *o->m_relative_transform );
			o->m_object->save			( current );
		}
		else
		{
			configs::lua_config_value current = t_objects[unmanaged_string(o->m_object->get_name()).c_str()];;
			configs::lua_config_value t_offset = current["offset"];
			save_transform				( t_offset, *o->m_relative_transform );
			o->m_object->save			( current );
		}
		
	}
	// save public properties desc
	configs::lua_config_value props = t["public_properties"];
	u32 idx = 0;
	for each (composite_object_item^ o in m_objects)
	{
		for each(composite_object_item_public_props_it it in o->m_props_dictionary)
		{
			if(it.Value)
			{
				System::String^ s = o->m_object->get_name() + "|" + it.Key;
				props[idx] = unmanaged_string(s).c_str();
				++idx;
			}
		}
	}
}

void object_composite::save( configs::lua_config_value t )
{
	super::save						(t);

	t["game_object_type"]	= "composite";

	for each (composite_object_item^ o in m_objects)
	{
		configs::lua_config_value		object_t = t["objects"][unmanaged_string(o->m_object->get_name()).c_str()];
		System::String^ supertable		= "library/";
		supertable						+= get_library_guid();
		supertable						+= "/objects/";
		supertable						+= o->m_object->get_name();
		supertable						= supertable->Replace('/', '.');

		object_t.add_super_table( m_tool_composite->get_library_module_name(), 
			unmanaged_string(supertable).c_str() );
		
		o->m_object->save				( object_t );
	}
}

void object_composite::load_props( configs::lua_config_value const& t )
{
	super::load_props				( t );

	for( int idx=0; idx<m_objects->Count; ++idx )
	{
		object_base^ o				= m_objects[idx]->m_object;
		unmanaged_string			object_name(o->get_name());

		if ( o->GetType() == object_scene::typeid )
			continue;
		
		if( t["objects"].value_exists(object_name.c_str()) )
		{
			configs::lua_config_value current = t["objects"][object_name.c_str()];

			if ( current.are_all_super_tables_loaded_correctly() ) 
			{
				System::String^ tn	= gcnew System::String(current["tool_name"]);
				R_ASSERT			(o->owner_tool()->name() == tn);
				o->load_props		( current );
			}
			else
				LOG_ERROR("missing some classes in composite library [%s]", object_name.c_str() );
		}else
		{
			LOG_ERROR("missing record about composite object member [%s]", object_name.c_str() );
		}
	}
}

void object_composite::load_from_library( configs::lua_config_value const& t )
{
	set_name					(gcnew System::String(t["name"]), true);

	if(t.value_exists("pivot"))
		*m_pivot			= t["pivot"];

	configs::lua_config_value t_objects = t["objects"];

	configs::lua_config::const_iterator it	 = t_objects.begin();
	configs::lua_config::const_iterator it_e = t_objects.end();

	for(; it!=it_e; ++it)
	{
		configs::lua_config_value current = *it;
		object_base^ o				= create_object( owner_tool()->get_level_editor(), *it );
		project_item_object^ item	= gcnew project_item_object( get_project(), o );
		item->assign_id				( 0 );
		item->m_guid				= System::Guid::NewGuid();
		
		math::float4x4				m;
		load_transform				( current["offset"], m );
		add_object					( o, &m );
	}

	if ( t.value_exists("logic") )
	{
		configs::lua_config_value current = t["logic"]["root_scene"];
		
		tool_scripting^ tool					= safe_cast< tool_scripting^ >( get_project()->get_tool("scripting") );

		project_item_object^ item				= gcnew project_item_object( get_project() );
		item->assign_id							( 0 );
		item->m_guid							= System::Guid::NewGuid();
		item->m_object_base						= tool->create_raw_object( "base:root_scene" );
		item->m_object_base->set_library_name	( "base:root_scene" );
		item->m_object_base->set_name			( "root_scene", false );

		logic_scene								= safe_cast< object_scene^ >( item->m_object_base );

		logic_scene->parent_composite			= this;
		item->m_object_base->owner_project_item	= item;	

		math::float4x4				m;
		load_transform				( current["offset"], m );
		add_object					( item->m_object_base, &m );

		item->m_object_base->load_defaults			( );
		item->m_object_base->load_props			( current );		
	}

	
}

} // namespace editor
} // namespace xray

