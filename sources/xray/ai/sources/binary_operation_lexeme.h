////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_OPERATION_LEXEME_H_INCLUDED
#define BINARY_OPERATION_LEXEME_H_INCLUDED

#include "base_lexeme.h"

namespace xray {
namespace ai {
namespace planning {

class binary_operation_lexeme : public base_lexeme
{
protected:
	enum operation_type_enum
	{
		operation_type_and,
		operation_type_or
	}; // enum operation_type_enum

public:
					binary_operation_lexeme	( 
						operation_type_enum const operation_type,
						base_lexeme const& left,
						base_lexeme const& right,
						bool const destroy_manually
					);

	virtual	void	invert_value			( ) const;
	virtual	void	destroy					( ) const;
	virtual	void add_to_target_world_state	( pddl_problem& problem ) const;

protected:
	mutable base_lexeme const*				m_left;
	mutable base_lexeme const*				m_right;
	mutable operation_type_enum				m_operation_type;
	bool const								m_destroy_manually;
}; // class binary_operation_lexeme

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef BINARY_OPERATION_LEXEME_H_INCLUDED