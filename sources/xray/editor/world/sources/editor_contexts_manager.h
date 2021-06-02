////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_CONTEXTS_MANAGER_H_INCLUDED
#define EDITOR_CONTEXTS_MANAGER_H_INCLUDED

using namespace System::Collections::Generic;

namespace xray {
namespace editor {

class editor_world;

public ref class editor_contexts_manager : public xray::editor_base::i_context_manager 
{
public:
	editor_contexts_manager( editor_world& w ):m_editor_world( w )
	{
	}

	virtual bool					context_fit			( System::String^ context ) ;
	virtual List<System::String^>^	get_contexts_list	( )	;
private:
	editor_world&				m_editor_world;

}; // class editor_contexts_manager


} // namespace editor
} // namespace xray

#endif // #ifndef EDITOR_CONTEXTS_MANAGER_H_INCLUDED