////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_PARAMETER_H_INCLUDED
#define ACTION_PARAMETER_H_INCLUDED

#include <xray/ai/parameter_types.h>

namespace xray {
namespace ai {
namespace planning {

class action_parameter : private boost::noncopyable
{
public:
							action_parameter	( parameter_type const type );

	inline	parameter_type	get_type			( ) const { return m_type; }

	inline	void			iterate_first		( bool value ) { m_iterate_only_first = value; }
	inline	bool			iterate_only_first	( ) const { return m_iterate_only_first; }
	inline	pcstr			get_selector_name	( ) const { return m_selector_name.c_str(); }
	inline	void			set_selector_name	( pcstr name ) { m_selector_name = name; }
	
private:
	parameter_type const						m_type;
	bool										m_iterate_only_first;
	fixed_string< 32 >							m_selector_name;						
}; // class action_parameter

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef ACTION_PARAMETER_H_INCLUDED