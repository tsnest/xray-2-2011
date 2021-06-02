////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_BASE_H_INCLUDED
#define ACTION_BASE_H_INCLUDED

#include "action.h"

namespace xray {
namespace rtp {



template< class action >
class action_base : private boost::noncopyable {

public:
	typedef action	action_type;

public:
		action_base ( const global_actions_params< action > &g_par );
		~action_base( );

public:

		u32		add						( action* a	);
const	action*	get						( u32 id ) const ;
		action*	get						( u32 id );

		u32		size					( ) const	{ return actions.size();}
inline	u32		total_blend_samples		( )const;
inline	u32		blend_sample_id_starts	( u32 action_id )const;

public:

#ifndef	MASTER_GOLD
void			save		( xray::configs::lua_config_value cfg )const;
#endif

void			load		( const xray::configs::binary_config_value &cfg );

global_actions_params< action >&		global_params()			{ return m_global_params; }
const global_actions_params< action >&	global_params()const	{ return m_global_params; }

private:
void			clear		();

private:
	rtp::vector<action*>			actions;

private:
	global_actions_params< action > m_global_params;
	mutable u32						m_total_blend_samples;
}; // class action_base

} // namespace rtp
} // namespace xray

#include "action_base_inline.h"

#endif // #ifndef ACTION_BASE_H_INCLUDED