////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game \ - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_EDITOR_ENGINE_H_INCLUDED
#define XRAY_EDITOR_ENGINE_H_INCLUDED

namespace xray {
	
namespace engine{
	class console;	
} //namespace math

namespace render {
	class world;
	class scene; 
	class scene_view;
} // namespace render

namespace sound {
	struct world;
} // namespace sound
namespace ui {	struct world;} // namespace ui


namespace editor {

struct XRAY_NOVTABLE engine {
	virtual	bool			on_before_editor_tick	( ) = 0;
	virtual	void			on_after_editor_tick	( ) = 0;

	virtual	void			run_renderer_commands	( ) = 0;
	virtual	void			on_resize				( ) = 0;
	virtual	void			enable_game				( bool value ) = 0;
	virtual	void			enter_editor_mode		( ) = 0;
	virtual	void 			load_level				( pcstr project_resource_name, pcstr project_resource_path=NULL ) = 0;
	virtual	pcstr			get_resources_path		( ) const = 0;
	virtual	void 			unload_level			( ) = 0;

	virtual	render::world&	get_renderer_world		( ) = 0;
	virtual	sound::world&	get_sound_world			( ) = 0;
	virtual xray::engine::console* create_editor_console	( xray::ui::world& uw ) = 0;
	virtual bool			is_application_active	( ) = 0;
	virtual void			on_application_activate	( ) = 0;
	virtual void			on_application_deactivate ( ) = 0;

	virtual bool			is_closing_timeout_set	( ) const = 0;
	virtual	int				get_exit_code			( ) const = 0;
	virtual	void			set_exit_code			( int exit_code ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( engine )
}; // class engine

} // namespace editor
} // namespace xray

#endif // #ifndef XRAY_EDITOR_ENGINE_H_INCLUDED