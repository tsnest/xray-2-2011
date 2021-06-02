////////////////////////////////////////////////////////////////////////////
//	Created		: 05.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PERCEPTOR_BASE_H_INCLUDED
#define PERCEPTOR_BASE_H_INCLUDED

namespace xray {
namespace ai {

struct npc;
class brain_unit;
class working_memory;

namespace sensors {
	struct sensed_object;
} // namespace sensors

namespace perceptors {

class XRAY_NOVTABLE perceptor_base : private boost::noncopyable
{
public:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR				( perceptor_base );
				
	virtual	void			on_sensed_object_retrieval	( sensors::sensed_object const& memory_object ) = 0;

			perceptor_base*	m_next;

protected:
	inline	perceptor_base	( npc& npc, brain_unit& brain, working_memory& memory ) :
		m_npc				( npc ),
		m_brain_unit		( brain ),
		m_working_memory	( memory ),
		m_next				( 0 )
	{
	}

protected:
	npc&					m_npc;
	brain_unit&				m_brain_unit;
	working_memory&			m_working_memory;
}; // class perceptor_base

} // namespace perceptors
} // namespace ai
} // namespace xray

#endif // #ifndef PERCEPTOR_BASE_H_INCLUDED