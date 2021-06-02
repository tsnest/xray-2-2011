////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_composite.h"
#include "tool_composite.h"
#include "level_editor.h"
#include "project_items.h"
#include "lua_config_value_editor.h"
#include "attribute.h"
#include "project.h"
#include "le_transform_control_helper.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include <xray/editor/world/engine.h>
#include <xray/collision/collision_object.h>
#include <xray/render/facade/debug_renderer.h>
#pragma managed( pop )

using namespace System;

namespace xray{
namespace editor{

composite_object_item::composite_object_item( )
:m_relative_transform	( NEW(math::float4x4)() ),
m_props_dictionary		( nullptr )
{}

composite_object_item::~composite_object_item( )
{
	DELETE	( m_relative_transform );
}

void composite_object_item::do_edit_transform( )
{
	level_editor^ le = m_object->owner_tool()->get_level_editor();
	le->set_transform_object( gcnew object_incompound_transform_data(this) );
}

void composite_object_item::set_edit_transform( math::float4x4 const& m )
{
	m_object->set_transform( m );
	safe_cast<object_composite^>(m_object->get_parent_object())->update_object_offset( this );
}

bool object_composite::touch_object( object_base^ o )
{
	if(!is_edit_mode())	
		return false;

	if(!m_project_item->get_selected())
		return false;


	// check transform property is is readwrite state
	int idx = index_of( o );
	m_objects[idx]->do_edit_transform();
	return				true;
}

object_composite::object_composite( tool_composite^ t )
:super					( t ),
m_tool_composite		( t )
{
	m_objects			= gcnew composite_object_item_list;
	image_index			= xray::editor_base::composite_tree_icon;
}

object_composite::~object_composite( )
{
}

void object_composite::destroy_collision	( )
{
	if ( m_collision->initialized() )
		m_collision->destroy( &debug::g_mt_allocator );
}

void object_composite::set_visible( bool bvisible )
{
	super::set_visible		(bvisible);

	for each (composite_object_item^ o in m_objects)
		o->m_object->set_visible( bvisible );
}

void object_composite::set_transform( math::float4x4 const& transform )
{
	super::set_transform			( transform );

	if(!is_edit_mode())
	{
		update_objects_transform	( );
	}else
	{
		for each (composite_object_item^ itm in m_objects)
			update_object_offset		( itm );
	}
}

bool object_composite::get_persistent( )
{
	if(is_edit_mode())
		return true;
	else
		return super::get_persistent( );
}

void object_composite::on_selected( bool selected )
{
	super::on_selected		(selected);

	for each (composite_object_item^ o in m_objects)
		o->m_object->owner_project_item->set_selected	( selected );
}

void object_composite::load_contents( )
{
	initialize_collision ();
	
	m_aabbox->identity();

	for each (composite_object_item^ o in m_objects)
	{
//		o->m_object->load_contents();
		m_aabbox->modify(o->m_relative_transform->c.xyz());
	}
	m_aabbox->modify( get_transform() );
}

void object_composite::unload_contents( bool /*bdestroy*/ )
{
	//destroy_collision();

	//for each (composite_object_item^ o in m_objects)
	//	o->m_object->unload_contents( bdestroy );
}

void object_composite::initialize_collision( )
{
	//if( m_collision->initialized() )
	//	m_collision->destroy			( );

	//collision::geometry* geom			= &*collision::new_box_geometry( &debug::g_mt_allocator, float3(1.0f, 1.0f, 1.0f) );
	//m_collision->create_from_geometry	(
	//				true,
	//				this,
	//				geom,
	//				editor_base::collision_object_type_dynamic | editor_base::collision_object_type_touch
	//			);

	//m_collision->insert					( );
	//update_collision_transform			( );
}

void object_composite::render( )
{
	super::render					( );

	//for each (composite_object_item^ o in m_objects)
	//	o->m_object->render					();
}

void object_composite::add_object( object_base^ o, math::float4x4* offset )
{
	if( -1!=index_of(o) )
	{
		LOG_WARNING				("object [%s] already a member of compound [%s]", o->get_name(), get_name());
		return;
	}
	R_ASSERT					( nullptr!=o->owner_project_item );
	composite_object_item^ new_item	= gcnew composite_object_item;
	new_item->m_object			= o;
	m_objects->Add				( new_item );

	if(offset)
	{
		*new_item->m_relative_transform = *offset;
	}else
	{
		update_object_offset		( new_item );
	}

	o->set_parent					( this );
	get_project()->on_item_added	( o->owner_project_item );  //push into cell manager
}

int object_composite::index_of( object_base^ o )
{
	for( int result =0; result< m_objects->Count; ++result)
	{
		if(m_objects[result]->m_object == o)
			return result;
	}

	return -1;
}

void object_composite::destroy_contents()
{
	for(int i=0; i<m_objects->Count; ++i)
	{
		composite_object_item^ child	= m_objects[i];
		tool_base^ tool					= child->m_object->owner_tool();
		get_project()->on_item_removed	( child->m_object->owner_project_item );
		tool->destroy_object			( child->m_object );
		delete							( child );
	}
	m_objects->Clear					( );
}

void object_composite::update_objects_transform( )
{
	m_aabbox->identity();

	for each (composite_object_item^ itm in m_objects)
	{
		update_object_transform( itm );
		m_aabbox->modify(itm->m_relative_transform->c.xyz());
	}
	m_aabbox->modify( get_transform() );

}

void object_composite::update_object_transform( composite_object_item^ itm )
{
	float4x4 pt = get_transform( );

	float4x4 object_transform		= *(itm->m_relative_transform) * pt;
	itm->m_object->set_transform	( object_transform );
}


void object_composite::update_object_offset( composite_object_item^ itm )
{
	// calc object relative transformation
	float4x4 pt					= get_transform( );
	float4x4					ti;
	ti.try_invert				( pt );

	float4x4 trel				= itm->m_object->get_transform() * ti;
	*(itm->m_relative_transform) = trel;
}

typedef System::Collections::Generic::Dictionary<System::String^, string_list^>	parsed_props;

void parse_public_props( configs::lua_config_value t, parsed_props^ dict)
{
	configs::lua_config_iterator it			= t.begin();
	configs::lua_config_iterator it_e		= t.end();
	cli::array<System::String^>^ tokens;
	for(; it!=it_e; ++it)
	{
		System::String^ s	= gcnew System::String((pcstr)*it);
		tokens				= s->Split('|');
		R_ASSERT			(tokens->Length==2);

		if(!dict->ContainsKey(tokens[0]) )
			dict->Add(tokens[0], gcnew string_list);

		dict[tokens[0]]->Add(tokens[1]);
	}
}

void object_composite::setup_object_props( composite_object_item^ itm )
{
	R_ASSERT								(itm->m_props_dictionary==nullptr);
	itm->m_props_dictionary					= gcnew composite_object_item_public_props;
	wpf_controls::property_container^ cont	= itm->m_object->get_property_container();

	lib_item^ li							= m_tool_composite->m_library->get_library_item_by_guid( get_library_guid() );
	configs::lua_config_value t				= (*(li->m_config))["public_properties"];
	parsed_props							dict;
	parse_public_props						( t, %dict );

	string_list^ current_list = dict.ContainsKey(itm->m_object->get_name()) ? dict[itm->m_object->get_name()] : nullptr;
	
	for each( wpf_controls::property_descriptor^ pd in cont->properties)
	{
		bool value = ( (current_list!=nullptr) && current_list->Contains(pd->full_name));
		itm->m_props_dictionary->Add	( pd->full_name, value );
	}
}

void object_composite::get_public_properties_container	( wpf_controls::property_container^ to_container )
{
	lib_item^ li							= m_tool_composite->m_library->get_library_item_by_guid( get_library_guid() );	
	if(!li)
		return;

	configs::lua_config_value t				= (*(li->m_config))["public_properties"];
	
	parsed_props							dict;
	parse_public_props						( t, %dict );

	for each (composite_object_item^ itm in m_objects)
	{
		string_list^ current_list = dict.ContainsKey(itm->m_object->get_name()) ? dict[itm->m_object->get_name()] : nullptr;

		wpf_controls::property_container^ item_container		= itm->m_object->get_property_container( );
		item_container->owner									= itm->m_object;
		wpf_controls::property_container^ public_container		= gcnew wpf_controls::property_container;
		public_container->owner									= itm->m_object;
		
		for each( wpf_controls::property_descriptor^ pd in item_container->properties)
		{
			if(current_list && current_list->Contains(pd->full_name))
			{
				wpf_controls::property_descriptor^ copy = gcnew wpf_controls::property_descriptor(pd);
				public_container->properties->add		( copy );
			}

			pd->is_read_only = true;
		}
		if(public_container->properties->count!=0)
			to_container->properties->add_container		( itm->m_object->get_name(), "Public properties", "no description", public_container );
	}

}

wpf_controls::property_container^ object_composite::get_property_container( )
{
	wpf_controls::property_container^ cont	= gcnew wpf_controls::property_container;
	xray::editor_base::object_properties::initialize_property_container	( this, cont );
	cont->properties["general/library name"]->is_read_only = true;

	get_public_properties_container( cont );

	for each (composite_object_item^ itm in m_objects)
	{
		wpf_controls::property_container^ item_container		= itm->m_object->get_property_container( );
		item_container->owner									= itm->m_object;		
		cont->properties->add_container			( itm->m_object->get_name(), "objects in group", "no description", item_container )->is_read_only = true;	
	}	
	return cont;
}

wpf_controls::property_container^ object_composite::get_object_property_container( object_base^ o )
{
	wpf_controls::property_container^ cont = o->get_property_container();
	
	if( is_edit_mode() )
	{
		int idx				= index_of( o );
		R_ASSERT								(idx!=-1);
		composite_object_item_public_props^ public_props = m_objects[idx]->m_props_dictionary;
		R_ASSERT								(public_props!=nullptr);

		for each (wpf_controls::property_descriptor^ desc in cont->properties)
		{
			if(desc->is_read_only)
				continue;

			wpf_controls::property_descriptor^ chk = 
				gcnew wpf_controls::property_descriptor(
					"public check",
					gcnew wpf_controls::dictionary_item_property_value< System::String^, bool >(public_props, desc->full_name)
				);

			desc->inner_properties->Add("public check", chk);
		}
	}

	return cont;
}

object_base^ object_composite::get_child_by_name	( System::String^ name )
{
	for each ( composite_object_item^ item in m_objects )
	{
		if ( item->m_object->get_name() == name )
			return item->m_object;
	}
	return nullptr;
}


} //namespace editor
} //namespace xray
