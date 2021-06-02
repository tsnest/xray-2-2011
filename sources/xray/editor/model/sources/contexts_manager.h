////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CONTEXTS_MANAGER_H_INCLUDED
#define CONTEXTS_MANAGER_H_INCLUDED

namespace xray {
namespace model_editor {

ref class model_editor;

ref class contexts_manager :public xray::editor_base::i_context_manager
{
public:
							contexts_manager	( model_editor^ ed );
	virtual bool			context_fit			( System::String^ context_str );

	virtual string_list^	get_contexts_list( );
	
private:
	model_editor^			m_model_editor;
}; // class contexts_manager

} // namespace model_editor
} // namespace xray

#endif // #ifndef CONTEXTS_MANAGER_H_INCLUDED