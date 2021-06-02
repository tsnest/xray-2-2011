////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.09.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_CORE_ENGINE_H_INCLUDED
#define XRAY_CORE_ENGINE_H_INCLUDED

#include <xray/core_debug_engine.h>

namespace xray {
namespace core {

struct engine : public core_debug_engine
{
	virtual			~engine					( ) { }
	virtual void 	exit					( int exit_code ) = 0;
	virtual void 	set_exit_code			( int exit_code ) = 0;
	virtual int  	get_exit_code			( ) const = 0;
	virtual	pcstr	get_resources_path		( ) const = 0;
	virtual	pcstr	get_mounts_path			( ) const = 0;
	virtual	void	create_physical_path	( string_path& result, pcstr resources_path, pcstr inside_resources_path ) const = 0;

}; // class engine

} // namespace core
} // namespace xray

#endif // #ifndef XRAY_CORE_ENGINE_H_INCLUDED