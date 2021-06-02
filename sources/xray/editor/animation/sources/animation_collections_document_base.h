////////////////////////////////////////////////////////////////////////////
//	Created		: 07.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_COLLECTIONS_DOCUMENT_BASE_H_INCLUDED
#define ANIMATION_COLLECTIONS_DOCUMENT_BASE_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace xray::editor::controls;

namespace xray
{
	namespace animation_editor
	{
		ref class animation_collections_editor;

		public ref class animation_collections_document_base : public document_editor_base
		{
			typedef document_editor_base super;
		public:
									animation_collections_document_base	( animation_collections_editor^ ed );
			virtual	void			load_document				( String^ full_path ) override;
					void			open_scene					( );
			virtual	void			on_document_closing			( document_base^ closing_doc ) override;

		private:
					void			initialize_menu				( );
					void			new_collection_document		( Object^, EventArgs^ );
					void			new_composite_document		( Object^, EventArgs^ );
					void			new_scene_document			( Object^, EventArgs^ );
					document_base^	create_document				( );

		public:
			ToolStripMenuItem^				m_menu_file_open;

		private:
			animation_collections_editor^	m_editor;
			ToolStripMenuItem^				m_menu_file_new;
			ToolStripMenuItem^				m_menu_file_new_scene;
			ToolStripMenuItem^				m_menu_file_new_composite;
			ToolStripMenuItem^				m_menu_file_new_collection;
			OpenFileDialog^					m_open_file_dialog;

		}; // class animation_collections_document_base
	} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATION_COLLECTIONS_DOCUMENT_BASE_H_INCLUDED#pragma once
