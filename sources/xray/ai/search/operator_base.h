////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_OPERATOR_BASE_H_INCLUDED
#define XRAY_AI_SEARCH_OPERATOR_BASE_H_INCLUDED

#include <xray/ai/search/world_state.h>

namespace xray {
namespace ai {
namespace planning {

class operator_base
{
public:
	typedef world_state					state_type;

protected:
	typedef operator_weight_type		operator_weight_type;
	typedef world_state_property		property_type;
	typedef property_id_type			property_id_type;

public:
	inline								operator_base		( );
	virtual								~operator_base		( );
	
	inline	state_type&					preconditions		( );
	inline	state_type&					effects				( );
			operator_weight_type const&	min_weight			( );

protected:
	inline	void						make_inactual		( ) const;

private:
	inline	void						init				( );
			void						compute_min_weight	( );

private:
	state_type							m_preconditions;
	state_type							m_effects;

	u32									m_preconditions_hash;
	u32									m_effects_hash;
	operator_weight_type				m_min_weight;
}; // class operator_base

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/operator_base_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_OPERATOR_BASE_H_INCLUDED