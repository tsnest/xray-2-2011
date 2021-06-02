////////////////////////////////////////////////////////////////////////////
//	Created		: 04.08.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef COMPOSITE_SOUND_COMMANDS_H_INCLUDED
#define COMPOSITE_SOUND_COMMANDS_H_INCLUDED

#include "sound_editor.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using xray::editor::controls::tree_node;

namespace xray {
namespace sound_editor {

	ref class composite_sound_document;

	public ref class composite_sound_add_items_command: public xray::editor_base::command
	{
	public:
		composite_sound_add_items_command	(composite_sound_document^ d, String^ root_path, List<String^>^ names_list, sound_object_type t);
		~composite_sound_add_items_command	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		composite_sound_document^	m_document;
		pstr						m_cfg_string;
		String^						m_root_path;
	}; // class composite_sound_add_items_command

	public ref class composite_sound_remove_items_command: public xray::editor_base::command
	{
	public:
		composite_sound_remove_items_command	(composite_sound_document^ d, List<tree_node^>^ items_list);
		~composite_sound_remove_items_command	();

		virtual bool	commit					() override;
		virtual void	rollback				() override;
				void	save_childs				(tree_node^ n, xray::configs::lua_config_value& root);
				void	remove_childs			(xray::configs::lua_config_value& root);
				void	add_childs				(xray::configs::lua_config_value& root);

	private:
		composite_sound_document^	m_document;
		pstr						m_cfg_string;
		List<tree_node^>^			m_saved_nodes;
	}; // class composite_sound_remove_items_command

	public ref class composite_sound_change_objects_property_command: public xray::editor_base::command
	{
		typedef List<String^> paths_list;
	public:
		composite_sound_change_objects_property_command		(composite_sound_document^ d, List<tree_node^>^ obj_list, String^ prop_name, Object^ old_val, Object^ new_val);
		~composite_sound_change_objects_property_command	();

		virtual bool	commit								() override;
		virtual void	rollback							() override;

	private:
		composite_sound_document^	m_document;
		paths_list^					m_nodes_paths;
		String^						m_property_name;
		Object^						m_old_val;
		Object^						m_new_val;
	}; // class composite_sound_change_objects_property_command

	public ref class composite_sound_create_collection_command: public xray::editor_base::command
	{
	public:
		composite_sound_create_collection_command	(composite_sound_document^ d, String^ root_path);
		~composite_sound_create_collection_command	();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		composite_sound_document^	m_document;
		pstr						m_cfg_string;
		String^						m_root_path;
		String^						m_id;
	}; // class composite_sound_create_collection_command
} // namespace sound_editor
} // namespace xray
#endif // #ifndef COMPOSITE_SOUND_COMMANDS_H_INCLUDED