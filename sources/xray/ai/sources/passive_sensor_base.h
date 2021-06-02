////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PASSIVE_SENSOR_BASE_H_INCLUDED
#define PASSIVE_SENSOR_BASE_H_INCLUDED

namespace xray {
namespace ai {

struct npc;
class ai_world;
class brain_unit;
struct npc_statistics;
class behaviour;

namespace sensors {

class XRAY_NOVTABLE passive_sensor_base : private boost::noncopyable
{
public:	
	virtual void		set_parameters	( behaviour const& behaviour_parameters )	= 0;
	virtual	void		dump_state		( npc_statistics& stats ) const				= 0;

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( passive_sensor_base );

	passive_sensor_base*				m_next;

protected:	
	inline	passive_sensor_base			( npc& npc, ai_world& world, brain_unit& brain )
			:	m_npc					( npc ),
				m_world					( world ),
				m_brain_unit			( brain ),
				m_next					( 0 ),
				m_enabled				( memory::uninitialized_value< bool >() )
	{
	}

	npc&								m_npc;
	ai_world&							m_world;
	brain_unit&							m_brain_unit;
	
	bool								m_enabled;
}; // class passive_sensor_base

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef PASSIVE_SENSOR_BASE_H_INCLUDED