////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PREDICATE_H_INCLUDED
#define PREDICATE_H_INCLUDED

#include "base_lexeme.h"
#include <xray/ai/parameter_types.h>
#include "object_instance.h"

namespace xray {
namespace ai {
namespace planning {

class pddl_predicate;

class predicate : public base_lexeme
{
public:
	inline			predicate				( u32 const predicate_id );
	inline			predicate				( u32 const predicate_id, object_instance_type instance );
	inline			predicate				(
						u32 const predicate_id,
						object_instance_type instance1,
						object_instance_type instance2
					);
	inline			predicate				(
						u32 const predicate_id,
						object_instance_type instance1,
						object_instance_type instance2,
						object_instance_type instance3
					);
	inline			predicate				(
						u32 const predicate_id,
						object_instance_type instance1,
						object_instance_type instance2,
						object_instance_type instance3,
						object_instance_type instance4
					);
	inline			predicate				( u32 const predicate_id, parameters_order_enum index );
	inline			predicate				(
						u32 const predicate_id,
						parameters_order_enum index1,
						parameters_order_enum index2
					);
	inline			predicate				(
						u32 const predicate_id,
						parameters_order_enum index1,
						parameters_order_enum index2,
						parameters_order_enum index3
					);
	inline			predicate				(
						u32 const predicate_id,
						parameters_order_enum index1,
						parameters_order_enum index2,
						parameters_order_enum index3,
						parameters_order_enum index4
					);

	virtual	void	add_to_target_world_state_as_predicate	( specified_problem& problem, u32& offset ) const;
	virtual	void	add_to_preconditions_as_predicate		( generalized_action& action ) const;
	virtual	void	add_to_effects_as_predicate				( generalized_action& action ) const;

private:
	inline	void	add_instance			( object_instance_type instance );
	inline	void	add_index				( parameters_order_enum index );

private:
	expression_parameters_type				m_parameters;
	u32 const								m_predicate_id;
}; // class predicate

} // namespace planning
} // namespace ai
} // namespace xray

#include "predicate_inline.h"

#endif // #ifndef PREDICATE_H_INCLUDED