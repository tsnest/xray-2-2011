//-------------------------------------------------------------------------------------------
//	Created		: 29.06.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
//-------------------------------------------------------------------------------------------
#ifndef SOUND_DOCUMENT_EDITOR_BASE_H_INCLUDED
#define SOUND_DOCUMENT_EDITOR_BASE_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace xray::editor::controls;

namespace xray {
namespace sound_editor {

	ref class sound_editor;

	public enum class sound_document_type 
	{
		collection_document = 1,
		composite_document	= 2,
		scene_document		= 3
	}; // enum class sound_document_type

	public ref class sound_document_editor_base : public document_editor_base
	{
		typedef document_editor_base super;
	public:
								sound_document_editor_base	(sound_editor^ ed);
				void			load_document				(System::String^ full_path, sound_document_type mode);
		virtual	void			load_document				(System::String^ full_path) override;
				void			open_scene					();

	private:
				void			initialize_menu				();
				void			new_collection_document		(Object^, EventArgs^);
				void			new_composite_document		(Object^, EventArgs^);
				void			new_scene_document			(Object^, EventArgs^);
				document_base^	create_document				();

	public:
		ToolStripMenuItem^  m_menu_file_open;

	private:
		sound_editor^		m_editor;
		sound_document_type	m_creating_mode;
		ToolStripMenuItem^  m_menu_file_new;
		ToolStripMenuItem^  m_menu_file_new_scene;
		ToolStripMenuItem^  m_menu_file_new_composite;
		ToolStripMenuItem^  m_menu_file_new_collection;
		OpenFileDialog^		m_open_file_dialog;
	};//class sound_document_editor_base
}//namespace sound_editor
}//namespace xray
#endif // #ifndef SOUND_DOCUMENT_EDITOR_BASE_H_INCLUDED