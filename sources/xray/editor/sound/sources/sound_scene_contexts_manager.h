////////////////////////////////////////////////////////////////////////////
//	Created		: 29.06.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_SCENE_CONTEXTS_MANAGER_H_INCLUDED
#define SOUND_SCENE_CONTEXTS_MANAGER_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;

namespace xray {
namespace sound_editor {
	
	ref class sound_scene_document;

	public ref class sound_scene_contexts_manager: public editor_base::i_context_manager 
	{
	public:
								sound_scene_contexts_manager	(sound_scene_document^ doc):m_document(doc) {};
		virtual bool			context_fit						(String^ context);
		virtual List<String^>^	get_contexts_list				();

	private:
		sound_scene_document^	m_document;
	}; // class sound_editor_contexts_manager
} // namespace sound_editor
} // namespace xray
#endif // #ifndef SOUND_SCENE_CONTEXTS_MANAGER_H_INCLUDED