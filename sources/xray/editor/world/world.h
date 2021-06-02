////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_EDITOR_WORLD_H_INCLUDED
#define XRAY_EDITOR_WORLD_H_INCLUDED

namespace xray {
namespace editor_base {
	class property_holder;
	class property_holder_collection;
	class property_holder_holder;
} // namespace editor_base
namespace editor {

struct XRAY_NOVTABLE engine;

struct XRAY_NOVTABLE world {
	virtual	void	load			( ) = 0;
	virtual	void	run				( ) = 0;
	virtual	void	clear_resources	( ) = 0;
	virtual	HWND	main_handle		( ) = 0;
	virtual	HWND	view_handle		( ) = 0;
	virtual	void	editor_mode		( bool beditor_mode) = 0;
	virtual	void	render			( u32 frame_id, math::float4x4 view_matrix ) = 0;
	virtual	int		exit_code		( ) const = 0;
	virtual void	message_out		( pcstr message ) = 0;
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world )
}; // class world

} // namespace editor
} // namespace xray

#endif // #ifndef XRAY_EDITOR_WORLD_H_INCLUDED