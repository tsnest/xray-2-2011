////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_OPERATOR_H_INCLUDED
#define XRAY_AI_SEARCH_OPERATOR_H_INCLUDED

#include <xray/ai/search/operator_base.h>

namespace xray {
namespace ai {
namespace planning {

class propositional_planner;

class operator_impl : public operator_base
{
private:
	typedef operator_base								super;

private:
	typedef state_type::properties_type					properties_type;
	typedef properties_type::const_iterator				const_iterator;

public:
	inline							operator_impl		( pcstr id, operator_weight_type const cost );
			void					on_after_addition	( propositional_planner* object );
			void					make_inactual		( );

public:
	virtual	void					initialize			( );
	virtual	void					execute				( );
	virtual	void					finalize			( );
	virtual	void					dump				( pcstr offset );
	virtual operator_weight_type	weight				( state_type const& state0, state_type const& state1 );

public:
	inline	pcstr					id					( ) const;

protected:
	fixed_string< 64 >				m_id;
	propositional_planner*			m_object;
	operator_weight_type			m_cost;
	bool							m_first_execute;
}; // class operator_impl

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/operator_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_OPERATOR_H_INCLUDED