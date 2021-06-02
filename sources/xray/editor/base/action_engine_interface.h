//-------------------------------------------------------------------------------------------
//	Created		: 04.06.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
//-------------------------------------------------------------------------------------------
#ifndef ACTION_ENGINE_INTERFACE_H_INCLUDED
#define ACTION_ENGINE_INTERFACE_H_INCLUDED

namespace xray {
	namespace editor {
		interface class action;
		ref class action_single;
		ref class action_continuous;
	}
namespace editor_base {

	public interface class action_engine_interface 
	{
	public:
		virtual System::Void	register_action	( xray::editor::action_single^ action ) = 0;
		virtual System::Void	register_action	( xray::editor::action_single^ action, System::String^ shorcut ) = 0;
		virtual System::Void	register_action	( xray::editor::action_single^ action, System::String^ shorcut1, System::String^ shorcut2 ) = 0;

		virtual System::Void	register_action	( xray::editor::action_continuous^ action ) = 0;
		virtual System::Void	register_action	( xray::editor::action_continuous^ action, System::String^ shorcut ) = 0;
		virtual System::Void	register_action	( xray::editor::action_continuous^ action, System::String^ shorcut1, System::String^ shorcut2 ) = 0;

		virtual bool			action_exists	( System::String^ name ) = 0;
		virtual System::Void	execute			( System::String^ name ) = 0;
		virtual xray::editor::action^ get_action( System::String^ name ) = 0;

		virtual System::Collections::ICollection^	get_actions_list( ) = 0;
	}; // class action_engine_interface
} // namespace editor_base
} // namespace xray
#endif // #ifndef ACTION_ENGINE_INTERFACE_H_INCLUDED
