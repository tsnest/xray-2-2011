////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_MODIFIER_PAINTER_H_INCLUDED
#define TERRAIN_MODIFIER_PAINTER_H_INCLUDED

#include "terrain_modifier_control.h"
struct FIBITMAP;

namespace xray {

namespace editor {

enum class painter_mode{texture=0, diffuse=1, both=3};

public ref class terrain_modifier_painter_base abstract : public terrain_modifier_control_base
{
	typedef terrain_modifier_control_base	super;
public:
								terrain_modifier_painter_base	(level_editor^ le, tool_terrain^ tool);
	virtual						~terrain_modifier_painter_base	();
	virtual		void			start_input					( int mode, int button, collision::object const* coll, math::float3 const& pos )	override;
	virtual		void			execute_input				( )							override;
	virtual		void			end_input					( )							override;

	virtual		void			load_settings				( RegistryKey^ key )		override;
	virtual		void			save_settings				( RegistryKey^ key )		override;
protected:
	virtual		void			on_working_set_changed		( ) override;
	virtual		void			create_command				( ) override;
	virtual		void			create_cursor				( render::scene_ptr const& scene )	override;

	xray::timing::timer*		m_timer;
	key_vertex_dict				m_stored_values_list;
}; // class terrain_modifier_painter_base

public ref class terrain_modifier_detail_painter : public terrain_modifier_painter_base
{
	typedef terrain_modifier_painter_base	super;
public:
								terrain_modifier_detail_painter		(level_editor^ le, tool_terrain^ tool);
	virtual						~terrain_modifier_detail_painter	();
	virtual		void			end_input					() override;
	virtual		void			do_work						()	override;

	virtual		void			load_settings			(RegistryKey^ key)	override;
	virtual		void			save_settings			(RegistryKey^ key)	override;
public:
	property System::String^	active_texture{
		System::String^ get		()						{return m_active_texture;}
	};


	property	painter_mode					mode;
				void				setup_diffuse_texture	( System::String^ fname );
				void				setup_active_texture	( System::String^ value );
				void				set_active_layer		( int  layer_id );
protected:
				void				active_texture_options_loaded( resources::queries_result& data );
	virtual		void				on_working_set_changed	( ) override;

				u32					get_diffuse_color		( float3 local_position );
	FIBITMAP*						m_diffuse_texture;
	u32								m_diffuse_size_pixels;
	u32								m_diffuse_size_meters;
	
	System::String^					m_active_texture;
	configs::binary_config_ptr*		m_active_texture_options_config;
//	typedef System::Collections::Generic::Dictionary<terrain_node_key, int>		id_layer_dict;
//	id_layer_dict					m_paint_layers;
	int m_active_layer;
}; // terrain_modifier_detail_painter

public ref class terrain_modifier_diffuse_painter : public terrain_modifier_painter_base
{
	typedef terrain_modifier_painter_base	super;
public:
								terrain_modifier_diffuse_painter	( level_editor^ le, tool_terrain^ tool );
	virtual						~terrain_modifier_diffuse_painter	( );
	virtual		void			do_work								( )	override;

	virtual		void			load_settings						( RegistryKey^ key )	override;
	virtual		void			save_settings						( RegistryKey^ key )	override;
protected:
				u32				get_diffuse_color					( float3 local_position );
public:
	[DisplayNameAttribute("color"), DescriptionAttribute("tool color"), CategoryAttribute("general")]
	property	xrayColor paint_color;
}; // terrain_modifier_detail_painter

} // namespace editor
} // namespace xray

#endif // #ifndef TERRAIN_MODIFIER_PAINTER_H_INCLUDED