////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GUI_ACTION_ENGINE_H_INCLUDED
#define GUI_ACTION_ENGINE_H_INCLUDED

namespace xray {
namespace editor {

	ref class input_engine;
	class editor_world;
	interface class action;
	ref class action_single;
	ref class action_continuous;

	public delegate void action_handler();

public ref class action_engine
{
public:
	action_engine( editor::input_engine^ input_engine );
	~action_engine( );
	!action_engine( );

	inline	System::Void	register_action	( action_single^ action );
	inline	System::Void	register_action	( action_single^ action, System::String^ shorcut );
	inline	System::Void	register_action	( action_single^ action, System::String^ shorcut1, System::String^ shorcut2 );

	inline	System::Void	register_action	( action_continuous^ action );
	inline	System::Void	register_action	( action_continuous^ action, System::String^ shorcut );
	inline	System::Void	register_action	( action_continuous^ action, System::String^ shorcut1, System::String^ shorcut2 );

	inline	bool			action_exists	( System::String^ name );
	inline	System::Void	execute			( System::String^ name );
	inline	action^			get_action		( System::String^ name );

	inline	System::Collections::ICollection^	get_actions_list( );
	inline	editor::input_engine^ get_input_engine ( );
private:

	editor::input_engine^			m_input_engine;
	System::Collections::Hashtable	m_actions;

}; // class action_engine


} // namespace editor
} // namespace xray

#endif // #ifndef GUI_ACTION_ENGINE_H_INCLUDED