////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/skeleton.h>

using xray::animation::skeleton_bone;
using xray::animation::bone_index_type;
using xray::animation::skeleton;
using xray::strings::compare;

skeleton::skeleton			( u32 const bones_count ) :
	m_bones_count			( bones_count )
{
}

skeleton::~skeleton			( )
{
	skeleton_bone* i		= &get_root();
	skeleton_bone* const e	= i + m_bones_count;
	for ( ; i != e; ++i )
		i->~skeleton_bone	( );
}

struct bone_id_predicate {
	inline	bone_id_predicate	( pcstr const bone_name ) :
		m_bone_name			( bone_name )
	{
	}

	inline bool operator ( )	( skeleton_bone const& skeleton_bone )
	{
		return				xray::strings::equal( skeleton_bone.id(), m_bone_name );
	}

	pcstr					m_bone_name;
}; // struct bone_id_predicate

bone_index_type xray::animation::skeleton_bone_index	( skeleton const& skeleton, pcstr const bone_name )
{
	u32 const bones_count	= skeleton.get_bones_count();
	skeleton_bone const* const begin	= &skeleton.get_bone(0);
	skeleton_bone const* const end		= &skeleton.get_bone( bones_count - 1 ) + 1;
	skeleton_bone const* const result	= std::find_if( begin, end, bone_id_predicate( bone_name ) );
	R_ASSERT				( result != end );
	return					result - begin;
}

pcstr skeleton::get_bone_name	( bone_index_type const index ) const
{
	return					get_bone( index ).id();
}	

bone_index_type	skeleton::get_bone_index	( pcstr const name ) const
{
	return					skeleton_bone_index( *this, name );
}