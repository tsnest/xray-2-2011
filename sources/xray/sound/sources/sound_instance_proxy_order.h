////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_INSTANCE_PROXY_ORDER_H_INCLUDED
#define SOUND_INSTANCE_PROXY_ORDER_H_INCLUDED

#include "sound_instance_proxy_internal.h"

namespace xray {
namespace sound {

class sound_world;



class sound_instance_proxy_order : public sound_order
{
public:
	typedef	boost::function< void ( ) >	functor_type;

public:
									sound_instance_proxy_order	(
																world_user& user,
																sound_instance_proxy_internal& proxy,
																functor_type const& functor_order
																);
	virtual	void				execute			( );
	inline	sound_instance_proxy_internal&	get_proxy		( )			{ return m_proxy; }
	inline	world_user&						get_world_user_base	( )			{ return m_world_user_base; }
	inline	functor_type*					get_functor		( ) 		{ return &m_functor; }
	inline	void				execute_functor	( )			{ return m_functor( ); }

public:
	sound_instance_proxy_internal&		m_proxy;
	world_user&							m_world_user_base;
	functor_type						m_functor;
}; // class sound_instance_proxy_order



template < class T >
class sound_instance_proxy_order_with_data : public sound_instance_proxy_order
{
public:
	typedef	boost::function< void ( T const& data ) >	functor_type;

								sound_instance_proxy_order_with_data (	
											world_user& user,
											sound_instance_proxy_internal& proxy,
											functor_type functor_order,
											T data 
											);
public:
	T					m_data;
	functor_type		m_functor;
};

template < class T >
sound_instance_proxy_order_with_data< T >::sound_instance_proxy_order_with_data (	
											world_user& user,
											sound_instance_proxy_internal& proxy,
											functor_type functor_order,
											T data 
											) :
	sound_instance_proxy_order	( user, proxy, boost::bind( boost::cref( m_functor ), boost::cref(m_data)) ),
	m_data						( data ),
	m_functor					( functor_order )
{
}

class destroy_sound_instance_proxy_order : public sound_order
{
public:
	enum internal_state
	{
		internal_state_initialize_destruction,
		internal_state_finalize_destruction
	};
						destroy_sound_instance_proxy_order	(	world_user& user,
																sound_instance_proxy_internal& proxy,
																sound_scene_ptr sound_scene
															);
	virtual				~destroy_sound_instance_proxy_order	( );

	virtual	void				execute					( );
	inline	internal_state					get_state				( ) const					{ return m_state; }
	inline	void							set_state				( internal_state state )	{ m_state = state; }
	inline	sound_instance_proxy_internal&	get_proxy				( )							{ return m_proxy; }
	inline	world_user&						get_user				( )							{ return m_user; }
private:
	world_user&						m_user;
	sound_instance_proxy_internal&	m_proxy;
	sound_scene_ptr					m_sound_scene;
	internal_state					m_state;
}; // class destroy_sound_instance_proxy_order

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_INSTANCE_PROXY_ORDER_H_INCLUDED