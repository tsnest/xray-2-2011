////////////////////////////////////////////////////////////////////////////
//	Module 		: shared_string_manager.h
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : shared string manager
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_SHARED_STRING_MANAGER_H_INCLUDED
#define CS_CORE_SHARED_STRING_MANAGER_H_INCLUDED

#include "shared_string_profile.h"

class CS_CORE_API shared_string_manager {
private:
	typedef shared_string_profile*	profile_ptr;

private:
	struct shared_string_data_predicate {
		inline bool	operator()	(profile_ptr const& _0, profile_ptr const& _1) const
		{
			return	_0->checksum_no_check() < _1->checksum_no_check();
		}
	};

private:
#if 1
	typedef cs_multiset<
		profile_ptr,
		shared_string_data_predicate
	>	STORAGE;
#else
	typedef cs_hash_multiset<
		profile_ptr,
		shared_string_data_predicate
	>	STORAGE;
#endif

private:
#pragma warning(push)
#pragma warning(disable:4251)
	STORAGE				m_storage;
#pragma warning(pop)
	threading::mutex	m_mutex;

public:
						~shared_string_manager	();
			profile_ptr	shared_string			(pcstr value);
			void		check_consistency		();
			void		collect_garbage			();
			int			compute_stats			(STORAGE const& storage);
			int			compute_stats			();
			void		dump					();
};

extern CS_CORE_API shared_string_manager* g_shared_string_manager;

#endif // #ifndef CS_CORE_SHARED_STRING_MANAGER_H_INCLUDED