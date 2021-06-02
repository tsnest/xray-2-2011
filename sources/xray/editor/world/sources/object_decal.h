////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_DECAL_H_INCLUDED
#define OBJECT_DECAL_H_INCLUDED

#include "object_base.h"

#include <xray/render/engine/base_classes.h>

namespace xray {
	namespace render{
		struct material_effects_instance_cook_data;
	}
namespace editor {

ref class object_decal: public object_base
{
	typedef object_base super;
public:
					object_decal			(tool_base^ t, render::scene_ptr const& scene);
					~object_decal			();
	virtual void	save					(configs::lua_config_value t)			override;
	virtual void	load_props				(configs::lua_config_value const& t)	override;
	virtual void	load_contents			()										override;
	virtual void	unload_contents			(bool)									override;
	virtual void	destroy_collision		()									override;
	virtual void	render					() override;
	virtual void	load_defaults			() override;
	virtual void	set_visible				(bool bvisible) override;
	virtual void	set_transform			(math::float4x4 const& transform) override;
	virtual void	on_selected				(bool is_selected)	override;
			void	initialize_collision	();
			void	update_render_decal		();
	virtual wpf_controls::property_container^ get_property_container()				override;
	
	[DisplayNameAttribute("decal material"), DescriptionAttribute("decal material"), CategoryAttribute("decal")] 
	property System::String^ material_name{
			System::String^		get			() { return m_material_name; }
			void				set			(System::String^ value);
	}
	
	[DisplayNameAttribute("on particle"), DescriptionAttribute("project decal on particle geometry"), CategoryAttribute("projection")] 
	property bool			on_particle_geometry {
		bool				get		( )				{ return m_projection_on_particle_geometry; }
		void				set		( bool v );
	}
	
	[DisplayNameAttribute("on skeleton"), DescriptionAttribute("project decal on skeleton geometry"), CategoryAttribute("projection")] 
	property bool			on_skeleton_geometry {
		bool				get		( )				{ return m_projection_on_skeleton_geometry; }
		void				set		( bool v );
	}

	[DisplayNameAttribute("on speedtree"), DescriptionAttribute("project decal on speedtree geometry"), CategoryAttribute("projection")] 
	property bool			on_speedtree_geometry {
		bool				get		( )				{ return m_projection_on_speedtree_geometry; }
		void				set		( bool v );
	}
	
	[DisplayNameAttribute("on static"), DescriptionAttribute("project decal on static geometry"), CategoryAttribute("projection")] 
	property bool			on_static_geometry {
		bool				get		( )				{ return m_projection_on_static_geometry; }
		void				set		( bool v );
	}
	
	[DisplayNameAttribute("on terrain"), DescriptionAttribute("project decal on terrain geometry"), CategoryAttribute("projection")] 
	property bool			on_terrain_geometry {
		bool				get		( )				{ return m_projection_on_terrain_geometry; }
		void				set		( bool v );
	}

	[DisplayNameAttribute("clip angle"), DescriptionAttribute("clip angle"), CategoryAttribute("decal")] 
	[ValueAttribute(ValueAttribute::value_type::e_def_val, -90.0f, 90.0f, 1.0f), ReadOnlyAttribute(false),ConnectionAttribute(enum_connection::inout)]
	property float			clip_angle{
		float				get		( )				{ return m_clip_angle; }
		void				set		( float v );
	}

	[DisplayNameAttribute("alpha angle"), DescriptionAttribute("alpha angle"), CategoryAttribute("decal")] 
	[ValueAttribute(ValueAttribute::value_type::e_def_val, -90.0f, 90.0f, 1.0f), ReadOnlyAttribute(false),ConnectionAttribute(enum_connection::inout)]
	property float			alpha_angle{
		float				get		( )				{ return m_alpha_angle; }
		void				set		( float v );
	}
	
	[DisplayNameAttribute("decal far distance"), DescriptionAttribute("decal far distance"), CategoryAttribute("decal")] 
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.001f, 1000.0f, 0.001f), ReadOnlyAttribute(false),ConnectionAttribute(enum_connection::inout)]
	property float			decal_far_distance{
		float				get		( )				{ return m_decal_far_distance; }
		void				set		( float v );
	}
	
	[DisplayNameAttribute("decal height"), DescriptionAttribute("decal height"), CategoryAttribute("decal")] 
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.001f, 1000.0f, 0.001f), ReadOnlyAttribute(false),ConnectionAttribute(enum_connection::inout)]
	property float			decal_height{
		float				get		( )				{ return m_decal_height; }
		void				set		( float v );
	}

	[DisplayNameAttribute("decal width"), DescriptionAttribute("decal width"), CategoryAttribute("decal")] 
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.001f, 1000.0f, 0.001f), ReadOnlyAttribute(false),ConnectionAttribute(enum_connection::inout)]
	property float			decal_width{
		float				get		( )				{ return m_decal_width; }
		void				set		( float v );
	}
private:
			void	requery_material		();
			void	material_ready			(resources::queries_result& data);
	
	System::String^							m_material_name;
	render::scene_ptr*						m_scene;
	
	wpf_controls::property_container^		m_current_property_container;
	resources::unmanaged_resource_ptr*		m_material_ptr;
	
	bool									m_projection_on_static_geometry;
	bool									m_projection_on_skeleton_geometry;
	bool									m_projection_on_terrain_geometry;
	bool									m_projection_on_speedtree_geometry;
	bool									m_projection_on_particle_geometry;
	
	float									m_alpha_angle;
	float									m_clip_angle;
	
	float									m_decal_far_distance;
	float									m_decal_width;
	float									m_decal_height;
	
	xray::render::material_effects_instance_cook_data** m_cook_data;
}; // class object_decal

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_DECAL_H_INCLUDED
