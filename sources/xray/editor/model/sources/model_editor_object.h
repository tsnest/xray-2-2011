////////////////////////////////////////////////////////////////////////////
//	Created		: 21.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_EDITOR_OBJECT_H_INCLUDED
#define MODEL_EDITOR_OBJECT_H_INCLUDED

namespace xray {
namespace model_editor {

ref class model_editor;
using WeifenLuo::WinFormsUI::Docking::IDockContent;

ref class model_editor_object abstract
{
public:
						model_editor_object				( model_editor^ parent );
	virtual void		tick							( );
	virtual bool		save							( );
	virtual void		revert							( );
			bool		has_changes						( );
			void		set_modified					( );
			void		reset_modified					( );
	virtual void		load							( System::String^ model_full_name );
	virtual void		clear_resources					( );
	virtual void		set_default_layout				( )										= 0;
	virtual	bool		complete_loaded					( )										= 0;

	virtual	void		reset_selection					( )										= 0;
	System::String^		full_name						( );
	System::String^		type							( )					{ return m_type_name; }
	virtual IDockContent^ find_dock_content				( System::String^ persist_string )		= 0;

	virtual math::aabb  focused_bbox					( )										= 0;
	model_editor^		get_model_editor				( )					{ return m_model_editor; }
//actions
	virtual void		register_actions				( bool bregister );
	virtual	void		action_refresh_preview_model	( )					{};
	virtual	void		action_draw_object_collision	( )					{ m_draw_object_collision=!m_draw_object_collision; }
			bool		get_draw_object_collision		( )					{ return m_draw_object_collision; }
	virtual	void		action_view_isolate_selected	( )					{ m_view_isolate_selected=!m_view_isolate_selected;}
			bool		get_view_isolate_selected		( )					{ return m_view_isolate_selected; }
protected:
			void		action_apply					( )				{ save(); }
	bool				m_changed;
	bool				m_view_isolate_selected;
	bool				m_draw_object_collision;
	System::String^		m_full_name;
	model_editor^		m_model_editor;
	System::String^		m_type_name;
}; // class model_editor_object


} // namespace model_editor
} // namespace xray

#endif // #ifndef MODEL_EDITOR_OBJECT_H_INCLUDED