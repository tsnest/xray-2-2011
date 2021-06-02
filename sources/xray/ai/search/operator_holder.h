////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_OPERATOR_HOLDER_H_INCLUDED
#define XRAY_AI_SEARCH_OPERATOR_HOLDER_H_INCLUDED

#include <xray/ai/search/operator_pair.h>

namespace xray {
namespace ai {
namespace planning {

class propositional_planner;

class operator_holder : private boost::noncopyable
{
public:
	typedef operator_pair							operator_pair_type;
	typedef operator_pair::operator_id_type			operator_id_type;
	typedef operator_pair::operator_type			operator_type;
	typedef operator_pair::operator_ptr_type		operator_ptr_type;
	typedef vector< operator_pair_type >			objects_type;

public:
	inline						operator_holder		( propositional_planner& total_order_planner );
								~operator_holder	( );
			
			void				clear				( );
			void				add					( operator_id_type const& operator_id, operator_ptr_type operator_impl );
	inline	void				remove				( operator_id_type const& operator_id );
			
			operator_ptr_type	object				( operator_id_type const& operator_id );
	inline	objects_type const&	objects				( ) const;

private:
			void				remove_impl			( operator_id_type const& operator_id, bool const notify_holder );

private:
	objects_type				m_objects;
	propositional_planner&		m_planner;
}; // class operator_holder

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/operator_holder_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_OPERATOR_HOLDER_H_INCLUDED