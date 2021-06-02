////////////////////////////////////////////////////////////////////////////
//	Created		: 01.11.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_EDITOR_CONTEXTS_MANAGER_H_INCLUDED
#define DIALOG_EDITOR_CONTEXTS_MANAGER_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;

namespace xray {
namespace dialog_editor {
	ref class dialog_editor_impl;

	public ref class dialog_editor_contexts_manager: public editor_base::i_context_manager 
	{
	public:
								dialog_editor_contexts_manager	(dialog_editor_impl^ editor):m_editor(editor) {};
		virtual bool			context_fit						(String^ context);
		virtual List<String^>^	get_contexts_list				();

	private:
		dialog_editor_impl^		m_editor;
	}; // class animation_editor_contexts_manager
} // namespace dialog_editor
} // namespace xray
#endif // #ifndef DIALOG_EDITOR_CONTEXTS_MANAGER_H_INCLUDED