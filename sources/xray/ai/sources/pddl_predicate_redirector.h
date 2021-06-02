////////////////////////////////////////////////////////////////////////////
//	Created		: 01.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_PREDICATE_REDIRECTOR_H_INCLUDED
#define PDDL_PREDICATE_REDIRECTOR_H_INCLUDED

#include <xray/ai/search/oracle.h>

namespace xray {
namespace ai {
namespace planning {

class specified_problem;

class pddl_predicate_redirector :
	public oracle,
	private boost::noncopyable
{
	typedef oracle						super;

public:
			pddl_predicate_redirector	(
				pcstr caption,
				specified_problem const& problem,
				property_id_type const& id
			);
	
	virtual	property_value_type	value	( );

private:
	specified_problem const&			m_problem;
	property_id_type					m_id;
}; // class pddl_predicate_redirector

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef PDDL_PREDICATE_REDIRECTOR_H_INCLUDED