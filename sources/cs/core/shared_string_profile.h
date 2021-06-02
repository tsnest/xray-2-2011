////////////////////////////////////////////////////////////////////////////
//	Module 		: shared_string_profile.h
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : shared string profile
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_SHARED_STRING_PROFILE_H_INCLUDED
#define CS_CORE_SHARED_STRING_PROFILE_H_INCLUDED

#include <boost/crc.hpp>
#include <cs/gtl/intrusive_ptr.h>

namespace string { namespace detail {
	struct intrusive_base {
		unsigned int	m_reference_count;

		inline			intrusive_base	() : m_reference_count(0)
		{
		}

		template <typename T>
		inline	void	destroy			(T* object)
		{
			CS_UNREFERENCED_PARAMETER	(object);
		}
	};
}}

#pragma pack(push)
#pragma pack(4)
class shared_string_profile : public string::detail::intrusive_base {
public:
	typedef gtl::intrusive_ptr<
		shared_string_profile,
		string::detail::intrusive_base
	>	shared_string_profile_ptr;

private:
	typedef boost::crc_32_type	crc_processor_type;

private:
	enum {
		max_length	= 4096,
	};

private:
	u32				m_length;
	u32				m_checksum;
#if CS_DEBUG_LIBRARIES
#	pragma warning(push)
#	pragma warning(disable:4200)
	char			m_value[];
#	pragma warning(pop)
#endif // #if CS_DEBUG_LIBRARIES

public:
	static			shared_string_profile*	create				(pcstr value, shared_string_profile const& temp);
	static			void					create_temp			(pcstr value, shared_string_profile&	result);
	static			void					destroy				(shared_string_profile* string);
	static	inline	u32						create_checksum		(pcvoid begin, pcvoid end);

public:
			inline	u32					reference_count	() const;

public:
			inline	u32					length				() const;
			inline	u32					length_no_check	() const;

public:
			inline	u32					checksum			() const;
			inline	u32					checksum_no_check	() const;

public:
			inline	pcstr					value				() const;
};
#pragma pack(pop)

#include "shared_string_profile_inline.h"

#endif // #ifndef CS_CORE_SHARED_STRING_PROFILE_H_INCLUDED