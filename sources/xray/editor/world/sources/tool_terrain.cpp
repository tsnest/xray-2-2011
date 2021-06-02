////////////////////////////////////////////////////////////////////////////
//	Created		: 13.01.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_terrain.h"
#include "terrain_object.h"
#include "terrain_tool_tab.h"
#include "terrain_modifier_bump.h"
#include "terrain_modifier_raise_lower.h"
#include "terrain_modifier_flatten.h"
#include "terrain_modifier_smooth.h"
#include "terrain_modifier_painter.h"
#include "terrain_modifier_uv_relax.h"
#include "terrain_selection_control.h"
#include "level_editor.h"
#include "project.h"
#include "actions.h"

namespace xray {
namespace editor {

tool_terrain::tool_terrain(level_editor^ le)
:super			( le, "terrain"),
m_terrain_core	( nullptr )
{
	load_library						( );

	m_terrain_modifier_bump				= gcnew terrain_modifier_bump(le, this);
	m_terrain_modifier_raise_lower		= gcnew terrain_modifier_raise_lower(le, this);
	m_terrain_modifier_flatten			= gcnew terrain_modifier_flatten(le, this);
	m_terrain_modifier_smooth			= gcnew terrain_modifier_smooth(le, this);
	m_terrain_modifier_detail_painter	= gcnew terrain_modifier_detail_painter(le, this);
	m_terrain_modifier_uv_relax			= gcnew terrain_modifier_uv_relax(le, this);
	m_terrain_modifier_diffuse_painter	= gcnew terrain_modifier_diffuse_painter(le, this);
	m_terrain_export_control			= gcnew terrain_selection_control(le, this);
	
	editor_base::input_engine^ input_engine = le->get_input_engine();
	editor_base::gui_binder^	gui_binder	= le->get_gui_binder();

	System::String^ action_name			= nullptr;


	action_name						= "select terrain bump modifier";
	le->register_editor_control		( m_terrain_modifier_bump );
	input_engine->register_action	( gcnew action_select_editor_control	( action_name, get_level_editor(), "terrain_modifier_bump" ) , "");
	gui_binder->register_image		( "base", action_name, editor_base::terrain_bump );

	action_name						= "select terrain raise-lower modifier";
	le->register_editor_control		( m_terrain_modifier_raise_lower );
	input_engine->register_action	( gcnew action_select_editor_control( action_name, get_level_editor(), "terrain_modifier_raise_lower" ) , "");
	gui_binder->register_image		( "base", action_name, editor_base::terrain_raise_lower );

	action_name						= "select terrain flatten modifier";
	le->register_editor_control		( m_terrain_modifier_flatten );
	input_engine->register_action	( gcnew action_select_editor_control( action_name, get_level_editor(), "terrain_modifier_flatten" ) , "");
	gui_binder->register_image		( "base", action_name, editor_base::terrain_flatten );

	action_name						= "select terrain smooth modifier";
	le->register_editor_control		( m_terrain_modifier_smooth );
	input_engine->register_action	( gcnew action_select_editor_control( action_name, get_level_editor(), "terrain_modifier_smooth" ) , "");
	gui_binder->register_image		( "base", action_name, editor_base::terrain_smooth );

	action_name						= "select terrain detail painter modifier";
	le->register_editor_control		( m_terrain_modifier_detail_painter );
	input_engine->register_action	( gcnew action_select_editor_control( action_name, get_level_editor(), "terrain_modifier_detail_painter" ) , "");
	gui_binder->register_image		( "base", action_name, editor_base::terrain_painter );

	action_name						= "select terrain diffuse painter modifier";
	le->register_editor_control		( m_terrain_modifier_diffuse_painter );
	input_engine->register_action	( gcnew action_select_editor_control( action_name, get_level_editor(), "terrain_modifier_diffuse_painter" ) , "");
	gui_binder->register_image		( "base", action_name,	editor_base::terrain_diffuse_painter );

	action_name						= "select terrain selection control";
	le->register_editor_control		( m_terrain_export_control );
	input_engine->register_action	( gcnew action_select_editor_control( action_name, get_level_editor(), "terrain_selection_control" ) , "");
	gui_binder->register_image		( "base", action_name, editor_base::terrain_selector );
	gui_binder->get_image			( "select terrain selection control" );

	action_name						= "select terrain detail uv relaxer";
	le->register_editor_control		( m_terrain_modifier_uv_relax );
	input_engine->register_action	( gcnew action_select_editor_control( action_name, get_level_editor(), "terrain_modifier_uv_realx" ), "");
	gui_binder->register_image		( "base", action_name, editor_base::terrain_uv_relax );

	gui_binder->get_image			( "select terrain selection control" );

	m_tool_tab 						= gcnew terrain_tool_tab(this);
}

tool_terrain::~tool_terrain()							
{
}

void tool_terrain::clear_resources( )
{
	get_level_editor()->unregister_editor_control			(m_terrain_modifier_bump);
	delete m_terrain_modifier_bump;
	get_level_editor()->unregister_editor_control			(m_terrain_modifier_raise_lower);
	delete m_terrain_modifier_raise_lower;
	get_level_editor()->unregister_editor_control			(m_terrain_modifier_flatten);
	delete m_terrain_modifier_flatten;
	get_level_editor()->unregister_editor_control			(m_terrain_modifier_smooth);
	delete m_terrain_modifier_smooth;
	get_level_editor()->unregister_editor_control			(m_terrain_modifier_detail_painter);
	delete m_terrain_modifier_detail_painter;
	get_level_editor()->unregister_editor_control			(m_terrain_modifier_uv_relax);
	delete m_terrain_modifier_uv_relax;
	get_level_editor()->unregister_editor_control			(m_terrain_modifier_diffuse_painter);
	delete m_terrain_modifier_diffuse_painter;
	
	get_level_editor()->unregister_editor_control			(m_terrain_export_control);
	delete m_terrain_export_control;
}

tool_tab^ tool_terrain::ui()
{
	return m_tool_tab;
}

object_base^ tool_terrain::create_raw_object( System::String^ id, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	object_base^ result			= nullptr;
	if(id=="base:terrain_node")
	{
		ASSERT			(m_terrain_core!=nullptr);
		result			= gcnew terrain_node(this, m_terrain_core);
	}else
	if(id=="base:terrain_core")
	{
		ASSERT			(m_terrain_core==nullptr);
		m_terrain_core	= gcnew terrain_core(this, scene, scene_view);
		result			= m_terrain_core;
	}

	ASSERT(result, "raw object not found", unmanaged_string(id).c_str());
	return result;
}

object_base^ tool_terrain::load_object( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	object_base^ result			= nullptr;
	System::String^ library_name = gcnew System::String(t["lib_name"]);

	if(library_name->StartsWith("base:"))
	{
		result						= create_raw_object( library_name, scene, scene_view );
		result->set_library_name	( library_name );
	}else
	{
		lib_item^ li			= m_library->extract_library_item( t );
		System::String^ raw		= gcnew System::String((*(li->m_config))["lib_name"]);
		result					= create_raw_object(raw, scene, scene_view);
		result->set_library_name( li->full_path_name() );
	}

	result->load_props			( t );

	return						result;
}

object_base^ tool_terrain::create_library_object_instance( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	lib_item^ li				= m_library->get_library_item_by_full_path( name, true );
	System::String^ ln			= gcnew System::String( (*(li->m_config))["lib_name"] );
	object_base^ result			= create_raw_object( ln, scene, scene_view );
	result->load_props			( *li->m_config );
	result->set_library_name	( name );

	return						result;
}

void tool_terrain::destroy_object( object_base^ o )
{
	if(m_terrain_core==o)
	{
		m_terrain_core->destroy_all();
		m_terrain_core			= nullptr;
	}

	delete o;
}


void tool_terrain::load_library( )
{
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &super::on_library_files_loaded), g_allocator);
	resources::query_resource(
		"resources/library/terrain/terrain.library",
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void tool_terrain::load_settings( RegistryKey^ key )
{
	super::load_settings							(key);
	RegistryKey^ self_key							= get_sub_key(key, name());

	m_terrain_modifier_bump->load_settings			(self_key);
	m_terrain_modifier_raise_lower->load_settings	(self_key);
	m_terrain_modifier_flatten->load_settings		(self_key);
	m_terrain_modifier_smooth->load_settings		(self_key);
	m_terrain_modifier_detail_painter->load_settings(self_key);
	m_terrain_modifier_uv_relax->load_settings		(self_key);
	m_terrain_modifier_diffuse_painter->load_settings(self_key);
	m_terrain_export_control->load_settings			(self_key);

	self_key->Close			();
}

void tool_terrain::save_settings(RegistryKey^ key)
{
	super::save_settings							(key);
	RegistryKey^ self_key							= get_sub_key(key, name());

	m_terrain_modifier_bump->save_settings			(self_key);
	m_terrain_modifier_raise_lower->save_settings	(self_key);
	m_terrain_modifier_flatten->save_settings		(self_key);
	m_terrain_modifier_smooth->save_settings		(self_key);
	m_terrain_modifier_detail_painter->save_settings(self_key);
	m_terrain_modifier_uv_relax->save_settings		(self_key);
	m_terrain_modifier_diffuse_painter->save_settings(self_key);
	m_terrain_export_control->save_settings			(self_key);

	self_key->Close			();
}

terrain_core^ tool_terrain::get_terrain_core()
{
	return m_terrain_core;
}

void tool_terrain::tick()
{
	if(m_terrain_core)
		m_terrain_core->update();
}

void tool_terrain::focus_camera_to(terrain_node_key^ k)
{
	float distance = m_level_editor->focus_distance();
	float3 position	(k->x*64.0f+32.0f, 0.0f, k->z*64.0f-32.0f);

	m_level_editor->focus_camera_to( position, distance );
}

void tool_terrain::on_new_project( render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	object_base^ o				= create_raw_object("base:terrain_core", scene, scene_view);
	o->set_library_name			( "base:terrain_core" );
	o->set_name					( "terrain", false );
	o->load_defaults			( );

	project_item_object^ terrain_item	= gcnew project_item_object( m_level_editor->get_project(), o );
	terrain_item->assign_id		( 0 );
	terrain_item->m_guid		= System::Guid::NewGuid();
	m_level_editor->get_project()->root()->add( terrain_item );
	o->set_visible				( true );
}

} //namespace editor
} // namespace xray
