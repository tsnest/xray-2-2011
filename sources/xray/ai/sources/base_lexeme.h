////////////////////////////////////////////////////////////////////////////
//	Created		: 12.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_LEXEME_H_INCLUDED
#define BASE_LEXEME_H_INCLUDED

#include "base_lexeme_ptr.h"

namespace xray {

namespace memory {
	class stack_allocator;
} // namespace memory

namespace ai {
namespace planning {

class specified_problem;
class generalized_action;
class pddl_domain;

struct operands_calculator
{
	inline	operands_calculator	( u32 const operands_count, u32 const and_count, u32 const or_count );

	u32 						operands_count;
	u32 						and_count;
	u32 						or_count;
}; // struct operands_calculator

class base_lexeme : private boost::noncopyable
{
	friend class base_lexeme_ptr;
	
public:
			void	invert_value						( ) const;
	virtual			~base_lexeme						( ) { }
			
	base_lexeme_ptr	expand_brackets						( memory::stack_allocator& allocator ) const;
	base_lexeme_ptr	expand_brackets						( memory::stack_allocator& allocator, base_lexeme const& right ) const;
	base_lexeme_ptr	generate_permutations				( memory::stack_allocator& allocator, base_lexeme const& left ) const;

operands_calculator	count_operands_for_brackets_expansion	( ) const;

			void	add_to_target_world_state			( specified_problem& problem, u32& offset ) const;
			void	add_to_preconditions				( generalized_action& action ) const;
			void	add_to_effects						( generalized_action& action ) const;

	inline	u32		memory_size_for_brackets_expansion	( ) const;

protected:
	inline			base_lexeme							( bool const destroy_manually );

protected:
	mutable bool	m_value;

private:
	enum operation_type_enum
	{
		operation_type_and,
		operation_type_or,
		operation_type_predicate
	}; // enum operation_type_enum

private:
	inline						base_lexeme				(
									operation_type_enum const operation_type,
									base_lexeme const& left,
									base_lexeme const& right,
									bool const destroy_manually
								);

private:
			void				reset_pointers			( ) const;
			void				increment_counter		( ) const;
			void				decrement_counter		( ) const;

			void				invert_value_as_and		( ) const;
			void				invert_value_as_or		( ) const;
			void			invert_value_as_predicate	( ) const;
		operands_calculator		count_operands_as_and	( ) const;
		operands_calculator		count_operands_as_or	( ) const;
		operands_calculator	count_operands_as_predicate	( ) const;
			base_lexeme_ptr		expand_brackets_as_and	( memory::stack_allocator& allocator ) const;
			base_lexeme_ptr		expand_brackets_as_or	( memory::stack_allocator& allocator ) const;
			base_lexeme_ptr	expand_brackets_as_predicate( memory::stack_allocator& allocator ) const;
			base_lexeme_ptr		expand_brackets_as_and	( memory::stack_allocator& allocator, base_lexeme const& right ) const;
			base_lexeme_ptr		expand_brackets_as_or	( memory::stack_allocator& allocator, base_lexeme const& right ) const;
			base_lexeme_ptr	expand_brackets_as_predicate( memory::stack_allocator& allocator, base_lexeme const& right ) const;
			base_lexeme_ptr	generate_permutations_as_and( memory::stack_allocator& allocator, base_lexeme const& left ) const;
			base_lexeme_ptr	generate_permutations_as_or	( memory::stack_allocator& allocator, base_lexeme const& left ) const;
			base_lexeme_ptr	generate_permutations_as_predicate( memory::stack_allocator& allocator, base_lexeme const& left ) const;
			void		add_to_target_world_state_as_and( specified_problem& problem, u32& offset ) const;
			void		add_to_target_world_state_as_or	( specified_problem& problem, u32& offset ) const;
	virtual	void add_to_target_world_state_as_predicate	( specified_problem& problem, u32& offset ) const;
			void			add_to_preconditions_as_and	( generalized_action& action ) const;
			void			add_to_preconditions_as_or	( generalized_action& action ) const;
	virtual	void	add_to_preconditions_as_predicate	( generalized_action& action ) const;
			void				add_to_effects_as_and	( generalized_action& action ) const;
			void				add_to_effects_as_or	( generalized_action& action ) const;
	virtual	void			add_to_effects_as_predicate	( generalized_action& action ) const;

	friend inline base_lexeme operator &&				( base_lexeme const& left, base_lexeme const& right );
	friend inline base_lexeme operator ||				( base_lexeme const& left, base_lexeme const& right );

private:
	class function_pointers
	{
	public:
								function_pointers		( operation_type_enum const operation_type );

	private:
		typedef void				( base_lexeme::*value_invertor )	( ) const;
		typedef operands_calculator	( base_lexeme::*operands_counter )	( ) const;
		typedef base_lexeme_ptr		( base_lexeme::*brackets_opener1 )	( memory::stack_allocator& allocator ) const;
		typedef base_lexeme_ptr		( base_lexeme::*brackets_opener2 )	( memory::stack_allocator& allocator, base_lexeme const& right ) const;
		typedef base_lexeme_ptr		( base_lexeme::*generator )			( memory::stack_allocator& allocator, base_lexeme const& left ) const;
		typedef void				( base_lexeme::*world_state_filler )( specified_problem& problem, u32& offset ) const;
		typedef void				( base_lexeme::*conditions_filler )	( generalized_action& action ) const;

	public:
		value_invertor				m_value_invertor;
		operands_counter			m_operands_counter;
		brackets_opener1			m_brackets_opener1;
		brackets_opener2			m_brackets_opener2;
		generator					m_generator;
		world_state_filler			m_world_state_filler;
		conditions_filler			m_preconditions_filler;
		conditions_filler			m_effects_filler;
	}; // class function_pointers

	static const function_pointers	s_or_function_pointers;
	static const function_pointers	s_and_function_pointers;
	static const function_pointers	s_predicate_function_pointers;

private:
	mutable function_pointers const* m_function_pointers;
	mutable base_lexeme_ptr			m_left;
	mutable base_lexeme_ptr			m_right;
	mutable u16						m_counter;
	u8 const						m_destroy_manually;
	mutable u8						m_operation_type;
}; // class base_lexeme

template < typename T >
inline T const& operator !				( T const& expression );

inline operands_calculator operator +	( operands_calculator const& left, operands_calculator const& right );

} // namespace planning
} // namespace ai
} // namespace xray

#include "base_lexeme_inline.h"

#endif // #ifndef BASE_LEXEME_H_INCLUDED