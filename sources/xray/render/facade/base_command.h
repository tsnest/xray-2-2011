////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_COMMAND_H_INCLUDED
#define BASE_COMMAND_H_INCLUDED

#include <xray/render/engine/base_classes.h>

namespace xray {
namespace render {

class XRAY_NOVTABLE base_command {
public:
	inline			base_command		( bool const is_deferred_command = false, bool const use_depth = true ) : remove_frame_id( 0 ), is_deferred_command( is_deferred_command ), use_depth( use_depth ) { }
	virtual	void	execute				( ) = 0;
	virtual	void	defer_execution		( ) { R_ASSERT( !is_deferred_command ); }
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( base_command )

public:
	base_command*	next;
	base_command*	deferred_next;
	bool			is_deferred_command;
	bool			use_depth;

public:
	XRAY_MAX_CACHE_LINE_PAD;
	u32				remove_frame_id;
}; // class base_command

} // namespace render
} // namespace xray

#endif // #ifndef BASE_COMMAND_H_INCLUDED