////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/search.h>
#include <xray/ai/search/plan_tracker.h>
#include <xray/ai/search/const_oracle.h>
#include <xray/math_randoms_generator.h>

#include "pddl_planner.h"
#include "pddl_domain.h"
#include "pddl_problem.h"
#include "specified_problem.h"
#include "generalized_action.h"
#include "action_instance.h"
#include <xray/ai/parameter_types.h>
#include "predicate.h"

namespace xray {
namespace ai {
namespace planning {

typedef	planning::search		search_type;

#ifdef DEBUG
#	define TEST_PLANNER
#endif // #ifdef DEBUG

//#ifdef TEST_PLANNER
//	static void planner_test();
//#endif // #ifdef TEST_PLANNER

static search_type*	s_head		= 0;

static void destroy_search_service	( search_type*& service )
{
	DELETE							( service );

	if ( s_head )
		return;
}

void decision_cleanup			( )
{
	if ( !s_head )
		return;

	u32 i						= 0;
	for ( search_type* temp = s_head; temp; temp = temp->m_next_search, ++i );
	LOG_WARNING					( "leaked %d search services", i );

	while ( s_head )
	{
		search_type* temp		= s_head;
		destroy_search_service	( temp );
	}
}

search_type::search		( ) :
	m_search_backward	( m_search ),
	m_search_forward	( m_search ),
	m_previous_search	( 0 ),
	m_next_search		( s_head )
{
	if ( s_head )
		s_head->m_previous_search		= this;

	s_head								= this;
}

search_type::~search	( )
{
	R_ASSERT			( s_head );

	if ( s_head == this )
	{
		R_ASSERT		( !m_previous_search );
		s_head			= m_next_search;
		return;
	}

	R_ASSERT			( m_previous_search );
	m_previous_search->m_next_search	= m_next_search;
	if ( !m_next_search )
		return;

	m_next_search->m_previous_search	= m_previous_search;
}

//#ifdef TEST_PLANNER
typedef planning::propositional_planner	planner_type;

static void add_const_oracle			( planner_type& planner, u32 const id, bool const value )
{
	string256							temp;
	xray::sprintf						( temp, "oracle#%d", id );
	planner.oracles().add				( id, NEW( const_oracle )( value, temp ) );
}

static void change_oracle				( planner_type& planner, u32 const id, bool const value )
{
	typedef planning::const_oracle		const_oracle;
	string256							temp;
	xray::sprintf						( temp, "oracle#%d", id );
	planner.oracles().remove			( id );
	planner.oracles().add				( id, NEW( const_oracle )( value,temp ) );
}

typedef planning::world_state_property	property_type;
typedef planning::world_state			state_type;
typedef planning::dnf_world_state		target_state_type;

static int offset						= 0;

static void reindex						( state_type& _0, state_type const& _1 )
{
	typedef state_type::properties_type	properties;
	properties::const_iterator iter		= _1.properties().begin();
	properties::const_iterator iter_end	= _1.properties().end();
	
	for ( ; iter != iter_end; ++iter )
	{
		if ( (*iter).id() < 65536 )
		{
			_0.add						( property_type( (*iter).id() + offset, (*iter).value() ) );
			continue;
		}

		_0.add							( property_type( (*iter).id(), (*iter).value() ) );
	}
}

static void add_operator				(
		planner_type& planner,
		u32 const id,
		state_type const& preconditions,
		state_type const& effects
	)
{
	typedef planning::operator_type		operator_type;
	string256							temp;
	xray::sprintf						( temp, "operator#%d", id + offset );
	operator_type* action				= NEW( operator_type )( temp, 10 );
	reindex								( action->preconditions(), preconditions );
	reindex								( action->effects(), effects );
	planner.operators().add				( id + offset, action );
}

void planner_test_N						( )
{
	u32 const N							= 64;
	planner_type						planner;
	state_type							preconditions, effects;
	target_state_type					target;
	math::random32						rnd( 13 );

	for ( u32 i = 0; i < N; ++i )
	{
		add_const_oracle				( planner, i, ( i > 0 ? ( rnd.random( N ) % i == 0 ) : false ) );

		preconditions.clear				( );
		preconditions.add				( property_type( i, false ) );
		effects.clear					( );
		effects.add						( property_type( i, true ) );
		add_operator					( planner, i, preconditions, effects );

		target.add						( property_type( i, true ), target.properties().begin() );
	}
	planner.target						( target );
	
	//planner.tracker().verbose			( true );
	planner.forward_search				( true );
	search								search_service;
	planner.update						( search_service, "64 actions" );
}

void planner_test						( )
{
	planner_type						planner;
	state_type							preconditions, effects;

	add_const_oracle					( planner, 65536, false );
	add_const_oracle					( planner, 65537, false );
	add_const_oracle					( planner, 65538, false );

	for ( int i = 0; i < 64; ++i )
	{
		add_const_oracle				( planner, offset + 0,  false );
		add_const_oracle				( planner, offset + 2,  false );
		add_const_oracle				( planner, offset + 10, false );
		add_const_oracle				( planner, offset + 11, false );
		add_const_oracle				( planner, offset + 12, false );
		add_const_oracle				( planner, offset + 13, false );
		add_const_oracle				( planner, offset + 14, false );
		add_const_oracle				( planner, offset + 15, false );
		add_const_oracle				( planner, offset + 16, false );
		add_const_oracle				( planner, offset + 17, false );
		add_const_oracle				( planner, offset + 18, false );
		add_const_oracle				( planner, offset + 19, false );
		add_const_oracle				( planner, offset + 20, false );
		add_const_oracle				( planner, offset + 21, false );
		add_const_oracle				( planner, offset + 23, false );

		preconditions.clear				( );
		preconditions.add				( property_type( 0, false ) );
		preconditions.add				( property_type( 65536, true ) );
		effects.clear					( );
		effects.add						( property_type( 0, true ) );
		effects.add						( property_type( 65536, false ) );
		add_operator					( planner, 0, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0, true ) );
		preconditions.add				( property_type( 65536, false ) );
		effects.clear					( );
		effects.add						( property_type( 0, false ) );
		effects.add						( property_type( 65536, true ) );
		add_operator					( planner, 1, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0, true ) );
		preconditions.add				( property_type( 65537, false ) );
		effects.clear					( );
		effects.add						( property_type( 2, true ) );
		effects.add						( property_type( 10, false ) );
		add_operator					( planner, 2, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0,	true ) );
		preconditions.add				( property_type( 65537, false ) );
		effects.clear					( );
		effects.add						( property_type( 10, false ) );
		effects.add						( property_type( 18, true ) );
		add_operator					( planner, 10, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0,	true ) );
		preconditions.add				( property_type( 65537,	false ) );
		effects.clear					( );
		effects.add						( property_type( 10, true ) );
		effects.add						( property_type( 11, true ) );
		add_operator					( planner, 11, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0,	true ) );
		preconditions.add				( property_type( 16, true ) );
		preconditions.add				( property_type( 65537,	false ) );
		effects.clear					( );
		effects.add						( property_type( 16, false ) );
		add_operator					( planner, 12, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0,	true ) );
		preconditions.add				( property_type( 10, true ) );
		preconditions.add				( property_type( 13, true ) );
		preconditions.add				( property_type( 16, false ) );
		preconditions.add				( property_type( 17, false ) );
		preconditions.add				( property_type( 65537,	false ) );
		effects.clear					( );
		effects.add						( property_type( 14, true ) );
		add_operator					( planner, 13, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0, true ) );
		preconditions.add				( property_type( 15, true ) );
		preconditions.add				( property_type( 65537, false ) );
		effects.clear					( );
		effects.add						( property_type( 10, false ) );
		effects.add						( property_type( 12, false ) );
		effects.add						( property_type( 13, true ) );
		effects.add						( property_type( 19, true ) );
		add_operator					( planner, 14, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0, true ) );
		preconditions.add				( property_type( 13, true ) );
		preconditions.add				( property_type( 65537, false ) );
		effects.clear					( );
		effects.add						( property_type( 10, true ) );
		effects.add						( property_type( 17, true ) );
		add_operator					( planner, 15, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0,	true ) );
		preconditions.add				( property_type( 15, false ) );
		preconditions.add				( property_type( 65537,	false ) );
		effects.clear					( );
		effects.add						( property_type( 15, true ) );
		add_operator					( planner, 16, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0,	true ) );
		preconditions.add				( property_type( 13, true ) );
		preconditions.add				( property_type( 19, true ) );
		preconditions.add				( property_type( 65537, false ) );
		effects.clear					( );
		effects.add						( property_type( 10, true ) );
		effects.add						( property_type( 20, true ) );
		add_operator					( planner, 17, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0,	true ) );
		preconditions.add				( property_type( 16, false ) );
		effects.clear					( );
		effects.add						( property_type( 21, true ) );
		add_operator					( planner, 18, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0,	true ) );
		preconditions.add				( property_type( 15, false ) );
		preconditions.add				( property_type( 23, false ) );
		preconditions.add				( property_type( 65537,	false ) );
		effects.clear					( );
		effects.add						( property_type( 10, false ) );
		effects.add						( property_type( 13, true ) );
		add_operator					( planner, 19, preconditions, effects );

		preconditions.clear				( );
		preconditions.add				( property_type( 0,	true ) );
		preconditions.add				( property_type( 15, false ) );
		preconditions.add				( property_type( 23, true ) );
		preconditions.add				( property_type( 65537,	false ) );
		effects.clear					( );
		effects.add						( property_type( 23, false ) );
		add_operator					( planner, 20, preconditions, effects );

		offset							+= 256;
	}

	// construction
	change_oracle						( planner, 10,  true );
	change_oracle						( planner, 13,	true );
	change_oracle						( planner, 15,	true );
	change_oracle						( planner, 266,	true );
	change_oracle						( planner, 268,	true );
	change_oracle						( planner, 271,	true );
	change_oracle						( planner, 512,	true );
	change_oracle						( planner, 522,	true );
	change_oracle						( planner, 525,	true );
	change_oracle						( planner, 531,	true );

	target_state_type					target;
	target.add							( property_type( 273, true ), target.properties().end() );
	planner.target						( target );

//	planner.tracker().verbose			( true );
//	planner.forward_search				( true );
	search								search_service;
	planner.update						( search_service, "Dima's mega-test" );
}

void planner_test_2_actions				( )
{
	planner_type						planner;
	state_type							preconditions, effects;

	add_const_oracle					( planner, 0, false );
	add_const_oracle					( planner, 1, false );
	
	effects.clear						( );
	effects.add							( property_type( 0, true ) );
	add_operator						( planner, 0, preconditions, effects );

	effects.clear						( );
	effects.add							( property_type( 1, true ) );
	add_operator						( planner, 1, preconditions, effects );

	target_state_type					target;
	target.add							( property_type( 0, true ), target.properties().end() );
	target.add							( property_type( 1, true ), target.properties().end() );
	planner.target						( target );

//	planner.tracker().verbose			( true );
//	planner.forward_search				( true );
	search								search_service;
	planner.update						( search_service, "2 actions" );
}

enum predicates_type_enum
{
	predicate_type_clear,
	predicate_type_on_table,
	predicate_type_hand_empty,
	predicate_type_holding,
	predicate_type_on,
 	
	predicate_type_clear_disk,
 	predicate_type_clear_peg,
 	predicate_type_on_peg,
 	predicate_type_on_disk,
 	predicate_type_smaller,
	
	predicate_type_in_city,
	predicate_type_at,
	predicate_type_in
};

enum actions_type_enum
{
	pick_up,
	put_down,
	stack,
	unstack,
	
	move_from_disk_to_disk,
	move_from_disk_to_peg,
	move_from_peg_to_disk,
	move_from_peg_to_peg,

	drive,
	fly,
	load,
	unload
};

enum parameters_type_enum
{
	parameter_type_block,
 	
	parameter_type_disk,
 	parameter_type_peg,

	parameter_type_vehicle,
	parameter_type_thing,
	parameter_type_location
};

enum block_location_type_enum
{
	block_on_table,
	block_in_the_air
};

struct block : private boost::noncopyable
{
	explicit block	( block_location_type_enum location, block const* const above = 0 ) :
		is_on_table	( location == block_on_table ),
		is_holding	( location == block_in_the_air ),
		block_above	( above )
	{
	}
	
	bool const		is_on_table;
	bool const		is_holding;
	block const*	block_above;
};

bool is_hand_empty_predicate( )
{
	return					true;
}

bool is_clear_predicate		( block const* const parameter )
{
	return					parameter->block_above == 0;
}

bool is_on_table_predicate	( block const* const parameter )
{
	return					parameter->is_on_table;
}

bool is_holding_predicate	( block const* const parameter )
{
	return					parameter->is_holding;
}

bool is_block_on_block		( block const* const first_block, block const* const second_block )
{
	return					second_block->block_above == first_block;
}

void blocks_world_test				( )
{
	pddl_domain						blocks_domain;
	blocks_domain.register_parameter_type< block const* >( parameter_type_block );

	blocks_domain.add_predicate		( predicate_type_clear,		"clear",		&is_clear_predicate		);
	blocks_domain.add_predicate		( predicate_type_on_table,	"on_table",		&is_on_table_predicate	);
	blocks_domain.add_predicate		( predicate_type_hand_empty, "hand_empty",	&is_hand_empty_predicate);
	blocks_domain.add_predicate		( predicate_type_holding,	"holding",		&is_holding_predicate	);
	blocks_domain.add_predicate		( predicate_type_on,		"on",			&is_block_on_block		);

	// actions preconditions and effects filling
	generalized_action				pick_up( blocks_domain, xray::ai::planning::pick_up, "pick_up_action", 10 );
	pick_up.add_parameter_type		( parameter_type_block );
	pick_up.set_preconditions		(
		predicate( predicate_type_clear, _0 ) &&
		predicate( predicate_type_on_table, _0 ) &&
		predicate( predicate_type_hand_empty )
	);
	pick_up.set_effects				( 
		!predicate( predicate_type_on_table, _0 ) &&
		!predicate( predicate_type_clear, _0 ) &&
		!predicate( predicate_type_hand_empty ) &&
		predicate( predicate_type_holding, _0 )
	);
	blocks_domain.add_action		( &pick_up );

	generalized_action				put_down( blocks_domain, xray::ai::planning::put_down, "put_down_action", 10 );
	put_down.add_parameter_type		( parameter_type_block );
	put_down.set_preconditions		(
		predicate( predicate_type_holding, _0 )
	);
	put_down.set_effects			(
		!predicate( predicate_type_holding, _0 ) &&
		predicate( predicate_type_clear, _0 ) &&
		predicate( predicate_type_hand_empty ) &&
		predicate( predicate_type_on_table, _0 )
	);
	blocks_domain.add_action		( &put_down );

	generalized_action				stack( blocks_domain, xray::ai::planning::stack, "stack_action", 10 );
	stack.add_parameter_type		( parameter_type_block );
	stack.add_parameter_type		( parameter_type_block );
	stack.set_preconditions			(
		predicate( predicate_type_holding, _0 ) &&
		predicate( predicate_type_clear, _1 )
	);
	stack.set_effects				(
		!predicate( predicate_type_holding, _0 ) &&
		!predicate( predicate_type_clear, _1 ) &&
		predicate( predicate_type_clear, _0 ) &&
		predicate( predicate_type_hand_empty ) &&
		predicate( predicate_type_on, _0, _1 )
	);
	blocks_domain.add_action		( &stack );

	generalized_action				unstack( blocks_domain, xray::ai::planning::unstack, "unstack_action", 10 );
	unstack.add_parameter_type		( parameter_type_block );
	unstack.add_parameter_type		( parameter_type_block );
	unstack.set_preconditions		(
		predicate( predicate_type_on, _0, _1 ) &&
		predicate( predicate_type_clear, _0 ) &&
		predicate( predicate_type_hand_empty )
	);
	unstack.set_effects				(
		predicate( predicate_type_holding, _0 ) &&
		predicate( predicate_type_clear, _1 ) &&
		!predicate( predicate_type_clear, _0 ) &&
		!predicate( predicate_type_hand_empty ) &&
		!predicate( predicate_type_on, _0, _1 )
	);
	blocks_domain.add_action		( &unstack );

	pddl_problem					blocks_problem( blocks_domain );
	
	action_parameter parameter_block( parameter_type_block );
 	parameter_block.iterate_first	( false );
	boost::function< void () >		empty_action;

	action_instance					pick_up_instance( &pick_up );
	pick_up_instance.add_parameter	( &parameter_block );
	blocks_problem.add_action_instance( pick_up_instance, empty_action, empty_action, empty_action );

	action_instance					put_down_instance( &put_down );
	put_down_instance.add_parameter	( &parameter_block );
	blocks_problem.add_action_instance( put_down_instance, empty_action, empty_action, empty_action );

	action_instance					stack_instance( &stack );
	stack_instance.add_parameter	( &parameter_block );
	stack_instance.add_parameter	( &parameter_block );
	blocks_problem.add_action_instance( stack_instance, empty_action, empty_action, empty_action );

	action_instance					unstack_instance( &unstack );
	unstack_instance.add_parameter	( &parameter_block );
	unstack_instance.add_parameter	( &parameter_block );
	blocks_problem.add_action_instance( unstack_instance, empty_action, empty_action, empty_action );

	// specific problem description
	block const						block_A( block_on_table );
	block const						block_D( block_on_table );
// 	block const						block_C( block_on_table );
// 	block const						block_B( block_on_table );
 	block const						block_C( block_on_table, &block_D );
 	block const						block_B( block_on_table, &block_C );

	specified_problem				specific_problem( blocks_domain, blocks_problem, 0 );
	specific_problem.add_object		( &block_A, "block_A" );
	specific_problem.add_object		( &block_B, "block_B" );
	specific_problem.add_object		( &block_C, "block_C" );
	specific_problem.add_object		( &block_D, "block_D" );

// 	blocks_problem.set_target_state	( 
// 		!(!(predicate( predicate_type_on, &block_D, &block_C ) &&
// 		predicate( predicate_type_on, &block_B, &block_A )) &&
// 		!(predicate( predicate_type_on, &block_D, &block_C ) &&
// 		predicate( predicate_type_on, &block_C, &block_B ) &&
// 		predicate( predicate_type_on, &block_B, &block_A )))
// 	);

	specific_problem.set_target_state( 
		predicate( predicate_type_on, &block_D, &block_C ) &&
		predicate( predicate_type_on, &block_C, &block_B ) &&
		predicate( predicate_type_on, &block_B, &block_A )
	);

	pddl_planner					planner;
	search_type						new_search_service;
	planner.build_plan				( new_search_service, specific_problem, "blocks world test", true );
}

//////////////////////////////////////////////////////////////////////////
struct disk : private boost::noncopyable
{
	explicit disk	( float const required_radius, disk const* const above = 0 ) :
		radius		( required_radius ),
		disk_above	( above )
	{
	}
	
	disk const*		disk_above;
	float const		radius;
};

struct peg : private boost::noncopyable
{
		peg			( disk const* const lower = 0 ) :
		lower_disk	( lower )
	{
	}
	
	disk const*		lower_disk;
};

bool is_disk_clear	( disk const* const parameter )
{
	return			parameter->disk_above == 0;
}

bool is_peg_clear	( peg const* const parameter )
{
	return			parameter->lower_disk == 0;
}

bool is_disk_smaller( disk const* const disk1, disk const* const disk2 )
{
	return			disk1->radius < disk2->radius;
}

bool is_disk_on_peg	( disk const* const disk_to_check, peg const* const some_peg )
{
	return			some_peg->lower_disk == disk_to_check;
}

bool is_disk_on_disk( disk const* const upper_disk, disk const* const lower_disk )
{
	return			lower_disk->disk_above == upper_disk;
}

void towers_of_hanoi_test			( )
{
	// domain description
	pddl_domain						hanoi_domain;
	hanoi_domain.register_parameter_type< disk const* >( parameter_type_disk );
	hanoi_domain.register_parameter_type< peg const* >( parameter_type_peg );

	hanoi_domain.add_predicate		( predicate_type_clear_disk,	"is_disk_clear",	&is_disk_clear		);
	hanoi_domain.add_predicate		( predicate_type_on_peg,		"is_disk_on_peg",	&is_disk_on_peg		);
	hanoi_domain.add_predicate		( predicate_type_clear_peg,		"is_peg_clear",		&is_peg_clear		);
	hanoi_domain.add_predicate		( predicate_type_on_disk,		"is_disk_on_disk",	&is_disk_on_disk	);
	hanoi_domain.add_predicate		( predicate_type_smaller,		"is_disk_smaller",	&is_disk_smaller	);

	// actions preconditions and effects filling
	generalized_action				move_disk1( hanoi_domain, move_from_disk_to_disk, "move_from_disk_to_disk_action", 10 );
	move_disk1.add_parameter_type	( parameter_type_disk );
	move_disk1.add_parameter_type	( parameter_type_disk );
	move_disk1.add_parameter_type	( parameter_type_disk );
	move_disk1.set_preconditions	(
		predicate( predicate_type_clear_disk, _0 ) &&
		predicate( predicate_type_on_disk, _0, _1 ) &&
		predicate( predicate_type_clear_disk, _2 ) &&
		predicate( predicate_type_smaller, _0, _2 )
	);
	move_disk1.set_effects			(
		predicate( predicate_type_on_disk, _0, _2 ) &&
		!predicate( predicate_type_on_disk, _0, _1 ) &&
		!predicate( predicate_type_clear_disk, _2 ) &&
		predicate( predicate_type_clear_disk, _1 )
	);
	hanoi_domain.add_action			( &move_disk1 );

	generalized_action				move_disk2( hanoi_domain, move_from_disk_to_peg, "move_from_disk_to_peg_action", 10 );
	move_disk2.add_parameter_type	( parameter_type_disk );
	move_disk2.add_parameter_type	( parameter_type_disk );
	move_disk2.add_parameter_type	( parameter_type_peg );
	move_disk2.set_preconditions	(
		predicate( predicate_type_clear_disk, _0 ) &&
		predicate( predicate_type_on_disk, _0, _1 ) &&
		predicate( predicate_type_clear_peg, _2 )
	);
	move_disk2.set_effects			(
		predicate( predicate_type_on_peg, _0, _2 ) &&
		!predicate( predicate_type_on_disk, _0, _1 ) &&
		!predicate( predicate_type_clear_peg, _2 ) &&
		predicate( predicate_type_clear_disk, _1 )
	);
	hanoi_domain.add_action			( &move_disk2 );

	generalized_action				move_disk3( hanoi_domain, move_from_peg_to_disk, "move_from_peg_to_disk_action", 10 );
	move_disk3.add_parameter_type	( parameter_type_disk );
	move_disk3.add_parameter_type	( parameter_type_peg );
	move_disk3.add_parameter_type	( parameter_type_disk );
	move_disk3.set_preconditions	(
		predicate( predicate_type_clear_disk, _0 ) &&
		predicate( predicate_type_on_peg, _0, _1 ) &&
		predicate( predicate_type_clear_disk, _2 ) &&
		predicate( predicate_type_smaller, _0, _2 )
	);
	move_disk3.set_effects			(
		predicate( predicate_type_on_disk, _0, _2 ) &&
		!predicate( predicate_type_on_peg, _0, _1 ) &&
		!predicate( predicate_type_clear_disk, _2 ) &&
		predicate( predicate_type_clear_peg, _1 )
	);
	hanoi_domain.add_action			( &move_disk3 );
	
	generalized_action				move_disk4( hanoi_domain, move_from_peg_to_peg, "move_from_peg_to_peg_action", 10 );
	move_disk4.add_parameter_type	( parameter_type_disk );
	move_disk4.add_parameter_type	( parameter_type_peg );
	move_disk4.add_parameter_type	( parameter_type_peg );
	move_disk4.set_preconditions	(
		predicate( predicate_type_clear_disk, _0 ) &&
		predicate( predicate_type_on_peg, _0, _1 ) &&
		predicate( predicate_type_clear_peg, _2 )
	);
	move_disk4.set_effects			(
		predicate( predicate_type_on_peg, _0, _2 ) &&
		!predicate( predicate_type_on_peg, _0, _1 ) &&
		!predicate( predicate_type_clear_peg, _2 ) &&
		predicate( predicate_type_clear_peg, _1 )
	);
	hanoi_domain.add_action			( &move_disk4 );

	action_parameter				parameter_disk( parameter_type_disk );
 	parameter_disk.iterate_first	( false );
	action_parameter				parameter_peg( parameter_type_peg );
 	parameter_peg.iterate_first		( false );

	boost::function< void () >		empty_action;

	pddl_problem					hanoi_problem( hanoi_domain );
	action_instance					move_disk1_instance( &move_disk1 );
	move_disk1_instance.add_parameter( &parameter_disk );
	move_disk1_instance.add_parameter( &parameter_disk );
	move_disk1_instance.add_parameter( &parameter_disk );
	hanoi_problem.add_action_instance( move_disk1_instance, empty_action, empty_action, empty_action );

	action_instance					move_disk2_instance( &move_disk2 );
	move_disk2_instance.add_parameter( &parameter_disk );
	move_disk2_instance.add_parameter( &parameter_disk );
	move_disk2_instance.add_parameter( &parameter_peg );
	hanoi_problem.add_action_instance( move_disk2_instance, empty_action, empty_action, empty_action );

	action_instance					move_disk3_instance( &move_disk3 );
	move_disk3_instance.add_parameter( &parameter_disk );
	move_disk3_instance.add_parameter( &parameter_peg );
	move_disk3_instance.add_parameter( &parameter_disk );
	hanoi_problem.add_action_instance( move_disk3_instance, empty_action, empty_action, empty_action );

	action_instance					move_disk4_instance( &move_disk4 );
	move_disk4_instance.add_parameter( &parameter_disk );
	move_disk4_instance.add_parameter( &parameter_peg );
	move_disk4_instance.add_parameter( &parameter_peg );
	hanoi_problem.add_action_instance( move_disk4_instance, empty_action, empty_action, empty_action );

	// problem description
	disk const						disk1( 1.f );
	disk const						disk2( 2.f, &disk1 );
	disk const						disk3( 3.f, &disk2 );
	disk const						disk4( 4.f, &disk3 );
	disk const						disk5( 5.f, &disk4 );
	
	peg	const						peg1( &disk5 );
	peg	const						peg2;
	peg	const						peg3;

	specified_problem				actual_problem( hanoi_domain, hanoi_problem, 0 );
	actual_problem.add_object		( &disk1, "disk1" );
	actual_problem.add_object		( &disk2, "disk2" );
	actual_problem.add_object		( &disk3, "disk3" );
	actual_problem.add_object		( &disk4, "disk4" );
	actual_problem.add_object		( &disk5, "disk5" );

	actual_problem.add_object		( &peg1, "peg1" );
	actual_problem.add_object		( &peg2, "peg2" );
	actual_problem.add_object		( &peg3, "peg3" );

	/*
	hanoi_problem.set_target_state	( 
			predicate( predicate_type_on_disk, &disk1, &disk2 ) &&
			predicate( predicate_type_on_disk, &disk2, &disk3 ) &&
			predicate( predicate_type_on_disk, &disk3, &disk4 ) &&
			predicate( predicate_type_on_disk, &disk4, &disk5 ) &&
			predicate( predicate_type_on_peg, &disk5, &peg3 )
	);
	*/
	actual_problem.set_target_state	( 
		(
			predicate( predicate_type_on_disk, &disk1, &disk2 ) &&
			predicate( predicate_type_on_disk, &disk2, &disk3 ) &&
			predicate( predicate_type_on_disk, &disk3, &disk4 ) &&
			predicate( predicate_type_on_disk, &disk4, &disk5 ) &&
			predicate( predicate_type_on_peg, &disk5, &peg3 )
 		) ||
		(
			//predicate( predicate_type_on_disk, &disk1, &disk2 ) &&
			predicate( predicate_type_on_disk, &disk2, &disk3 ) &&
			predicate( predicate_type_on_disk, &disk3, &disk4 ) &&
			predicate( predicate_type_on_disk, &disk4, &disk5 ) &&
			predicate( predicate_type_on_peg, &disk5, &peg2 )
		)
	);

	pddl_planner					planner;
	search_type						new_search_service;
	planner.build_plan				( new_search_service, actual_problem, "towers of Hanoi test", true );
}

//#endif // #ifdef TEST_PLANNER

} // namespace planning
} // namespace ai
} // namespace xray