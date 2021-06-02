////////////////////////////////////////////////////////////////////////////
//	Created		: 17.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROJECT_DEFINES_H_INCLUDED
#define PROJECT_DEFINES_H_INCLUDED

#include <xray/render/engine/base_classes.h>

namespace xray {
namespace editor {

public delegate void execute_delegate();

typedef controls::tree_node							tree_node;
typedef System::Windows::Forms::TreeNodeCollection	tree_node_collection;

enum	enum_selection_method{ 
		enum_selection_method_set,
		enum_selection_method_add, 
		enum_selection_method_invert, 
		enum_selection_method_subtract, 
		enum_selection_method_NONE };

ref class project;
ref class object_base;
ref class project_item_base;
ref class project_item_object;
ref class project_item_folder;
ref class tool_base;
ref class level_editor;

typedef System::Collections::Generic::List<System::String^>			string_list;
typedef System::Collections::Generic::List<project_item_base^>		project_items_list;

typedef System::Collections::Generic::List<object_base^>			object_base_list;
typedef System::Collections::Generic::List<u32>						item_id_list;

public ref struct add_object_to_scene
{
	tool_base^			m_tool;
	System::String^		m_library_item_name;
};

public value struct id_matrix
{
	u32				id;
	math::float4x4*	matrix;
	math::float3*	pivot;
};	
typedef System::Collections::Generic::List<id_matrix>		id_matrices_list;

public enum class subst_resource_result{
	subst_one,
	subst_all_by_name,
	subst_all_any_name,
	ignore_one,
	ignore_all_by_name,
	ignore_all_any_name
};

public ref class subst_record{
public:
	subst_resource_result	result;
	System::String^			source_name;
	System::String^			dest_name;
};

} // namespace editor
} // namespace xray

#endif // #ifndef PROJECT_DEFINES_H_INCLUDED