////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PREDICATE_INLINE_H_INCLUDED
#define PREDICATE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline void predicate::add_instance	( object_instance_type instance )
{
	expression_parameter			new_parameter;
	object_instance::set_instance	( new_parameter.instance, instance );
	m_parameters.push_back			( new_parameter );
}

inline void predicate::add_index	( parameters_order_enum index )
{
	expression_parameter			new_parameter;
	new_parameter.index				= index;
	m_parameters.push_back			( new_parameter );
}

inline predicate::predicate		( u32 const predicate_id ) :
	base_lexeme					( false ),
	m_predicate_id				( predicate_id )
{
}

inline predicate::predicate		( u32 const predicate_id, object_instance_type instance ) :
	base_lexeme					( false ),
	m_predicate_id				( predicate_id )
{
	add_instance				( instance );
}

inline predicate::predicate		( u32 const predicate_id, object_instance_type instance1, object_instance_type instance2 ) :
	base_lexeme					( false ),
	m_predicate_id				( predicate_id )
{
	add_instance				( instance1 );
	add_instance				( instance2 );
}

inline predicate::predicate		(
		u32 const predicate_id,
		object_instance_type instance1,
		object_instance_type instance2,
		object_instance_type instance3
	) :
	base_lexeme					( false ),
	m_predicate_id				( predicate_id )
{
	add_instance				( instance1 );
	add_instance				( instance2 );
	add_instance				( instance3 );
}

inline predicate::predicate		(
		u32 const predicate_id,
		object_instance_type instance1,
		object_instance_type instance2,
		object_instance_type instance3,
		object_instance_type instance4
	) :
	base_lexeme					( false ),
	m_predicate_id				( predicate_id )
{
	add_instance				( instance1 );
	add_instance				( instance2 );
	add_instance				( instance3 );
	add_instance				( instance4 );
}

inline predicate::predicate		( u32 const predicate_id, parameters_order_enum index ) :
	base_lexeme					( false ),
	m_predicate_id				( predicate_id )
{
	add_index					( index );
}

inline predicate::predicate		(
		u32 const predicate_id,
		parameters_order_enum index1,
		parameters_order_enum index2
	) :
	base_lexeme					( false ),
	m_predicate_id				( predicate_id )
{
	add_index					( index1 );
	add_index					( index2 );
}

inline predicate::predicate		(
		u32 const predicate_id,
		parameters_order_enum index1,
		parameters_order_enum index2,
		parameters_order_enum index3
	) :
	base_lexeme					( false ),
	m_predicate_id				( predicate_id )
{
	add_index					( index1 );
	add_index					( index2 );
	add_index					( index3 );
}

inline predicate::predicate		(
		u32 const predicate_id,
		parameters_order_enum index1,
		parameters_order_enum index2,
		parameters_order_enum index3,
		parameters_order_enum index4
	) :
	base_lexeme					( false ),
	m_predicate_id				( predicate_id )
{
	add_index					( index1 );
	add_index					( index2 );
	add_index					( index3 );
	add_index					( index4 );
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef PREDICATE_INLINE_H_INCLUDED