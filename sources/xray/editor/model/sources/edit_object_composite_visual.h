////////////////////////////////////////////////////////////////////////////
//	Created		: 24.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EDIT_OBJECT_COMPOSITE_MESH_H_INCLUDED
#define EDIT_OBJECT_COMPOSITE_MESH_H_INCLUDED

#include "model_editor_object.h"
#include <xray/collision/collision_object.h>

namespace xray {
namespace model_editor {

ref class property_grid_panel;
ref class render_model_wrapper;
ref class edit_object_composite_visual;
class composite_visual_item_collision;

using namespace System;

using xray::editor::wpf_controls::control_containers::button;

ref class composite_visual_item : public editor_base::transform_control_object
{
	typedef editor_base::transform_control_object super;
public:
							composite_visual_item	( edit_object_composite_visual^ parent );
							~composite_visual_item	( );
	virtual void			start_modify			( editor_base::transform_control_base^ control ) override;
	virtual void			execute_preview			( editor_base::transform_control_base^ control ) override;
	virtual void			end_modify				( editor_base::transform_control_base^ control ) override;
	virtual float4x4		get_ancor_transform		( ) override;
	virtual u32				get_collision			( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% ) override;
	void					set_selected			( bool value );
	bool					get_selected			( );
	math::aabb				get_aabb				( );
public:
	[System::ComponentModel::DisplayNameAttribute("position"), CategoryAttribute("transform")]
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 0.0f, 0.0f), ReadOnlyAttribute(false)]
	property Float3					position{
		Float3						get( );
		void						set( Float3 p );
	}

	[System::ComponentModel::DisplayNameAttribute("rotation"), CategoryAttribute("transform")]
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 0.0f, 0.0f), ReadOnlyAttribute(false)]
	property Float3					rotation{
		Float3						get( );
		void						set( Float3 p );
	}
	[System::ComponentModel::DisplayNameAttribute("scale"), CategoryAttribute("transform")]
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 1.0f, 1.0f, 1.0f), ReadOnlyAttribute(false)]
	property Float3					scale{
		Float3						get( );
		void						set( Float3 p );
	}

	property System::String^ model_name{
		System::String^				get( )	{ return m_model_name; }
		void						set( System::String^ value );
	}

	render_model_wrapper^				m_model;
	math::float4x4*						m_matrix;
	math::float3*						m_pivot;
private:
	void								set_matrix			( float4x4 const& m );
	void								load_model			( );
	void								on_model_ready		( resources::queries_result& data );
	void								on_matrix_changed	( );
	void								destroy_contents	( );
	System::String^						m_model_name;
	composite_visual_item_collision*	m_active_collision_object;
	edit_object_composite_visual^		m_parent;
public:
	resources::managed_resource_ptr*	m_hq_collision_vertices;
	resources::managed_resource_ptr*	m_hq_collision_indices;
};

class composite_visual_item_collision : public collision::collision_object
{
	typedef collision::collision_object	super;
public:
					composite_visual_item_collision	( composite_visual_item^ parent );
	virtual bool	touch							( ) const;
private:
	gcroot<composite_visual_item^>	m_parent;
};


ref class edit_object_composite_visual: public model_editor_object 
{
	typedef model_editor_object super;
public:
						edit_object_composite_visual	( model_editor^ parent );
	virtual				~edit_object_composite_visual	( );

	virtual void		tick						( ) override;
	virtual bool		save						( ) override;
	virtual void		revert						( ) override;

	virtual void		load						( System::String^ model_full_name ) override;
	virtual void		clear_resources				( ) override;
	virtual void		set_default_layout			( ) override;
	virtual	bool		complete_loaded				( ) override;
	virtual	void		reset_selection				( ) override;

	virtual IDockContent^ find_dock_content			( System::String^ persist_string ) override;
	virtual math::aabb  focused_bbox				( ) override;
	
	virtual void		register_actions			( bool bregister ) override;
	virtual	void		action_refresh_preview_model( ) override		{};
	virtual	void		action_draw_object_collision( ) override;
	virtual	void		action_view_isolate_selected( ) override;
	void				add_visual_clicked			( button^ );
	void				remove_item_clicked			( button^ );
private:
	void				action_contents_panel_view	( );
	bool				contents_panel_visible		( );

	xray::editor::wpf_controls::property_container^	get_contents_property_container	( );

	void				on_model_config_loaded		( resources::queries_result& data );
	void				refresh_surfaces_panel		( );
	void				load_model_from_config		( );

	System::Collections::Generic::List<composite_visual_item^>	m_contents;
	property_grid_panel^										m_object_contents_panel;
	configs::lua_config_ptr*									m_model_config;
}; // class edit_object_composite_visual

} // namespace model_editor
} // namespace xray

#endif // #ifndef EDIT_OBJECT_COMPOSITE_MESH_H_INCLUDED