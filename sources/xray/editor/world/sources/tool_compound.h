////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_COMPOUND_H_INCLUDED
#define TOOL_COMPOUND_H_INCLUDED

#include "tool_base.h"

namespace xray {
namespace editor {

ref class compound_tool_tab;
ref class compound_object_tab;

public ref class tool_compound :public tool_base
{
	typedef tool_base	super;
public:
						tool_compound			(level_editor^ le);
	virtual				~tool_compound			();

	virtual object_base^ load_object			(xray::configs::lua_config_value const& t) override;
	virtual object_base^ create_object			(System::String^ name) override;
	virtual object_base^ create_raw_object		(System::String^ id) override;
	virtual void		destroy_object			(object_base^ o) override;

	virtual void		on_make_new_library_object() override;
	virtual void		on_edit_library_object	(System::String^ name) override;

	virtual void		get_objects_list		(names_list list, enum_list_mode mode) override;
	virtual	void		save_library			() override;
	virtual tool_tab^	ui						() override;
	
protected:
	virtual void		on_library_loaded		() override;
			void		fill_tree_view			(tree_node_collection^ nodes);
			void		load_library			();

	compound_tool_tab^	m_tool_tab;
	compound_object_tab^ m_object_tab;
}; // class tool_compound

} // namespace editor
} // namespace xray

#endif // #ifndef TOOL_COMPOUND_H_INCLUDED