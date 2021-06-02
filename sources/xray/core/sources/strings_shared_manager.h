////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STRINGS_SHARED_MANAGER_H_INCLUDED
#define STRINGS_SHARED_MANAGER_H_INCLUDED

#include <xray/strings_shared_profile.h>
#include <xray/hash_multiset.h>

namespace xray {
namespace strings {
namespace shared {

extern memory::doug_lea_allocator_type		g_allocator;

class manager {
public:
	typedef profile*	profile_ptr;

public:
						~manager			( );
			profile_ptr	string				( pcstr value );
			void		remove				( profile* profile );
			void		check_consistency	( );
			void		collect_garbage		( );
			int			compute_stats		( );
			void		dump				( );

private:
	struct hash_function {
		inline u32	operator( )	( profile const& object ) const
		{
			return		( object.checksum_no_check( ) );
		}

		inline bool	operator( )	( profile const& left, profile const& right ) const
		{
			return		( left.checksum_no_check( ) == right.checksum_no_check( ) );
		}
	}; // struct hash_function

private:
#if 1
	typedef hash_multiset<
		profile,
		profile *,
		& profile::next_in_hashset,
		xray::detail::fixed_size_policy<32*1024>,
		hash_function,
		hash_function
	>					Storage;
#else // #if 0
	typedef multiset<
		profile_ptr,
		hash_function
	>					Storage;
#endif // #if 0


private:
			int			compute_stats		( Storage const& storage );

private:
	threading::mutex	m_mutex;
	Storage				m_storage;
}; // class manager

} // namespace shared

void initialize	( );
void finalize	( );

} // namespace strings
} // namespace xray

#endif // #ifndef STRINGS_SHARED_MANAGER_H_INCLUDED