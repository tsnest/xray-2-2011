////////////////////////////////////////////////////////////////////////////
//	Module 		: shared_string_profile_inline.h
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : shared string profile inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_SHARED_STRING_PROFILE_INLINE_H_INCLUDED
#define CS_CORE_SHARED_STRING_PROFILE_INLINE_H_INCLUDED

inline u32  shared_string_profile::reference_count	() const
{
	return				m_reference_count;
}

inline u32  shared_string_profile::length				() const
{
	ASSERT				(sz_len(value()) == m_length, "shared string is corrupted");
	return				m_length;
}

inline u32  shared_string_profile::length_no_check	() const
{
	return				m_length;
}

inline u32  shared_string_profile::checksum			() const
{
#if CS_DEBUG_LIBRARIES
	crc_processor_type	temp;
	pcstr				value = this->value();
	temp.process_block	(value,value + m_length);
	ASSERT				(m_checksum == temp.checksum(), "shared string is corrupted");
#endif // #if CS_DEBUG_LIBRARIES

	return				m_checksum;
}

inline u32  shared_string_profile::checksum_no_check	() const
{
	return				m_checksum;
}

inline pcstr shared_string_profile::value					() const
{
	return				((pcstr)this) + sizeof(shared_string_profile);
}

#endif // #ifndef CS_CORE_SHARED_STRING_PROFILE_INLINE_H_INCLUDED