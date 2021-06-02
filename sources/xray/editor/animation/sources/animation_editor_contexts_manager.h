////////////////////////////////////////////////////////////////////////////
//	Created		: 01.11.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_EDITOR_CONTEXTS_MANAGER_H_INCLUDED
#define ANIMATION_EDITOR_CONTEXTS_MANAGER_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;

namespace xray {
namespace animation_editor {
	ref class animation_editor;

	public ref class animation_editor_contexts_manager: public editor_base::i_context_manager 
	{
	public:
								animation_editor_contexts_manager	(animation_editor^ editor):m_editor(editor) {};
		virtual bool			context_fit							(String^ context);
		virtual List<String^>^	get_contexts_list					();

	private:
		animation_editor^		m_editor;
	}; // class animation_editor_contexts_manager
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_EDITOR_CONTEXTS_MANAGER_H_INCLUDED