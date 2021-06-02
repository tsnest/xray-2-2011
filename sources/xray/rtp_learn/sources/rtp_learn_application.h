////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RTP_LEARN_APPLICATION_H_INCLUDED
#define RTP_LEARN_APPLICATION_H_INCLUDED

#include <xray/animation/world.h>
#include <xray/rtp/world.h>

namespace xray {

namespace rtp {

class base_controller;

	} // namespace rtp 

namespace rtp_learn {

class application {
public:
			
					application		( );
			void	initialize		( );
			void	execute			( );
			void	finalize		( );
	inline	u32		get_exit_code	( ) const	{ return m_exit_code; }

private:

//			void	on_resources_loaded			( resources::queries_result& data );
			bool	is_loaded					( );



private:
	memory::doug_lea_allocator_type			m_rtp_allocator;
	memory::doug_lea_allocator_type			m_animation_allocator;
	

	rtp::base_controller					*m_controller;

	u32		m_exit_code;

}; // class application

} // namespace rtp_learn
} // namespace xray

#endif // #ifndef RTP_LEARN_APPLICATION_H_INCLUDED