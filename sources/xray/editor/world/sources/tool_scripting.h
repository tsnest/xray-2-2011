////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SCENES_TAB_H_INCLUDED
#define SCENES_TAB_H_INCLUDED

#include "tool_base.h"

using System::String;
using namespace System::Collections::Generic;

namespace xray {
namespace editor {

ref class scripting_tool_tab;
ref class object_scene;

public ref class tool_scripting : public tool_base {
	typedef tool_base	super;
public:
	tool_scripting			( level_editor^ le );

	virtual object_base^	load_object						( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )	override; // load
	virtual object_base^	create_library_object_instance	( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )					override; // create new instance
	virtual void			on_library_object_created		( project_item_object^ obj)				override;
	virtual void			destroy_object					( object_base^ o )						override;
	void					load_library					( );
	virtual tool_tab^		ui								( )										override;
	virtual void			load_contents					( )		{}
	virtual void			unload_contents					( bool ) {}
	virtual void			on_new_project					( render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )		override;

	object_base^			create_raw_object				( System::String^ id );

protected:
	virtual void			on_library_loaded				( )										override;
private:
	
	void					fill_tree_view					( tree_node_collection^ nodes );
	

public: 
	static System::Collections::Generic::Dictionary< System::String^, System::Collections::Generic::List< System::String^ > ^ > ^	behaviour_types_by_object_types;
	static System::Collections::Generic::Dictionary< System::String^, System::String^ > ^											job_resource_type_by_job_type;

private:
	scripting_tool_tab^							m_tool_tab;

}; // class tool_scripting


} // namespace editor
} // namespace xray

#endif // #ifndef SCENES_TAB_H_INCLUDED