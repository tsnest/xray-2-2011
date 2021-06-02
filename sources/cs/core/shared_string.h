////////////////////////////////////////////////////////////////////////////
//	Module 		: shared_string.h
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : shared string
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_SHARED_STRING_H_INCLUDED
#define CS_CORE_SHARED_STRING_H_INCLUDED

#include "shared_string_manager.h"

class shared_string {
private:
	typedef shared_string_profile::shared_string_profile_ptr	profile_ptr;
	typedef pcstr (shared_string::*unspecified_bool_type) () const;

private:
	profile_ptr			m_pointer;

public:
	inline				shared_string					();
	inline				shared_string					(pcstr value);
	inline				shared_string					(shared_string const& object);

public:
	inline	pcstr		c_str							() const;
	inline	pcstr		operator*						() const;
	inline	bool		operator!						() const;
	inline	bool		operator<						(shared_string const& string) const;
	inline	bool		operator==						(shared_string const& string) const;
	inline	bool		operator!=						(shared_string const& string) const;
	inline				operator unspecified_bool_type	() const;
	inline	u32 	length							() const;
	inline	u32			size							() const;
};

#include "shared_string_inline.h"

#endif // #ifndef CS_CORE_SHARED_STRING_H_INCLUDED