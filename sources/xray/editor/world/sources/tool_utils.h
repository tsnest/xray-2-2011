////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TOOL_UTILS_H_INCLUDED
#define TOOL_UTILS_H_INCLUDED

#include "tool_base.h"
#include <xray/render/base/world.h>

namespace xray{
namespace editor{

ref class utils_tool_tab;

public ref class tool_utils :public tool_base
{
	typedef tool_base super;
public:
						tool_utils				(detail::world& w);
	virtual				~tool_utils				();
	
	virtual object_base^ load_object			(xray::configs::lua_config_value const& t) override;
	virtual object_base^ create_object			(System::String^ name) override;
	virtual object_base^ create_raw_object		(System::String^ id) override;

	virtual void		destroy_object			(object_base^ o) override;
	virtual System::String^ type_name			()  override		{return "utils";};

	virtual void		select_library_name		(System::String^ name) override;
	void				load_library			();
	render::base::visual_ptr get_preview_model	(u32 idx) {return *(m_preview_models[idx]);}
	virtual	void		save_library			() override;

protected:
	virtual void		on_library_loaded		() override;

private:
			void		on_preview_visual_ready	(resources::unmanaged_resource_ptr data);
			void		fill_ui					();

	utils_tool_tab^								m_tool_tab;
	cli::array<render::base::visual_ptr*>^		m_preview_models;
}; // class tool_utils

} // namespace editor
} // namespace xray

#endif // #ifndef TOOL_UTILS_H_INCLUDED