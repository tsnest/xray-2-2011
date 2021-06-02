////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_ORACLE_HOLDER_H_INCLUDED
#define XRAY_AI_SEARCH_ORACLE_HOLDER_H_INCLUDED

#include <xray/ai/search/base_types.h>

namespace xray {
namespace ai {
namespace planning {

class oracle;
class propositional_planner;

class oracle_holder : private boost::noncopyable
{
private:
	typedef property_id_type	oracle_id_type;
	typedef oracle*				oracle_ptr_type;

public:
	typedef map< oracle_id_type, oracle_ptr_type >	objects_type;

public:
	inline						oracle_holder		( propositional_planner& total_order_planner );
								~oracle_holder		( );
			
			void				clear				( );
			void				add					( oracle_id_type const& oracle_id, oracle_ptr_type oracle );
	inline	void				remove				( oracle_id_type const& oracle_id );
			
			oracle_ptr_type		object				( oracle_id_type const& oracle_id );
	inline	objects_type const&	objects				( ) const;

private:
			void				remove_impl			( oracle_id_type const& oracle_id, bool const notify_holder );

private:
	objects_type				m_objects;
	propositional_planner&		m_planner;
}; // class oracle_holder

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/oracle_holder_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_ORACLE_HOLDER_H_INCLUDED