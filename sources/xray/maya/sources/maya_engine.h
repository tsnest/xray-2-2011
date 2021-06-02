////////////////////////////////////////////////////////////////////////////
//	Created		: 29.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MAYA_ENGINE_H_INCLUDED
#define MAYA_ENGINE_H_INCLUDED

#include <xray/core/simple_engine.h>
#include <xray/maya_animation/engine.h>
#include <xray/animation/engine.h>

namespace xray {

namespace animation {	struct world;}

namespace maya_animation {	struct world;}

namespace maya{

class maya_engine :
	public xray::core::engine,
	public xray::animation::engine,
	public xray::maya_animation::engine
{
public:
	maya_engine			( );

	virtual void			exit						( int error_code )	{ m_exit_code = error_code; xray::debug::terminate(""); }
	virtual int				get_exit_code				( ) const			{ return m_exit_code; }
	virtual void			set_exit_code				( int error_code )	{ m_exit_code = error_code; }
	virtual	pcstr			get_resources_path			( ) const;
	virtual	pcstr			get_mounts_path				( ) const;
	virtual	void			create_physical_path		( string_path& result, pcstr resources_path, pcstr inside_resources_path ) const;
//			pcstr			get_resources_root_path		( ) const;

			void			register_callback_id		( MCallbackId const& id);
			void			unregister_callback_id		( MCallbackId const& id );
			void			clear_all_callbacks			( );
	xray::animation::world*			animation_world;
	xray::maya_animation::world*	maya_animation_world;
public:
			void			assign_resource_path		( );

private:
	MCallbackIdArray		m_callback_ids;
	string_path				m_mounts_path;
	string_path				m_resources_path;

private:
	int						m_exit_code;

public:
	bool					m_is_scene_loading;
}; // class simple_engine

extern	maya_engine	&g_maya_engine;

} // naemspace maya
} // namespace xray

#endif // #ifndef MAYA_ENGINE_H_INCLUDED