////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/profiler.h>

#ifdef USE_PROFILER

#include <xray/linkage_helper.h>
#include <xray/text_tree_item.h>

XRAY_DECLARE_LINKAGE_ID(core_profiler)

using xray::profiler;
using xray::profile_result_portion;
using xray::profile_portion;
using xray::profile_stats;
using xray::strings::text_tree_item;

#ifdef PROFILE_CRITICAL_SECTIONS
static xray::threading::atomic64_type critical_section_counter = 0;

void add_profile_portion(shared_string id, const u64& time)
{
	if(!id)
		return;

	xray::profile_result_portion temp;
	temp.m_timer_id = id;
	temp.m_time = time;
	xray::get_profiler().add_profile_portion(temp);
}
#endif // PROFILE_CRITICAL_SECTIONS

namespace xray {
	XRAY_CORE_API xray::profiler* g_profiler = 0;
} // namespace xray

struct profile_portion_predicate 
{
	inline bool operator() (const profile_result_portion& _1, const profile_result_portion& _2) const
	{
		return (_1.m_timer_id < _2.m_timer_id);
	}
};

profiler::profiler()
#ifdef PROFILE_CRITICAL_SECTIONS
	:m_section()
#endif // PROFILE_CRITICAL_SECTIONS
{
	m_actual = true;
	m_timer.start();
}

profiler::~profiler()
{
#ifdef PROFILE_CRITICAL_SECTIONS
	//set_add_profile_portion(0);
#endif // PROFILE_CRITICAL_SECTIONS
}

inline u64 profiler::get_elapsed_ticks()
{
	return m_timer.get_elapsed_ticks();
}

inline void profiler::convert_string(xray::shared_string str, xray::shared_string& out)
{
	string256 m_temp;
	pcstr i, j = str.c_str();
	u32 count = 0;
	while((i = strchr(j,'/')) != 0) 
	{
		j = i = i + 1;
		++count;
	}

	xray::strings::copy(m_temp, "");
	xray::strings::join(m_temp, j);
	out = m_temp;
}

void profiler::setup_timer(xray::shared_string timer_id, const u64& timer_time, const u32& call_count)
{
	string256 m_temp;
	float _time = float(timer_time) * 1000.f / xray::timing::g_qpc_per_second;
	profile_stats_type::iterator i = m_stats.find(timer_id);
	if(i==m_stats.end()) 
	{
		xray::strings::copy(m_temp, timer_id.c_str());
		pstr j, k = m_temp;
		while((j=strchr(k,'/'))!=0) 
		{
			*j = 0;
			profile_stats_type::iterator m = m_stats.find(m_temp);
			if(m==m_stats.end())
				m_stats.insert(std::make_pair(shared_string(m_temp), profile_stats()));

			*j = '/';
			k = j + 1;
		}

		i = m_stats.insert(std::make_pair(timer_id, profile_stats())).first;

		profile_stats& current = (*i).second;
		current.m_min_time = _time;
		current.m_max_time = _time;
		current.m_total_time = _time;
		current.m_count = 1;
		current.m_call_count = call_count;
		m_actual = false;
	}
	else 
	{
		profile_stats& current = (*i).second;
		current.m_min_time = xray::math::min(current.m_min_time,_time);
		current.m_max_time = xray::math::max(current.m_max_time,_time);
		current.m_total_time += _time;
		++current.m_count;
		current.m_call_count += call_count;
	}

	if(_time>(*i).second.m_time)
		(*i).second.m_time = _time;
	else
		(*i).second.m_time = .01f*_time + .99f*(*i).second.m_time;

	(*i).second.m_update_time = m_timer.get_elapsed_msec();
}

void profiler::clear()
{
#ifdef PROFILE_CRITICAL_SECTIONS
	while(xray::threading::interlocked_exchange(critical_section_counter, 1))
		xray::threading::yield(0);
#endif // PROFILE_CRITICAL_SECTIONS

	m_section.lock();
	m_portions.clear();
	m_stats.clear();
	m_section.unlock();

	m_call_count = 0;

#ifdef PROFILE_CRITICAL_SECTIONS
	xray::threading::interlocked_exchange(critical_section_counter, 0);
#endif // PROFILE_CRITICAL_SECTIONS
}

void profiler::show_stats(text_tree_item* text_tree_root, bool show)
{
	if(!show) 
	{
#ifdef PROFILE_CRITICAL_SECTIONS
//		set_add_profile_portion(0);
#endif // PROFILE_CRITICAL_SECTIONS
		clear();
		return;
	}

#ifdef PROFILE_CRITICAL_SECTIONS
//	set_add_profile_portion(&::add_profile_portion);
#endif // PROFILE_CRITICAL_SECTIONS

	++m_call_count;
		
#ifdef PROFILE_CRITICAL_SECTIONS
	while(xray::threading::interlocked_exchange(critical_section_counter, 1))
		xray::threading::yield(0);
#endif // PROFILE_CRITICAL_SECTIONS

	m_section.lock();

	if(!m_portions.empty()) 
	{
		std::sort(m_portions.begin(), m_portions.end(), profile_portion_predicate());
		u64 timer_time = 0;
		u32 call_count = 0;

		portions_type::const_iterator i = m_portions.begin(), j = i;
		portions_type::const_iterator e = m_portions.end();
		for(; i!=e; ++i) 
		{
			if((*i).m_timer_id == (*j).m_timer_id) 
			{
				setup_timer((*j).m_timer_id, timer_time, call_count);
				timer_time = 0;
				call_count = 0;
				j = i;
			}

			++call_count;
			timer_time += (*i).m_time;
		}

		setup_timer((*j).m_timer_id, timer_time, call_count);
		m_portions.clear();
		m_section.unlock();
		if(!m_actual)
		{
			profile_stats_type::iterator i = m_stats.begin();
			for(; i != m_stats.end(); ++i)
				convert_string((*i).first, (*i).second.m_name);

			m_actual = true;
		}
	}
	else
		m_section.unlock();

#ifdef PROFILE_CRITICAL_SECTIONS
	xray::threading::interlocked_exchange(critical_section_counter,0);
#endif // PROFILE_CRITICAL_SECTIONS

	text_tree_item* child = text_tree_root->new_child("name");
	child->add_column("time");
	child->add_column("average time");
	child->add_column("max time");
	child->add_column("???");
	child->add_column("call count");
	child->add_column("total time");
	profile_stats_type::iterator i = m_stats.begin();
	profile_stats_type::iterator e = m_stats.end();
	for(; i != e; ++i)
	{
		if((*i).second.m_update_time != m_timer.get_elapsed_msec())
			(*i).second.m_time	*= .99f;

		float average = (*i).second.m_count ? (*i).second.m_total_time/float((*i).second.m_count) : 0.f;
		text_tree_item* child = text_tree_root->new_child((*i).second.m_name.c_str());
		child->add_column((*i).second.m_time);
		child->add_column(average);
		child->add_column((*i).second.m_max_time);
		child->add_column(float((*i).second.m_call_count)/m_call_count);
		child->add_column((*i).second.m_call_count);
		child->add_column((*i).second.m_total_time);
	}
}

void profiler::add_profile_portion(const profile_result_portion& profile_portion)
{
#ifdef PROFILE_CRITICAL_SECTIONS
	if(xray::threading::interlocked_exchange(critical_section_counter, 1))
		return;

	do
	{
		xray::threading::yield(0);
	}
	while(!xray::threading::interlocked_exchange(critical_section_counter, 1));
#endif // PROFILE_CRITICAL_SECTIONS

	m_section.lock();
	m_portions.push_back(profile_portion);
	m_section.unlock();

#ifdef PROFILE_CRITICAL_SECTIONS
	xray::threading::interlocked_exchange(critical_section_counter, 0);
#endif // PROFILE_CRITICAL_SECTIONS
}

#endif // #ifdef USE_PROFILER