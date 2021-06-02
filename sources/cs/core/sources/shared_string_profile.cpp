////////////////////////////////////////////////////////////////////////////
//	Module 		: shared_string_profile.cpp
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : shared string profile
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

inline u32 shared_string_profile::create_checksum			(pcvoid begin, pcvoid end)
{
	crc_processor_type				temp;
	temp.process_block				(begin,end);
	return							temp.checksum();
}

shared_string_profile* shared_string_profile::create		(pcstr value, shared_string_profile const& temp)
{
	STATIC_CHECK					(sizeof(shared_string_profile) == 12,Check_if_your_changes_to_shared_string_profile_or_intrusive_base_are_correct_and_then_change_this_constant);

	ASSERT							(value);
	ASSERT							(sz_len(value) == temp.m_length, "shared string is corrupted");
	ASSERT							(create_checksum(value, value + temp.m_length) == temp.m_checksum, "shared string is corrupted");

	u32								buffer_length = (temp.m_length + 1)*sizeof(char);
	R_ASSERT						((sizeof(shared_string_profile) + buffer_length) < max_length, "alignment is corrupted, check compiler options");

	shared_string_profile*			result = (shared_string_profile*)cs_malloc( sizeof(shared_string_profile) + buffer_length, "shared_string" );
	result->m_reference_count		= 0;
	result->m_length				= temp.m_length;
	result->m_checksum				= temp.m_checksum;

	mem_copy						(const_cast<pstr>(result->value()),value,buffer_length);

	return							result;
}

void shared_string_profile::create_temp						(pcstr value, shared_string_profile&	result)
{
#if CS_DEBUG_LIBRARIES
	result.m_reference_count		= u32(-1);
#endif
	result.m_length					= sz_len(value);
	result.m_checksum				= create_checksum(value,value + result.m_length);
}

void shared_string_profile::destroy							(shared_string_profile* string)
{
	string->~shared_string_profile	();
	cs_free							(string);
}
