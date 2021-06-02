////////////////////////////////////////////////////////////////////////////
//	Created		: 15.01.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SIMPLE_ENGINE_H_INCLUDED
#define SIMPLE_ENGINE_H_INCLUDED

#include <xray/core/engine.h>

namespace xray {
namespace core {

class simple_engine : public engine
{
public:
	simple_engine() : m_exit_code(0) {}

	virtual void	exit					( int error_code )	{ m_exit_code = error_code; xray::debug::terminate(""); }
	virtual int		get_exit_code			( ) const			{ return m_exit_code; }
	virtual void	set_exit_code			( int error_code )	{ m_exit_code = error_code; }
	virtual	pcstr	get_resources_path		( ) const			{ return "../../resources"; }
	virtual	pcstr	get_mounts_path			( ) const			{ return NULL; }
	virtual	void	create_physical_path	( string_path& result, pcstr resources_path, pcstr inside_resources_path ) const { strings::join(result, resources_path, inside_resources_path); }

private:
	int				m_exit_code;
}; // class simple_engine

} // namespace core
} // namespace xray

#endif // #ifndef SIMPLE_ENGINE_H_INCLUDED