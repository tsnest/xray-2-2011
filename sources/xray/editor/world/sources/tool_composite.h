////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_COMPOSITE_H_INCLUDED
#define TOOL_COMPOSITE_H_INCLUDED

#include "tool_base.h"

namespace xray {
namespace editor {

ref class compound_tool_tab;
ref class object_composite;
typedef System::Collections::Generic::List<System::String^>		compound_details_list;

public ref class tool_composite : public tool_base {
	typedef tool_base	super;
public:
						tool_composite			( level_editor^ le );
	virtual				~tool_composite			( );

	virtual object_base^ load_object			( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual object_base^ create_library_object_instance	( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view ) override;
	virtual void		destroy_object			( object_base^ o ) override;

	virtual tool_tab^	ui						( ) override;
	virtual void		on_project_clear		( ) override;
	virtual bool		has_uncommited_changes		( System::String^% reason ) override;

	bool				commit_compound_object	( object_composite^ o );
	void				undo_current_edit_object( );
	void				get_library_item_details(System::String^ library_item_name, compound_details_list^ dest );
	pcstr				get_library_module_name	( );
	virtual void		save_library			( )	override;
protected:
	virtual void		on_library_loaded		( ) override;
			void		fill_tree_view			( tree_node_collection^ nodes );
			void		load_library			( );
			bool		valid_library_name		( System::String^ s );
			void		on_library_changed		( System::Guid lib_guid );

	compound_tool_tab^	m_tool_tab;
	object_composite^	m_current_edit_object;
	configs::lua_config_ptr*	m_backup_cfg;
public:
	property object_composite^	current_edit_object{
		object_composite^	get		( )						{ return m_current_edit_object;}
		void				set		( object_composite^ o );
	}

}; // class tool_compound

} // namespace editor
} // namespace xray

#endif // #ifndef TOOL_COMPOSITE_H_INCLUDED