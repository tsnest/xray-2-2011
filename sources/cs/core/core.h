////////////////////////////////////////////////////////////////////////////
//	Module 		: core.h
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : core functionality
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_CORE_H_INCLUDED
#define CS_CORE_CORE_H_INCLUDED

class CS_CORE_API cs_core {
public:
				cs_core					();
	void		initialize				(pcstr library_user, bool use_debug_engine, bool use_logging);
	void		uninitialize			();

private:
	void		initialize_impl			(pcstr library_user, bool use_debug_engine, bool use_logging);

public:
	pcstr		m_application_name;
	pcstr		m_user_name;
};

namespace cs { namespace core {
	extern CS_CORE_API cs_core core;
}}

#endif // #ifndef CS_CORE_CORE_H_INCLUDED