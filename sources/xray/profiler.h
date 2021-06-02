////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PROFILER_H_INCLUDED
#define XRAY_PROFILER_H_INCLUDED

#ifndef MASTER_GOLD
#	define USE_PROFILER
#endif // #ifndef MASTER_GOLD

#ifdef USE_PROFILER

namespace xray {
	namespace strings {
		class text_tree_item;
	} // namespace strings

#ifdef PROFILE_CRITICAL_SECTIONS
	extern void add_profile_portion(pcstr id, const u64& time);
#endif // #ifdef PROFILE_CRITICAL_SECTIONS

#pragma pack(push,4)
struct XRAY_CORE_API profile_result_portion 
{
	u64					m_time;
	shared_string		m_timer_id;
};
#pragma pack(pop)

struct XRAY_CORE_API profile_portion : public profile_result_portion 
{
	inline	profile_portion	(shared_string timer_id);
	inline	~profile_portion();
};

struct XRAY_CORE_API profile_stats 
{
	u32				m_update_time;
	shared_string	m_name;
	float			m_time;
	float			m_min_time;
	float			m_max_time;
	float			m_total_time;
	u32				m_count;
	u32				m_call_count;

	inline			profile_stats	();
};

class XRAY_CORE_API profiler
{
public:
						profiler			();
						~profiler			();
			void		show_stats			(strings::text_tree_item* text_tree_root, bool show);
			void		clear				();
	inline  u64			get_elapsed_ticks	();
			void		add_profile_portion	(const profile_result_portion& p);

private:
			void		setup_timer			(shared_string timer_id, const u64 &timer_time, const u32& call_count);
	inline	void		convert_string		(shared_string str, shared_string& out);

private:
	struct pred_rstr
	{
		inline bool operator() (shared_string _1, shared_string _2) const
		{
			return (_1 < _2);
		}
	};

	typedef xray::debug::vector<profile_result_portion>					portions_type;
	typedef xray::debug::map<shared_string, profile_stats, pred_rstr>	profile_stats_type;

private:
	portions_type			m_portions;
	profile_stats_type		m_stats;
	xray::timing::timer		m_timer;
	bool					m_actual;
	xray::threading::mutex	m_section;
	u32						m_call_count;
};

extern XRAY_CORE_API profiler* g_profiler;
inline XRAY_CORE_API profiler& get_profiler();

#	define START_PROFILE(a)		if ( ::xray::identity(true) ) { profile_portion	__profile_portion__(a)
#	define STOP_PROFILE			} else (void)0
#	define PROFILE_FUNCTION(a)	profile_portion __profile_portion__(a)

#include <xray/profiler_inline.h>

} // namespace xray

#else // #ifdef USE_PROFILER
#	define START_PROFILE(a)		if ( ::xray::identity(true) ) {
#	define STOP_PROFILE			} else (void)0
#	define PROFILE_FUNCTION(a)	profile_portion __profile_portion__(a)
#endif // #ifdef USE_PROFILER

#endif // #ifndef XRAY_PROFILER_H_INCLUDED

