////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_COMPOUND_H_INCLUDED
#define OBJECT_COMPOUND_H_INCLUDED

#include "project_defines.h"
#include "object_base.h"

namespace xray {
namespace editor {
ref class tool_compound;

public ref class object_compound :public object_base
{
	typedef object_base		super;
public:
					object_compound			(tool_compound^ t);
	virtual			~object_compound		();
	virtual void	load					(xray::configs::lua_config_value const& t) override;
	virtual void	save					(xray::configs::lua_config_value t) override;
	virtual void	render					() override;
	virtual void	set_selected			(bool selected) override;
	virtual	void	set_visible				(bool bvisible) override;

	u32				count					() 					{return m_objects->Count;}
	object_base^	get_object				(u32 i);
	bool			add_object				(object_base^ o);
	bool			remove_object			(object_base^ o, bool b_destroy);
	void			set_pivot				(float4x4 const& m);
	void			block_collision_rebuild	();
	void			unblock_collision_rebuild();
private:
			void	initialize_collision	();
			void	on_child_object_collision_initialized(object_collision^ collision);

	object_list^	m_objects;
	tool_compound^	m_tool_compound;
	float3*			m_pivot_offset;
	bool			m_block_collision;
public:
	[DisplayNameAttribute("content"), DescriptionAttribute("object content"), CategoryAttribute("general"), 
	ReadOnlyAttribute(false)]
	property object_list^ objects{
		object_list^	get()	{return m_objects;}
	}

}; // class object_compound

} //namespace editor
} // namespace xray

#endif // #ifndef OBJECT_COMPOUND_H_INCLUDED