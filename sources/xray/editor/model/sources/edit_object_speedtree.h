////////////////////////////////////////////////////////////////////////////
//	Created		: 22.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EDIT_OBJECT_SPEEDTREE_H_INCLUDED
#define EDIT_OBJECT_SPEEDTREE_H_INCLUDED

#include "model_editor_object.h"
#include <xray/render/facade/model.h>

namespace xray {
namespace model_editor {

ref class edit_object_speedtree;
ref class property_grid_panel;

ref class speedtree_surface
{
public:
			speedtree_surface							( edit_object_speedtree^ parent, System::String^ name );
			~speedtree_surface							( );

			void	save_material_settings				( configs::lua_config_value& t );
			bool	load_material_settings				( configs::lua_config_value const& t );

	property System::String^ name{
		System::String^		get							( )	{ return m_name;}
	}
	property System::String^ material_name{
		System::String^		get							( )	{ return m_material_name;}
		void				set							( System::String^ value );
	}

	void					on_selected					( System::Boolean value );
	void					apply_material				( );

private:

	void					query_material				( );
	void					on_material_ready			( resources::queries_result& data );

	bool								m_visible;
	bool								m_selected;
	edit_object_speedtree^				m_parent_object;
	resources::unmanaged_resource_ptr*	m_material;
	System::String^						m_material_name;
	System::String^						m_name;
};

ref class edit_object_speedtree : public model_editor_object
{
	typedef model_editor_object super;
public:
						edit_object_speedtree		( model_editor^ parent );
	virtual				~edit_object_speedtree		( );

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
	virtual	void		action_refresh_preview_model( ) override;
	virtual	void		action_draw_object_collision( ) override;
	virtual	void		action_view_isolate_selected( ) override;

	render::speedtree_instance_ptr get_model		( ) { return *m_speedtree_instance_ptr; }
private:

	void				action_surfaces_panel_view	( );
	bool				surfaces_panel_visible		( );

	xray::editor::wpf_controls::property_container^	get_surfaces_property_container	( );

	void			on_model_loaded					( resources::queries_result& data );
	void			on_model_config_loaded			( resources::queries_result& data );
	void			refresh_surfaces_panel			( );
	void			load_model_from_config			( );

	System::Collections::Generic::Dictionary<System::String^, speedtree_surface^>	m_surfaces;
	xray::render::speedtree_instance_ptr*		m_speedtree_instance_ptr;
	property_grid_panel^						m_object_surfaces_panel;
	configs::lua_config_ptr*					m_model_config;
}; // class edit_object_speedtree

} // namespace model_editor
} // namespace xray

#endif // #ifndef EDIT_OBJECT_SPEEDTREE_H_INCLUDED