////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_EDITOR_RESOURCE_H_INCLUDED
#define DIALOG_EDITOR_RESOURCE_H_INCLUDED

#pragma managed( push, off )

namespace xray {
namespace dialog_editor {

class dialog;
struct dialog_graph_node_layout;

typedef	resources::resource_ptr<dialog, resources::unmanaged_resource> dialog_ptr;

struct dialog_resources : public resources::unmanaged_resource
{
				dialog_resources	();
				~dialog_resources	();
	void		save				(pcstr path);
	dialog*		get_dialog			();

	dialog_ptr						m_dialog;
	dialog_graph_node_layout*		m_layout;
}; // struct dialog_resources

} // namespace dialog_editor
} // namespace xray

#pragma managed( pop )

#endif // #ifndef DIALOG_EDITOR_RESOURCE_H_INCLUDED