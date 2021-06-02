////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_BASE_H_INCLUDED
#define TOOL_BASE_H_INCLUDED

#include "project_defines.h"
#include "objects_library.h"
#include <xray/render/engine/base_classes.h>

namespace xray {
namespace editor {

ref class level_editor;
ref class tool_base;
ref class window_ide;
interface class tool_tab;
class editor_world;

public ref class tool_base abstract
{
public:
						tool_base					( level_editor^ le, System::String^ name );
	virtual				~tool_base					( );
	virtual void		tick						( ){}


	virtual object_base^ load_object				( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )	abstract; // load
	virtual object_base^ create_library_object_instance( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )	abstract; // create new instance
	virtual void		on_library_object_created	( project_item_object^ ) {};

	virtual void		on_new_project				( render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) { XRAY_UNREFERENCED_PARAMETERS( &scene, &scene_view ); }
			void		on_library_files_loaded		( resources::queries_result& data );
	virtual	void		clear_resources				( ){};

	virtual void		load_settings				( RegistryKey^ );
	virtual void		save_settings				( RegistryKey^ );
	virtual void		save_library				( )					{ UNREACHABLE_CODE(); }

	virtual void		destroy_object				( object_base^ o )						abstract;
	virtual tool_tab^	ui							( )										abstract;
	virtual void		on_project_clear			( )					{};
	virtual bool		has_uncommited_changes		( System::String^%  )				{ return false;}
	virtual void		item_double_clicked			( lib_item_base^ ){};

			void		select_library_name			( System::String^ name );
	System::String^		get_selected_library_name	( )									{ return m_selected_library; }

	System::String^		name						( )									{ return m_tool_name; }

	window_ide^			ide							( );

	level_editor^		get_level_editor			( )									{ return m_level_editor; }

			bool		ready						( )									{ return m_b_ready; }
			void		on_library_changed			( )									{ on_library_loaded(); };

	objects_library^	m_library;
	property int	default_image_index;
	virtual void	initialize						( ){};
protected:
	virtual void		on_library_loaded			( ){};
			void		on_commit_library_item		( lib_item^ lib_item );

	System::String^				m_tool_name;
	level_editor^				m_level_editor;
	System::String^				m_selected_library;
	bool						m_b_ready;

}; // class tool_base


} // namespace editor
} // namespace xray

#endif // #ifndef TOOL_BASE_H_INCLUDED