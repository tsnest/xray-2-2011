////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ACTIVE_SENSOR_BASE_H_INCLUDED
#define ACTIVE_SENSOR_BASE_H_INCLUDED

namespace xray {
namespace ai {

struct npc;
class ai_world;
class brain_unit;
struct npc_statistics;
class behaviour;

namespace sensors {

class XRAY_NOVTABLE active_sensor_base : private boost::noncopyable
{
public:	
	virtual	void		tick			( )											= 0;
	virtual void		set_parameters	( behaviour const& behaviour_parameters )	= 0;
	virtual	void		dump_state		( npc_statistics& stats ) const				= 0;
			
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( active_sensor_base );

	active_sensor_base*					m_next;

protected:
	inline	active_sensor_base			( npc& npc, ai_world& world, brain_unit& brain )
			:	m_npc					( npc ),
				m_world					( world ),
				m_brain_unit			( brain ),
				m_next					( 0 )
	{
	}

	npc&								m_npc;
	ai_world&							m_world;
	brain_unit&							m_brain_unit;
}; // class active_sensor_base

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef ACTIVE_SENSOR_BASE_H_INCLUDED