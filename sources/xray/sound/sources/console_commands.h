////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CONSOLE_COMMANDS_H_INCLUDED
#define CONSOLE_COMMANDS_H_INCLUDED

#ifndef MASTER_GOLD

namespace xray {
namespace sound {

class sound_world;
class world_user;

class console_commands : private boost::noncopyable
{
public:
						console_commands			( sound_world const& world );
private:
	void				on_time_factor_changed		( ) const;
	void				on_stream_writing_changed	( ) const;
	void				on_dump_stream_writing		( ) const;
	void				on_debug_draw_mode_changed	( ) const;
	void				on_show_proxy_stats_changed	( ) const;
	world_user*			get_thread_world_user		( ) const;
private:
	sound_world const&					m_sound_world;

	float								m_time_factor;
	u32									m_debug_draw_mode;
	u32									m_proxy_stats;
	bool								m_is_debug_stream_writing_enabled;
}; // class console_commands

} // namespace sound
} // namespace xray

#endif // #ifndef MASTER_GOLD

#endif // #ifndef CONSOLE_COMMANDS_H_INCLUDED