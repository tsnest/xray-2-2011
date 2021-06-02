////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include "project_defines.h"
#include "attribute.h"
#include "object_collision.h"

using xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute;
using xray::editor::wpf_controls::property_editors::attributes::external_editor_attribute;
using xray::editor_base::transform_control_base;

namespace xray {

namespace render {
	namespace editor { class renderer; }
} // namespace render
namespace editor_base { ref class ValueAttribute; }

namespace editor {



ref struct object_collision;
ref class object_base;
ref class level_editor;
ref class tool_base;
class editor_world;

object_base^ create_object( level_editor^ le, configs::lua_config_value cfg );

float3 rad2deg		( float3 const& rad );
float3 deg2rad		( float3 const& deg );
void set_scale_impl	( float3 s, float4x4& transform );

public delegate void object_message( object_base^ );

public enum class enum_terrain_interaction : int {tr_free=0, tr_vertical_snap=1, tr_physical_snap=2 };

public ref class object_base abstract
{
public:
									object_base			( tool_base^ t);
	virtual							~object_base		( );
	virtual void					load_props			( configs::lua_config_value const& t );
	virtual void					save				( configs::lua_config_value t );
	virtual void					load_defaults		( ) {}
	virtual void					destroy_collision	( ) {}
			tool_base^				owner_tool			( )								{ return m_owner_tool; }
	
			void					set_parent			( object_base^ p );
			object_base^			get_root_object		( )								{ return (m_parent_object_base) ? m_parent_object_base->get_root_object() : this; }
			object_base^			get_parent_object	( )								{ return m_parent_object_base; }

			System::String^			get_name			( );
			System::String^			get_full_name		( );
	virtual	void					set_name			( System::String^ name, bool refresh_ui );
			void					set_name_revertible	( System::String^ name );

			System::String^			get_library_name	( ) 						{ return m_lib_name; }
			System::Guid			get_library_guid	( ) 						{ return m_lib_guid; }
	virtual void					set_library_name	( System::String^ name );

	virtual void					on_selected			( bool bselected )			{ XRAY_UNREFERENCED_PARAMETERS(bselected); }
	virtual bool					get_persistent		( )							{ return false;}
	virtual bool					get_selectable		( );
	virtual bool					visible_for_project_tree( )						{ return true;}
			bool					get_visible			( )							{ return m_visible; }
	virtual	void					set_visible			( bool bvisible )			{ m_visible = bvisible; }
	virtual void					on_before_manual_delete	( )	{ if( before_manual_delete  != nullptr ) before_manual_delete( ); };
			System::Action^			before_manual_delete;
			
			u32						id					( );

	virtual void					render				( )							{}

	virtual wpf_controls::property_container^	get_property_container	( );
	virtual bool					attach_transform	( transform_control_base^ )	{ return true; }
	
	virtual enum_terrain_interaction get_terrain_interaction( )						{ return enum_terrain_interaction::tr_free; }
			float4x4 const&			get_transform		( ) 						{ return *m_transform; }
	virtual	void					set_transform		( float4x4 const& transform );

			float3 const&			get_pivot			( ) 						{ return *m_pivot; }
			void					set_pivot			( float3 const& pivot )		{ *m_pivot = pivot; }

	virtual aabb					get_aabb			( );
	virtual aabb					get_focus_aabb		( );
			float3					get_position		( )							{ return (*m_transform).c.xyz(); }
			void					set_position		( float3 p );
			void					set_position_revertible( math::float3 p );
			math::float3			get_rotation		( );
			void					set_rotation		( math::float3 p );
			void					set_rotation_revertible( math::float3 p );

			math::float3			get_scale			( );
			void					set_scale			( math::float3 p );
			void					set_scale_revertible( math::float3 p );
			virtual void			load_contents		( )							= 0;
			virtual void			unload_contents		( bool bdestroy )			= 0;
public:

	render::editor::renderer&		get_editor_renderer ( );
	render::debug::renderer&		get_debug_renderer	( );
	editor_world&					get_editor_world	( );
	project^						get_project			( );
	level_editor^					get_level_editor	( );

protected:
	void							on_property_changed	( System::String^ prop_name );
	bool							is_slave_attribute	( System::String^ prop_name );
	System::String^	check_valid_library_name	( System::String^ name );

//	virtual void					update_collision_transform( );

	tool_base^						m_owner_tool;
	object_base^					m_parent_object_base; // compound?
	System::String^					m_name;
	
	System::String^					m_lib_name;
	System::Guid					m_lib_guid;

	math::aabb*						m_aabbox;
	math::float3*					m_previous_angles;
	bool							m_visible;
	bool							m_needed_quality_;

public:
	float4x4*						m_transform; // tmp
	float3*							m_pivot;

	event object_message^			m_object_transform_changed;

	object_collision^				m_collision;

	[DisplayNameAttribute("position"), DescriptionAttribute("object position"), CategoryAttribute("general")]
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 0.0f, 0.0f), ReadOnlyAttribute(false),ConnectionAttribute(enum_connection::inout)]
	property Float3					object_position{
		Float3						get()					{return Float3(get_position());}
		void						set(Float3 p)			{set_position_revertible(p);}
	}

	[DisplayNameAttribute("rotation"), DescriptionAttribute("object rotation"), CategoryAttribute("general")]
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 0.0f, 0.0f), ReadOnlyAttribute(false),ConnectionAttribute(enum_connection::inout)]
	property Float3					object_rotation{
		Float3						get()					{return Float3(rad2deg(get_rotation()));}
		void						set(Float3 p)			{set_rotation_revertible(deg2rad(p));}
	}
	[DisplayNameAttribute("scale"), DescriptionAttribute("object scale"), CategoryAttribute("general")]
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 1.0f, 1.0f, 1.0f), ReadOnlyAttribute(false),ConnectionAttribute(enum_connection::inout)]
	property Float3					object_scale{
		Float3						get()					{return Float3(get_scale());}
		void						set(Float3 p)			{set_scale_revertible(float3(p));}
	}

	[DisplayNameAttribute("name"), DescriptionAttribute("object name"), CategoryAttribute("general")]
	[DefaultValueAttribute(""), ReadOnlyAttribute(false), MergablePropertyAttribute(false)]
	property System::String^		object_name{
		System::String^ 			get()					{return get_name();}
		void						set(System::String^ n);
	}

	[DisplayNameAttribute("library name"), DescriptionAttribute("library reference name"), CategoryAttribute("general")]
	[DefaultValueAttribute(""), ReadOnlyAttribute(false)]
	[external_editor_attribute( library_name_ui_type_editor::typeid )]
	property System::String^		object_library_name{
		System::String^ 			get()					{return get_library_name();}
		void						set(System::String^ n)	{set_library_name(n);on_property_changed("library name");}
	}

	[DisplayNameAttribute("id"), DescriptionAttribute("object identifier"), CategoryAttribute("general")]
	[DefaultValueAttribute(0), value_range_and_format_attribute(0, 100000)]
	[ReadOnlyAttribute(true), MergablePropertyAttribute(false)]
	property int					object_id{
		int							get()					{return id();}
		void						set(int)				{}
	}

	[ReadOnlyAttribute(true)]
	property int	image_index;
public:
	virtual property project_item_object^ owner_project_item
	{
		project_item_object^	get( )							{ return m_project_item; }
		virtual void			set( project_item_object^ pi )	{ m_project_item = pi; }
	}
	property object_base^				parent_object_base
	{
		object_base^			get( )							{ return m_parent_object_base; }
	}

protected:
	project_item_object^		m_project_item;
}; // class object_base


math::float4x4 place_object		( object_base^ o, float3 const& position );
void load_transform				( configs::lua_config_value const& t, math::float4x4& m );
void save_transform				( configs::lua_config_value& t, math::float4x4 const& m );

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_H_INCLUDED