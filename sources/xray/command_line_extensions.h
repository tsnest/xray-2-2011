////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COMMAND_LINE_EXTENSIONS_H_INCLUDED
#define XRAY_COMMAND_LINE_EXTENSIONS_H_INCLUDED

#include <xray/fixed_string.h>

namespace xray {

namespace core {
	struct engine;
} // namespace core

namespace command_line {

class XRAY_CORE_API key
{
public:
								key						(pcstr full_name, pcstr short_name, pcstr category, pcstr description, pcstr argument_description = "");

								operator bool			() { return is_set(); }

	bool						is_set					();
	bool						is_set_as_string		(buffer_string * out_value);
	bool						is_set_as_number		(float * out_value);

	template <class int_type>
	bool						is_set_as_number		(int_type * out_value)
	{
		float float_value	=	0;
		if ( !is_set_as_number(& float_value) )
			return				false;
		* out_value			=	(int_type)float_value;
		return					true;
	}

	pcstr						full_name				() const { return m_full_name; }
	pcstr						short_name				() const { return m_short_name; }
	pcstr						category				() const { return m_category; }
	pcstr						description				() const { return m_description; }
	pcstr						argument_description	() const { return m_argument_description; }

	void						initialize				(pcstr value);

private:
	void						protected_construct		();
	void						initialize				();

	friend void					protected_key_construct (pvoid);

private:
	fixed_string512				m_string_value;
	float						m_number_value;
	pcstr						m_full_name;
	pcstr						m_short_name;
	pcstr						m_category;
	pcstr						m_description;
	pcstr						m_argument_description;

	enum type_enum				{ type_uninitialized, type_unset, type_void, type_number, type_string };
	type_enum					m_type;
	
public:
	command_line::key *			m_next_key;
};

enum contains_application_bool
{
	contains_application_false,
	contains_application_true,
};

				void	initialize	( core::engine * engine, pcstr command_line, contains_application_bool );
				bool	initialized ( );
XRAY_CORE_API	void	handle_help_key ( );
XRAY_CORE_API	void	show_help_and_exit	( );
XRAY_CORE_API	void	check_keys	( );
XRAY_CORE_API	bool	show_help	( );
XRAY_CORE_API	void	set_finger_print ( pcstr );

} // namespace command_line
} // namespace xray

#endif // #ifndef XRAY_COMMAND_LINE_EXTENSIONS_H_INCLUDED