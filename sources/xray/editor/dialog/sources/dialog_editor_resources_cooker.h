////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_EDITOR_RESOURCES_COOKER_H_INCLUDED
#define DIALOG_EDITOR_RESOURCES_COOKER_H_INCLUDED

#pragma managed( push, off )

#include <xray/resources_cook_classes.h>

namespace xray {
namespace dialog_editor {

class dialog_editor_resources_cooker : public resources::translate_query_cook
{
public:
					dialog_editor_resources_cooker	();
	virtual void	translate_query					(xray::resources::query_result_for_cook & parent);
	virtual void	delete_resource					(resources::resource_base * resource);
			void	on_loaded						(resources::queries_result & result);
}; // class dialog_editor_resources_cooker

} // namespace dialog_editor
} // namespace xray

#pragma managed( pop )

#endif // #ifndef DIALOG_EDITOR_RESOURCES_COOKER_H_INCLUDED