////////////////////////////////////////////////////////////////////////////
//	Created		: 12.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_PREDICATE_LEXEME_H_INCLUDED
#define PDDL_PREDICATE_LEXEME_H_INCLUDED

#include "base_lexeme.h"
#include "and_lexeme.h"
#include "or_lexeme.h"

namespace xray {
namespace ai {
namespace planning {

class pddl_predicate;

class pddl_predicate_lexeme : public base_lexeme
{
public:
	inline explicit		pddl_predicate_lexeme	( pddl_predicate const* required_predicate );

	virtual	void				invert_value	( ) const;

	virtual	base_lexeme const&	expand_brackets	( memory::stack_allocator& allocator, base_lexeme const& right ) const;
	virtual	base_lexeme const&	generate_permutations	( memory::stack_allocator& allocator, base_lexeme const& left ) const;

	virtual	u32	memory_size_for_brackets_expansion		( ) const;
	virtual	u32 memory_size_for_brackets_expansion		( base_lexeme const& right ) const;
	virtual	u32	memory_size_for_permutations_generation	( base_lexeme const& left ) const;

	virtual	void				destroy			( ) const;

protected:
	pddl_predicate const*		m_predicate;
	mutable bool				m_value;
	bool						m_destroy_manually;
}; // class pddl_predicate_lexeme

inline	and_lexeme	operator &&	( base_lexeme const& left, base_lexeme const& right );
inline	or_lexeme	operator ||	( base_lexeme const& left, base_lexeme const& right );

} // namespace planning
} // namespace ai
} // namespace xray

#include "pddl_predicate_lexeme_inline.h"

#endif // #ifndef PDDL_PREDICATE_LEXEME_H_INCLUDED