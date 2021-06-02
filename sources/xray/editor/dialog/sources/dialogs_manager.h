////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_MANAGER_H_INCLUDED
#define DIALOG_MANAGER_H_INCLUDED

#pragma managed( push, off )

#include "dialog_editor_resource.h"
#include "game_dialog.h"

namespace xray {
namespace dialog_editor {

	struct dialog_resources;
	class game_dialog;

	typedef	resources::resource_ptr<game_dialog, resources::unmanaged_intrusive_base> game_dialog_ptr;
	typedef	boost::function<void(game_dialog_ptr ptr)>	game_dialog_callback;

	typedef	resources::resource_ptr<dialog_resources, resources::unmanaged_intrusive_base> dialog_resources_ptr;
	typedef	boost::function<void(dialog_resources_ptr ptr)>	dialog_manager_callback;

namespace dialog_manager {

	struct compare_predicate
	{
		inline bool	operator()	(pcstr s1, pcstr s2) const
		{
			return (strings::compare(s1, s2)<0);
		}
	}; // struct compare_predicate

	class dialogs_manager;

	void				create_dialogs_manager	();
	dialogs_manager*	get_dialogs_manager		();
	void				destroy_dialogs_manager	();

	class dialogs_manager : private boost::noncopyable
	{
		typedef associative_vector<pcstr, dialog_resources_ptr, vector, compare_predicate> editor_dialogs_list;
		typedef xray::dialog_editor::map<pstr, dialog_manager_callback, compare_predicate> editor_callbacks_list;

		typedef associative_vector<pcstr, game_dialog_ptr, vector, compare_predicate> game_dialogs_list;
		typedef xray::dialog_editor::map<pstr, game_dialog_callback, compare_predicate> game_callbacks_list;

	public:
				dialogs_manager				();
				~dialogs_manager			();
		void	request_editor_dialog		(pcstr name, dialog_manager_callback const& callback);
		void	request_game_dialog			(pcstr name, game_dialog_callback const& callback);
		void	tick						();

	private:
		void	on_editor_dialog_loaded		(xray::resources::queries_result& result);
		void	on_game_dialog_loaded		(xray::resources::queries_result& result);

	private:
		editor_dialogs_list		m_editor_dialogs_list;
		editor_callbacks_list	m_editor_callbacks;

		game_dialogs_list		m_game_dialogs_list;
		game_callbacks_list		m_game_callbacks;
	}; // class dialogs_manager
} // namespace dialog_manager
} // namespace dialog_editor 
} // namespace xray 
#pragma managed( pop )
#endif // #ifndef DIALOG_MANAGER_H_INCLUDED
