////////////////////////////////////////////////////////////////////////////
//	Created 	: 05.02.2008
//	Author		: Konstantin Slipchenko
//	Description : dynamic_object
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_DYNAMIC_OBJECT_H_INCLUDED
#define XRAY_DYNAMIC_OBJECT_H_INCLUDED
#include "step_object.h"
#include "space_object.h"
#include "island_object.h"
#include "space_item.h"
#include "step_counter.h"

class island;
class collide_element;

class dynamic_object:
	protected step_object,
	protected space_object,
	private island_object
{
		//space_item			*m_space_item;
		step_counter_ref	m_step_counter;
		step_counter_ref	m_step_mark_recursion;
		bool				m_collision_recursing;
		bool				m_awake;

private:
		virtual bool	step					( scheduler& w, float time_delta );
		virtual	void	collide					( island& i, space_object& o, bool reverted_oreder );

private:
		virtual	void	island_connect			( island	&i );

protected:
		virtual	void	on_step					( scheduler & ){};
		virtual void	island_step_end			( island	&i, float time_delta );
		void			wake_up					( scheduler &s );
		inline void		sleep_internal			() { m_awake = false; }
		inline bool		awake					() const { return m_awake; }  //m_awake

protected:
	~dynamic_object()							{}
	dynamic_object()		:  m_collision_recursing( false ), m_awake( false )	{}

};




#endif