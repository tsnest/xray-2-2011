////////////////////////////////////////////////////////////////////////////
//	Created		: 23.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_DOCUMENT_EDITOR_SHORTCUT_RECEIVER_H_INCLUDED
#define I_DOCUMENT_EDITOR_SHORTCUT_RECEIVER_H_INCLUDED

namespace xray {
	namespace editor {
	namespace controls {

public interface class  i_document_editor_shortcut_receiver {
public:
	virtual	void		save			()			= 0;
	virtual	void		load			()			= 0;
	virtual	void		undo			()			= 0;
	virtual	void		redo			()			= 0;
	virtual	void		copy			(bool del)	= 0;
	virtual	void		del				()			= 0;
	virtual	void		paste			()			= 0;
	virtual	void		select_all		()			= 0;


}; // class i_document_editor_shortcut_receiver

} // namespace controls
} // namespace editor
} // namespace xray

#endif // #ifndef I_DOCUMENT_EDITOR_SHORTCUT_RECEIVER_H_INCLUDED