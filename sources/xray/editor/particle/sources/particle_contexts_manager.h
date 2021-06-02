////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_CONTEXTS_MANAGER_H_INCLUDED
#define PARTICLE_CONTEXTS_MANAGER_H_INCLUDED

using namespace System::Collections::Generic;

namespace xray {
namespace particle_editor {

	ref class particle_editor;

public ref class particle_contexts_manager : public editor_base::i_context_manager {
public:
	particle_contexts_manager(particle_editor^ editor):m_editor(editor)
	{

	}

	virtual bool				context_fit			( System::String^ context );
	virtual List<String^>^		get_contexts_list	( )	;
private:
	particle_editor^ m_editor;

}; // class particle_contexts_manager


} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_CONTEXTS_MANAGER_H_INCLUDED