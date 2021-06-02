////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "ai_world.h"
#include <xray/ai/api.h>
#include <xray/linkage_helper.h>
#include <xray/ai/sound_collection_types.h>
#include <xray/ai/goal_types.h>
#include <xray/ai/action_types.h>
#include <xray/ai/parameter_types.h>
#include <xray/ai/filter_types.h>
#include "predicate_types.h"
#include <xray/memory_stack_allocator.h>

XRAY_DECLARE_LINKAGE_ID( ai_entry_point )

xray::ai::allocator_type* xray::ai::g_allocator	= 0;

namespace xray {

namespace foo {
	void bar	( );
} // namespace foo

namespace ai {

static bool are_enums_exported = false;

} // namespace ai
} // namespace xray

xray::ai::world* xray::ai::create_world	( engine& engine )
{	
#ifndef MASTER_GOLD	
	if ( !xray::ai::are_enums_exported )
	{
		xray::configs::export_enum( "sound_collections", xray::ai::sounds_tuples, xray::ai::sounds_tuples + array_size( xray::ai::sounds_tuples ) );
		xray::configs::export_enum( "npc_goals", xray::ai::planning::goals_tuples, xray::ai::planning::goals_tuples + array_size( xray::ai::planning::goals_tuples ) );
		xray::configs::export_enum( "npc_actions", xray::ai::planning::actions_tuples, xray::ai::planning::actions_tuples + array_size( xray::ai::planning::actions_tuples ) );
		xray::configs::export_enum( "selector_filters", xray::ai::filter_tuples, xray::ai::filter_tuples + array_size( xray::ai::filter_tuples ) );
		xray::configs::export_enum( "enemy_filters", xray::ai::enemy_filter_tuples, xray::ai::enemy_filter_tuples + array_size( xray::ai::enemy_filter_tuples ) );
		xray::configs::export_enum( "weapon_filters", xray::ai::weapon_filter_tuples, xray::ai::weapon_filter_tuples + array_size( xray::ai::weapon_filter_tuples ) );
		xray::configs::export_enum( "cover_filters", xray::ai::cover_filter_tuples, xray::ai::cover_filter_tuples + array_size( xray::ai::cover_filter_tuples ) );
		xray::configs::export_enum( "animation_filters", xray::ai::animation_filter_tuples, xray::ai::animation_filter_tuples + array_size( xray::ai::animation_filter_tuples ) );
		xray::configs::export_enum( "sound_filters", xray::ai::sound_filter_tuples, xray::ai::sound_filter_tuples + array_size( xray::ai::sound_filter_tuples ) );
		xray::configs::export_enum( "position_filters", xray::ai::position_filter_tuples, xray::ai::position_filter_tuples + array_size( xray::ai::position_filter_tuples ) );
		xray::configs::export_enum( "world_state_properties", xray::ai::world_state_properties_tuples, xray::ai::world_state_properties_tuples + array_size( xray::ai::world_state_properties_tuples ) );
		xray::ai::are_enums_exported = true;
	}
#endif // #ifndef MASTER_GOLD
	//foo::bar	( );
	return		NEW( ai_world )( engine );
}

void xray::ai::destroy_world	( xray::ai::world*& world )
{
	ai_world* world_instance	= static_cast_checked< xray::ai::ai_world* >( world );
	DELETE						( world_instance );
	world						= 0;
}

void xray::ai::set_memory_allocator		( allocator_type& allocator )
{
	ASSERT						( !g_allocator );
	g_allocator					= &allocator;
}

////////////////////////////////////////////////////////////////////////
#ifndef MASTER_GOLD

namespace xray {
namespace foo {

union property_value {
	pvoid	instance_ptr;
	u32		value;
}; // union property_value

enum parameters_types_enum
{
	parameter_type_npc_ptr,
	parameter_type_weapon_ptr
};

struct npc_base
{
	virtual bool is_alive() const = 0;
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( npc_base )
};

struct extra_memory {
	virtual ~extra_memory() {}
	u64 memory;
};

struct npc : private extra_memory, public npc_base
{
	virtual bool is_alive() const { return alive; }	
	
	bool	alive;
}; // struct npc

struct weapon {
	bool	is_loaded;
}; // struct weapon


bool callee	( npc* const npc )
{
	return	npc->alive;
}

bool callee_base( npc_base* const npc )
{
	return	npc->is_alive();
}

bool callee1( weapon* const ruzhzho )
{
	return	ruzhzho->is_loaded;
}

struct memory_leak : public intrusive_base {
	~memory_leak()
	{
		LOG_INFO	( "memory_leak::~memory_leak called" );
	}
}; // struct memory_leak

typedef intrusive_ptr< memory_leak, intrusive_base, threading::single_threading_policy > memory_leak_ptr;

bool callee2( memory_leak_ptr const& leak, npc_base* const npc, weapon* const ruzhzho )
{
	XRAY_UNREFERENCED_PARAMETER	( leak );
	return	npc->is_alive() && ruzhzho->is_loaded;
}

bool callee3( npc* const first, npc* const second )
{
	return	first < second;
}

typedef fixed_vector< property_value, 4 >	property_values_type;
typedef std::pair< pcstr, parameters_types_enum > parameter_type;
typedef fixed_vector< parameter_type, 8 > registered_parameters_type;
typedef fixed_vector< parameters_types_enum, 4 > predicate_parameters_type;

struct pddl_predicate;
typedef bool ( *predicate_binder_function_ptr )( pddl_predicate const& predicate, property_values_type const& values );

struct pddl_predicate {
//	char							buffer[ sizeof( boost::function< bool () > ) ];
	boost::function< bool () >		buffer;
	predicate_binder_function_ptr	predicate_binder;
	u32								predicate_id;
	predicate_parameters_type		predicate_parameters;
}; // struct pddl_predicate

template < typename T >
inline void parse_parameter			( pddl_predicate& predicate, registered_parameters_type const& types_dictionary )
{
	for ( u32 i = 0; i < types_dictionary.size(); ++i )
	{
		if ( strings::equal( types_dictionary[i].first, typeid( T ).name() ) )
		{
			predicate.predicate_parameters.push_back( types_dictionary[i].second );
			return;
		}
	}
	LOG_ERROR						( "unregistered parameter type" );
	UNREACHABLE_CODE				( );
}

inline bool predicate_binder( pddl_predicate const& predicate, property_values_type const& values )
{
	R_ASSERT_CMP		( values.size(), ==, 0 );
	return				( *horrible_cast< pcvoid, boost::function< bool ( ) >* >( &predicate.buffer ).second )();
}

template < typename T1, typename T2 >
inline bool predicate_binder( pddl_predicate const& predicate, property_values_type const& values )
{
	R_ASSERT_CMP		( values.size(), ==, 1 );
	return
		(*horrible_cast<
			pcvoid,
			boost::function< bool ( T2 ) >*
		>( &predicate.buffer ).second)(
			static_cast_checked< T2 > (
				horrible_cast< pvoid, T1 >( values[0].instance_ptr ).second
			)
		);
}

template < typename T1, typename T2, typename T3, typename T4 >
inline bool predicate_binder( pddl_predicate const& predicate, property_values_type const& values )
{
	R_ASSERT_CMP		( values.size(), ==, 2 );
	return
		(*horrible_cast<
			pcvoid,
			boost::function< bool ( T3, T4 ) >*
		>( &predicate.buffer ).second)(
			static_cast_checked< T3 >(
				horrible_cast< pvoid, T1 >( values[0].instance_ptr ).second
			),
			static_cast_checked< T4 >(
				horrible_cast< pvoid, T2 >( values[1].instance_ptr ).second
			)
		);
}

template < typename T1, typename T2 >
inline pddl_predicate new_predicate	(
		u32 const predicate_id,
		boost::function< bool ( T2 ) > const& predicate,
		registered_parameters_type const& types_dictionary
	)
{
	pddl_predicate			result;

	result.buffer.~function< bool () >( );
	new ( &result.buffer ) boost::function< bool ( T2 ) >( predicate );

	result.predicate_binder	= &predicate_binder< T1, T2 >;
	result.predicate_id		= predicate_id;
	parse_parameter< T1 >	( result, types_dictionary );

	return					result;
}

template < typename T >
inline pddl_predicate new_predicate	(
		u32 const predicate_id,
		boost::function< bool ( T ) > const& predicate,
		registered_parameters_type const& types_dictionary
	)
{
	return					new_predicate< T, T >( predicate_id, predicate, types_dictionary );
}

template < typename T1, typename T2, typename T3, typename T4 >
inline pddl_predicate new_predicate2	(
		u32 const predicate_id,
		boost::function< bool ( T3, T4 ) > const& predicate,
		registered_parameters_type const& types_dictionary
	)
{
	pddl_predicate			result;

	result.buffer.~function< bool () >( );
	new ( &result.buffer ) boost::function< bool ( T3, T4 ) >( predicate );

	result.predicate_binder	= &predicate_binder< T1, T2, T3, T4 >;
	result.predicate_id		= predicate_id;
	parse_parameter< T1 >	( result, types_dictionary );
	parse_parameter< T2 >	( result, types_dictionary );

	return					result;
}

template < typename T1, typename T2 >
inline pddl_predicate new_predicate	(
		u32 const predicate_id,
		boost::function< bool ( T1, T2 ) > const& predicate,
		registered_parameters_type const& types_dictionary
	)
{
	return					new_predicate2< T1, T2, T1, T2 >( predicate_id, predicate, types_dictionary );
}

template < typename T >
inline pddl_predicate new_predicate	(
		u32 const predicate_id,
		bool (*predicate) ( T ),
		registered_parameters_type const& types_dictionary
	)
{
	return new_predicate( predicate_id, boost::function< bool ( T ) >( predicate ), types_dictionary );
}

template < typename T1, typename T2 >
inline pddl_predicate new_predicate	(
		u32 const predicate_id,
		bool (*predicate) ( T2 ),
		registered_parameters_type const& types_dictionary
	)
{
	return new_predicate<T1>( predicate_id, boost::function< bool ( T2 ) >( predicate ), types_dictionary );
}

template < typename T1, typename T2 >
inline pddl_predicate new_predicate	(
		u32 const predicate_id,
		bool (*predicate) ( T1, T2 ),
		registered_parameters_type const& types_dictionary
	)
{
	return new_predicate<T1, T2>( predicate_id, boost::function< bool ( T1, T2 ) >( predicate ), types_dictionary );
}

template < typename T1, typename T2, typename T3, typename T4 >
inline pddl_predicate new_predicate2	(
		u32 const predicate_id,
		bool (*predicate) ( T3, T4 ),
		registered_parameters_type const& types_dictionary
	)
{
	return new_predicate2<T1, T2, T3, T4>( predicate_id, boost::function< bool ( T3, T4 ) >( predicate ), types_dictionary );
}

typedef xray::fixed_string<4096>	string_type;

struct operands_calculator
{
	inline	operands_calculator	( u32 const operands_count, u32 const and_count, u32 const or_count ) :
		operands_count			( operands_count ),
		and_count				( and_count ),
		or_count				( or_count )
	{
	}

	u32 						operands_count;
	u32 						and_count;
	u32 						or_count;
}; // struct operands_calculator

inline operands_calculator operator +	( operands_calculator const& left, operands_calculator const& right )
{
	return operands_calculator	(
				left.operands_count + right.operands_count,
				left.and_count + right.and_count,
				left.or_count + right.or_count
			);
}

class base_lexeme_ptr;

struct XRAY_NOVTABLE base_lexeme : private boost::noncopyable {
	virtual						~base_lexeme			( ) { }
	virtual	string_type			get_name				( ) const = 0;
	virtual	void				invert_value			( ) const = 0;

	virtual	base_lexeme_ptr		open_brackets			( memory::stack_allocator& allocator ) const = 0;
	virtual	base_lexeme_ptr		open_brackets			( memory::stack_allocator& allocator, base_lexeme const& right ) const = 0;
	virtual	base_lexeme_ptr		generate_permutations	( memory::stack_allocator& allocator, base_lexeme const& left ) const = 0;

	inline	u32	get_memory_size_after_opening_brackets	( ) const;

	virtual	operands_calculator fill_operands_count_after_opening_brackets	( ) const = 0;

private:
	friend class base_lexeme_ptr;
	virtual	void				increment_counter		( ) const = 0;
	virtual	void				decrement_counter		( ) const = 0;
}; // struct base_lexeme

class base_lexeme_ptr {
public:
	inline	base_lexeme_ptr	( ) : m_lexeme( 0 ) { }
	inline	base_lexeme_ptr	( base_lexeme const* const lexeme ) :
		m_lexeme	( lexeme )
	{
		if ( m_lexeme )
			m_lexeme->increment_counter	( );
	}

	inline	base_lexeme_ptr	( base_lexeme_ptr const& other ) :
		m_lexeme	( other.m_lexeme )
	{
		if ( m_lexeme )
			m_lexeme->increment_counter	( );
	}

	inline	~base_lexeme_ptr( ) {
		if ( !m_lexeme )
			return;

		m_lexeme->decrement_counter	( );
	}

	inline	base_lexeme_ptr& operator =	( base_lexeme_ptr const& other )
	{
		base_lexeme_ptr( other ).swap( *this );
		return						*this;
	}

	inline void swap					( base_lexeme_ptr& other )
	{
		std::swap					( m_lexeme, other.m_lexeme );
	}

	inline base_lexeme const * operator ->		( ) const
	{
		return						m_lexeme;
	}

	inline base_lexeme const& operator *		( ) const
	{
		R_ASSERT					( m_lexeme );
		return						*m_lexeme;
	}

private:
	base_lexeme const* m_lexeme;
}; // class base_lexeme_ptr

u32 g_and_ctor_counter			= 0;
u32 g_and_dtor_counter			= 0;

u32 g_or_ctor_counter			= 0;
u32 g_or_dtor_counter			= 0;

class binary_operation_lexeme : public base_lexeme
{
public:
	enum operation_type_enum
	{
		operation_type_and,
		operation_type_or
	}; // enum operation_type_enum

public:
	inline	binary_operation_lexeme	(
		operation_type_enum const operation_type,
		base_lexeme const& left,
		base_lexeme const& right,
		bool const destroy_manually
	) :
		m_left						( &left ),
		m_right						( &right ),
		m_operation_type			( static_cast<u8>(operation_type) ),
		m_destroy_manually			( destroy_manually ),
		m_counter					( 0 )
	{
		reset_pointers				( );
		( this->*m_function_pointers->m_ctor )			( );
	}

	inline	~binary_operation_lexeme( )
	{
		( this->*m_function_pointers->m_dtor )			( );
	}

	virtual	string_type			get_name				( ) const
	{
		return ( this->*m_function_pointers->m_name_getter )( );
	}
	
	virtual	void				invert_value			( ) const
	{
		( this->*m_function_pointers->m_value_invertor )( );
	}

	virtual	base_lexeme_ptr		open_brackets			( memory::stack_allocator& allocator ) const
	{
		return ( this->*m_function_pointers->m_brackets_opener1 )( allocator );
	}
	
	virtual	base_lexeme_ptr		open_brackets			( memory::stack_allocator& allocator, base_lexeme const& right ) const
	{
		return ( this->*m_function_pointers->m_brackets_opener2 )( allocator, right );
	}
	
	virtual	base_lexeme_ptr		generate_permutations	( memory::stack_allocator& allocator, base_lexeme const& left ) const
	{
		base_lexeme_ptr result	= ( this->*m_function_pointers->m_generator )( allocator, left );
		return result;
	}

	virtual	operands_calculator fill_operands_count_after_opening_brackets	( ) const
	{
		return ( this->*m_function_pointers->m_operands_counter )( );
	}

	virtual	void				increment_counter		( ) const
	{
		if ( !m_destroy_manually )
			return;

		++m_counter;
	}

	virtual	void				decrement_counter		( ) const
	{
		if ( !m_destroy_manually )
			return;

		R_ASSERT				( m_counter );
		if ( !--m_counter )
			this->~binary_operation_lexeme	( );
	}

private:
	void						reset_pointers			( ) const;

	void						construct_as_and		( ) const;
	void						construct_as_or			( ) const;
	void						destruct_as_and			( ) const;
	void						destruct_as_or			( ) const;

	string_type					get_name_as_and			( ) const;
	string_type					get_name_as_or			( ) const;
	void						invert_value_as_and		( ) const;
	void						invert_value_as_or		( ) const;
	operands_calculator			count_operands_as_and	( ) const;
	operands_calculator			count_operands_as_or	( ) const;
	base_lexeme_ptr				open_brackets_as_and	( memory::stack_allocator& allocator ) const;
	base_lexeme_ptr				open_brackets_as_or		( memory::stack_allocator& allocator ) const;
	base_lexeme_ptr				open_brackets_as_and	( memory::stack_allocator& allocator, base_lexeme const& right ) const;
	base_lexeme_ptr				open_brackets_as_or		( memory::stack_allocator& allocator, base_lexeme const& right ) const;
	base_lexeme_ptr			generate_permutations_as_and( memory::stack_allocator& allocator, base_lexeme const& left ) const;
	base_lexeme_ptr			generate_permutations_as_or	( memory::stack_allocator& allocator, base_lexeme const& left ) const;

private:
	class function_pointers {
	public:
				function_pointers	( operation_type_enum const operation_type )
		{
			switch ( operation_type )
			{
				case operation_type_and:
				{
					m_ctor				= &binary_operation_lexeme::construct_as_and;
					m_dtor				= &binary_operation_lexeme::destruct_as_and;
					m_name_getter		= &binary_operation_lexeme::get_name_as_and;
					m_value_invertor	= &binary_operation_lexeme::invert_value_as_and;
					m_operands_counter	= &binary_operation_lexeme::count_operands_as_and;
					m_brackets_opener1	= &binary_operation_lexeme::open_brackets_as_and;
					m_brackets_opener2	= &binary_operation_lexeme::open_brackets_as_and;
					m_generator			= &binary_operation_lexeme::generate_permutations_as_and;
				}
				break;

				case operation_type_or:
				{
					m_ctor				= &binary_operation_lexeme::construct_as_or;
					m_dtor				= &binary_operation_lexeme::destruct_as_or;
					m_name_getter		= &binary_operation_lexeme::get_name_as_or;
					m_value_invertor	= &binary_operation_lexeme::invert_value_as_or;
					m_operands_counter	= &binary_operation_lexeme::count_operands_as_or;
					m_brackets_opener1	= &binary_operation_lexeme::open_brackets_as_or;
					m_brackets_opener2	= &binary_operation_lexeme::open_brackets_as_or;
					m_generator			= &binary_operation_lexeme::generate_permutations_as_or;
				}
				break;

				default:
					NODEFAULT			( );
			}
		}

	private:
		typedef void				( binary_operation_lexeme::*constructor )		( ) const;
		typedef void				( binary_operation_lexeme::*destructor )		( ) const;
		typedef string_type			( binary_operation_lexeme::*name_getter )		( ) const;
		typedef void				( binary_operation_lexeme::*value_invertor )	( ) const;
		typedef operands_calculator	( binary_operation_lexeme::*operands_counter )	( ) const;
		typedef base_lexeme_ptr		( binary_operation_lexeme::*brackets_opener1 )	( memory::stack_allocator& allocator ) const;
		typedef base_lexeme_ptr		( binary_operation_lexeme::*brackets_opener2 )	( memory::stack_allocator& allocator, base_lexeme const& right ) const;
		typedef base_lexeme_ptr		( binary_operation_lexeme::*generator )			( memory::stack_allocator& allocator, base_lexeme const& left ) const;

	public:
		constructor					m_ctor;
		destructor					m_dtor;
		name_getter					m_name_getter;
		value_invertor				m_value_invertor;
		operands_counter			m_operands_counter;
		brackets_opener1			m_brackets_opener1;
		brackets_opener2			m_brackets_opener2;
		generator					m_generator;
	}; // class function_pointers

	static const function_pointers	s_or_function_pointers;
	static const function_pointers	s_and_function_pointers;

private:
	mutable function_pointers const*m_function_pointers;
	mutable base_lexeme_ptr			m_left;
	mutable base_lexeme_ptr			m_right;
	mutable u16						m_counter;
	u8 const						m_destroy_manually;
	mutable u8						m_operation_type;
};

binary_operation_lexeme::function_pointers const	binary_operation_lexeme::s_or_function_pointers = binary_operation_lexeme::function_pointers( binary_operation_lexeme::operation_type_or );
binary_operation_lexeme::function_pointers const	binary_operation_lexeme::s_and_function_pointers = binary_operation_lexeme::function_pointers( binary_operation_lexeme::operation_type_and );

void binary_operation_lexeme::reset_pointers	( ) const
{
	m_function_pointers				= m_operation_type == operation_type_or ? &s_or_function_pointers : &s_and_function_pointers;
}

debug::set< pcvoid > g_pointers;

static inline void register_pointer		( pcvoid const pointer )
{
	R_ASSERT			( g_pointers.find( pointer ) == g_pointers.end() );
	g_pointers.insert	( pointer );
}

static inline void unregister_pointer	( pcvoid const pointer )
{
	R_ASSERT			( g_pointers.find( pointer ) != g_pointers.end() );
	g_pointers.erase	( g_pointers.find( pointer ) );
}

void binary_operation_lexeme::construct_as_and	( ) const
{
	register_pointer			( this );
	LOG_INFO					( " AND (%2d) : 0x%08x, (%s && %s)", g_and_ctor_counter++, this, m_left->get_name().c_str(), m_right->get_name().c_str() );
}

void binary_operation_lexeme::construct_as_or	( ) const
{
	register_pointer			( this );
	LOG_INFO					( " OR  (%2d) : 0x%08x, (%s || %s)", g_or_ctor_counter++, this, m_left->get_name().c_str(), m_right->get_name().c_str() );
}

void binary_operation_lexeme::destruct_as_and	( ) const
{
	unregister_pointer			( this );
	LOG_INFO					( "~AND (%2d) : 0x%08x, (%s && %s)", g_and_dtor_counter++, this, m_left->get_name().c_str(), m_right->get_name().c_str() );
}

void binary_operation_lexeme::destruct_as_or	( ) const
{
	unregister_pointer			( this );
	LOG_INFO					( "~OR  (%2d) : 0x%08x, (%s || %s)", g_or_dtor_counter++, this, m_left->get_name().c_str(), m_right->get_name().c_str() );
}

string_type binary_operation_lexeme::get_name_as_and( ) const
{
	string_type					temp;
	temp.appendf				( "(%s && %s)", m_left->get_name().c_str(), m_right->get_name().c_str() );
	return						temp;
}

string_type binary_operation_lexeme::get_name_as_or	( ) const
{
	string_type					temp;
	temp.appendf				( "(%s || %s)", m_left->get_name().c_str(), m_right->get_name().c_str() );
	return						temp;
}

base_lexeme_ptr binary_operation_lexeme::open_brackets_as_and			( memory::stack_allocator& allocator ) const
{
	base_lexeme_ptr left		= m_left->open_brackets( allocator );
	base_lexeme_ptr right		= m_right->open_brackets( allocator );
	return						left->open_brackets( allocator, *right );
}

base_lexeme_ptr binary_operation_lexeme::open_brackets_as_or			( memory::stack_allocator& allocator ) const
{
	XRAY_UNREFERENCED_PARAMETER	( allocator );
	return						this;
}

base_lexeme_ptr binary_operation_lexeme::open_brackets_as_and			( memory::stack_allocator& allocator, base_lexeme const& right ) const
{
	return						right.generate_permutations( allocator, *this );
}

base_lexeme_ptr binary_operation_lexeme::open_brackets_as_or			( memory::stack_allocator& allocator, base_lexeme const& right ) const
{
	return						XRAY_NEW_IMPL( allocator, binary_operation_lexeme )(
									operation_type_or,
									*m_left->open_brackets( allocator, right ),
									*m_right->open_brackets( allocator, right ),
									true
								);
}

base_lexeme_ptr binary_operation_lexeme::generate_permutations_as_and	( memory::stack_allocator& allocator, base_lexeme const& left ) const
{
	return						XRAY_NEW_IMPL( allocator, binary_operation_lexeme )( operation_type_and, left, *this, true );
}

base_lexeme_ptr binary_operation_lexeme::generate_permutations_as_or	( memory::stack_allocator& allocator, base_lexeme const& left ) const
{
	return						XRAY_NEW_IMPL( allocator, binary_operation_lexeme )(
									operation_type_or,
									*m_left->generate_permutations( allocator, left ),
									*m_right->generate_permutations( allocator, left ),
									true
								);
}

void binary_operation_lexeme::invert_value_as_and		( ) const
{
	( this->*m_function_pointers->m_dtor )		( );

	m_left->invert_value						( );
	m_right->invert_value						( );

	m_operation_type							= operation_type_or;
	reset_pointers								( );
	( this->*m_function_pointers->m_ctor )		( );
}

void binary_operation_lexeme::invert_value_as_or		( ) const
{
	( this->*m_function_pointers->m_dtor )		( );

	m_left->invert_value						( );
	m_right->invert_value						( );

	m_operation_type							= operation_type_and;
	reset_pointers								( );
	( this->*m_function_pointers->m_ctor )		( );
}

operands_calculator binary_operation_lexeme::count_operands_as_and	( ) const
{
	operands_calculator const& left				= m_left->fill_operands_count_after_opening_brackets( );
	operands_calculator const& right			= m_right->fill_operands_count_after_opening_brackets( );
	return operands_calculator					(
			left.operands_count * right.operands_count,
			left.operands_count * right.operands_count + left.and_count + right.and_count,
			left.or_count + right.or_count + ( left.operands_count * right.operands_count - 1 )
		);
}

operands_calculator binary_operation_lexeme::count_operands_as_or	( ) const
{
	return										m_left->fill_operands_count_after_opening_brackets( ) + 
												m_right->fill_operands_count_after_opening_brackets( );
}

class predicate_lexeme : public base_lexeme
{
public:
	inline explicit predicate_lexeme	( pcstr const predicate_id ) :
		m_predicate_id			( predicate_id ),
		m_value					( true )
	{
		register_pointer		( this );
	}

	inline ~predicate_lexeme			( )
	{
		unregister_pointer		( this );
	}

	virtual	string_type	get_name	( ) const
	{
		if ( m_value )
			return				m_predicate_id;

		string_type				temp;
		temp.appendf			( "!%s", m_predicate_id );
		return					temp;
	}

	virtual	void invert_value		( ) const
	{
		m_value					= !m_value;
	}

	virtual	base_lexeme_ptr		open_brackets	( memory::stack_allocator& allocator ) const
	{
		XRAY_UNREFERENCED_PARAMETER	( allocator );
		return					this;
	}

	virtual	base_lexeme_ptr		open_brackets	( memory::stack_allocator& allocator, base_lexeme const& right ) const
	{
		return					right.generate_permutations( allocator, *this );
	}

	virtual	base_lexeme_ptr		generate_permutations	( memory::stack_allocator& allocator, base_lexeme const& left ) const
	{
		return					XRAY_NEW_IMPL( allocator, binary_operation_lexeme ) ( binary_operation_lexeme::operation_type_and, left, *this, true );
	}

	virtual	void				increment_counter	( ) const
	{
	}

	virtual	void				decrement_counter	( ) const
	{
	}

	virtual	operands_calculator fill_operands_count_after_opening_brackets	( ) const
	{
		return					operands_calculator( 1, 0, 0 );
	}

private:
	pcstr const					m_predicate_id;
	mutable bool				m_value;
}; // class predicate_lexeme

inline u32 base_lexeme::get_memory_size_after_opening_brackets	( ) const
{
	operands_calculator const& result	= fill_operands_count_after_opening_brackets( );
	return sizeof( binary_operation_lexeme ) * ( result.or_count  + result.and_count );
}

inline binary_operation_lexeme operator &&	( base_lexeme const& left, base_lexeme const& right )
{
	return						binary_operation_lexeme( binary_operation_lexeme::operation_type_and, left, right, false );
}

inline binary_operation_lexeme operator ||	( base_lexeme const& left, base_lexeme const& right )
{
	return						binary_operation_lexeme( binary_operation_lexeme::operation_type_or, left, right, false );
}

template < typename T >
inline T const& operator !		( T const& expression )
{
	expression.invert_value		( );
	return						expression;
}

inline void functor				( base_lexeme const& expression )
{
	memory::stack_allocator		allocator;
	u32 const memory_size		= expression.get_memory_size_after_opening_brackets( );
	allocator.initialize		( ALLOCA( memory_size ), memory_size, "PDDL expressions" );
	base_lexeme_ptr const new_expression	= expression.open_brackets( allocator );
	R_ASSERT_CMP				( allocator.total_size(), == , allocator.allocated_size() );
	LOG_INFO					( "old expression : %s", expression.get_name().c_str() );
	LOG_INFO					( "new expression : %s", new_expression->get_name().c_str() );
}

struct mega_classets {
	void foo()
	{
		LOG_TRACE	( "aga = %d", aga );
	}

	int aga;
}; // struct mega_classets

void bar	( )
{
	mega_classets instance_of_classets;
	instance_of_classets.aga = 25101983;
	boost::function< void (mega_classets*) > foo = boost::bind( &mega_classets::foo, _1 );
	foo( &instance_of_classets );

	//functor	(
	//	!( predicate_lexeme( "a" ) || predicate_lexeme( "b" ) )
	//);
// 	
// 	R_ASSERT_CMP				( g_and_ctor_counter, ==, g_and_dtor_counter );
// 	R_ASSERT_CMP				( g_or_ctor_counter, ==, g_or_dtor_counter );
	
	functor	(
		(
			!predicate_lexeme( "a" ) || 
			 predicate_lexeme( "b" ) || 
			!predicate_lexeme( "c" )
		) &&
		!(
			 predicate_lexeme( "d" ) || 
			!predicate_lexeme( "e" ) || 
			 predicate_lexeme( "f" )
		) &&
		!(
			predicate_lexeme( "g" ) ||
			predicate_lexeme( "h" )
		)
	);
	

	R_ASSERT_CMP				( g_and_ctor_counter, ==, g_and_dtor_counter );
	R_ASSERT_CMP				( g_or_ctor_counter, ==, g_or_dtor_counter );
	
	g_and_ctor_counter			= 0;
	g_and_dtor_counter			= 0;
	g_or_ctor_counter			= 0;
	g_or_dtor_counter			= 0;

	functor	(
		(
			predicate_lexeme( "a" ) ||
			predicate_lexeme( "b" )
		) 
		&&
		(
			predicate_lexeme( "c" )	||
			predicate_lexeme( "d" ) 
		)
		&&
		(
			predicate_lexeme( "e" )	||
			predicate_lexeme( "f" ) 
		)
	);

	R_ASSERT_CMP				( g_and_ctor_counter, ==, g_and_dtor_counter );
	R_ASSERT_CMP				( g_or_ctor_counter, ==, g_or_dtor_counter );
	/*
	functor	(
		!predicate_lexeme( "predicate_type_clear" ) &&
		!(
			predicate_lexeme( "predicate_type_on_table" ) &&
			predicate_lexeme( "predicate_type_hand_empty" )
		) ||
		predicate_lexeme( "predicate_type_holding" )
	);
	*/

	functor	(
		( !predicate_lexeme( "predicate_type_clear" ) || predicate_lexeme( "predicate_type_holding" ) ) &&
		   predicate_lexeme( "predicate_type_on_table" )
	);

	//base_lexeme const& expression =
	//	!predicate_lexeme( "predicate_type_clear" ) &&
	//	!(
	//		predicate_lexeme( "predicate_type_on_table" ) &&
	//		predicate_lexeme( "predicate_type_hand_empty" )
	//	) ||
	//	predicate_lexeme( "predicate_type_holding" );
// 	string256	aga;
// 	memory::fill8( aga, 'T', sizeof(aga) );
// 	printf("", aga );
// 	functor ( expression && predicate_lexeme( "nu ni figa sebe!" ) );

	registered_parameters_type registered_parameters;
	registered_parameters.push_back( std::make_pair( typeid( npc* ).name(), parameter_type_npc_ptr ) );
	registered_parameters.push_back( std::make_pair( typeid( weapon* ).name(), parameter_type_weapon_ptr ) );
	
	npc npc0;
	npc0.alive	= true;
	npc npc1;
	npc1.alive	= false;
	weapon ruzhzho1;
	ruzhzho1.is_loaded = true;
	//weapon ruzhzho2;
	//ruzhzho2.is_loaded = false;

//	npc_base* npc_test = &npc0;
	
	property_value first_value;
	first_value.instance_ptr	= &npc0;

	property_value first_value1;
	first_value1.instance_ptr	= &ruzhzho1;

// 	if ( first_value == first_value1 )
// 		int s = 0;

	property_value second_value;
	second_value.instance_ptr	= &npc1;

	//property_value second_value1;
	//second_value1.instance_ptr	= &ruzhzho2;

	property_values_type first;
	first.push_back	( first_value );

	property_values_type first1;
	first1.push_back	( first_value );
	first1.push_back	( first_value1 );

	property_values_type second;
	second.push_back( second_value );
	
	property_values_type third;
	third.push_back( first_value );
	third.push_back( second_value );

 	pddl_predicate const& predicate = new_predicate( 0, &callee, registered_parameters );
 	bool result1 = ( *predicate.predicate_binder )( predicate, first );
// 	bool result2 = ( *predicate.predicate_binder )( predicate, second );

//	pddl_predicate const& predicate_test = new_predicate<npc*>( 8, &callee_base, registered_parameters );
//	bool result_test = ( *predicate_test.predicate_binder )( predicate_test, first );

 	//pddl_predicate const& predicate1 = new_predicate( 1, &callee1, registered_parameters );
 	//result1 = ( *predicate1.predicate_binder )( predicate1, first1 );
  	
//  	pddl_predicate const& predicate2 = new_predicate<npc*, weapon*>( 2, &callee2, registered_parameters );
	memory_leak* const pointer = MT_NEW( memory_leak );
	{
		memory_leak_ptr leak = pointer;
  		pddl_predicate const& predicate2 =
//			new_predicate2<npc*, weapon*, npc_base*, weapon*>(
//				2,
////				boost::function<bool (npc_base*, weapon*) >( boost::bind( &callee2, leak, _1, _2 ) ),
//				boost::bind( &callee2, leak, _1, _2 ),
//				registered_parameters
//			);
			new_predicate(
				2,
				&callee3,
				registered_parameters
			);
 		result1 = ( *predicate2.predicate_binder )( predicate2, first1 );
		pddl_predicate dummy_predicate = predicate2;
 		result1 = ( *dummy_predicate.predicate_binder )( predicate2, first1 );
	}

	LOG_INFO	( "%d", pointer->reference_count() );
}

} // namespace foo
} // namespace xray

////////////////////////////////////////////////////////////////////////
#endif // #ifndef MASTER_GOLD