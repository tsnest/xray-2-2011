////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_SKELETON_INLINE_H_INCLUDED
#define XRAY_ANIMATION_SKELETON_INLINE_H_INCLUDED

namespace xray {
namespace animation {

inline u32 skeleton::get_bone_index				( skeleton_bone const& bone ) const
{
	u32 const result	= u32(&bone - &get_root());
	R_ASSERT_CMP		( result, <, m_bones_count );
	return				result;
}

inline u32 skeleton::get_bones_count			( ) const
{
	return				m_bones_count;
}

inline u32 skeleton::get_root_bones_count		( ) const
{
	skeleton_bone const* const roots_begin	= &get_root();
	skeleton_bone const* const roots_end	= roots_begin->children_begin();
	R_ASSERT_CMP		( get_bones_count(), >=, u32( roots_end - roots_begin ) );
	return				u32( roots_end - roots_begin );
}

inline u32 skeleton::get_non_root_bones_count	( ) const
{
	return				get_bones_count() - get_root_bones_count();
}

inline skeleton_bone const& skeleton::get_bone	( bone_index_type const index ) const
{
	R_ASSERT_CMP		( index, <, m_bones_count );
	return				*(&get_root() + index);
}

inline skeleton_bone const& skeleton::get_root	( ) const
{
	return				*pointer_cast<skeleton_bone const*>( static_cast<pcvoid>(this + 1) );
}

inline skeleton_bone& skeleton::get_root		( )
{
	return				*pointer_cast<skeleton_bone*>( static_cast<pvoid>(this + 1) );
}

inline bool skeleton::is_root_bone				( bone_index_type const index ) const
{
	return				index >= get_root_bones_count();
}

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_SKELETON_INLINE_H_INCLUDED