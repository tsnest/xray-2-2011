////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_CONTEXT_MANAGER_H_INCLUDED
#define I_CONTEXT_MANAGER_H_INCLUDED

namespace xray {
namespace editor_base {

public interface class i_context_manager {
public:
	virtual bool			context_fit			( System::String^ context_str )	= 0;	
	virtual System::Collections::Generic::List<System::String^>^	get_contexts_list	( )								= 0;

}; // class i_context_manager

} // namespace editor_base
} // namespace xray

#endif // #ifndef I_CONTEXT_MANAGER_H_INCLUDED