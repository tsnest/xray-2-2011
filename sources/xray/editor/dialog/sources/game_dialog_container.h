////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef GAME_DIALOG_CONTAINER_H_INCLUDED
#define GAME_DIALOG_CONTAINER_H_INCLUDED

#include "dialogs_manager.h"
#include "dialog_node_base.h"
#pragma managed( push, off )

namespace xray {
namespace dialog_editor {
	class game_dialog_container : public dialog_node_base
	{
	public:
								game_dialog_container	(pstr new_text);
		virtual					~game_dialog_container	();
		virtual	void			set_text				(pcstr new_text);
		virtual pcstr			text					();
		void					request_dialog			();
		const game_dialog_ptr*	get_resource			() {return m_resource;};
		dialog_node_base*		get_root				(u32 index) const;
				u32				get_roots_count			() const;

	private:
		void					on_dialog_loaded		(game_dialog_ptr p);

	private:
		game_dialog_ptr*	m_resource;
		pstr				m_text;
	}; // class game_dialog_container
} // namespace dialog_editor
} // namespace xray
#pragma managed( pop )
#endif // #ifndef GAME_DIALOG_CONTAINER_H_INCLUDED