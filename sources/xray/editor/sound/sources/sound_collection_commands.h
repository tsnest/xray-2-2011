////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_COLLECTION_COMMANDS_H_INCLUDED
#define SOUND_COLLECTION_COMMANDS_H_INCLUDED

#include "sound_editor.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using xray::editor::controls::tree_node;

namespace xray {
namespace sound_editor {

	ref class sound_collection_document;
	ref class sound_collection_item;

	public ref class sound_collection_add_items_command: public xray::editor_base::command
	{
	public:
		sound_collection_add_items_command	(sound_collection_document^ d, String^ root_path, List<String^>^ names_list, sound_object_type t);
		~sound_collection_add_items_command	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		sound_collection_document^	m_document;
		pstr						m_cfg_string;
		String^						m_root_path;
	}; // class sound_collection_add_items_command

	public ref class sound_collection_remove_items_command: public xray::editor_base::command
	{
	public:
		sound_collection_remove_items_command	(sound_collection_document^ d, List<tree_node^>^ items_list);
		~sound_collection_remove_items_command	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;
				void	save_childs			(tree_node^ n, xray::configs::lua_config_value& root);
				void	remove_childs		(xray::configs::lua_config_value& root);
				void	add_childs			(xray::configs::lua_config_value& root);

	private:
		sound_collection_document^	m_document;
		pstr						m_cfg_string;
		List<tree_node^>^			m_saved_nodes;
	}; // class sound_collection_remove_items_command

	public ref class sound_collection_change_objects_property_command: public xray::editor_base::command
	{
		typedef List<String^> paths_list;
	public:
		sound_collection_change_objects_property_command	(sound_collection_document^ d, List<tree_node^>^ obj_list, String^ prop_name, Object^ old_val, Object^ new_val);
		~sound_collection_change_objects_property_command	();

		virtual bool	commit								() override;
		virtual void	rollback							() override;

	private:
		sound_collection_document^	m_document;
		paths_list^					m_nodes_paths;
		String^						m_property_name;
		Object^						m_old_val;
		Object^						m_new_val;
	}; // class sound_collection_change_objects_property_command

	public ref class sound_collection_create_collection_command: public xray::editor_base::command
	{
	public:
		sound_collection_create_collection_command	(sound_collection_document^ d, String^ root_path);
		~sound_collection_create_collection_command	();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		sound_collection_document^	m_document;
		pstr						m_cfg_string;
		String^						m_root_path;
		String^						m_id;
	}; // class sound_collection_create_collection_command
} // namespace sound_editor
} // namespace xray
#endif // #ifndef SOUND_COLLECTION_COMMANDS_H_INCLUDED